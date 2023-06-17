#ifndef _EASY_WIN32_H_
#define _EASY_WIN32_H_

// 이 라이브러리는 C 언어로 Win32 프로그래밍을 쉽게 할수 있도록 도와주기 위해 만들었습니다.
// 이 방법을 사용해서 C 언어를 배우면서 Win32 프로그래밍으로 실습을 하면 됩니다.
//
// 제작자 : 김성엽 (tipsware@naver.com, https://blog.naver.com/tipsware, https://cafe.naver.com/easywin32)
//
// 개발 시작 : 2019년 9월 3일 화요일
// 최근 업데이트 : 2023년 6월 12일 월요일
//
// 이 라이브러리의 저작권은 '(주)팁스웨어'에 있습니다.
// 이 라이브러리는 C 언어를 공부하는 사람들을 위해 만들어졌습니다.
// 따라서 출처만 정확하게 밝혀주면 모든 상황에서 무료로 사용이 가능합니다.
// 이 라이브러리는 후원 하시는 분들의 도움으로 계속 발전하고 있습니다.
// 후원에 참여하실 분들은 아래의 링크를 참고하세요.
// https://blog.naver.com/tipsware/221552898585
// 여러분의 작은 관심이 개발에 큰 힘이 됩니다. 감사합니다!!


typedef void (*FP_MOUSE_MSG)(int a_mixed_key, POINT a_pos);
typedef void (*FP_COMMAND)(INT32 a_ctrl_id, INT32 a_notify_code, void *ap_ctrl);
typedef void (*FP_DESTROY)();
typedef int (*FP_USER_MESSAGE)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
 
typedef struct UserImageInformation // 프로그램에서 사용할 내부 데이터
{
	HDC h_mem_dc; // 이미지를 출력할 때사용할 메모리 DC 
	HBITMAP h_mem_bmp; // 사용자가 사용할 비트맵
	int bmp_cx, bmp_cy; // 이미지의 폭과 높이
	UINT8 *p_raw_data; // 이미지 패턴의 주소를 기억할 포인터
} UII;

struct TargetData
{
	HWND h_wnd, h_base_wnd;
	int win_x, win_y, win_cx, win_cy;
	int x, y, cx, cy;
};

#define MAX_ANI_IMAGE_COUNT     32    // 애니메이션을 위한 최대 이미지 개수

typedef struct TW_AnimationImageData
{
	// 애니메이션을 구성하는 이미지의 개수와 현재 출력중인 이미지의 위치
	UINT16 image_count, index;
	// 애니메이션에 사용할 이미지의 목록
	void *p_image[MAX_ANI_IMAGE_COUNT];
} TW_AID;

#define IMAGE_BMP   0
#define IMAGE_JPEG  1
#define IMAGE_GIF   2
#define IMAGE_TIFF  3
#define IMAGE_PNG   4

#define PI     3.141592
#define H_PI   1.570796

#define GDI_COLOR(c) ((c >> 16) & 0x000000FF) | (c & 0x0000FF00) | ((c << 16) & 0x00FF0000)

class SystemMessageMap
{
protected:
	void *mp_app_data;
	FP_MOUSE_MSG mp_left_down, mp_left_up, mp_mouse_move;
	FP_COMMAND mp_command;
	FP_DESTROY mp_destroy;
	FP_USER_MESSAGE mp_user_message;

public:
	SystemMessageMap(FP_MOUSE_MSG ap_left_down, FP_MOUSE_MSG ap_left_up, FP_MOUSE_MSG ap_mouse_move, FP_COMMAND ap_command, FP_DESTROY ap_destroy, FP_USER_MESSAGE ap_user_message);
	virtual ~SystemMessageMap();

	void SetAppData(void *ap_data, int a_data_size);
	void *GetAppData();

	FP_MOUSE_MSG GetLeftBtnDown();
	FP_MOUSE_MSG GetLeftBtnUp();
	FP_MOUSE_MSG GetMouseMove();
	FP_COMMAND GetCommand();
	FP_DESTROY GetDestroy();
	FP_USER_MESSAGE GetUserMessage();
};

namespace EasyAPI_Tipsware
{
	void Rectangle(RECT *ap_rect);
	void Rectangle(int a_sx, int a_sy, int a_ex, int a_ey);
	void Rectangle(int a_sx, int a_sy, int a_ex, int a_ey, COLORREF a_color);
	void Rectangle(int a_sx, int a_sy, int a_ex, int a_ey, COLORREF a_line_color, COLORREF a_fill_color);

	void Ellipse(int a_sx, int a_sy, int a_ex, int a_ey);
	void Ellipse(int a_sx, int a_sy, int a_ex, int a_ey, COLORREF a_color);
	void Ellipse(int a_sx, int a_sy, int a_ex, int a_ey, COLORREF a_line_color, COLORREF a_fill_color);

	void TextOut(int a_x, int a_y, const char *ap_format, ...);
	void TextOut(int a_x, int a_y, COLORREF a_color, const char *ap_format, ...);
	void TextCenterOut(RECT *ap_rect, const char *ap_format, ...);
	void DrawText(RECT *ap_rect, int a_text_align, const char *ap_format, ...);

	void Line(int a_sx, int a_sy, int a_ex, int a_ey);
	void Line(int a_sx, int a_sy, int a_ex, int a_ey, COLORREF a_color, int a_tick = 1, int a_style = PS_SOLID);
	void MoveTo(int a_x, int a_y);
	void LineTo(int a_x, int a_y);
	void Pie(int a_left, int a_top, int a_right, int a_bottom, int a_xr1, int a_yr1, int a_xr2, int a_yr2);
	void PieRadian(int a_left, int a_top, int a_right, int a_bottom, double a_s_radian, double a_e_radian);
	void PieDegree(int a_left, int a_top, int a_right, int a_bottom, int a_s_degree, int a_e_degree);

	void GetIntPosFromCircle(int a_center_x, int a_center_y, int a_radius, double a_radian, int *ap_x, int *ap_y);
	void GetIntPosFromCircle(int a_center_x, int a_center_y, int a_radius, int a_degree, int *ap_x, int *ap_y);

	void GetPosFromCircle(double a_center_x, double a_center_y, double a_radius, double a_radian, double *ap_x, double *ap_y);
	void GetPosFromCircle(double a_center_x, double a_center_y, double a_radius, int a_degree, double *ap_x, double *ap_y);

