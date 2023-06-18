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

const char *gp_level_string[] = { "손님", "임시회원", "일반회원", "성실회원", "우수회원", "공동체", "운영진", "VIP", "관리자" };
int g_level_len[] = { 4, 8, 8, 8, 8, 6, 6, 3, 6 };

#define REQ_USER_CHAT                 21    // C -> S 채팅 데이터 전달
#define ANS_USER_CHAT                 22    // S -> C 채팅 데이터 전달 (브로드 캐스팅)

{
    gp_window_title = "로그인";
    g_wnd_style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPCHILDREN;
}

typedef struct ApplicationData  
{
    NeoServerData server_data;
    void *p_user_list; 
    void *p_event_list;
} AD;

void AddEventString(AD *ap_app, const char *ap_string)
{
    ListBox_InsertString(ap_app->p_event_list, -1, ap_string); 
}

int OnUserMsg(HWND ah_wnd, UINT a_msg_id, WPARAM wParam, LPARAM lParam)
{
    AD *p_app = (AD *)GetAppData();

    if (a_msg_id == SERVER_SOCKET_USER) { 
        ProcessUserSocketEvent(&p_app->server_data, (SOCKET)wParam, WSAGETSELECTEVENT(lParam));
        return 0;
    } else if (a_msg_id == SERVER_SOCKET_CLIENT) { 
        ProcessCommonSocketEvent(&p_app->server_data, (SOCKET)wParam, WSAGETSELECTEVENT(lParam));
        return 0;
    } else if (a_msg_id == SERVER_SOCKET_ACCEPT) {
        ProcessCommonAccept(&p_app->server_data);
        return 0;
    } else if (a_msg_id == SERVER_STATE_NOTIFY) { 
        Invalidate(p_app->p_user_list);
        return 0;
    }
    return 0;
}

int UserMessageProc(NeoServerData *ap_server, NRUD *ap_user)
{
    AD *p_app = (AD *)GetAppData();

    if (ap_user->msg_id == REQ_USER_CHAT) {
        int len = sprintf_s(ap_server->p_buffer2, TSB_SIZE, "%s: %s", ap_user->info.name, (char *)ap_user->p_recv_data);
        SendBroadcastUserFrameData(ap_server, ANS_USER_CHAT, ap_server->p_buffer2, len + 1);
        AddEventString(p_app, ap_server->p_buffer2);
    }
    return 1;
}

void LoadUserData(AD *ap_app)
{
    FILE *p_file = NULL;
    NRUD *p_user = ap_app->server_data.p_users; 
    UINT32 count = ap_app->server_data.user_count, index;  

    for (UINT32 i = 0; i < count; i++) ListBox_SetItemDataPtrEx(ap_app->p_user_list, i, "", p_user++, 0);
    ListBox_SetCurSel(ap_app->p_user_list, 0); 


    if (0 == fopen_s(&p_file, "user.dat", "rb") && p_file != NULL) {
        fread(&count, sizeof(UINT32), 1, p_file);
        for (UINT32 i = 0; i < count; i++) {
            fread(&index, sizeof(int), 1, p_file);
            p_user = (NRUD *)ListBox_GetItemDataPtr(ap_app->p_user_list, index);
            fread(&p_user->info, sizeof(NUD), 1, p_file);
        }
        fclose(p_file);  
        Invalidate(ap_app->p_user_list);
        if (count) ap_app->server_data.p_last_user = p_user; 
    }
}

void SaveUserData(AD *ap_app)
{
    FILE *p_file = NULL;
    NRUD *p_users = ap_app->server_data.p_users; 
    NRUD *p_user = p_users, *p_last_user = ap_app->server_data.p_last_user;

    if (0 == fopen_s(&p_file, "user.dat", "wb") && p_file != NULL) {
        UINT32 count = 0;
        while (p_user <= p_last_user) {
            if (p_user->info.id_len) count++; 
            p_user++;
        }

        int index;
        p_user = p_users; 
        fwrite(&count, sizeof(UINT32), 1, p_file);
        while (p_user <= p_last_user) {
            if (p_user->info.id_len) { 
                index = p_user - p_users;
                fwrite(&index, sizeof(int), 1, p_file);  
                fwrite(&p_user->info, sizeof(NUD), 1, p_file); 
            }
            p_user++; 
        }
        fclose(p_file); 
    }
}

void StartServerService(AD *ap_app)
{
    if (ap_app->server_data.h_listen_socket == INVALID_SOCKET) { 
        void *p_ctrl = FindControl(IDC_IP_COMBO); 
        int index = ComboBox_GetCurSel(p_ctrl); 
        if (index != LB_ERR) {
            char str[128];
            int len = ComboBox_GetTextLength(p_ctrl, index); 
            ComboBox_GetText(p_ctrl, index, str, 32); 
            StartListenService(str, 12023, &ap_app->server_data); 
            memcpy(str + len, " 에서 리슨 서비스를 시작합니다!", 32);
            AddEventString(ap_app, str);
        }
    } else AddEventString(ap_app, "리슨 서비스가 이미 진행중입니다...");
}

