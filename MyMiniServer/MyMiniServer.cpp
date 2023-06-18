#include "pch.h"
#define _USE_INIT_WINDOW_  
#include <stdio.h> 
#include "tipsware.h"

#define IDC_SERVICE_START_BTN    1000  // 서비스 시작 버튼의 아이디
#define IDC_SERVICE_STOP_BTN     1001  // 서비스 멈춤 버튼의 아이디
#define IDC_MODIFY_USER_BTN      1100  // 사용자 정보 변경 버튼의 아이디
#define IDC_DEL_USER_BTN         1101  // 사용자 정복 삭제 버턴의 아이디
#define IDC_USER_LIST            2000  // 사용자 목록을 관리하는 리스트 박스의 아이디
#define IDC_EVENT_LIST           2001  // 상태 목록을 관리하는 리스트 박스의 아이디
#define IDC_IP_COMBO             3000  // 서비스 아피 목록을 관리하는 콤보 박스의 아이디
#define IDC_USER_LEVEL_COMBO     3001  // 사용자 등급 목록을 관리하는 콤보 박스의 아이디
#define IDC_USER_ID_EDIT         4000  // 아이디 입력용 에디트 컨트롤의 아이디
#define IDC_USER_PASSWORD_EDIT   4001  // 암호 입력용 에디트 컨트롤의 아이디
#define IDC_USER_NAME_EDIT       4002  // 이름 입력용 에디트 컨트롤의 아이디
#define IDC_USER_ETC_EDIT        4003  // 기타 정보 입력용 에디트 컨트롤의 아이디

const char* gp_level_string[] = { "손님", "임시회원", "일반회원", "성실회원", "우수회원", "공동체", "운영진", "VIP", "관리자" };
int g_level_len[] = { 4, 8, 8, 8, 8, 6, 6, 3, 6 };

#define REQ_USER_CHAT                 21
#define ANS_USER_CHAT                 22  

void InitWindow()
{
    // 창 제목을 수정한다.
    gp_window_title = "로그인";
    // 윈도우 속성을 수정한다. 캡션과 테두리가 없는 기본 윈도우를 생성한다.
    g_wnd_style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPCHILDREN;
}

typedef struct ApplicationData  // 프로그램에서 사용할 내부 데이터
{
    NeoServerData server_data;  // 서버 소켓 서비스를 위해 필요한 정보
    void* p_user_list;  // 사용자 목록을 관리하는 리스트 박스
    void* p_event_list;  // 프로그램 상태 목록을 관리하는 리스트 박스
} AD;

// 이벤트를 목록에 추가하는 함수
void AddEventString(AD* ap_app, const char* ap_string)
{
    ListBox_InsertString(ap_app->p_event_list, -1, ap_string); // 프로그램의 상태를 콤보트 박스에 추가
}

// 윈도우에 발생하는 일반 메시지를 처리하는 함수
int OnUserMsg(HWND ah_wnd, UINT a_msg_id, WPARAM wParam, LPARAM lParam)
{
    AD* p_app = (AD*)GetAppData();  // 프로그램에 저장된 데이터를 얻는다.

    if (a_msg_id == SERVER_SOCKET_USER) { // 로그인된 사용자를 위한 네트워크 메시지
        ProcessUserSocketEvent(&p_app->server_data, (SOCKET)wParam, WSAGETSELECTEVENT(lParam));
        return 0;
    }
    else if (a_msg_id == SERVER_SOCKET_CLIENT) { // 로그인 전에 발생하는 네트워크 메시지
        ProcessCommonSocketEvent(&p_app->server_data, (SOCKET)wParam, WSAGETSELECTEVENT(lParam));
        return 0;
    }
    else if (a_msg_id == SERVER_SOCKET_ACCEPT) { // 서버에 접속을 시도할 때 발생하는 메시지
        ProcessCommonAccept(&p_app->server_data);
        return 0;
    }
    else if (a_msg_id == SERVER_STATE_NOTIFY) { // 서버 소켓에 발생한 여러가지 상태에 대한 메시지
        Invalidate(p_app->p_user_list); // 사용자 리스트를 갱신한다.
        return 0;
    }
    return 0;
}

