/***************************************************************************
*
*							MainWindow.cpp �����ں���ʵ��
*
***************************************************************************/
#pragma region û�õ�

#include "MainWindow.h"

HWND MainWindow::hWnd;
HACCEL MainWindow::hAccelTable;
NOTIFYICONDATA MainWindow::nid;
HMENU MainWindow::hPopupTray;
HMENU MainWindow::hPopupMenu;
HWND MainWindow::hTrackBar;

//��ʼ������
int MainWindow::InitWindow(HINSTANCE hInstance)
{
	WNDCLASS wc;

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc = WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_VIDEO));
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
	wc.lpszMenuName = NULL;
	wc.lpszClassName = CLASS_NAME;

	RegisterClassW(&wc);

	MainWindow::hWnd = CreateWindowEx(WS_EX_ACCEPTFILES, CLASS_NAME, L"ý�岥����", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	SetWindowLong(hWnd, GWL_STYLE, WS_VISIBLE);
	ShowWindow(MainWindow::hWnd, SW_SHOWNORMAL);
	UpdateWindow(MainWindow::hWnd);

	//��ݼ���
	ACCEL accel[ACCEL_COUNT];
	accel[0].cmd = CMD_CLOSE;
	accel[0].fVirt = FVIRTKEY | FCONTROL;
	accel[0].key = 'W';
	MainWindow::hAccelTable = CreateAcceleratorTable(accel, ACCEL_COUNT);

	//�����˵�
	hPopupMenu = CreatePopupMenu();
	AppendMenu(hPopupMenu, MF_STRING, CMD_FILE_OPEN, L"���ļ�(&O)");
	AppendMenu(hPopupMenu, MF_SEPARATOR, 0, 0);
	AppendMenu(hPopupMenu, MF_STRING, CMD_MP_PLAY, L"��������(&P)");
	AppendMenu(hPopupMenu, MF_STRING, CMD_MP_PAUSE, L"�ݡ���ͣ(&B)");
	AppendMenu(hPopupMenu, MF_SEPARATOR, 0, 0);
	AppendMenu(hPopupMenu, MF_STRING, CMD_CLOSE, L"�رճ���(&C)");
	hPopupTray = hPopupMenu;

	return 0;
}

HACCEL MainWindow::GetAccelTable()
{
	return MainWindow::hAccelTable;
}

HWND MainWindow::GetHwnd()
{
	return MainWindow::hWnd;
}
#pragma endregion

int w, h, x, y;
HBITMAP hBmp;
HDC hM, hMain;

