/***************************************************************************
*
*									������
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
	//����DPI��֪���������ģ��
	SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);		
	
	//��ʼ������
	MainWindow::InitWindow(hInstance);

	// ����Ϣѭ��:
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