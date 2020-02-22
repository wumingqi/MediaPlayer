/**************************
*	�ļ���CustomDraw.cpp
**************************/

#include "CustomDraw.h"

//���캯��������һЩ��ʼ������
CustomDraw::CustomDraw(HWND hWnd)
{
	//����Direct 2D����
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &this->pf);
	RECT rc;
	GetClientRect(hWnd, &rc);
	this->width = rc.right;
	this->height = rc.bottom;

	//����������Ŀ��
	pf->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(hWnd, D2D1::SizeU(rc.right,rc.bottom)),&this->phrt);

	//����һ����ˢ
	phrt->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green), &this->pBack);
}

//�����������ͷ����뵽����Դ
CustomDraw::~CustomDraw()
{
	SafeRelease(&pf);
	SafeRelease(&phrt);
	SafeRelease(&pBack);
}

//���Ʋ�������
void CustomDraw::MyDraw()
{
	phrt->BeginDraw();
	//phrt->Clear(D2D1::ColorF(1.0, 1.0, 1.0));		//ˢ����
	//�ڴ�������ƴ���
	IWICImagingFactory *pf = NULL;

	// Create the COM imaging factory
	HRESULT hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&pf)
	);
	IWICBitmapDecoder *pDecoder = NULL;

	pf->CreateDecoderFromFilename(
		L"background.jpg",                      // Image to be decoded
		NULL,									// Do not prefer a particular vendor
		GENERIC_READ,							// Desired read access to the file
		WICDecodeMetadataCacheOnDemand,			// Cache metadata when needed
		&pDecoder								// Pointer to the decoder
	);
	IWICBitmapFrameDecode *pFrame = NULL;
	pDecoder->GetFrame(0, &pFrame);
	IWICFormatConverter *m_pConvertedSourceBitmap;
	pf->CreateFormatConverter(&m_pConvertedSourceBitmap);
	m_pConvertedSourceBitmap->Initialize(
		pFrame,									// Input bitmap to convert
		GUID_WICPixelFormat32bppPBGRA,			// Destination pixel format
		WICBitmapDitherTypeNone,				// Specified dither pattern
		NULL,									// Specify a particular palette 
		0.f,									// Alpha threshold
		WICBitmapPaletteTypeCustom				// Palette translation type
	);
	ID2D1Bitmap *m_pD2DBitmap;
	phrt->CreateBitmapFromWicBitmap(m_pConvertedSourceBitmap, NULL, &m_pD2DBitmap);
	phrt->DrawBitmap(m_pD2DBitmap, D2D1::RectF(0, 0, 960, 540));

	phrt->EndDraw();
	m_pD2DBitmap->Release();
	pDecoder->Release();
	pFrame->Release();
	m_pConvertedSourceBitmap->Release();
	phrt->EndDraw();
}