	void MakeArrowHeadPos(POINT a_start_pos, POINT a_end_pos, POINT *ap_pos_list, int a_head_size = 15);
	void DrawArrow(POINT a_start_pos, POINT a_end_pos, int a_type, int a_head_size = 15);
	void DrawArrow(int a_sx, int a_sy, int a_ex, int a_ey, int a_type, int a_head_size = 15);

	HGDIOBJ SelectStockObject(int a_stock_index);
	HGDIOBJ SelectPenObject(COLORREF a_color, int a_tick = 1, int a_style = PS_SOLID);
	HGDIOBJ SelectBrushObject(COLORREF a_color);
	HGDIOBJ SelectFontObject(const char *ap_name, short a_size, unsigned char a_style = 0);

	void SetSimpleColorMode(char a_use_pen = 1, char a_use_brush = 1);
	void ChangePenColor(COLORREF a_color);
	void ChangeBrushColor(COLORREF a_color);
	void ChangeObjectColor(COLORREF a_color);

	void SetTextColor(COLORREF a_color);

	void UseDisplay(int a_index, int a_cx = 800, int a_cy = 600, HWND ah_wnd = NULL);
	void SetWindowStartPos(int a_x, int a_y);
	void ChangeWorkSize(int a_cx, int a_cy, int a_auto_change_display_size = 1);
	void ShowDisplay(int a_index = 0);

	void *FindControl(int a_id);
	void *CreateButton(const char *ap_name, int a_x, int a_y, int a_width, int a_height, int a_id, int a_style = 0, int a_ex_syle = 0);
	void *CreateEdit(int a_x, int a_y, int a_width, int a_height, int a_id, int a_style, int a_ex_syle = 0);
	void *CreateListBox(int a_x, int a_y, int a_width, int a_height, int a_id, void *afp_draw = NULL, int a_style = 0, int a_ex_syle = 0);
	void *CreateComboBox(int a_x, int a_y, int a_width, int a_height, int a_id, void *afp_draw = NULL, int a_style = 0, int a_ex_syle = 0);
	void DestroyControl(void *ap_ctrl);
	void ChangeCtrlColor(void *ap_ctrl, COLORREF a_fill_color, COLORREF a_out_border_color, COLORREF a_in_border_color, COLORREF a_text_color);
	void ChangeEditColor(void *ap_ctrl, COLORREF a_fill_color, COLORREF a_text_color);
	void SetCtrlName(void *ap_ctrl, const char *ap_name);
	void GetCtrlName(void *ap_ctrl, char *ap_name, int a_limit_size);
	void ShowControl(void *ap_ctrl, int a_show_type);
	HWND GetCtrlHandle(void *ap_ctrl);
	void SetIntDataToControl(int a_ctrl_id, int a_value);
	int GetIntDataFromControl(int a_ctrl_id);
	void Invalidate(void *ap_ctrl, int a_bk_flag = 1);
	void SetCtrlFont(void *ap_ctrl, const char *ap_font_name, int a_font_size, int a_attr = 0);
	void SetFocus(void *ap_ctrl);
	void CallDrawItem(int a_id, LPARAM lParam);
	void CallMeasureItem(int a_id, LPARAM lParam);
	INT32 CallCtrlColor(WPARAM wParam, LPARAM lParam);

	void ListBox_ChangeDrawFunc(void *ap_ctrl, void *afp_draw);
	int ListBox_AddString(void *ap_ctrl, const char *ap_string, UINT8 a_auto_select = 1);
	int ListBox_InsertString(void *ap_ctrl, INT32 a_index, const char *ap_string, UINT8 a_auto_select = 1);
	int ListBox_PrintFormat(void *ap_ctrl, const char *ap_format, ...);
	int ListBox_GetTextLength(void *ap_ctrl, INT32 a_index);
	int ListBox_GetText(void *ap_ctrl, INT32 a_index, char *ap_string, int a_max_len);
	void ListBox_SetCurSel(void *ap_ctrl, INT32 a_index);
	INT32 ListBox_GetCurSel(void *ap_ctrl);
	INT32 ListBox_GetCount(void *ap_ctrl);
	void ListBox_DeleteString(void *ap_ctrl, INT32 a_index);
	void ListBox_ResetContent(void *ap_ctrl);
	void ListBox_SetItemDataPtr(void *ap_ctrl, INT32 a_index, void *ap_data);
	void ListBox_SetItemDataPtrEx(void *ap_ctrl, INT32 a_index, const char *ap_string, void *ap_data, UINT8 a_auto_select = 1);
	void *ListBox_GetItemDataPtr(void *ap_ctrl, INT32 a_index);
	void ListBox_SetItemData(void *ap_ctrl, INT32 a_index, INT32 a_data);
	void ListBox_SetItemDataEx(void *ap_ctrl, INT32 a_index, const char *ap_string, INT32 a_data, UINT8 a_auto_select = 1);
	INT32 ListBox_GetItemData(void *ap_ctrl, INT32 a_index);
	INT32 ListBox_FindString(void *ap_ctrl, INT32 a_index, const char *ap_string);
	INT32 ListBox_FindStringExact(void *ap_ctrl, INT32 a_index, const char *ap_string);
	void ListBox_SetItemWidth(void *ap_ctrl, INT32 a_width);
	void ListBox_SetItemHeight(void *ap_ctrl, INT32 a_height);

