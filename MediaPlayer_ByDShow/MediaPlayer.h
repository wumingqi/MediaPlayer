#pragma once
#include "App.h"

class MediaPlayer
{
private:
	HWND hWnd;
	TCHAR szFileName[MAX_PATH];								//��ǰ�����ļ���
	PLAYER_STATUS status;									//��������ǰ��״̬
	MEDIA_FILE_TYPE mediaType;									//

	//DShow�ӿڶ���
	IGraphBuilder *pgb;										//ɸѡ��ͼ������
	IMediaControl *pmc;										//ý����ƽӿ�
	IMediaEvent *pme;										//ý���¼��ӿ�
	IMediaSeeking *pms;										//ý�嶨λ�ӿ�
	IVMRWindowlessControl* pwc;

	HRESULT InitVMR();
	HRESULT InitGraph();
	HRESULT TearDown();
	MEDIA_FILE_TYPE TestType();
public:
	MediaPlayer(HWND hWnd);									//���캯��
	HRESULT AddFile();
	HRESULT OpenFile(wchar_t *file);
	HRESULT Run();											//����
	HRESULT Pause();										//��ͣ
	HRESULT Stop();											//ֹͣ
	HRESULT Seek(LONGLONG pos);								//��λ


	IVMRWindowlessControl* GetWC() { return this->pwc; };
	MEDIA_FILE_TYPE GetMediaType() { return this->mediaType; };
	LONGLONG GetTimeLength();
	LONGLONG GetCurrentPos();
	PLAYER_STATUS GetStatus();
	void Size(DWORD w, DWORD h);
	~MediaPlayer();
};