// 클라이언트가 전송한 데이터를 처리하는 함수
int UserMessageProc(NeoServerData* ap_server, NRUD* ap_user)
{
    AD* p_app = (AD*)GetAppData();  // 프로그램에 저장된 데이터를 얻는다.

    if (ap_user->msg_id == REQ_USER_CHAT) { // 사용자가 채팅 정보를 전송한 경우
        // 수신한 채팅글에 채팅을 전송한 사람의 이름을 붙인다.
        int len = sprintf_s(ap_server->p_buffer2, TSB_SIZE, "%s: %s", ap_user->info.name, (char*)ap_user->p_recv_data);
        // 접속한 모든 클라이언트에게 채팅 메시지를 전송
        SendBroadcastUserFrameData(ap_server, ANS_USER_CHAT, ap_server->p_buffer2, len + 1);
        // 서버의 이벤트 리스트에도 채팅 정보를 추가한다.
        AddEventString(p_app, ap_server->p_buffer2); // 서버의 이벤트 리스트에도 채팅 정보를 추가
    }
    return 1;
}

// 사용자 정보를 구성하는 함수 (user.dat 파일에서 사용자 읽어오는 기능 포함)
void LoadUserData(AD* ap_app)
{
    FILE* p_file = NULL;
    NRUD* p_user = ap_app->server_data.p_users; // 사용자 가능한 사용자 목록 정보
    UINT32 count = ap_app->server_data.user_count, index;  // 등록 가능한 사용자 수

    // 리스트 박스에 등록 가능한 사용자 공간을 추가한다.
    for (UINT32 i = 0; i < count; i++) ListBox_SetItemDataPtrEx(ap_app->p_user_list, i, "", p_user++, 0);
    ListBox_SetCurSel(ap_app->p_user_list, 0); // 가장 첫 항목을 선택

    // 파일에 저장된 사용자 정보를 읽어서 리스트 박스에 추가한다.
    if (0 == fopen_s(&p_file, "user.dat", "rb") && p_file != NULL) {
        // 실제 등록된 사용자 수를 읽는다.
        fread(&count, sizeof(UINT32), 1, p_file);
        for (UINT32 i = 0; i < count; i++) {
            // 정보가 위치했던 index를 얻는다.
            fread(&index, sizeof(int), 1, p_file);
            // index 위치의 사용자 정보를 얻는다.
            p_user = (NRUD*)ListBox_GetItemDataPtr(ap_app->p_user_list, index);
            // 사용자 정보를 파일에서 읽는다.
            fread(&p_user->info, sizeof(NUD), 1, p_file);
        }
        fclose(p_file);  // 파일을 닫는다!
        Invalidate(ap_app->p_user_list);  // 리스트 박스 화면 갱신
        if (count) ap_app->server_data.p_last_user = p_user;   // 가장 마지막 위치의 사용자 주소 저장
    }
}

// 편집된 사용자 정보를 'user.dat' 파일에 저장하는 함수
void SaveUserData(AD* ap_app)
{
    FILE* p_file = NULL;
    NRUD* p_users = ap_app->server_data.p_users; // 사용자 목록의 시작 위치
    NRUD* p_user = p_users, * p_last_user = ap_app->server_data.p_last_user; // 시작 위치와 마지막 위치

    // 등록된 사용자 정보를 'user.dat' 파일에 저장한다.
    if (0 == fopen_s(&p_file, "user.dat", "wb") && p_file != NULL) {
        UINT32 count = 0; // 리스트 박스에 추가된 항목 수를 얻는다.
        while (p_user <= p_last_user) { // 실제 등록된 사용자의 수를 계산한다.
            if (p_user->info.id_len) count++; // id에 문자열이 저장되어 있으면 등록된 사용자로 처리한다.
            p_user++;
        }

        int index;
        p_user = p_users; // 시작 위치 초기화
        fwrite(&count, sizeof(UINT32), 1, p_file); // 실제 등록된 사용자 수를 저장한다.
        while (p_user <= p_last_user) {
            if (p_user->info.id_len) { // id에 문자열이 저장되어 있으면 등록된 사용자로 처리한다.
                index = p_user - p_users; // 정보가 위치한 index를 계산한다.
                fwrite(&index, sizeof(int), 1, p_file);  // 계산된 index를 파일에 저장한다.
                fwrite(&p_user->info, sizeof(NUD), 1, p_file);  // 사용자 정보를 저장한다.
            }
            p_user++; // 다음 사용자로 이동
        }
        fclose(p_file); // 파일을 닫는다.
    }
}