	void ComboBox_ChangeDrawFunc(void *ap_ctrl, void *afp_draw);
	int ComboBox_AddString(void *ap_ctrl, const char *ap_string, UINT8 a_auto_select = 1);
	int ComboBox_InsertString(void *ap_ctrl, INT32 a_index, const char *ap_string, UINT8 a_auto_select = 1);
	int ComboBox_PrintFormat(void *ap_ctrl, const char *ap_format, ...);
	int ComboBox_GetTextLength(void *ap_ctrl, INT32 a_index);
	int ComboBox_GetText(void *ap_ctrl, INT32 a_index, char *ap_string, int a_max_len);
	void ComboBox_SetCurSel(void *ap_ctrl, INT32 a_index);
	INT32 ComboBox_GetCurSel(void *ap_ctrl);
	INT32 ComboBox_GetCount(void *ap_ctrl);
	void ComboBox_DeleteString(void *ap_ctrl, INT32 a_index);
	void ComboBox_ResetContent(void *ap_ctrl);
	void ComboBox_SetItemDataPtr(void *ap_ctrl, INT32 a_index, void *ap_data);
	void ComboBox_SetItemDataPtrEx(void *ap_ctrl, INT32 a_index, const char *ap_string, void *ap_data, UINT8 a_auto_select = 1);
	void *ComboBox_GetItemDataPtr(void *ap_ctrl, INT32 a_index);
	void ComboBox_SetItemData(void *ap_ctrl, INT32 a_index, INT32 a_data);
	void ComboBox_SetItemDataEx(void *ap_ctrl, INT32 a_index, const char *ap_string, INT32 a_data, UINT8 a_auto_select = 1);
	INT32 ComboBox_GetItemData(void *ap_ctrl, INT32 a_index);
	INT32 ComboBox_FindString(void *ap_ctrl, INT32 a_index, const char *ap_string);
	INT32 ComboBox_FindStringExact(void *ap_ctrl, INT32 a_index, const char *ap_string);
	void ComboBox_SetItemHeight(void *ap_ctrl, INT32 a_height);

	// 에디트에 입력된 길이를 얻는 함수
	UINT32 Edit_GetLength(void *ap_ctrl);  
	// 에디트의 배경에 문자열을 출력하는 함수
	void Edit_SetBgTipText(void *ap_ctrl, const char *ap_text, int a_text_len, COLORREF a_color);
	// 에디트 배경에 쓰인 문자열을 지우는 함수
	void Edit_ResetTipText(void *ap_ctrl);
	// 지정한 텍스트 파일에서 문자열을 읽어 에디트에 표시하는 함수
	INT32 Edit_ReadTextFromFile(void *ap_ctrl, const char *ap_file_path);
	// 에디트에 입력된 문자열을 지정한 텍스트에 저장하는 함수
	INT32 Edit_SaveTextToFile(void *ap_ctrl, const char *ap_file_path);

	void EnableEnterKey(void *ap_ctrl, WNDPROC afp_filter = NULL);
	void Clear(int a_index = 0, COLORREF a_color = RGB(255, 255, 255));

	void SetAppData(void *ap_data, int a_data_size);
	void *GetAppData();

	HDC GetCurrentDC();
	HBITMAP GetCurrentBitmap();
	void *GetMemDC(int a_index);
	void *GetCurrentMemDC();
	void BitBltByIndex(int a_dest_index, int a_dest_x, int a_dest_y, int a_dest_width, int a_dest_height, int a_src_index, int a_src_x = 0, int a_src_y = 0);
	void BitBltByIndex(int a_dest_x, int a_dest_y, int a_src_index, int a_src_x = 0, int a_src_y = 0);

	void SetTimer(UINT a_timer_id, UINT a_elapse, void *ap_timer_proc);
	void SetTimer(UINT ah_wnd, UINT a_timer_id, UINT a_elapse, void *ap_timer_proc);

	void KillTimer(UINT a_timer_id);
	void KillTimer(UINT ah_wnd, UINT a_timer_id);

	// ...GP 는 GDI+ 관련 함수들입니다!
	// ap_image_path 경로에 있는 이미지 파일을 읽는 함수!
	void *LoadImageGP(const char *ap_image_path, UINT8 a_is_alpha_fix = 0, UINT8 a_alpha_limit = 254);
	// ap_image에 저장된 이미지 정보를 ap_image_path경로에 a_image_type 형식의 이미지 파일로 저장합니다.
	void SaveImageGP(void *ap_image, const char *ap_image_path, int a_image_type = IMAGE_PNG);
	void *CreateBitmapGP(int a_width, int a_height, DWORD a_flag = 0, int a_bpp = 32);
	void DeleteImageGP(void *ap_image);

	void DrawImageGP(void *ap_image, int a_x, int a_y);
	void DrawImageGP(void *ap_image, int a_x, int a_y, int a_width, int a_height);
	void DrawImageGP(void *ap_image, int a_x, int a_y, double a_cx_rate, double a_cy_rate);
	void TransparentDrawGP(void *ap_image, int a_x, int a_y, COLORREF a_remove_color);
	void TransparentDrawGP(void *ap_image, int a_x, int a_y, int a_width, int a_height, COLORREF a_remove_color);
	void TransparentDrawGP(void *ap_image, int a_x, int a_y, double a_cx_rate, double a_cy_rate, COLORREF a_remove_color);

	// 지정한 경로에 있는 같은 계열의 이미지 파일을 읽는 함수
	// ".\\image\\run_00.png", ".\\image\\run_01.png", ".\\image\\run_02.png" 파일을
	// 읽고 싶다면 LoadImageAID(L".\\image\\run_", 3); 이라고 호출한다.
	// 파일 확장자는 .png만 가능하고 패턴 번호는 00, 01, ... 순서로 나열해야 한다.
	void LoadImageAID(TW_AID *ap_data, const char *ap_path, UINT16 a_count);
	// 할당된 이미지를 모두 제거하는 함수 (호출하지 않아도 프로그램 종료시에 자동으로 제거됩니다.)
	void DeleteImageAID(TW_AID *ap_data);
	// 현재 위치의 이미지를 ah_dc에 출력(비율로 축소 또는 확대)하고 다음 이미지 위치로 이동하는 함수
	UINT16 DrawImageAID(TW_AID *ap_data, int a_x, int a_y, double a_cx_rate, double a_cy_rate);
	// 지정한 위치의 이미지를 ah_dc에 출력(비율로 축소 또는 확대)하는 함수
	void DrawImageByIndexAID(TW_AID *ap_data, int a_x, int a_y, int a_index, double a_cx_rate, double a_cy_rate);

	HDC GetDCGP(void *ap_image);
	HBITMAP GetBitmapGP(void *ap_image);
	int GetWidthGP(void *ap_image);
	int GetHeightGP(void *ap_image);
	int GetBppGP(void *ap_image);
	int GetPitchGP(void *ap_image);
	void *GetBitsGP(void *ap_image);

	void *CaptureScreenGP(int a_sx, int a_sy, int a_ex, int a_ey);
	void *CaptureScreenGP();

	void StartSocketSystem();
	void StopSocketSystem();