long MainWindow::WinProc(HWND hWnd, unsigned int msg, WPARAM wParam, LPARAM lParam)
{
	static MediaPlayer *mp;
	static IVMRWindowlessControl *pwc;
	static MEDIA_FILE_TYPE mediaType = MT_AUDIO;
	static bool isFullScreen;
	switch (msg)
	{
	case WM_CREATE:
	{
		LPCREATESTRUCT lpCreateStruct = (LPCREATESTRUCT)lParam;
		InitTray(lpCreateStruct->hInstance, hWnd);
		mp = new MediaPlayer(hWnd);

		wchar_t file[32] = L"1.bmp";

		hMain = GetDC(hWnd);
		hBmp = (HBITMAP)LoadImage(lpCreateStruct->hInstance,
			file, IMAGE_BITMAP, 1920, 1080, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		hM = CreateCompatibleDC(hMain);
		SelectObject(hM, hBmp);

		/*hTrackBar = CreateWindow(TRACKBAR_CLASS, nullptr, WS_CHILD | WS_VISIBLE | TBS_BOTH | TBS_NOTICKS | TBS_TOOLTIPS,
			0, 0, 0, 0, hWnd, (HMENU)IDC_TRACKBAR, 0, 0);
		SendMessage(hTrackBar, TBM_SETPAGESIZE, 0, 10);*/
		isFullScreen = false;
		break;
	}
	case WM_LBUTTONDOWN:
	{
		SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
		break;
	}
	case WM_LBUTTONDBLCLK:			//���˫����Ϣ
	{
		static RECT rc;				//����ȫ��ǰ�Ĵ��ھ��Σ���������λ������ʹ��ڳߴ磩
		if (isFullScreen)			//�˳�ȫ��
		{
			SetWindowPos(hWnd, HWND_NOTOPMOST, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, 0);
			isFullScreen = false;
		}
		else						//����ȫ��
		{
			GetWindowRect(hWnd, &rc);
			SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 1920, 1080, 0);
			isFullScreen = true;
		}
		break;
	}
	case WM_TRAY:					//���½�������Ϣ
	{
		HandleTray(wParam, lParam);
		break;
	}
	case WM_DROPFILES:				//��ק�ļ���Ϣ�����������ȷ����Բ���
	{
		HDROP hDrop = (HDROP)wParam;
		wchar_t file[MAX_PATH];
		DragQueryFile(hDrop, 0, file, MAX_PATH);
		mp->OpenFile(file);
		pwc = mp->GetWC();
		InvalidateRect(hWnd, 0, 1);
		mp->Size(w, h);
		mediaType = mp->GetMediaType();
		DragFinish(hDrop);
		break;
	}
	case WM_RBUTTONDOWN:			//�Ҽ���Ϣ�������˵�
	{
		POINT p;
		p.x = LOWORD(lParam);
		p.y = HIWORD(lParam);
		ClientToScreen(hWnd, &p);
		TrackPopupMenu(hPopupMenu, TPM_LEFTALIGN, p.x, p.y, 0, hWnd, NULL);
		break;
	}
	case WM_KEYDOWN:				//������Ϣ��ĳЩ��ݼ�
	{
		if (wParam == VK_F5)					//��ͣ�򲥷�
		{
			if (mp->GetStatus() == PS_PAUSE)
				mp->Run();
			else
				mp->Pause();
		}
		if (wParam == VK_F6)					//���ļ�
		{
			SendMessage(hWnd, WM_COMMAND, MAKEWORD(CMD_FILE_OPEN, 0), 0);
		}
		if (wParam == VK_LEFT)
		{
			MessageBeep(1);
			LONGLONG curPos = mp->GetCurrentPos() / ONE_SECOND;
			curPos = max(10, curPos - 5);
			mp->Seek(curPos);
		}
		if (wParam == VK_RIGHT)
		{
			MessageBeep(MB_ICONERROR);
			LONGLONG curPos = mp->GetCurrentPos() / ONE_SECOND;
			curPos = min(mp->GetTimeLength()/ONE_SECOND, curPos + 5);
			mp->Seek(curPos);
		}
		break;
	}
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);

		switch (wmId)
		{
		//�رճ���
		case CMD_CLOSE:
			DestroyWindow(hWnd);
			break;
		case CMD_FILE_OPEN:
			if (!mp->AddFile())
			{
				pwc = mp->GetWC();
				InvalidateRect(hWnd, 0, 1);
				mp->Size(w, h);
				mediaType = mp->GetMediaType();
			}
			break;
		case CMD_MP_PLAY:
			mp->Run();
			break;
		case CMD_MP_PAUSE:
			mp->Pause();
			break;
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
		break;
	}

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		//��ʼDirect����
		/*CustomDraw c(hWnd);
		c.MyDraw();*/
		
		if (mediaType == MT_VIDEO)
		{
			if (pwc)
			{
				pwc->RepaintVideo(hWnd, hdc);
			}
		}
		else
		{
			BitBlt(hMain, 0, 0, 1920, 1080, hM, 0, 0, SRCCOPY);
		}

		EndPaint(hWnd, &ps);
		break;
	}
	case WM_GETMINMAXINFO:
	{
		MINMAXINFO *mmi=(LPMINMAXINFO)lParam;
		mmi->ptMinTrackSize = {960,540};
		break;
	}

