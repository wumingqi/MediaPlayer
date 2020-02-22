#pragma once
#include "App.h"
#include "CustomDraw.h"
#include "MediaPlayer.h"

class MainWindow
{
public:
	static int InitWindow(HINSTANCE hInstance);				//��ʼ������
	static HACCEL GetAccelTable();							//��ȡ���ټ���
	static HWND GetHwnd();									//��ȡ���ھ��
private:
	static long _stdcall WinProc(HWND hWnd, unsigned int msg, WPARAM wParam, LPARAM lParam);
	static void InitTray(HINSTANCE hInstance, HWND hWnd);
	static void HandleTray(WPARAM wParam, LPARAM lParam);

	static HWND hWnd;
	static HACCEL hAccelTable;
	static NOTIFYICONDATA nid;
	static HMENU hPopupTray;
	static HMENU hPopupMenu;
	static HWND hTrackBar;
};