	// [[[ 서버 소켓 관련 ]]]
	void *CreateServerSocket(unsigned int a_user_data_size, void *ap_accept_user, void *ap_proc_net_msg, void *ap_close_user,
								int a_index = 0, int a_max_user_count = 100, unsigned char a_key = 0x51);
	void DeleteServerSocket(void *ap_object);
	void DeleteServerSockets();
	int GetServerSocketIndex(void *ap_object);

	void *GetUsersData(void *ap_object);
	void *GetUserDataByHandle(void *ap_object, unsigned int ah_socket);
	void *GetUserDataByIndex(void *ap_object, int a_index);

	void StartListenService(void *ap_object, const char *ap_ip_address, int a_port);
	void SendFrameDataToClient(void *ap_object, unsigned int ah_socket, char a_msg_id, const void *ap_data, int a_size);
	void BroadcastFrameData(void *ap_object, char a_msg_id, const void *ap_data, int a_size);
	void *GetProcessServerData(void *ap_object);
	void CloseSocketInServer(void *ap_object, unsigned int ah_socket, char *ap_close_user);
	void DestroySocketsInServer(void *ap_object);

	// [[[ 클라이언트 소켓 관련 ]]]
	void *CreateClientSocket(void *ap_connect, void *ap_net_msg, void *ap_close, int a_index = 0, unsigned char a_key = 0x51);
	int GetClientSocketIndex(void *ap_object);
	void DeleteClientSocket(void *ap_object);
	void DeleteClientSockets();
	void *GetProcessClientData(void *ap_object);
	int IsConnect(void *ap_object);
	void ConnectToServer(void *ap_object, const char *ap_ip_address, int a_port);
	void CloseSocket(void *ap_object);
	void SendFrameDataToServer(void *ap_object, char a_msg_id, const void *ap_data, int a_size);

	// [[ 문자열 처리 관련 일반 유틸리티 ]]
	char *GetNextString(char *ap_src_str, char a_delimiter, char *ap_buffer);
	char *GetNextStringEx(char *ap_src_str, char a_delimiter, char *ap_buffer, char a_remove_prefix = ' ');
	char* ConvertTabToSpace(const char* ap_src_string);
	// UTF8 기반의 문자열을 ASCII 형식의 문자열로 변환하는 함수.
	// 이 함수는 내부적으로 메모리를 할당해서 ap_dest_ascii_str 포인터에 주소를 대입해 줌
	// 따라서 이 함수를 사용하는 개발자가 free 함수를 사용하여 ap_dest_ascii_str 주소를 해제해야함!
	// 이 함수의 반환 값은 변환된 문자열의 길이입니다. (NULL 문자 포함 길이)
	int Utf8ToAscii(char **ap_dest_ascii_str, char *ap_src_utf8_str);

	// [[ 연산 수식 분석기 관련 ]]
	void *CreateExpParsor();
	const char *GetExpString(void *ap_parsor);
	void InitExpParser(void *ap_parsor);
	void SetExpString(void *ap_parsor, const char *ap_string);
	void RunExpParsing(void *ap_parsor);   // 사용자가 지정한 수식을 분석한다

	int GetExpVariableCount(void *ap_parsor);
	const char *GetExpVariableName(void *ap_parsor, int a_index);
	// 지정된 변수에 값을 대입한다.
	void SetExpVarValue(void *ap_parsor, int a_index, double a_value);
	// 지정된 변수가 참조할 값이 있는 위치를 주소로 알려준다
	void SetExpVariablePointer(void *ap_parsor, int a_index, double *ap_data);
	// 변수값을 주소로 참조하도록 되어 있는 변수가 있다면 해당 주소에서 값을 읽어 변수값을 업데이트 한다.
	void UpdateExpVariableValue(void *ap_parsor);
	// 이미 수식이 parsing되었다면 다시 parsing할 필요가 없고 이함수를 호출한후, ExecExp 함수를 호출하면 된다
	char RestoreExpParserInfo(void *ap_parsor);
	char GetExpReadyFlag(void *ap_parsor);   // Parsing 이 되어있는지를 알려준다 ( 0 -> 파싱안됨, 1 -> 파싱됨 )
	double ExecExp(void *ap_parsor, unsigned int a_prev_depth = 0);  // Parsing된 수식을 실행하여 수식의 결과값을 얻는다
	void DestroyExpParsor(void *ap_parsor);

	void *CreateExpParsorEx();
	const char *GetExpStringEx(void *ap_parsor);
	void InitExpParserEx(void *ap_parsor);
	void SetExpStringEx(void *ap_parsor, const char *ap_string);
	void RunExpParsingEx(void *ap_parsor);   // 사용자가 지정한 수식을 분석한다

	int GetExpVariableCountEx(void *ap_parsor);
	const char *GetExpVariableNameEx(void *ap_parsor, int a_index);
	// 지정된 변수가 참조할 값이 있는 위치를 주소로 알려준다
	void SetExpVariablePointerEx(void *ap_parsor, int a_index, unsigned char a_data_type, double *ap_data);
	// 변수값을 주소로 참조하도록 되어 있는 변수가 있다면 해당 주소에서 값을 읽어 변수값을 업데이트 한다.
	void UpdateExpVariableValueEx(void *ap_parsor);
	// 이미 수식이 parsing되었다면 다시 parsing할 필요가 없고 이함수를 호출한후, ExecExp 함수를 호출하면 된다
	char RestoreExpParserInfoEx(void *ap_parsor);
	char GetExpReadyFlagEx(void *ap_parsor);   // Parsing 이 되어있는지를 알려준다 ( 0 -> 파싱안됨, 1 -> 파싱됨 )
	ExpDataType ExecExpEx(void *ap_parsor, unsigned int a_prev_depth = 0);  // Parsing된 수식을 실행하여 수식의 결과값을 얻는다
	void DestroyExpParsorEx(void *ap_parsor);

