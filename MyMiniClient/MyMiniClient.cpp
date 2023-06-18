#include "pch.h"
#define _USE_INIT_WINDOW_  // 윈도우 전역 초기화 함수를 직접 구현하도록 지정 (InitWindow)
#include "tipsware.h"

#define IDC_CONNECT_BTN          1000  // 서버에 접속 버튼의 아이디
#define IDC_DISCONNECT_BTN       1001  // 접속 해제 버튼의 아이디
#define IDC_SEND_CHAT_BTN        1002  // 채팅을 전송하는 버튼의 아이디
#define IDC_EVENT_LIST           2001  // 상태 목록을 관리하는 리스트 박스의 아이디
#define IDC_SERVER_IP_EDIT       3000  // 서버 주소 입력용 에디트 컨트롤의 아이디
#define IDC_USER_ID_EDIT         3001  // 아이디 입력용 에디트 컨트롤의 아이디
#define IDC_USER_PASSWORD_EDIT   3002  // 암호 입력용 에디트 컨트롤의 아이디
#define IDC_INPUT_CHAT_EDIT      3003  // 채팅 입력용 에디트 컨트롤의 아이디

#define REQ_USER_CHAT                 21    // C -> S 채팅 데이터 전달
#define ANS_USER_CHAT                 22    // S -> C 채팅 데이터 전달 (브로드 캐스팅)

// 자신이 사용할 윈도우의 전역 속성을 초기화 하는 함수
void InitWindow()
{
    // 창 제목을 수정한다.
    gp_window_title = "로그인 기반 채팅 클라이언트 만들기 - Step 1 (김성엽, https://blog.naver.com/tipsware)";
    // 윈도우 속성을 수정한다. 캡션과 테두리가 없는 기본 윈도우를 생성한다.
    g_wnd_style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPCHILDREN;
}

typedef struct ApplicationData  // 프로그램에서 사용할 내부 데이터
{
    NeoSocketData client;  // 서버 소켓 서비스를 위해 필요한 정보
    void* p_event_list;  // 프로그램 상태 목록을 관리하는 리스트 박스
} AD;

// 이벤트를 목록에 추가하는 함수
void AddEventString(AD* ap_app, const char* ap_string)
{
    // 현재 프로그램의 상태를 콤보트 박스에 추가한다.
    ListBox_InsertString(ap_app->p_event_list, -1, ap_string);
}

// 윈도우에 발생하는 일반 메시지를 처리하는 함수
int OnUserMsg(HWND ah_wnd, UINT a_msg_id, WPARAM wParam, LPARAM lParam)
{
    AD* p_app = (AD*)GetAppData();  // 프로그램에 저장된 데이터를 얻는다.

    if (a_msg_id == CLIENT_SOCKET_CONNECT) ProcessConnectionResult(&p_app->client, WSAGETSELECTERROR(lParam)); // 접속 여부
    else if (a_msg_id == CLIENT_SOCKET_COMMON) ProcessSocketEvent(&p_app->client, WSAGETSELECTEVENT(lParam)); // 데이터 처리

    return 0;
}

// 사용자의 상태 통보를 사용하는 함수
void ClientSocketNotifyProc(NeoSocketData* ap_data, UINT32 a_state)
{
    //i f (a_state == 1);   // 서버에 접속을 성공 
    // if (a_state == 50); // 서버와 연결 해제
}

// 네트웤 메시지를 처리하는 함수
int ProcessingSocketMessage(NeoSocketData* ap_data)
{
    AD* p_app = (AD*)GetAppData();  // 프로그램에 저장된 데이터를 얻는다.
    if (ap_data->msg_id == ANS_WELCOME_MSG) {
        LoginData login_data; // 로그인 구조체
        GetCtrlName(FindControl(IDC_USER_ID_EDIT), login_data.id, 32);  // 아이디
        GetCtrlName(FindControl(IDC_USER_PASSWORD_EDIT), login_data.password, 32);  // 암호
        SendFrameData(&p_app->client, REQ_LOGIN_DATA, &login_data, sizeof(login_data));  // 로그인 시도
    }
    else if (ap_data->msg_id == ANS_LOGIN_OK) {
        AddEventString(p_app, "서버에 성공적으로 로그인했습니다.");
    }
    else if (ap_data->msg_id == ANS_LOGIN_FAIL) {
        AddEventString(p_app, (char*)ap_data->p_recv_data);
        DestroySocket(&p_app->client);
        AddEventString(p_app, "서버와 접속을 해제합니다.");
    }
    else if (ap_data->msg_id == ANS_USER_CHAT) {  // 서버가 보내준 채팅 정보
        AddEventString(p_app, (char*)ap_data->p_recv_data);
    }
    else if (ap_data->msg_id == ANS_SYSTEM_MESSAGE) {  // 서버가 보내준 내부 시스템 메시지
        AddEventString(p_app, (char*)ap_data->p_recv_data);
    }

    return 1;
}

void OnDestroy()  // 윈도우가 종료될 때 호출될 함수
{
    AD* p_app = (AD*)GetAppData();  // 프로그램에 저장된 데이터를 얻는다.
    CleanUpClientData(&p_app->client);  // 사용하던 소켓 정보를 정리한다.
}

