#pragma once
#include "App.h"

class MediaPlayer
{
private:
	HWND hWnd;
	TCHAR szFileName[MAX_PATH];								//当前播放文件名
	PLAYER_STATUS status;									//播放器当前的状态
	MEDIA_FILE_TYPE mediaType;									//

	//DShow接口定义
	IGraphBuilder *pgb;										//筛选器图表构造器
	IMediaControl *pmc;										//媒体控制接口
	IMediaEvent *pme;										//媒体事件接口
	IMediaSeeking *pms;										//媒体定位接口
	IVMRWindowlessControl* pwc;

	HRESULT InitVMR();
	HRESULT InitGraph();
	HRESULT TearDown();
	MEDIA_FILE_TYPE TestType();
public:
	MediaPlayer(HWND hWnd);									//构造函数
	HRESULT AddFile();
	HRESULT OpenFile(wchar_t *file);
	HRESULT Run();											//播放
	HRESULT Pause();										//暂停
	HRESULT Stop();											//停止
	HRESULT Seek(LONGLONG pos);								//定位


	IVMRWindowlessControl* GetWC() { return this->pwc; };
	MEDIA_FILE_TYPE GetMediaType() { return this->mediaType; };
	LONGLONG GetTimeLength();
	LONGLONG GetCurrentPos();
	PLAYER_STATUS GetStatus();
	void Size(DWORD w, DWORD h);
	~MediaPlayer();
};

