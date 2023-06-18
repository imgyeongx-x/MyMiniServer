#include "pch.h"
#define _USE_INIT_WINDOW_ 
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
void InitWindow()
{
    gp_window_title = "로그인 채팅 ";
    g_wnd_style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPCHILDREN;
}

typedef struct ApplicationData  
{
    NeoSocketData client;
    void *p_event_list;  
} AD;

void AddEventString(AD *ap_app, const char *ap_string)
{
    ListBox_InsertString(ap_app->p_event_list, -1, ap_string);
}

int OnUserMsg(HWND ah_wnd, UINT a_msg_id, WPARAM wParam, LPARAM lParam)
{
    AD *p_app = (AD *)GetAppData();

    if (a_msg_id == CLIENT_SOCKET_CONNECT) ProcessConnectionResult(&p_app->client, WSAGETSELECTERROR(lParam));
    else if (a_msg_id == CLIENT_SOCKET_COMMON) ProcessSocketEvent(&p_app->client, WSAGETSELECTEVENT(lParam));

    return 0;
}

void ClientSocketNotifyProc(NeoSocketData *ap_data, UINT32 a_state)
{
    //i f (a_state == 1);   // 서버에 접속을 성공 
    // if (a_state == 50); // 서버와 연결 해제
}

int ProcessingSocketMessage(NeoSocketData *ap_data)
{
    AD *p_app = (AD *)GetAppData(); 
    if (ap_data->msg_id == ANS_WELCOME_MSG) {
        LoginData login_data; 
        GetCtrlName(FindControl(IDC_USER_ID_EDIT), login_data.id, 32); 
        GetCtrlName(FindControl(IDC_USER_PASSWORD_EDIT), login_data.password, 32); 
        SendFrameData(&p_app->client, REQ_LOGIN_DATA, &login_data, sizeof(login_data)); 
    } else if (ap_data->msg_id == ANS_LOGIN_OK) {
        AddEventString(p_app, "서버에 성공적으로 로그인했습니다.");
    } else if (ap_data->msg_id == ANS_LOGIN_FAIL) {
        AddEventString(p_app, (char *)ap_data->p_recv_data);
        DestroySocket(&p_app->client);
        AddEventString(p_app, "서버와 접속을 해제합니다.");
    } else if (ap_data->msg_id == ANS_USER_CHAT) {  
        AddEventString(p_app, (char *)ap_data->p_recv_data);
    } else if (ap_data->msg_id == ANS_SYSTEM_MESSAGE) { 
        AddEventString(p_app, (char *)ap_data->p_recv_data);
    }

    return 1;
}

void OnDestroy() 
{
    AD *p_app = (AD *)GetAppData(); 
    CleanUpClientData(&p_app->client); 
}

void SendChatData(AD *ap_app)
{
    if (ap_app->client.is_connected == 3) {
        char str[512];
        void *p_edit = FindControl(IDC_INPUT_CHAT_EDIT); 
        int len = Edit_GetLength(p_edit); 
        GetCtrlName(p_edit, str, 512);   
        SendFrameData(&ap_app->client, REQ_USER_CHAT, str, len + 1);
        SetCtrlName(p_edit, ""); 
    }
}

void OnCommand(INT32 a_ctrl_id, INT32 a_notify_code, void *ap_ctrl)
{
    AD *p_app = (AD *)GetAppData();

    if (a_ctrl_id == IDC_CONNECT_BTN) {
        if (p_app->client.h_socket == INVALID_SOCKET) {
            char str[32];
            GetCtrlName(FindControl(IDC_SERVER_IP_EDIT), str, 32); 
            ConnectToServer(&p_app->client, str, 12023);
            AddEventString(p_app, "서버에 접속을 시도 합니다!");
        }
        else AddEventString(p_app, "이미 서버에 접속된 상태입니다.");
    } else if (a_ctrl_id == IDC_DISCONNECT_BTN) {
        if (p_app->client.h_socket != INVALID_SOCKET) {
            DestroySocket(&p_app->client);
            AddEventString(p_app, "서버와 연결을 끊었습니다.");
        }
    } else if (a_ctrl_id == IDC_SEND_CHAT_BTN) SendChatData(p_app);
    else if (a_ctrl_id == IDC_INPUT_CHAT_EDIT) { 
        if (a_notify_code == 1000) SendChatData(p_app); 
    }
}

CMD_USER_MESSAGE(OnCommand, OnDestroy, OnUserMsg)

void ClientAddEventString(NeoSocketData *ap_server, const char *ap_string)
{
    AD *p_app = (AD *)GetAppData(); 
    AddEventString(p_app, ap_string);
}

int main()
{
    ChangeWorkSize(660, 237); 
    Clear(0, RGB(82, 97, 124));

    AD app_data = { 0, };
    ClientAddEventString, ClientSocketNotifyProc);
    app_data.p_event_list = CreateListBox(5, 50, 650, 160, IDC_EVENT_LIST, NULL, 0);
    SetAppData(&app_data, sizeof(AD)); 

    CreateButton("서버에 접속", 260, 17, 80, 26, IDC_CONNECT_BTN); 
    CreateButton("접속 해제", 343, 17, 80, 26, IDC_DISCONNECT_BTN); 
    void *p_edit = CreateEdit(5, 23, 80, 20, IDC_SERVER_IP_EDIT, 0);
    SetCtrlName(p_edit, "192.168.0.51"); 
    CreateEdit(88, 23, 80, 20, IDC_USER_ID_EDIT, 0); 
    CreateEdit(171, 23, 80, 20, IDC_USER_PASSWORD_EDIT, ES_PASSWORD); 
    p_edit = CreateEdit(5, 212, 600, 20, IDC_INPUT_CHAT_EDIT, 0); 
    EnableEnterKey(p_edit);  

    CreateButton("전송", 608, 212, 48, 20, 1002); 

    SelectFontObject("맑은 고딕", 18, 0);  
    SetTextColor(RGB(128, 202, 128)); 
    TextOut(9, 5, "서버 주소");
    TextOut(92, 5, "아이디");
    TextOut(175, 5, "암호");

    ShowDisplay();
    return 0;
}