// 서버 서비스를 시작할 때 호출하는 함수
void StartServerService(AD* ap_app)
{
    if (ap_app->server_data.h_listen_socket == INVALID_SOCKET) { // 리슨 서비스가 중단중이면 서비스를 시작
        void* p_ctrl = FindControl(IDC_IP_COMBO);  // IP 목록이 구성된 콤보 박스의 주소를 얻는다.
        int index = ComboBox_GetCurSel(p_ctrl); // 선택 위치를 얻는다.
        if (index != LB_ERR) {
            char str[128];
            int len = ComboBox_GetTextLength(p_ctrl, index); // 선택된 주소의 길이를 얻는다.
            ComboBox_GetText(p_ctrl, index, str, 32); // 선택한 IP를 얻는다.
            StartListenService(str, 12023, &ap_app->server_data); // 선택한 IP에서 12023 포트로 리슨 서비스 시작
            memcpy(str + len, " 에서 리슨 서비스를 시작합니다!", 32); // 출력 멘트를 구성한다.
            AddEventString(ap_app, str); // 멘트를 이벤트 목록에 추가한다.
        }
    }
    else AddEventString(ap_app, "리슨 서비스가 이미 진행중입니다...");
}

// 서버 서비스를 중지할 때 호출하는 함수
void StopServerService(AD* ap_app)
{
    if (ap_app->server_data.h_listen_socket != INVALID_SOCKET) { // 리슨 서비스가 진행중인지 체크
        DestroyServerSocket(&ap_app->server_data); // 생성된 소켓을 모두 제거하고 리슨 서비스를 중단한다.
        AddEventString(ap_app, "리슨 서비스를 중지합니다!"); // 중지 상태를 표시한다.
    }
}

void OnDestroy()  // 윈도우가 종료될 때 호출될 함수
{
    AD* p_app = (AD*)GetAppData();  // 프로그램에 저장된 데이터를 얻는다.

    SaveUserData(p_app); // 사용자 정보 저장
    StopServerService(p_app); // 서버 서비스 중지
    CleanUpServerData(&p_app->server_data);  // 소켓 서비스를 위해 사용하던 정보를 정리
}

// 막지막 위치에 있던 사용자를 삭제하여 다시 마지막 사용자를 찾을 때 사용하는 함수
void FindLastUser(AD* ap_app)
{
    if (ap_app->server_data.p_last_user != NULL) {
        NRUD* p_users = ap_app->server_data.p_users; // 사용자 목록의 시작 주소
        NRUD* p_user = ap_app->server_data.p_last_user; // 마지막이었던 사용자 정보 위치로 이동한다.
        ap_app->server_data.p_last_user = NULL; // 마지막 사용자 주소는 초기화한다.

        while (p_user >= p_users) {  // 뒤에서 앞쪽으로 이동하면서 사용자 등록 여부를 체크한다.
            if (p_user->info.id_len) {  // 등록된 사용자가 있는지 체크
                ap_app->server_data.p_last_user = p_user;  // 해당 사용자를 마지막 사용자로 등록
                break;
            }
            p_user--;  // 앞으로 이동한다.
        }
    }
}

// 에디트 컨트롤에서 문자열을 얻는 함수
int GetStringFromEdit(int a_ctrl_id, char* ap_str, int a_limit)
{
    void* p_edit = FindControl(a_ctrl_id);  // 에디트 컨트롤의 주소를 얻는다.
    GetCtrlName(p_edit, ap_str, a_limit); // 사용자 암호
    return Edit_GetLength(p_edit);  // 입력된 문자열의 길이를 반환한다.
}

