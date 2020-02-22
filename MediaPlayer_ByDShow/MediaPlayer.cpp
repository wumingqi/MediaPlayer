#include "MediaPlayer.h"

//
MediaPlayer::MediaPlayer(HWND hWnd)
{
	this->hWnd = hWnd;
	this->status = PS_STOP;

	//��ʼ��
	InitGraph();
	//��ʼ��VMR-7
	InitVMR();
}

HRESULT MediaPlayer::AddFile()
{
	OPENFILENAME ofn = { 0 };

	ofn.lStructSize = sizeof(OPENFILENAME);//�ṹ���С
	ofn.hwndOwner = this->hWnd;//ӵ���Ŵ��ھ����ΪNULL��ʾ�Ի����Ƿ�ģ̬�ģ�ʵ��Ӧ����һ�㶼Ҫ��������
	//ofn.lpstrFilter = TEXT("�����ļ�\0*.*\0C/C++ Flie\0*.cpp;*.c;*.h\0\0");//���ù���
	ofn.lpstrFilter = TEXT("ý���ļ���֧��mp3��avi���ļ���\0*.mp3;*.avi\0");
	ofn.nFilterIndex = 1;//����������
	ofn.lpstrFile = this->szFileName;//���շ��ص��ļ�����ע���һ���ַ���ҪΪNULL
	ofn.nMaxFile = MAX_PATH;//����������
	ofn.lpstrInitialDir = L"E:\\KwDownload\\����";//��ʼĿ¼ΪĬ��
	ofn.lpstrTitle = TEXT("��ѡ��һ��ý���ļ�");//ʹ��ϵͳĬ�ϱ������ռ���
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;//�ļ���Ŀ¼������ڣ�����ֻ��ѡ��
	ofn.lpstrDefExt = L"���ļ�";

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
	//����Filter Graph Manager
	CoCreateInstance(CLSID_FilterGraph, nullptr, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&pgb);

	//��ȡ�ӿڶ���
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