	// [ 매크로 관련 함수들 ]
	// 시스템 키를 입력하는 함수. 
	void SystemKeyWrite(char a_key, int a_delay = 250);  // (Key Down & Up을 한 번에 처리)
	void SystemKeyDown(char a_key);
	void SystemKeyUp(char a_key);
	// 일반 키를 입력하는 함수. Shift 키를 사용하는 경우에는 a_is_shift에 1을 입력해야 한다. 
	void InputNormalChar(char a_is_shift, char a_key);  // (Key Down & Up을 한 번에 처리)
	void InputNormalCharDown(char a_is_shift, char a_key);
	void InputNormalCharUp(char a_is_shift, char a_key);
	// ap_string에 전달된 키보드로 입력한다.
	char InputNormalString(const char *ap_string, char a_is_han_mode, int a_delay = 100);
	void WaitInputSystem(int a_delay);

	void MouseMoveWrite(int a_x, int a_y);  // 마우스 이동!
	void MouseClickWrite(int a_x, int a_y, int a_is_left = 1, int a_delay = 0);
	void MouseClickDown(int a_x, int a_y, int a_is_left = 1);
	void MouseClickUp(int a_x, int a_y, int a_is_left = 1);

	void GetMousePosFromTarget(TargetData *ap_target, int *ap_x, int *ap_y, int a_target_x, int a_target_y);

	void *MakeTargetBaseData(int a_x, int a_y, int a_cx, int a_cy);  // 내부 캡쳐 이미지 엔진을 생성하는 함수
	void DeleteTargetBaseData();

	void *GetTargetBaseData();
	UINT32 GetTargetBaseImageDataSize();
	UINT8 *GetTargetBaseImageData();

	void UpdateTargetImage();
	void UpdateTargetPos(TargetData *ap_target);
	// a_is_area_type (0: Client Area, 1: Window Area, 2: Screen Area) - 32Bit Color Only
	unsigned int GetTargetColor(TargetData *ap_target, int a_x, int a_y, int a_is_area_type = 0);
	TargetData *FindTargetImage(int a_index, const char *ap_class_name, const char *ap_title_name);
	TargetData *FindTargetImage(int a_index, int a_x, int a_y);
	void ShowTargetImage(TargetData *ap_target, int a_x, int a_y, int a_is_area_type);
	void ShowTargetImageEx(TargetData *ap_target, int a_x, int a_y, int a_cx, int a_cy, int a_is_area_type, int a_src_x, int a_src_y);
	void GetTargetImageData(unsigned int *ap_image_data, TargetData *ap_target, int a_is_area_type);
	void GetTargetImageDataEx(unsigned int *ap_image_data, TargetData *ap_target, int a_is_area_type, int a_x, int a_y, int a_cx, int a_cy);
	// 지정한 좌표(a_x, a_y)에 있는 색상(a_org_color)을 기준으로 주위 색상을 체크하여 대상의 영역를 계산한다.
	// 체크할 이미지는 ap_image_data이고 찾은 영역의 좌표는 p_target_rect에 사각형 좌표로 저장된다.
	// ap_image_data에 저장된 이미지 패턴의 폭은 a_image_width이고 높이는 a_image_height이다.
	void CheckOBjectPos(RECT *p_target_rect, int a_x, int a_y, unsigned int a_org_color, unsigned int a_change_color, unsigned int *ap_image_data, int a_image_width, int a_image_height);

	// 이미지를 카톡창으로 전송하는 함수
	void SendImageToTalk(int a_cx, int a_cy, UINT8 *ap_image, int a_image_size, HWND ah_send_wnd);
	// 이미지 전송을 확인하는 창을 체크해서 전송 버튼을 눌러주는 함수
	HWND CheckSendImageDlg();

	// 클립보드 관련 함수
	int CopyTextToClipboard(const char *ap_string);
	int CopyTextFromClipboard(char **ap_string, char a_is_clear);
	HBITMAP MakeDupBitmap(HBITMAP ah_bitmap);
	HBITMAP MakeDupBitmap(HBITMAP ah_bitmap, void *ap_image_data);
	HBITMAP MakeDupBitmap(int a_cx, int a_cy, int a_bits, void *ap_image_data, int a_data_size);

	HBITMAP CopyBitmapFromClipBoard();
	void CopyBitmapToClipBoard(HBITMAP ah_bitmap);
	void CopyBitmapToClipBoard(int a_cx, int a_cy, int a_bits, void *ap_image_data, int a_data_size);

	// Common Dialog 관련 함수
	int ChooseOpenFileName(char *ap_open_path, int a_path_size, const char *ap_filter = "All\0*.*\0Text\0*.txt\0", int a_filter_index = 1, const char *ap_init_path = NULL, int a_add_attr = 0);
	int ChooseSaveFileName(char *ap_open_path, int a_path_size, const char *ap_filter = "All\0*.*\0Text\0*.txt\0", int a_filter_index = 1, const char *ap_init_path = NULL, int a_add_attr = 0);
	int ChooseColorDlg(COLORREF a_init_color, COLORREF *ap_selected_color);
	int ChooseFontDlg(char *ap_font_name, int *ap_font_size, int *ap_font_style, COLORREF a_init_color = RGB(0, 0, 0), COLORREF *ap_selected_color = NULL);

	// 시리얼 통신 관련 함수
	void *CreateSerialObject();
	int Serial_OpenPort(void *ap_object, int a_port_number, HWND ah_notify_wnd, void(*ap_modify_dcb)(DCB *ap_dcb) = NULL, int a_notify_msg_id = 21000);
	int Serial_IsOpenPort(void *ap_object);  // 포트가 오픈되어 있지 않으면 0, 오픈되어있다면 포트 번호가 반환됨. COM1이면 1이 반환!
	int Serial_GetRecvDataSize(void *ap_object);
	char *Serial_GetRecvDataBuffer(void *ap_object);
	void Serial_RemoveDataFromBuffer(void *ap_object, int a_remove_size);
	void Serial_ResumeWatchThread(void *ap_object);
	void Serial_SetRecvTimeoutInfo(void *ap_object, int a_count = 5, int a_interval = 300);
	int Serial_WriteCommData(void *ap_object, const char *ap_data, int a_length, int(*ap_error_process)(DWORD, COMSTAT) = NULL);
	void Serial_ClearEvent(void *ap_object, int a_tx = 1, int a_rx = 1);
	void DestroySerialObject(void *ap_object);