void StopServerService(AD *ap_app)
{
    if (ap_app->server_data.h_listen_socket != INVALID_SOCKET) { 
        DestroyServerSocket(&ap_app->server_data); //
        AddEventString(ap_app, "리슨 서비스를 중지합니다!");
    }
}

void OnDestroy() {
    AD *p_app = (AD *)GetAppData(); 

    SaveUserData(p_app);
    StopServerService(p_app); 
    CleanUpServerData(&p_app->server_data);
}

void FindLastUser(AD *ap_app)
{
    if (ap_app->server_data.p_last_user != NULL) {
        NRUD *p_users = ap_app->server_data.p_users;
        NRUD *p_user = ap_app->server_data.p_last_user; 
        ap_app->server_data.p_last_user = NULL; 

        while (p_user >= p_users) { 
            if (p_user->info.id_len) {  
                ap_app->server_data.p_last_user = p_user; 
                break;
            }
            p_user--;  
        }
    }
}

int GetStringFromEdit(int a_ctrl_id, char *ap_str, int a_limit)
{
    void *p_edit = FindControl(a_ctrl_id); 
    GetCtrlName(p_edit, ap_str, a_limit);
    return Edit_GetLength(p_edit); 
}

void ModifyUserData(AD *ap_app)
{
    char id[32];
    UINT32 len = GetStringFromEdit(IDC_USER_ID_EDIT, id, 32);
    if (len) {
        int index = ListBox_GetCurSel(ap_app->p_user_list);
        NRUD *p_user = (NRUD *)ListBox_GetItemDataPtr(ap_app->p_user_list, index);
        if (CheckStringThatMakesUpID(id)) { 
            if (len != p_user->info.id_len || memcmp(id, p_user->info.id, len)) { 
                if (GetDupUserByID(&ap_app->server_data, p_user, id, len)) {
                    AddEventString(ap_app, "동일한 아이디가 존재해서 사용자 정보를 변경할 수 없습니다.");
                    return;
                }
                p_user->info.id_len = len;
                memcpy(p_user->info.id, id, len + 1);
            }

            p_user->info.password_len = GetStringFromEdit(IDC_USER_PASSWORD_EDIT, p_user->info.password, 32); 
            p_user->info.name_len = GetStringFromEdit(IDC_USER_NAME_EDIT, p_user->info.name, 32); 
            p_user->info.level = ComboBox_GetCurSel(FindControl(IDC_USER_LEVEL_COMBO)); 
            p_user->info.etc_len = GetStringFromEdit(IDC_USER_ETC_EDIT, p_user->info.etc, 128);

            ListBox_SetCurSel(ap_app->p_user_list, index); 
            if (ap_app->server_data.p_last_user < p_user) { 
                ap_app->server_data.p_last_user = p_user; 
            }
        } else AddEventString(ap_app, "아이디 오류: 아이디는 영문자와 숫자로 구성해야 합니다.");
    } else AddEventString(ap_app, "아이디 오류: 아이디는 반드시 입력해야 합니다.");
}

void DelUserData(AD *ap_app)
{
    int index = ListBox_GetCurSel(ap_app->p_user_list);
    NRUD *p_user = (NRUD *)ListBox_GetItemDataPtr(ap_app->p_user_list, index);
    if (p_user->info.id_len) { 
        char str[256];
        sprintf_s(str, 256, "%s (%s)", p_user->info.id, p_user->info.name); 
        if (IDYES == MessageBox(gh_main_wnd, str, "선택한 사용자를 삭제하시겠습니까?", MB_ICONQUESTION | MB_YESNO)) {
            DisconnectUserByError(&ap_app->server_data, p_user, "사용자 삭제를 위해 접속 해제", 1);
            p_user->info.id_len = 0; 
            *p_user->info.id = 0;
            p_user->ip_len = 0; 
            *p_user->ip = 0;

            ListBox_SetCurSel(ap_app->p_user_list, index); 
            if (ap_app->server_data.p_last_user == p_user) {
                FindLastUser(ap_app);
            }
        }
    }
}

void ShowUserData(AD *ap_app)
{
    int index = ListBox_GetCurSel(ap_app->p_user_list);
    NRUD *p_user = (NRUD *)ListBox_GetItemDataPtr(ap_app->p_user_list, index);
    SetCtrlName(FindControl(IDC_USER_ID_EDIT), p_user->info.id); 
    SetCtrlName(FindControl(IDC_USER_PASSWORD_EDIT), p_user->info.password); 
    SetCtrlName(FindControl(IDC_USER_NAME_EDIT), p_user->info.name); 
    ComboBox_SetCurSel(FindControl(IDC_USER_LEVEL_COMBO), p_user->info.level); 
    SetCtrlName(FindControl(IDC_USER_ETC_EDIT), p_user->info.etc);
}

