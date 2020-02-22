/***************************************************************************
*
*									主函数
*
***************************************************************************/
#include "MainWindow.h"
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	if (HWND h = FindWindow(CLASS_NAME, NULL))
	{
		SetForegroundWindow(h);
		return 0;
	}
	CoInitialize(NULL);
	//设置DPI感知，避免程序模糊
	SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);		
	
	//初始化窗口
	MainWindow::InitWindow(hInstance);

	// 主消息循环:
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, MainWindow::GetAccelTable(), &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	CoUninitialize();
	return (int)msg.wParam;
}