#include "pch.h"
#include <stdio.h>
#include "tipsware.h"

struct AppData   // 프로그램에서 사용할 내부 데이터
{
    void* p_socket;
};

// 서버에 접속했거나 접속에 실패했을 때 호출되는 함수
void OnServerConnection(void* ap_this, int a_client_index)
{
    char temp_str[64];
    if (IsConnect(ap_this)) sprintf_s(temp_str, 64, "서버에 접속했습니다!!");
    else sprintf_s(temp_str, 64, "서버에 접속할 수 없습니다.!!");
    // 서버 접속 결과를 리스트 박스(컨트롤 아이디 1000번)에 추가한다.
    ListBox_InsertString(FindControl(1000), -1, temp_str);
}

// 서버가 데이터를 전송 함수
int OnServerMessage(CurrentClientNetworkData* ap_data, void* ap_this, int a_client_index)
{
    if (ap_data->m_net_msg_id == 1) {
        ListBox_InsertString(FindControl(1000), -1, ap_data->mp_net_body_data);
    }
    return 1;
}

// 서버와 접속 상태가 변경 호출
void OnCloseUser(void* ap_this, int a_error_flag, int a_client_index)
{
    char temp_str[64];
    if (a_error_flag == 1) sprintf_s(temp_str, 64, "서버에서 접속을 해제했습니다.");
    else sprintf_s(temp_str, 64, "서버와 접속을 해제했습니다.");
    ListBox_InsertString(FindControl(1000), -1, temp_str);
}

// 서버에 접속 시도 함수
void OnConnectBtn(AppData* ap_data)
{
    if (ap_data->p_socket == NULL) {
        ap_data->p_socket = CreateClientSocket(OnServerConnection, OnServerMessage, OnCloseUser);
    }

    if (!IsConnect(ap_data->p_socket)) {
        ConnectToServer(ap_data->p_socket, "127.0.0.1", 25001);
    }
}

// 서버와 연결 해제 함수
void OnDisconnectBtn(AppData* ap_data)
{
    if (ap_data->p_socket != NULL) {
        DeleteClientSocket(ap_data->p_socket);
        ap_data->p_socket = NULL;
    }
}

// 채팅 데이터 서버 전송
void SendChatData(AppData* ap_data)
{
    void* p_edit = FindControl(1020);
    char str[128];

    GetCtrlName(p_edit, str, 128);
    SetCtrlName(p_edit, "");
    if (ap_data->p_socket && IsConnect(ap_data->p_socket)) {
        SendFrameDataToServer(ap_data->p_socket, 1, str, strlen(str) + 1);
    }
}

void OnCommand(INT32 a_ctrl_id, INT32 a_notify_code, void* ap_ctrl)
{
    AppData* p_data = (AppData*)GetAppData();
    if (a_ctrl_id == 1013 || (a_ctrl_id == 1020 && a_notify_code == 1000)) SendChatData(p_data);
    else if (a_ctrl_id == 1011) OnConnectBtn(p_data);
    else if (a_ctrl_id == 1012) OnDisconnectBtn(p_data);
}

// 버튼을 클릭했을 때 호출할 함수를 지정한다.
CMD_MESSAGE(OnCommand)

int main()
{
    AppData data = { NULL };
    SetAppData(&data, sizeof(AppData));

    ChangeWorkSize(520, 180);
    Clear(0, RGB(82, 97, 124));
    StartSocketSystem();

    CreateListBox(10, 36, 500, 100, 1000);

    CreateButton("접속", 407, 3, 50, 28, 1011);
    CreateButton("해제", 460, 3, 50, 28, 1012);
    CreateButton("입력", 460, 140, 50, 28, 1013);

    void* p = CreateEdit(10, 143, 446, 24, 1020, 0);
    EnableEnterKey(p);

    SelectFontObject("굴림", 12);
    TextOut(15, 16, RGB(200, 255, 200), "사용자 채팅글 목록");

    ShowDisplay();
    return 0;
}