// 사용자가 입력한 데이터로 지정한 계정을 업데이트 하는 함수
void ModifyUserData(AD* ap_app)
{
    char id[32];
    UINT32 len = GetStringFromEdit(IDC_USER_ID_EDIT, id, 32); // 사용자 아이디
    if (len) {
        int index = ListBox_GetCurSel(ap_app->p_user_list);
        NRUD* p_user = (NRUD*)ListBox_GetItemDataPtr(ap_app->p_user_list, index);
        if (CheckStringThatMakesUpID(id)) { // 아이디 문자 구성에 대한 유효성 체크
            if (len != p_user->info.id_len || memcmp(id, p_user->info.id, len)) { // 아이디 변경 여부 체크
                if (GetDupUserByID(&ap_app->server_data, p_user, id, len)) { // 이미 사용된 아이디인지 체크
                    AddEventString(ap_app, "동일한 아이디가 존재해서 사용자 정보를 변경할 수 없습니다.");
                    return;
                }
                p_user->info.id_len = len; // 아이디 길이 저장
                memcpy(p_user->info.id, id, len + 1); // 아이디 저장
            }

            p_user->info.password_len = GetStringFromEdit(IDC_USER_PASSWORD_EDIT, p_user->info.password, 32); // 암호
            p_user->info.name_len = GetStringFromEdit(IDC_USER_NAME_EDIT, p_user->info.name, 32); // 사용자 이름
            p_user->info.level = ComboBox_GetCurSel(FindControl(IDC_USER_LEVEL_COMBO)); // 사용자 등급
            p_user->info.etc_len = GetStringFromEdit(IDC_USER_ETC_EDIT, p_user->info.etc, 128); // 기타 정보

            ListBox_SetCurSel(ap_app->p_user_list, index); // 선택된 항목을 갱신하여 변경된 사항 출력
            if (ap_app->server_data.p_last_user < p_user) { // 변경된 정보가 가장 마지막 사용자인지 체크
                ap_app->server_data.p_last_user = p_user;  // 소켓 정보에도 마지막 사용자 정보를 갱신한다.
            }
        }
        else AddEventString(ap_app, "아이디 오류: 아이디는 영문자와 숫자로 구성해야 합니다.");
    }
    else AddEventString(ap_app, "아이디 오류: 아이디는 반드시 입력해야 합니다.");
}

// 선택한 사용자 정보를 삭제하는 함수
void DelUserData(AD* ap_app)
{
    int index = ListBox_GetCurSel(ap_app->p_user_list);
    NRUD* p_user = (NRUD*)ListBox_GetItemDataPtr(ap_app->p_user_list, index);
    if (p_user->info.id_len) { // 등록된 사용자가 있는지 체크
        char str[256];
        sprintf_s(str, 256, "%s (%s)", p_user->info.id, p_user->info.name); // 아이디와 이름을 사용해서 문자열 구성
        if (IDYES == MessageBox(gh_main_wnd, str, "선택한 사용자를 삭제하시겠습니까?", MB_ICONQUESTION | MB_YESNO)) {
            DisconnectUserByError(&ap_app->server_data, p_user, "사용자 삭제를 위해 접속 해제", 1);
            p_user->info.id_len = 0; // 아이디 정보 리셋
            *p_user->info.id = 0;
            p_user->ip_len = 0; // 아이피 정보 리셋
            *p_user->ip = 0;

            ListBox_SetCurSel(ap_app->p_user_list, index); // 선택된 항목을 갱신하여 변경된 사항 출력
            if (ap_app->server_data.p_last_user == p_user) { // 변경된 정보가 가장 마지막 사용자인지 체크
                FindLastUser(ap_app); // 막지막 위치에 있던 사용자를 삭제하여 다시 마지막 사용자를 찾는다.
            }
        }
    }
}

// 선택한 사용자 정보를 컨트롤에 표시하는 함수
void ShowUserData(AD* ap_app)
{
    int index = ListBox_GetCurSel(ap_app->p_user_list);
    NRUD* p_user = (NRUD*)ListBox_GetItemDataPtr(ap_app->p_user_list, index);
    // 사용자 정보를 각 컨트롤에 출력한다.
    SetCtrlName(FindControl(IDC_USER_ID_EDIT), p_user->info.id); // 사용자 아이디
    SetCtrlName(FindControl(IDC_USER_PASSWORD_EDIT), p_user->info.password); // 사용자 암호
    SetCtrlName(FindControl(IDC_USER_NAME_EDIT), p_user->info.name); // 사용자 이름
    ComboBox_SetCurSel(FindControl(IDC_USER_LEVEL_COMBO), p_user->info.level); // 사용자 등급
    SetCtrlName(FindControl(IDC_USER_ETC_EDIT), p_user->info.etc); // 기타 정보
}

