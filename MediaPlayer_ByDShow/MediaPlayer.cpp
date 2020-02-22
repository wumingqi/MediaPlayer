#include "MediaPlayer.h"

//
MediaPlayer::MediaPlayer(HWND hWnd)
{
	this->hWnd = hWnd;
	this->status = PS_STOP;

	//初始化
	InitGraph();
	//初始化VMR-7
	InitVMR();
}

HRESULT MediaPlayer::AddFile()
{
	OPENFILENAME ofn = { 0 };

	ofn.lStructSize = sizeof(OPENFILENAME);//结构体大小
	ofn.hwndOwner = this->hWnd;//拥有着窗口句柄，为NULL表示对话框是非模态的，实际应用中一般都要有这个句柄
	//ofn.lpstrFilter = TEXT("所有文件\0*.*\0C/C++ Flie\0*.cpp;*.c;*.h\0\0");//设置过滤
	ofn.lpstrFilter = TEXT("媒体文件（支持mp3、avi等文件）\0*.mp3;*.avi\0");
	ofn.nFilterIndex = 1;//过滤器索引
	ofn.lpstrFile = this->szFileName;//接收返回的文件名，注意第一个字符需要为NULL
	ofn.nMaxFile = MAX_PATH;//缓冲区长度
	ofn.lpstrInitialDir = L"E:\\KwDownload\\音乐";//初始目录为默认
	ofn.lpstrTitle = TEXT("请选择一个媒体文件");//使用系统默认标题留空即可
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;//文件、目录必须存在，隐藏只读选项
	ofn.lpstrDefExt = L"打开文件";

	int rtn = GetOpenFileName(&ofn);
	if (rtn)
	{
		TestType();
		this->TearDown();
		this->InitGraph();
		this->InitVMR();
		rtn = this->pgb->RenderFile(this->szFileName, NULL);
		this->status = PS_STOP;
		this->Run();
	}
	return rtn;
}

HRESULT MediaPlayer::OpenFile(wchar_t * file)
{
	StringCchCopy(this->szFileName, MAX_PATH, file);
	if (TestType() == MT_NOT_VALID)
		return -1;
	this->TearDown();
	this->InitGraph();
	this->InitVMR();
	HRESULT rtn = this->pgb->RenderFile(this->szFileName, NULL);
	this->status = PS_STOP;
	this->Run();
	return rtn;
}

HRESULT MediaPlayer::Run()
{
	if (pmc&&this->status != PS_PLAY)
	{
		this->status = PS_PLAY;
		return pmc->Run();
	}
	return -1;
}

HRESULT MediaPlayer::Pause()
{
	if (pmc&&this->status != PS_PAUSE)
	{
		this->status = PS_PAUSE;
		return pmc->Pause();
	}
	return -1;
}

HRESULT MediaPlayer::Stop()
{
	if (pmc&&this->status != PS_STOP)
	{
		this->status = PS_STOP;
		ZeroMemory(this->szFileName, MAX_PATH);
		pgb->Abort();
		return pmc->StopWhenReady();
	}
	return E_NOTIMPL;
}

HRESULT MediaPlayer::Seek(LONGLONG pos)
{
	if (pms&&this->status != PS_STOP)
	{
		LONGLONG p = pos*ONE_SECOND;
		return pms->SetPositions(&p, AM_SEEKING_AbsolutePositioning, nullptr, AM_SEEKING_NoPositioning);
	}
	return -1;
}

LONGLONG MediaPlayer::GetTimeLength()
{
	LONGLONG cur, stop;
	pms->GetPositions(&cur, &stop);
	return stop;
}

LONGLONG MediaPlayer::GetCurrentPos()
{
	LONGLONG cur, stop;
	pms->GetPositions(&cur, &stop);
	return cur;
}

PLAYER_STATUS MediaPlayer::GetStatus()
{
	return this->status;
}

void MediaPlayer::Size(DWORD w, DWORD h)
{
	long lWidth, lHeight;
	HRESULT hr = pwc->GetNativeVideoSize(&lWidth, &lHeight, NULL, NULL);
	RECT rcSrc, rc;
	SetRect(&rcSrc, 0, 0, lWidth, lHeight);
	GetClientRect(hWnd, &rc);

	double w1 = lWidth, h1 = lHeight, w2 = rc.right, h2 = rc.bottom;

	if (w2 / h2 > w1 / h1)
	{
		lWidth = (LONG)(rc.bottom*(w1 / h1));
		rc.top = 0;
		rc.left = (rc.right - lWidth) / 2;
		rc.right = (rc.right + lWidth) / 2;
	}
	else
	{
		lHeight = (LONG)(rc.right / (w1/h1));
		rc.left = 0;
		rc.top = (rc.bottom - lHeight) / 2;
		rc.bottom = (rc.bottom + lHeight) / 2;
	}
	hr = pwc->SetVideoPosition(&rcSrc, &rc);
}

HRESULT MediaPlayer::InitVMR()
{
	IBaseFilter* pVmr = NULL;
	HRESULT hr = CoCreateInstance(CLSID_VideoMixingRenderer, NULL,
		CLSCTX_INPROC, IID_IBaseFilter, (void**)&pVmr);
	this->pgb->AddFilter(pVmr, L"Video Mixing Renderer");
	IVMRFilterConfig* pConfig;
	hr = pVmr->QueryInterface(IID_IVMRFilterConfig, (void**)&pConfig);
	pConfig->SetRenderingMode(VMRMode_Windowless);
	pVmr->QueryInterface(IID_IVMRWindowlessControl, (void**)&this->pwc);
	this->pwc->SetVideoClippingWindow(hWnd);
	pConfig->Release();
	pVmr->Release();
	return 0;
}

HRESULT MediaPlayer::InitGraph()
{
	//创建Filter Graph Manager
	CoCreateInstance(CLSID_FilterGraph, nullptr, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&pgb);

	//获取接口对象
	pgb->QueryInterface<IMediaControl>(&pmc);
	pgb->QueryInterface<IMediaEvent>(&pme);
	pgb->QueryInterface<IMediaSeeking>(&pms);

	return 0;
}

HRESULT MediaPlayer::TearDown()
{
	SafeRelease(&pwc);
	SafeRelease(&pmc);
	SafeRelease(&pme);
	SafeRelease(&pms);
	SafeRelease(&pgb);

	return 0;
}

MEDIA_FILE_TYPE MediaPlayer::TestType()
{
	wchar_t buf[16];
	int len = wcslen(this->szFileName);
	len--;
	int i = 14;
	buf[15] = 0;
	for (; i > 0; i--, len--)
	{
		if (szFileName[len] == '.')
			break;
		buf[i] = szFileName[len];
	}
	//MessageBox(hWnd, buf + i+1, 0, 0);
	i++;
	if (!wcscmp(buf + i, L"mp3"))
	{
		this->mediaType = MT_AUDIO;
	}
	else if (!wcscmp(buf + i, L"avi"))
	{
		this->mediaType = MT_VIDEO;
	}
	else
	{
	}
	return this->mediaType;
}

MediaPlayer::~MediaPlayer()
{
	SafeRelease(&pwc);
	SafeRelease(&pmc);
	SafeRelease(&pme);
	SafeRelease(&pms);
	SafeRelease(&pgb);
}