void OnCommand(INT32 a_ctrl_id, INT32 a_notify_code, void *ap_ctrl)
{
    AD *p_app = (AD *)GetAppData(); 

    if (a_ctrl_id == IDC_MODIFY_USER_BTN) ModifyUserData(p_app); 
    else if (a_ctrl_id == IDC_DEL_USER_BTN) DelUserData(p_app);  
    else if (a_ctrl_id == IDC_USER_LIST) { 
        if (a_notify_code == LBN_SELCHANGE) ShowUserData(p_app); 
    } else if (a_ctrl_id == IDC_SERVICE_START_BTN) StartServerService(p_app); 
    else if (a_ctrl_id == IDC_SERVICE_STOP_BTN) StopServerService(p_app); 
}

CMD_USER_MESSAGE(OnCommand, OnDestroy, OnUserMsg)

void ServerAddEventString(NeoServerData *ap_server, const char *ap_string)  
{
    AD *p_app = (AD *)GetAppData(); 
    AddEventString(p_app, ap_string);
}

void UserNotifyProc(NeoServerData *ap_server, NRUD *ap_user, UINT32 a_state)
{
    //if (a_state == 1); // 새로운 사용자가 로그인에 성공함
    //if (a_state == 50);  // 사용자가 접속을 해제함
}

void CheckServiceIpAddress(void *ap_combo_ctrl)
{
    char ip_list[10][16];
    int count = GetLocalNetworkAddress(ip_list);
    for (int i = 0; i < count; i++) ComboBox_InsertString(ap_combo_ctrl, i, ip_list[i]);
    ComboBox_SetCurSel(ap_combo_ctrl, 0);
}
void CheckUserLevel(void *ap_combo_ctrl)
{
    for (int i = 0; i < 9; i++) ComboBox_InsertString(ap_combo_ctrl, i, gp_level_string[i]);
    ComboBox_SetCurSel(ap_combo_ctrl, 0);
}

void DrawUserInfoItem(int index, char *ap_str, int a_str_len, void *ap_data, int a_is_selected, RECT *ap_rect)
{
    if (a_is_selected) SelectPenObject(RGB(212, 228, 228)); 
    else SelectPenObject(RGB(62, 77, 104)); 
    SelectBrushObject(RGB(62, 77, 104));  
    Rectangle(ap_rect->left, ap_rect->top, ap_rect->right, ap_rect->bottom);

    NRUD *p = (NRUD *)ap_data;
    HDC h_dc = GetCurrentDC();
    SelectFontObject("맑은 고딕", 15, 0); 

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
    } else {
        ::SetTextColor(h_dc, RGB(138, 138, 138));
        ::TextOut(h_dc, 5, ap_rect->top + 2, "--- 비어있음 ---", 17);
    }
}

int main()
{
    ChangeWorkSize(660, 515); 
    Clear(0, RGB(82, 97, 124));  

    AD app_data = { 0, };
    InitServerData(&app_data.server_data, UserMessageProc, ServerAddEventString, UserNotifyProc);
    app_data.p_user_list = CreateListBox(5, 35, 650, 230, IDC_USER_LIST, DrawUserInfoItem, 0);
    ListBox_SetItemHeight(app_data.p_user_list, 19); 
    LoadUserData(&app_data); 

    app_data.p_event_list = CreateListBox(5, 350, 650, 160, IDC_EVENT_LIST, NULL, 0);
    SetAppData(&app_data, sizeof(AD));

    void *p_ctrl = CreateComboBox(5, 6, 120, 300, IDC_IP_COMBO); 
    CheckServiceIpAddress(p_ctrl); 

    CreateButton("서비스 시작", 130, 5, 80, 26, IDC_SERVICE_START_BTN); 

    CreateButton("서비스 멈춤", 213, 5, 80, 26, IDC_SERVICE_STOP_BTN); 
    CreateButton("사용자 정보 변경", 200, 270, 120, 26, IDC_MODIFY_USER_BTN); 
    CreateButton("사용자 정보 삭제", 323, 270, 120, 26, IDC_DEL_USER_BTN); 

    CreateEdit(5, 323, 80, 20, IDC_USER_ID_EDIT, 0);
    CreateEdit(88, 323, 80, 20, IDC_USER_PASSWORD_EDIT, ES_PASSWORD);  
    CreateEdit(171, 323, 80, 20, IDC_USER_NAME_EDIT, 0); 
    p_ctrl = CreateComboBox(255, 321, 80, 300, IDC_USER_LEVEL_COMBO);
    CheckUserLevel(p_ctrl); 
    CreateEdit(340, 323, 314, 20, IDC_USER_ETC_EDIT, 0);  

    SelectFontObject("맑은 고딕", 18, 0); 
    SetTextColor(RGB(128, 202, 128)); 
    TextOut(9, 303, "아이디");
    TextOut(92, 303, "암호");
    TextOut(175, 303, "이름");
    TextOut(259, 303, "등급");
    TextOut(344, 303, "기타");

    ShowDisplay();
    return 0;
}