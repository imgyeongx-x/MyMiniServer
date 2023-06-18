#include "pch.h"
#include <stdio.h>   // sprintf_s 함수를 사용하기 위해!!
#include "tipsware.h"

struct UserData
{
    unsigned int h_socket;   // 소켓 핸들
    char ip_address[16];     // 접속한 클라이언트의 주소
};

// 새로운 클라이언트가 접속 함수
void OnNewUser(UserData* ap_user_data, void* ap_server, int a_server_index)
{
    char temp_str[64];
    sprintf_s(temp_str, 64, "새로운 사용자가 %s에서 접속을 했습니다!", ap_user_data->ip_address);
    ListBox_InsertString(FindControl(1000), -1, temp_str);
}

// 클라이언트 데이터 전송 함수
int OnClientMessage(CurrentServerNetworkData* ap_data, void* ap_server, int a_server_index)
{
    UserData* p_user_data = (UserData*)ap_data->mp_net_user;
    char temp_str[128];
    if (ap_data->m_net_msg_id == 1) { 
        sprintf_s(temp_str, 128, "%s : %s", p_user_data->ip_address, ap_data->mp_net_body_data);
        ListBox_InsertString(FindControl(1000), -1, temp_str);
        BroadcastFrameData(ap_server, 1, temp_str, strlen(temp_str) + 1);
    }
    return 1;
}

// 클라이언트가 접속 해제 함수
void OnCloseUser(UserData* ap_user_data, void* ap_server, int a_error_flag, int a_server_index)
{
    char temp_str[64];
    if (a_error_flag == 1) {
        sprintf_s(temp_str, 64, "%s에서 접속한 사용자를 강제로 접속 해제했습니다.", ap_user_data->ip_address);
    }
    else {
        sprintf_s(temp_str, 64, "%s에서 사용자가 접속을 해제하였습니다!", ap_user_data->ip_address);
    }
 
    ListBox_InsertString(FindControl(1000), -1, temp_str);
}

NOT_USE_MESSAGE

int main()
{
    ChangeWorkSize(620, 240);
    Clear(0, RGB(72, 87, 114)); 
    StartSocketSystem();  
    void* p_server = CreateServerSocket(sizeof(UserData), OnNewUser, OnClientMessage, OnCloseUser);
    StartListenService(p_server, "127.0.0.1", 25001);

    CreateListBox(10, 30, 600, 200, 1000);
    SelectFontObject("굴림", 12); 
    TextOut(15, 10, RGB(200, 255, 200), "사용자 채팅글 목록");

    ShowDisplay();
    return 0;
}
