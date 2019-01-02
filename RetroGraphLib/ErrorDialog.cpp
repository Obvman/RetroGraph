#include "stdafx.h"
#include "ErrorDialog.h"

#include "resource1.h"

#include "utils.h"

namespace rg {

LRESULT CALLBACK proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    (void)hwnd;
    (void)lParam;
    switch (message) {
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDOK:
                    break;
                case IDABORT:
                    break;
                //case IDBREAK:
                //    break;
                //case IDCANCEL:
                //    break;
            }
            break;
        default:
            return FALSE;
    }
    return TRUE;
}

ErrorDialog::ErrorDialog() {
    auto* window{ GetActiveWindow() };
    HWND dlgWnd{ CreateDialogParam(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDD_DIALOG1), window, proc, 0) };
    ShowWindow(dlgWnd, SW_SHOW);
}


}