// 컨트롤의 아이디(a_ctrl_id), 컨트롤의 조작 상태(a_notify_code), 선택한 컨트롤 객체(ap_ctrl)
void OnCommand(INT32 a_ctrl_id, INT32 a_notify_code, void* ap_ctrl)
{
    AD* p_app = (AD*)GetAppData(); // 프로그램의 내부 데이터 주소를 얻는다.

    if (a_ctrl_id == IDC_MODIFY_USER_BTN) ModifyUserData(p_app); // [사용자 정보 변경] 버튼을 누른 경우
    else if (a_ctrl_id == IDC_DEL_USER_BTN) DelUserData(p_app);  // [사용자 정보 삭제] 버튼을 누른 경우
    else if (a_ctrl_id == IDC_USER_LIST) { // 사용자 목록에서 사용자 선택을 변경했을 때
        if (a_notify_code == LBN_SELCHANGE) ShowUserData(p_app); // 선택 사용자 정보를 컨트롤에 표시        
    }
    else if (a_ctrl_id == IDC_SERVICE_START_BTN) StartServerService(p_app); // [서비스 시작] 버튼을 누름
    else if (a_ctrl_id == IDC_SERVICE_STOP_BTN) StopServerService(p_app); // [서비스 멈춤] 버튼을 누름
}

CMD_USER_MESSAGE(OnCommand, OnDestroy, OnUserMsg)

void ServerAddEventString(NeoServerData* ap_server, const char* ap_string)  // 이벤트 메시지를 추가하는 함수
{
    AD* p_app = (AD*)GetAppData();  // 프로그램에 저장된 데이터를 얻는다.
    AddEventString(p_app, ap_string);
}

// 사용자의 상태 통보를 사용하는 함수
void UserNotifyProc(NeoServerData* ap_server, NRUD* ap_user, UINT32 a_state)
{
    //if (a_state == 1); // 새로운 사용자가 로그인에 성공함
    //if (a_state == 50);  // 사용자가 접속을 해제함
}

// 현재 시스템에 있는 서비스 가능한 IP 주소를 콤보 박스에 추가하는 함수
void CheckServiceIpAddress(void* ap_combo_ctrl)
{
    char ip_list[10][16];
    // 시스템에서 사용 가능한 IP 주소를 얻는다.
    int count = GetLocalNetworkAddress(ip_list);
    for (int i = 0; i < count; i++) ComboBox_InsertString(ap_combo_ctrl, i, ip_list[i]);
    ComboBox_SetCurSel(ap_combo_ctrl, 0);
}

// 사용자 등급을 목록으로 구성하는 함수
void CheckUserLevel(void* ap_combo_ctrl)
{
    for (int i = 0; i < 9; i++) ComboBox_InsertString(ap_combo_ctrl, i, gp_level_string[i]);
    ComboBox_SetCurSel(ap_combo_ctrl, 0);
}

// 리스트 박스의 각 항목을 출력하는 함수. 이 함수는 ListBox에 추가된 항목 수만큼 호출된다.
void DrawUserInfoItem(int index, char* ap_str, int a_str_len, void* ap_data, int a_is_selected, RECT* ap_rect)
{
    // 리스트 박스의 각 항목이 선택되었을 때와 아닐 때의 항목 테두리 색상을 다르게 처리함!
    if (a_is_selected) SelectPenObject(RGB(212, 228, 228));  // 선택 됨
    else SelectPenObject(RGB(62, 77, 104));  // 선택 안됨

    SelectBrushObject(RGB(62, 77, 104));  // 각 항목의 배경을 색상을 지정한다.
    // 각 항목의 배경으로 사용할 사각형을 그린다.
    Rectangle(ap_rect->left, ap_rect->top, ap_rect->right, ap_rect->bottom);

    NRUD* p = (NRUD*)ap_data; // 현재 항목에 연결된 정보의 주소를 얻는다.
    HDC h_dc = GetCurrentDC();  // 현재 DC 핸들 값을 얻는다.
    SelectFontObject("맑은 고딕", 15, 0);  // 글꼴을 변경한다.

    if (p->info.id_len) {
        ::SetTextColor(h_dc, RGB(98, 190, 255));
        ::TextOut(h_dc, 5, ap_rect->top + 2, p->info.id, p->info.id_len);
        ::SetTextColor(h_dc, RGB(228, 228, 198));
        ::TextOut(h_dc, 105, ap_rect->top + 2, p->info.name, p->info.name_len);
        ::TextOut(h_dc, 185, ap_rect->top + 2, gp_level_string[p->info.level], g_level_len[p->info.level]);
        ::TextOut(h_dc, 265, ap_rect->top + 2, p->info.etc, p->info.etc_len);

        if (p->is_block) ::TextOut(h_dc, ap_rect->right - 120, ap_rect->top + 2, "보안 문제로 중지", 17);
        else {
            if (p->ip_len) ::TextOut(h_dc, ap_rect->right - 120, ap_rect->top + 2, p->ip, p->ip_len);
        }
    }
    else {
        ::SetTextColor(h_dc, RGB(138, 138, 138));
        ::TextOut(h_dc, 5, ap_rect->top + 2, "--- 비어있음 ---", 17);
    }
}