	// 시스템에 등록된 장치 목록을 얻는 함수
	int GetRegisteredPrinterList(char a_printer_list[][512], int *ap_def_index);
	// 프린터 장치와 드라이버의 이름을 얻는 함수
	int GetNameFromPrinterInfo(char *ap_driver_name, char *ap_device_name, char *ap_printer_info);
	// 프린터에 정보를 출력하기 위한 DC를 얻는 함수
	// 출력 모드를 변경한다. 0.1 mm단위, 증가 방향(x → , y ↑), A4 용지의 크기: 210mm x 297mm (2100 x -2970)
	HDC CreatePrintDC(char *ap_driver_name, char *ap_device_name);
	// 사용할 폰트를 직접 생성하는 함수
	HFONT CreateDirectFont(const char *ap_name, int a_size, int a_bold, int a_italic, int a_underline, int a_strikeout);
	// 엑셀 파일에서 시트를 분리해주는 함수
	int GetExcelSheetFromZip(const char *ap_excel_file_name);

	UINT32 GetFIleDataToMemory(UINT8 **ap_data, const char *ap_file_path);
	UINT8 IsNormalUnicodeChar(const wchar_t a_char);
	// 유니코드 문자열을 일반 아스키 문자열로 변경하는 함수
	int UnicodeToAscii(char **ap_dest_str, const wchar_t *ap_src_str);
	int UnicodeToSimpleAscii(char *ap_dest_str, const wchar_t *ap_src_str);

	// 지정한 위치를 캡쳐해서 모자이크 이미지를 구성하는 함수
	void MakeMosaicImage(POINT *ap_start_pos, UII *ap_image_info);
	// 사용자가 직접 관리하는 이미지 정보를 만드는 함수
	void CreateUserImage(int a_cx, int a_cy, UII *ap_image_info);
	// 사용자가 직접 관리하는 이미지 정보를 제거하는 함수
	void DestroyUserImage(UII *ap_image_info);
	// 사용자가 직접 관리하는 이미지 정보를 화면에 출력하는 함수
	void DrawUserImage(int a_x, int a_y, UII *ap_image_info);

	// 지정한 DC에 연결된 글꼴에서 사용 가능한 문자의 목록을 얻는 함수
	// 이 함수는 ap_char_list에 동적으로 메모리를 할당해서 전달하기 때문에 
	// 사용 후에 ap_char_list가 가리키는 메모리를 반드시 해제해야 한다
	int GetUseUnicodeCharListInFont(wchar_t **ap_char_list, HDC ah_base_dc);

	// 리다이렉션 관련 함수
	void *CreateRedirectionData();  // 리다이렉션을 사용하기 위해 필요한 데이터를 만드는 함수
	void DestroyRedirectionData(void *ap_data); // 리다이렉션을 사용하기 위해 만든 데이터를 제거하는 함수
	void StopRedirection(void *ap_data); // 진행 중인 리다이렉션 작업을 중지 시키는 함수
	int IsExistPath(const char *ap_path);  // 지정한 경로에 있는 파일 또는 폴더가 있는지 체크하는 함수
	int GetRedirectionData(void *ap_data, char *ap_str, int a_limit);  // 진행중인 리다이렉션 정보의 상태를 얻는 함수
	int CheckRedirectionState(void *ap_data);  // 리다이렉션 작업이 중단되었는지 체크하는 함수

	// 리다이렉션과 관련된 타이머 정보를 다시 설정하는 함수
	void ModifyRedirectionTimerInfo(void *ap_data, UINT32 a_interval = 300, UINT32 a_timer_id = 77);
	// 지정된 경로에 있는 프로그램을 사용해서 리다이렉션을 시작하는 함수
	int ExecRedirection(void *ap_data, const char *ap_exe_path, const char *ap_arg_data, 
			const char *ap_working_path, void *afp_timer_proc);
}

#define MAX_TABLE_COUNT          65536  // 내부 구성 로직 수
#define COMMON_RECV_SIZE          8192  // 기본 수신 버퍼 (8K)
#define COMMON_SEND_SIZE          8192  // 기본 송신 버퍼 (8K)
#define USER_RECV_SIZE          204800  // 기본 수신 버퍼 (200K)
#define USER_SEND_SIZE          204800  // 기본 송신 버퍼 (200K)
#define TSB_SIZE                  4096  // 이 라이브러리 내에서 임시로 사용할 버퍼의 크기

#define SERVER_SOCKET_ACCEPT     21901  // 서버에 클라이언트가 접속할 때 발생하는 메시지
#define SERVER_SOCKET_CLIENT     21902  // 서버에 접속한 클라이언트와 통신 또는 해제할 때 발생하는 메시지
#define SERVER_SOCKET_USER       21903  // 서버에 로그인한 사용자와 통신 또는 해제할 때 발생하는 메시지

#define CLIENT_SOCKET_CONNECT    21911  // 클라이언트가 서버에 접속을 시도한 결과를 알려주는 메시지
#define CLIENT_SOCKET_COMMON     21912  // 서버와 통신 또는 접속이 해제될 때 발생하는 메시지

#define SERVER_STATE_NOTIFY      21950  // 서버 소켓의 상태를 전송하는 메시지
#define CLIENT_STATE_NOTIFY      21951  // 클라이언트 소켓의 상태를 전송하는 메시지

// [네트워크 접두어 규칙]  Client -> Server (Request) : REQ_,  Server -> Client (Answer) : ANS_
#define ANS_WELCOME_MSG             0    // S -> C 접속 허락 메시지
#define ANS_ALIVE_MSG               1    // S -> C 접속 유지 메시지
#define REQ_MAKE_ID                 6    // C -> S 계정 등록을 요청하는 메시지
#define ANS_MAKED_ID                7    // S -> C 계정 등록이 성공됨
#define REQ_LOGIN_DATA             10    // C -> S 로그인 시도
#define ANS_LOGIN_OK               11    // S -> C 로그인 성공
#define ANS_LOGIN_FAIL             12    // S -> C 로그인 실패
#define ANS_SYSTEM_MESSAGE         13    // S -> C 서버에서 전달된 시스템 상태 메시지   

#define LEN_TO_SIZE(x)                (x+1) // 문자열 길이를 문자열을 저장하는데 필요한 크기로 변경
#define SIZE_TO_LEN(x)                (x-1) // 문자열을 저장하는데 필요한 크기를 문자열 길이로 변경
#define PushStrToStream(s, str, size) *s++ = size; memcpy(s, str, size); s += size

