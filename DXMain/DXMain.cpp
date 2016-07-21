// DXMain.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "DXMain.h"

#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

#include <iostream>
#include <vector>
#include <string>
#include <chrono>

using namespace std;
using namespace std::chrono;

using namespace DirectX;
using namespace DirectX::PackedVector;


template<typename Ty, size_t N>
constexpr size_t GetArraySize(Ty (&)[N]) noexcept
{
	return N;
}


class CDXFramework {

private:

	ID3D11Device			*	m_pd3dDevice				{ nullptr }	;
	IDXGISwapChain			*	m_pdxgiSwapChain			{ nullptr }	;

	ID3D11RenderTargetView	*	m_pd3dRenderTargetView		{ nullptr }	;
	ID3D11DeviceContext		*	m_pd3dDeviceContext		{ nullptr }	;

	HINSTANCE					m_hInstance					{ NULL }	;
	HWND						m_hWnd						{ NULL }	;
	RECT						m_rcClientRect							;
public:

	CDXFramework() = default;
	~CDXFramework()
	{
		// Release 객체
	}

	void Initialize(HINSTANCE hInstance, HWND hWnd)
	{
		m_hInstance = hInstance;
		m_hWnd = hWnd;
		GetClientRect(m_hWnd, &m_rcClientRect);

		CreateD3DXDeivce();
	}

	void CreateD3DXDeivce()
	{
		DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
		::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
		dxgiSwapChainDesc.BufferCount = 1;
		dxgiSwapChainDesc.BufferDesc.Width = m_rcClientRect.right;
		dxgiSwapChainDesc.BufferDesc.Height = m_rcClientRect.bottom;
		dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

		dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;

		dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		dxgiSwapChainDesc.OutputWindow = m_hWnd;

		dxgiSwapChainDesc.SampleDesc = DXGI_SAMPLE_DESC{ 1, 0 };
		dxgiSwapChainDesc.Windowed = TRUE;

		dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH/*2*/;



		D3D_DRIVER_TYPE d3dDriverTypes[]
		{
			  D3D_DRIVER_TYPE_HARDWARE
			, D3D_DRIVER_TYPE_WARP
			, D3D_DRIVER_TYPE_REFERENCE
		};

		D3D_FEATURE_LEVEL d3dFeatureLevels[]
		{
			  D3D_FEATURE_LEVEL_11_1
			, D3D_FEATURE_LEVEL_11_0
			, D3D_FEATURE_LEVEL_10_1
			, D3D_FEATURE_LEVEL_10_0
		};
		size_t szFeatureLevel = GetArraySize(d3dFeatureLevels);

		D3D_FEATURE_LEVEL nd3dFeatureLevel = D3D_FEATURE_LEVEL_11_1;

		for (D3D_DRIVER_TYPE &p : d3dDriverTypes)
		{
			D3D11CreateDevice(	  NULL
								, p
								, 0
								, 0
								, d3dFeatureLevels
								, szFeatureLevel
								, D3D11_SDK_VERSION
								, &m_pd3dDevice
								, &nd3dFeatureLevel
								, &m_pd3dDeviceContext
				);
		}




	}

};





#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);










int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DXMAIN, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 응용 프로그램 초기화를 수행합니다.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DXMAIN));

    MSG msg;

    // 기본 메시지 루프입니다.
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DXMAIN));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_DXMAIN);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다.
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