#pragma region NC����

	case WM_NCHITTEST:
	{
		// Get the point coordinates for the hit test.
		POINT ptMouse = { LOWORD(lParam), HIWORD(lParam) };
		// Get the window rectangle.
		RECT rcWindow;
		GetWindowRect(hWnd, &rcWindow);
		// Get the frame rectangle, adjusted for the style without a caption.
		RECT rcFrame = { 0 };
		AdjustWindowRectEx(&rcFrame, WS_OVERLAPPEDWINDOW & ~WS_CAPTION, FALSE,
			NULL);
		// Determine if the hit test is for resizing. Default middle (1,1).
		USHORT uRow = 1;
		USHORT uCol = 1;
		bool fOnResizeBorder = false;
		// Determine if the point is at the top or bottom of the window.
		if (ptMouse.y >= rcWindow.top && ptMouse.y < rcWindow.top +
			8)
		{
			fOnResizeBorder = (ptMouse.y < (rcWindow.top - rcFrame.top));
			uRow = 0;
		}
		else if (ptMouse.y < rcWindow.bottom && ptMouse.y >= rcWindow.bottom -
			8)
		{
			uRow = 2;
		}
		// Determine if the point is at the left or right of the window.
		if (ptMouse.x >= rcWindow.left && ptMouse.x < rcWindow.left +
			8)
		{
			uCol = 0; // left side
		}
		else if (ptMouse.x < rcWindow.right && ptMouse.x >= rcWindow.right -
			8)
		{
			uCol = 2; // right side
		}
		// Hit test (HTTOPLEFT, ... HTBOTTOMRIGHT)
		LRESULT hitTests[3][3] =
		{
			{ HTTOPLEFT, fOnResizeBorder ? HTTOP : HTCAPTION, HTTOPRIGHT },
			{ HTLEFT, HTCLIENT, HTRIGHT },{ HTBOTTOMLEFT, HTBOTTOM, HTBOTTOMRIGHT },
		};
		return hitTests[uRow][uCol];
	}

#pragma endregion

	case WM_SIZE:
	{
		w = LOWORD(lParam);
		h = HIWORD(lParam);
		mp->Size(w, h);
		//MoveWindow(hTrackBar, 50, h - 50, w - 100, 30, TRUE);
		break;
	}

	case WM_DESTROY:
	{
		delete(mp);
		Shell_NotifyIcon(NIM_DELETE, &nid);
		PostQuitMessage(0);
		break;
	}
	default:
		if(msg==WM_TASKBAR_CREATED)
			Shell_NotifyIcon(NIM_ADD, &nid);
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

#pragma region ����

void MainWindow::InitTray(HINSTANCE hInstance, HWND hWnd)
{
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = 103;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;
	nid.uCallbackMessage = WM_TRAY;
	nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_VIDEO));
	lstrcpy(nid.szTip, L"ý�岥����");
	Shell_NotifyIcon(NIM_ADD, &nid);
}

//����������Ϣ
void MainWindow::HandleTray(WPARAM wParam,LPARAM lParam)
{
	switch (lParam)
	{
	case WM_RBUTTONDOWN:
	{
		//��ȡ�������  
		POINT pt; GetCursorPos(&pt);

		//����ڲ˵��ⵥ������˵�����ʧ������  
		SetForegroundWindow(hWnd);

		//ʹ�˵�ĳ����  
		//EnableMenuItem(hMenu, ID_SHOW, MF_GRAYED);      

		//��ʾ����ȡѡ�еĲ˵�  
		int cmd = TrackPopupMenu(hPopupTray, TPM_RETURNCMD, pt.x, pt.y, NULL, hWnd,
			NULL);
		if (cmd == CMD_CLOSE)
			PostMessage(hWnd, WM_DESTROY, NULL, NULL);
	}
	break;
	case WM_LBUTTONDOWN:
		SetForegroundWindow(hWnd);
		break;
	case WM_LBUTTONDBLCLK:
		SetForegroundWindow(hWnd);
		break;
	}
}
#pragma endregion