struct NeoClientData  // 1개의 클라이언트 정보를 저장할 구조체
{
	UINT8 msg_id;                // 현재 처리중인 네트워크 메시지 ID
	UINT32 data_size, cur_size;  // 전체 수신할 크기와 현재 수신된 크기
	UINT8 *p_recv_data;          // 수신된 데이터의 실제 정보
	SOCKET h_socket;             // 개별 클라이언트와 통신할 소켓 핸들
	char ip_address[16];         // 접속한 클라이언트의 주소
	UINT32 connect_time;         // 30초 이내에 로그인을 하는지 체크
};

struct LoginData
{
	char id[32];                 // 아이디
	char password[32];           // 암호
};

typedef struct NeoUserData
{
	UINT8 level;  // (예시) 0:손님, 1:일반, 2:우수, 3:운영진, 4:관리자
	UINT8 id_len, password_len, name_len, etc_len;  // 문자열 길에는 NULL 문자 제외
	char id[32];                // 사용자 아이디
	char password[32];          // 암호
	char name[32];              // 이름
	char etc[128];              // 기타 정보
	UINT32 last_connected_time; // 마지막으로 접속한 시간 정보
	void *p_ext;                // 추가 정보가 필요한 경우 사용
} NUD;

typedef struct NeoRuntimeUserData
{
	NUD info;  // 사용자 기본 정보

	UINT8 is_try_pw_count;       // 로그인 시도 횟수
	UINT8 is_block;              // 로그인 시도를 10번 실패하면 계정 사용 중지!
	UINT8 ip_len;                // ip 주소의 길이
	UINT8 key;                   // 통신에 사용할 키
	UINT8 msg_id;                // 현재 처리중인 네트워크 메시지 ID
	UINT32 data_size, cur_size;  // 전체 수신할 크기와 현재 수신된 크기
	UINT32 last_use_time;        // 가장 마지막에 소켓을 사용한 시간 (송신, 수신 포함)
	UINT8 *p_recv_data;          // 수신된 데이터의 실제 정보
	char ip[16];                 // 접속한 사용자의 IP 주소
	SOCKET h_socket;             // 접속한 클라이언트의 소켓 핸들
} NRUD;

struct NeoServerData;
typedef void (*FP_EVENT_STR)(NeoServerData *ap_server, const char *ap_string);  // 이벤트를 추가하기 위한 함수
typedef int (*FP_USER_MSG)(NeoServerData *ap_server, NRUD *ap_user); // 사용자와 통신하기 위해 정의한 함수
typedef void (*FP_USER_NOTIFY)(NeoServerData *ap_server, NRUD *ap_user, UINT32 a_state); // 사용자 상태 통보용 함수

struct NeoServerData
{
	SOCKET h_listen_socket;            // 클라이언트의 접속을 처리하기 위한 소켓 핸들
	UINT8 *p_send_data, *p_recv_data;  // 클라이언트와 데이터를 전송하거나 수신할 때 사용할 메모리
	char *p_buffer1, *p_buffer2;       // TSB_SIZE 크기로 할당
	UINT8 base_key;                    // 프로토콜에 사용할 기본 키
	UINT16 client_count;               // 접속 시도에 사용할 기본 클라이언트 수
	UINT16 user_count;                 // 관리할 실제 사용자 수
	NeoClientData *p_clients;          // 접속한 클라이언트 정보
	NRUD *p_users;                     // 등록된 사용자 정보
	NRUD *p_last_user;                 // 사용자 정보에서 가장 마지막에 위치한 사용자 위치가 저장된 메모리의 주소

	void *p_table[MAX_TABLE_COUNT];    // 이 라이브러리가 사용하는 내부 테이블
	FP_USER_MSG p_user_msg_func;       // 사용자가 전송한 메시지를 처리할 함수의 주소
	FP_EVENT_STR p_event_func;         // 이벤트 로그를 처리할 함수의 주소
	FP_USER_NOTIFY p_user_notify_func; // 사용자 상태를 통보할때 사용할 함수의 주소
};

namespace TW_NeoServerSocket
{
	// 현재 서버가 실행된 컴퓨터에서 사용 가능한 IP 목록을 얻는 함수
	int GetLocalNetworkAddress(char ap_ip_list[][16]);
	// 서버 서비스의 정보를 초기화한다.
	void InitServerData(NeoServerData *ap_server, FP_USER_MSG ap_user_msg_func, FP_EVENT_STR ap_event_func, FP_USER_NOTIFY ap_user_notify_func, int a_max_client_count = 50, int a_max_user_count = 100, int a_key = 0x29);
	// 리슨 서비스를 시작하는 함수
	void StartListenService(const char *ap_ip_address, int a_port, NeoServerData *ap_server);
	// 서버 서비스를 위해 생성했던 정보를 제거하는 함수
	void CleanUpServerData(NeoServerData *ap_server);
	// 사용중인 서버 소켓과 관련된 정보를 모두 제거하는 함수
	void DestroyServerSocket(NeoServerData *ap_server);
	// 대기 없이 소켓을 닫는 함수 (closesocket으로 대체 가능)
	void CloseSocketNoWait(SOCKET ah_socket);

	// 클라이언트 정보를 저장할 빈 슬롯을 찾는 함수
	NeoClientData *CheckEmptyCommonSlot(NeoServerData *ap_server);
	// 클라이언트로 데이터를 전송하는 함수, (6바이트 프레임 구성 : "[ Key(1), Message ID(1), Body size(4)] +  Body Data")
	void SendCommonFrameData(NeoServerData *ap_server, NeoClientData *ap_client, UINT8 a_msg_id, const void *ap_data, UINT32 a_size);
	// 클라이언트 접속 시도를 처리하는 함수
	void ProcessCommonAccept(NeoServerData *ap_server);
	// 특정 클라이언트가 프로토콜 오류로 해제할 때 사용하는 함수
	void DisconnectCommonClientByError(NeoServerData *ap_server, NeoClientData *ap_client, const char *ap_error_msg);
	// 수신된 데이터가 프로토콜 형식에 맞는지 그리고 완전히 수신되었는지 진행하는 함수
	int ProcessingCommonMessage(NeoServerData *ap_server, NeoClientData *ap_client, UINT8 *ap_recv_data, int a_recv_size);
	// 소켓에 발생하는 이벤트를 처리하는 함수
	void ProcessCommonSocketEvent(NeoServerData *ap_server, SOCKET ah_socket, int a_event_type);
	// 아이디를 구성한 문자열의 유효성을 검사하는 함수
	int CheckStringThatMakesUpID(const char *ap_id_str);
	// 중복된 ID를 가진 사용자를 찾는 함수
	NRUD *GetDupUserByID(NeoServerData *ap_server, NRUD *ap_cur_user, char *ap_id, int a_len);
	// 사용자 정보에서 아직 사용되지 않은 공간을 찾는 함수
	NRUD *GetEmptySlot(NeoServerData *ap_server);