int main()
{
    ChangeWorkSize(660, 515);  // 창의 크기를 조정한다.    
    Clear(0, RGB(82, 97, 124));  // 전체 색상을 RGB(82, 97, 124)으로 채운다.

    AD app_data = { 0, };
    // 서버 소켓을 사용하기 위한 기본 정보를 사용해서 초기화를 한다.
    InitServerData(&app_data.server_data, UserMessageProc, ServerAddEventString, UserNotifyProc);
    // 사용자 목록을 표시할 리스트 박스 생성
    app_data.p_user_list = CreateListBox(5, 35, 650, 230, IDC_USER_LIST, DrawUserInfoItem, 0);
    ListBox_SetItemHeight(app_data.p_user_list, 19); // 항목의 높이를 19로 지정
    LoadUserData(&app_data); // 사용자 목록을 구성한다.

    app_data.p_event_list = CreateListBox(5, 350, 650, 160, IDC_EVENT_LIST, NULL, 0);
    SetAppData(&app_data, sizeof(AD)); // 프로그램 정보를 저장한다.    

    void* p_ctrl = CreateComboBox(5, 6, 120, 300, IDC_IP_COMBO); // 서비스 주소 선택 콤보 박스를 생성
    CheckServiceIpAddress(p_ctrl); // 현재 컴퓨터에서 사용 가능한 서비스 주소를 콤보 박스에 추가한다.

    CreateButton("서비스 시작", 130, 5, 80, 26, IDC_SERVICE_START_BTN);  // '서비스 시작' 버튼 생성
    CreateButton("서비스 멈춤", 213, 5, 80, 26, IDC_SERVICE_STOP_BTN);  // '서비스 멈춤' 버튼 생성
    CreateButton("사용자 정보 변경", 200, 270, 120, 26, IDC_MODIFY_USER_BTN);  // '사용자 정보 변경' 버튼 생성
    CreateButton("사용자 정보 삭제", 323, 270, 120, 26, IDC_DEL_USER_BTN);  // '사용자 정보 삭제' 버튼 생성

    CreateEdit(5, 323, 80, 20, IDC_USER_ID_EDIT, 0);  // 아이디 입력을 위한 에디트 컨트롤을 생성
    CreateEdit(88, 323, 80, 20, IDC_USER_PASSWORD_EDIT, ES_PASSWORD);  // 암호 입력을 위한 에디트 컨트롤을 생성
    CreateEdit(171, 323, 80, 20, IDC_USER_NAME_EDIT, 0);  // 이름 입력을 위한 에디트 컨트롤을 생성
    p_ctrl = CreateComboBox(255, 321, 80, 300, IDC_USER_LEVEL_COMBO); // 등급 선택 콤보 박스를 생성
    CheckUserLevel(p_ctrl); // 사용자 등급에 대한 목록을 구성한다.
    CreateEdit(340, 323, 314, 20, IDC_USER_ETC_EDIT, 0);  // 기타 정보 입력을 위한 에디트 컨트롤을 생성

    SelectFontObject("맑은 고딕", 18, 0);  // 글꼴을 변경한다.
    SetTextColor(RGB(128, 202, 128));  // 문자열의 색상을 지정한다.
    TextOut(9, 303, "아이디");
    TextOut(92, 303, "암호");
    TextOut(175, 303, "이름");
    TextOut(259, 303, "등급");
    TextOut(344, 303, "기타");

    ShowDisplay(); // 정보를 윈도우에 출력한다.
    return 0;
}