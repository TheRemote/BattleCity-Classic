#include "CLogin.h"

HBRUSH g_hbrBackgroundLogin;
HDC hdcStaticLogin;

void *CLogPointer;

int CALLBACK LoginDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	CGame *p = (CGame *)CLogPointer;
    switch(Message)
    {
        case WM_INITDIALOG:
			{
				SendDlgItemMessage(hwnd, IDUSER, EM_LIMITTEXT, 15, 0);
				SendDlgItemMessage(hwnd, IDPASS, EM_LIMITTEXT, 15, 0);
				SetDlgItemText(hwnd, IDVERSION, p->Login->VersionString.c_str());

				return 1;
			}
			break;
		case WM_CTLCOLORSTATIC:
			{
				SetBkMode((HDC)wParam, TRANSPARENT);
				
				SetTextColor((HDC)wParam, RGB(255, 255, 255));
				return (LRESULT)GetStockObject(HOLLOW_BRUSH);
			}
			break;
		case WM_ERASEBKGND:
			{
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hwnd, &ps);
				HBITMAP foo = LoadBitmap (p->hInst, MAKEINTRESOURCE(IDB_BITMAP1));
				HDC memDC = CreateCompatibleDC (hdc);

				int save = SaveDC (memDC);

				SelectObject (memDC, foo);

				for (int i = 0; i < 5; i++)
				{
					for (int j = 0; j < 4; j++)
					{
						BitBlt (hdc, i * 64, j * 64, 64, 64, memDC, 0, 0, SRCCOPY);
					}
				}

				RestoreDC (memDC, save);
				EndPaint(hwnd, &ps);
				DeleteObject (foo);
			}
			break;
        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case 1:
                    GetDlgItemText(hwnd, IDUSER, p->Login->user, 15);
					GetDlgItemText(hwnd, IDPASS, p->Login->pass, 15);
					if ((strlen(p->Login->user) > 0) && (strlen(p->Login->pass) > 0))
					{
						p->Send->SendLogin(0);
						p->Dialog->StartDialog = 0;
						EndDialog(hwnd, 1);
					}
					else MessageBox(p->hWnd, "Invalid Credentials!", 0, 0);
                break;
                case 2:
					SendMessage(p->hWnd, WM_CLOSE, 0, 0);
                break;
				case 3:
					p->NewAccount->ShowNewDlg();
					EndDialog(hwnd, 3);
				break;
				case 4:
                    GetDlgItemText(hwnd, IDUSER, p->Login->user, 15);
					GetDlgItemText(hwnd, IDPASS, p->Login->pass, 15);
					if ((strlen(p->Login->user) > 0) && (strlen(p->Login->pass) > 0))
					{
						p->Send->SendAccountEdit();
						p->Dialog->StartDialog = 0;
						EndDialog(hwnd, 4);
					}
					else MessageBox(p->hWnd, "Invalid Credentials!", 0, 0);
				break;
				case 5:
					p->Recover->ShowRecoverDlg();
					EndDialog(hwnd, 5);
				break;
            }
			break;
        default:
            return 0;
			break;
    }
    return 1;
}

CLogin::CLogin(CGame *game)
{
	p = game;
	CLogPointer = game;

	VersionString = "BattleCity Classic ";
	VersionString += VERSION;
}

CLogin::~CLogin()
{

}

void CLogin::ShowLoginDlg()
{
	p->State = STATE_LOGIN;
	p->Dialog->StartDialog = ID_LOGIN;
	p->Dialog->DialogProcessor = &LoginDlgProc;
}