#pragma once
#include "App.h"

class CustomDraw
{
public:
	CustomDraw(HWND hWnd);
	~CustomDraw();
	void MyDraw();							//�Զ�����ƺ���
private:
	ID2D1Factory *pf;						//����
	ID2D1HwndRenderTarget *phrt;			//���ڳ���Ŀ��
	ID2D1SolidColorBrush *pBack;			//��ɫ��ˢ

	DWORD width;
	DWORD height;
};