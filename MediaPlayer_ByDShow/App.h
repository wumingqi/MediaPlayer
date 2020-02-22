#pragma once

#define MADE_BY_WMQ

#include <Windows.h>

#include <ShellScalingApi.h>
#pragma comment(lib, "shcore.lib")

#include <d2d1.h>
#pragma comment(lib, "d2d1.lib")

#include <dshow.h>
#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "Quartz.lib")

#include <wincodec.h>
#include <wincodecsdk.h>
#pragma comment(lib, "windowscodecs.lib")

#include <commdlg.h>
#pragma comment(lib, "Comdlg32.lib")

#include <dwrite.h>
#pragma comment(lib, "dwrite.lib")

#include <CommCtrl.h>
#pragma comment(lib, "ComCtl32.lib")

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "resource.h"

#define CLASS_NAME TEXT("MainWindow")
#define ONE_SECOND 10000000

#define WM_TRAY (WM_USER+100)
#define WM_TASKBAR_CREATED RegisterWindowMessage(TEXT("TaskbarCreated"))  


#define ACCEL_COUNT 1			//¿ì½Ý¼üÊýÄ¿
#define CMD_CLOSE	10001

#define CMD_FILE_OPEN		201
#define CMD_MP_PLAY			202
#define CMD_MP_PAUSE		203

#define IDC_TRACKBAR	101



template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

enum MEDIA_FILE_TYPE
{
	MT_AUDIO,
	MT_VIDEO,
	MT_NOT_VALID
};

enum PLAYER_STATUS
{
	PS_STOP,
	PS_PLAY,
	PS_PAUSE,
};