// 사용자 채팅 메시지를 서버로 전송하는 함수
void SendChatData(AD* ap_app)
{
    if (ap_app->client.is_connected == 3) { // 로그인된 상황에서만 사용
        char str[512];
        void* p_edit = FindControl(IDC_INPUT_CHAT_EDIT);  // 채팅 입력용 에디트 컨트롤
        int len = Edit_GetLength(p_edit);  // 입력된 문자열의 길이를 얻는다.
        GetCtrlName(p_edit, str, 512);     // 입력한 채팅 정보를 얻는다.
        // 사용자가 입력한 정보를 서버로 전송한다.
        SendFrameData(&ap_app->client, REQ_USER_CHAT, str, len + 1);
        SetCtrlName(p_edit, ""); // 입력된 채팅 글을 지운다.
    }
}

// 컨트롤의 아이디(a_ctrl_id), 컨트롤의 조작 상태(a_notify_code), 선택한 컨트롤 객체(ap_ctrl)
void OnCommand(INT32 a_ctrl_id, INT32 a_notify_code, void* ap_ctrl)
{
    AD* p_app = (AD*)GetAppData(); // 프로그램의 내부 데이터 주소를 얻는다.

    if (a_ctrl_id == IDC_CONNECT_BTN) { // [서버에 접속] 버튼을 누른 경우
        if (p_app->client.h_socket == INVALID_SOCKET) { // 서버와 연결되지 않은 상태에서만 서버에 접속
            char str[32];
            GetCtrlName(FindControl(IDC_SERVER_IP_EDIT), str, 32); // 사용자가 입력한 서버 주소를 얻는다.
            ConnectToServer(&p_app->client, str, 12023);  // 서버에 접속을 시도한다.
            AddEventString(p_app, "서버에 접속을 시도 합니다!");
        }
        else AddEventString(p_app, "이미 서버에 접속된 상태입니다.");
    }
    else if (a_ctrl_id == IDC_DISCONNECT_BTN) { // [접속 해제] 버튼을 누른 경우
        if (p_app->client.h_socket != INVALID_SOCKET) {
            DestroySocket(&p_app->client);
            AddEventString(p_app, "서버와 연결을 끊었습니다.");
        }
    }
    else if (a_ctrl_id == IDC_SEND_CHAT_BTN) SendChatData(p_app);// [전송] 버튼을 누른 경우
    else if (a_ctrl_id == IDC_INPUT_CHAT_EDIT) {  // 채팅 내용을 입력하는 에디트 컨트롤
        if (a_notify_code == 1000) SendChatData(p_app);  // 에디트 컨트롤에 엔터키를 입력한 경우
    }
}

CMD_USER_MESSAGE(OnCommand, OnDestroy, OnUserMsg)

void ClientAddEventString(NeoSocketData* ap_server, const char* ap_string)  // 이벤트 메시지를 추가하는 함수
{
    AD* p_app = (AD*)GetAppData();  // 프로그램에 저장된 데이터를 얻는다.
    AddEventString(p_app, ap_string);
}

int main()
{
    ChangeWorkSize(660, 237);  // 창의 크기를 조정한다.    
    Clear(0, RGB(82, 97, 124));  // 전체 색상을 RGB(82, 97, 124)으로 채운다.

    AD app_data = { 0, };
    // 기본 값을 사용해서 클라이언트 소켓 서비스를 초기화한다.
    InitData(&app_data.client, ProcessingSocketMessage, ClientAddEventString, ClientSocketNotifyProc);
    app_data.p_event_list = CreateListBox(5, 50, 650, 160, IDC_EVENT_LIST, NULL, 0);
    SetAppData(&app_data, sizeof(AD)); // 프로그램 정보를 저장한다.    

    CreateButton("서버에 접속", 260, 17, 80, 26, IDC_CONNECT_BTN);  // '서버에 접속' 버튼 생성
    CreateButton("접속 해제", 343, 17, 80, 26, IDC_DISCONNECT_BTN);  // '접속 해제' 버튼 생성

    void* p_edit = CreateEdit(5, 23, 80, 20, IDC_SERVER_IP_EDIT, 0);  // 서버 주소 입력을 위한 에디트 컨트롤을 생성
    SetCtrlName(p_edit, "192.168.0.51"); // 테스트 코드
    CreateEdit(88, 23, 80, 20, IDC_USER_ID_EDIT, 0);  // 아이디 입력을 위한 에디트 컨트롤을 생성
    CreateEdit(171, 23, 80, 20, IDC_USER_PASSWORD_EDIT, ES_PASSWORD);  // 암호 입력을 위한 에디트 컨트롤을 생성
    p_edit = CreateEdit(5, 212, 600, 20, IDC_INPUT_CHAT_EDIT, 0);  // 채팅 입력을 위한 에디트 컨트롤을 생성
    EnableEnterKey(p_edit);  // 엔터키를 누르면 notify code 1000 발생

    CreateButton("전송", 608, 212, 48, 20, 1002);  // '전송' 버튼 생성

    SelectFontObject("맑은 고딕", 18, 0);  // 글꼴을 변경한다.
    SetTextColor(RGB(128, 202, 128));  // 문자열의 색상을 지정한다.
    TextOut(9, 5, "서버 주소");
    TextOut(92, 5, "아이디");
    TextOut(175, 5, "암호");

    ShowDisplay(); // 정보를 윈도우에 출력한다.
    return 0;
}
