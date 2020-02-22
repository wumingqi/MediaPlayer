#pragma once
#include "App.h"

class CustomDraw
{
public:
	CustomDraw(HWND hWnd);
	~CustomDraw();
	void MyDraw();							//自定义绘制函数
private:
	ID2D1Factory *pf;						//工厂
	ID2D1HwndRenderTarget *phrt;			//窗口呈现目标
	ID2D1SolidColorBrush *pBack;			//纯色画刷

	DWORD width;
	DWORD height;
};