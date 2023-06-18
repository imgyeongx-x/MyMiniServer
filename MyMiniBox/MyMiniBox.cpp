#include "pch.h"
#include "tipsware.h"

void OnCommand(INT32 a_ctrl_id, INT32 a_notify_code, void* ap_ctrl)
{
    if (a_ctrl_id == 1002 || (a_ctrl_id == 1001 && a_notify_code == 1000)) {
        char str[64]; 
        void* p_edit = FindControl(1001); 
        GetCtrlName(p_edit, str, 64);
        SetCtrlName(p_edit, "");
        ListBox_InsertString(FindControl(1000), -1, str, 1);
    }
}

CMD_MESSAGE(OnCommand)

int main()
{
    ChangeWorkSize(480, 200);
    Clear(0, RGB(72, 87, 114));
    CreateListBox(10, 10, 460, 150, 1000);
    void* p_edit = CreateEdit(10, 167, 406, 24, 1001, 0);
    EnableEnterKey(p_edit);
    CreateButton("추가", 420, 164, 50, 28, 1002);

    ShowDisplay();
    return 0;
}