	//// 로그인된 사용자를 위한 함수 ////

	// 사용자 목록에서 실제로 등록된 마지막 사용자의 주소를 설정하는 함수
	void SetLastUser(NeoServerData *ap_server, NRUD *ap_last_users);
	// 문자열의 길이를 구하는 함수. 하지만 이 문자열의 길이는 a_max_len보다 클수 없다.
	int GetStringLength(const char *ap_str, int a_max_len);
	// 로그인 정보를 등록된 사용자와 비교하는 함수
	NRUD *CheckLoginData(NeoServerData *ap_server, LoginData *ap_data);
	// 접속된 사용자를 강제로 접속 해제시킨다.
	void CloseUserSocket(NeoServerData *ap_server, NRUD *ap_user, char a_reset_flag);
	// 클라이언트로 데이터를 전송하는 함수, (6바이트 프레임 구성 : "[ Key(1), Message ID(1), Body size(4)] +  Body Data")
	void SendUserFrameData(NeoServerData *ap_server, NRUD *ap_user, UINT8 a_msg_id, const void *ap_data, UINT32 a_size);
	// 접속한 전체 사용자에게 메시지를 보내는 함수
	void SendBroadcastUserFrameData(NeoServerData *ap_server, UINT8 a_msg_id, const void *ap_data, UINT32 a_size);
	// 지정한 사용자들에게 메시지를 보내는 함수
	void SendBroadcastUserFrameDataEx(NeoServerData *ap_server, NRUD **ap_user, int a_count, UINT8 a_msg_id, const void *ap_data, UINT32 a_size);
	// 사용자 소켓에 발생한 이벤트를 처리하는 함수
	void ProcessUserSocketEvent(NeoServerData *ap_server, SOCKET ah_socket, int a_event_type);
	// 수신된 데이터가 프로토콜 형식에 맞는지 그리고 완전히 수신되었는지 진행하는 함수
	int ProcessingUserMessage(NeoServerData *ap_server, NRUD *ap_user, UINT8 *ap_recv_data, int a_recv_size);
	// 특정 사용자 오류로 연결을 해제할 때 사용하는 함수
	void DisconnectUserByError(NeoServerData *ap_server, NRUD *ap_user, const char *ap_error_msg, char a_reset_flag);
}


// 클라이언트 소켓 파트

struct NeoSocketData;
typedef void (*FP_EVENT_STR_CLIENT)(NeoSocketData *ap_client, const char *ap_string);  // 이벤트를 추가하기 위한 함수
typedef int (*FP_USER_MSG_CLIENT)(NeoSocketData *ap_client); // 서버와 통신하기 위해 정의한 함수
typedef void (*FP_CLIENT_NOTIFY)(NeoSocketData *ap_client, UINT32 a_state); // 클라이언트 상태 통보용 함수

struct NeoSocketData
{
	UINT8 msg_id, is_connected;  // 현재 처리중인 네트워크 메시지 ID, 서버와의 접속 상태(0:접속안됨, 1:연결중, 2:연결상태, 3: 로그인 상태)
	UINT8 base_key;  // 통신에 사용할 기본 키값
	UINT32 data_size, cur_size;  // 전체 수신할 크기와 현재 수신된 크기
	UINT8 *p_recv_data;          // 수신된 데이터의 실제 정보
	UINT8 *p_send_data, *p_temp_recv_data; // 서버와 데이터를 전송하거나 수신할 때 사용할 메모리
	SOCKET h_socket;  // 서버와 통신을 하기 위한 소켓 핸들
	LoginData login_info;  // 마지막 로그인에 사용한 계정 저장

	FP_USER_MSG_CLIENT p_user_msg_func; // 서버가 전송한 메시지를 처리할 함수의 주소
	FP_EVENT_STR_CLIENT p_event_func;   // 이벤트 로그를 처리할 함수의 주소
	FP_CLIENT_NOTIFY p_notify_func;    // 클라이언트 상태 통보용 함수의 주소
};

namespace TW_NeoClientSocket
{
	void InitData(NeoSocketData *ap_data, FP_USER_MSG_CLIENT ap_user_msg_func, FP_EVENT_STR_CLIENT ap_event_func, FP_CLIENT_NOTIFY ap_notify_func, int a_key = 0x29);  // 클라이언트 서비스의 정보를 초기화한다.
	void DestroySocket(NeoSocketData *ap_data);  // 사용중인 클라이언트 소켓을 제거하는 함수
	void CleanUpClientData(NeoSocketData *ap_data);  // 클라이언트 서비스를 위해 생성했던 정보를 제거하는 함수
	void ConnectToServer(NeoSocketData *ap_data, const char *ap_ip_address, int a_port); // 서버에 접속을 시도하는 함수
	// 서버로 데이터를 전송하는 함수, (6바이트 프레임 구성 : "[ Key(1), Message ID(1), Body size(4)] +  Body Data")
	void SendFrameData(NeoSocketData *ap_socket_data, UINT8 a_msg_id, const void *ap_data, UINT32 a_size);
	void ProcessConnectionResult(NeoSocketData *ap_data, int a_error_code); // 서버 접속 결과를 처리하는 함수

	// 수신된 데이터가 프로토콜 형식에 맞는지 그리고 완전히 수신되었는지 진행하는 함수
	int ProcessingMessage(NeoSocketData *ap_data, UINT8 *ap_recv_data, int a_recv_size);
	// 소켓에 발생하는 이벤트를 처리하는 함수
	void ProcessSocketEvent(NeoSocketData *ap_data, int a_event_type);
}

extern const char *gp_app_name;
extern const char *gp_window_title;
extern HWND gh_main_wnd;
#endif