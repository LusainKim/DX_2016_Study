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
	RECT						m_rcClient							;
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
		GetClientRect(m_hWnd, &m_rcClient);

		CreateD3DXDeivce();
	}

	bool CreateD3DXDeivce()
	{
		// Swap Chain Description 구조체
		DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
		#pragma region [DXGI_SWAP_CHAIN_DESC 초기화]
		// 구조체 비우기
		::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
		// BufferCount : 후면 버퍼의 수를 지정
		dxgiSwapChainDesc.BufferCount = 1;
		
		// BufferDesc : 후면 버퍼의 디스플레이 형식을 지정
		{
			// Width : 버퍼의 가로 크기를 픽셀로 지정
			dxgiSwapChainDesc.BufferDesc.Width = m_rcClient.right;
			// Height : 버퍼의 세로 크기를 픽셀로 지정
			dxgiSwapChainDesc.BufferDesc.Height = m_rcClient.bottom;
			// Format : 후면 버퍼 픽셀 형식
			/// DirectX 11-1(Chap 01)-Device, p.49 참조
			dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			// RefreshRate : 화면 갱신 비율을 Hz 단위로 지정
			{
				// Denominator : 분모
				dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
				// Numerator : 분자
				dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
			}
			// ScanlineOrdering : scan line 그리기 모드 지정(기본 0)
			//	DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED        (0) : 스캔 라인 순서를 지정하지 않음	
			//	DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE        (1) : 프로그레시브 스캔 라인 순서 지정
			//	DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST  (2) : 상위 필드로 이미지 생성
			//	DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST  (3) : 하위 필드로 이미지 생성
			dxgiSwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		}

		// BufferUsage : 후면 버퍼에 대한 표면 사용 방식과 CPU의 접근 방법 지정
		//	DXGI_USAGE_SHADER_INPUT				: shader 의 입력으로 사용
		//	DXGI_USAGE_RENDER_TARGET_OUTPUT		: render target으로 사용
		//	DXGI_USAGE_BACK_BUFFER         		: back buffer 로 사용
		//	DXGI_USAGE_SHARED              		: 공유 목적
		//	DXGI_USAGE_READ_ONLY           		: 읽기 전용
		//	DXGI_USAGE_DISCARD_ON_PRESENT  		: DXGI 내부 전용 사용(사용자가 사용하지 않음)
		//	DXGI_USAGE_UNORDERED_ACCESS    		: 무순서화 접근
		dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		// OutputWindow : 출력 윈도우의 핸들을 지정(반드시 유효해야 함)
		dxgiSwapChainDesc.OutputWindow = m_hWnd;

		// SampleDesc : 다중 샘플링의 품질을 지정
		// CheckMultisampleQualityLevels 함수를 사용하여 다중 샘플링 가능 여부를 확인한 뒤에 값 변경
		{
			// Count : 픽셀 당 샘플 개수
			//	1  : 다중 샘플링을 하지 않음
			//	2~ : 해당 수만큼의 다중 샘플링
			dxgiSwapChainDesc.SampleDesc.Count = 1;
			// Quality : 품질 레벨
			// 0 : 다중 샘플링을 하지 않음
			dxgiSwapChainDesc.SampleDesc.Quality = 0;
		}

		// Windowed : 윈도우 모드 또는 전체 화면 모드 지정 ~ TRUE  | 창 모드 
		//												   ~ FALSE | 전체 화면
		dxgiSwapChainDesc.Windowed = TRUE;

		// Flags : Swap Chain 동작에 대한 선택 사항을 지정
		//	DXGI_SWAP_CHAIN_FLAG_NONPREROTATED		(1) : 전체 화면 모드에서 전면 버퍼의 내용을 화면으로 옮길 때 자동 회전하지 않음
		//	DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH	(2) : 응용 프로그램이 디스플레이 모드를 변경할 수 있음
		//	DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE		(4) : 응용 프로그램이 GDI 로 랜더링 할 수 있음. 후면 버퍼에 GetDC() 사용 가능
		dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH/*2*/;

		// SwapEffect : Swaping을 처리하는 선택사항을 지정(기본 : 0)
		//	DXGI_SWAP_EFFECT_DISCARD	(0) : 버퍼 내용을 폐기
		//	DXGI_SWAP_EFFECT_SEQUENTIAL	(1) : 순차 복사
		dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		
		#pragma endregion


		// 지원하는 하드웨어 그래픽 드라이버를 열거합니다.
		D3D_DRIVER_TYPE d3dDriverTypes[]
		{
			// 하드웨어 드라이버, Direct3D의 기능이 최대한 하드웨어로 구현 
			// 하드웨어적으로 제공하지 않는 기능은 소프트웨어를 통해 구현
			  D3D_DRIVER_TYPE_HARDWARE
			// DirectX SDK에서 제공하는 고성능의 소프트웨어 드라이버
			// 특성 레벨 9_1에서 특성 레벨 10.1까지 제공
			// - 별도의 소프트웨어 드라이버(래스터라이저)가 필요없음
			// - 소프트웨어 드라이버이면서 그래픽 하드웨어의 최대 성능을 지원
			//   Direct3D 11을 지원하지 않는 그래픽 하드웨어에서도 사용
			, D3D_DRIVER_TYPE_WARP
			// 참조 드라이버(모든 Direct3D 특성을 지원하는 소프트웨어 드라이버)
			// DirectX SDK에서 속도보다는 정확성을 위해 제공
			// CPU를 사용하여 래스터라이저 구현 
			// Direct3D 11의 모든 특성 레벨에서 이 드라이버를 사용할 수 있음
			// 응용 프로그램의 테스트와 디버깅을 위해 주로 사용
			, D3D_DRIVER_TYPE_REFERENCE
		};

		// 이 배열은 이 응용 프로그램에서 지원하는 DirectX 하드웨어 기능 수준 집합을 정의합니다.
		// 순서를 유지해야 합니다.
		// 설명에서 응용 프로그램에 필요한 최소 기능 수준을 선언해야 합니다.
		// 별도로 지정하지 않은 경우 모든 응용 프로그램은 9.1을 지원하는 것으로 간주됩니다.
		D3D_FEATURE_LEVEL d3dFeatureLevels[]
		{
			  D3D_FEATURE_LEVEL_11_1
			, D3D_FEATURE_LEVEL_11_0
			, D3D_FEATURE_LEVEL_10_1
			, D3D_FEATURE_LEVEL_10_0
		};
		// 지원 가능한 모든 하드웨어 기능 수준 집합 수를 구합니다.
		size_t szFeatureLevel = GetArraySize(d3dFeatureLevels);

		// 최종적으로 사용될 장치의 기능 수준을 저장하기 위한 변수입니다.
		D3D_FEATURE_LEVEL nd3dFeatureLevel = D3D_FEATURE_LEVEL_11_1;

		// Result Handle 입니다. 장치가 성공적으로 생성도는지 검사합니다.
		HRESULT hResult = S_OK;

		for (D3D_DRIVER_TYPE &p : d3dDriverTypes)
		{
			if (SUCCEEDED(hResult = D3D11CreateDevice(	  NULL						// 기본 어댑터를 사용하려면 nullptr을 지정합니다.
														, p							// 하드웨어 그래픽 드라이버를 사용하여 장치를 만듭니다.
														, 0							// 드라이버가 D3D_DRIVER_TYPE_SOFTWARE가 아닌 경우 0이어야 합니다.
														, 0							// 디버그 및 Direct2D 호환성 플래그를 설정합니다.
														, d3dFeatureLevels			// 이 응용 프로그램이 지원할 수 있는 기능 수준 목록입니다.
														, szFeatureLevel			// 위 목록의 크기입니다.
														, D3D11_SDK_VERSION			// Windows 스토어 앱의 경우 항상 이 값을 D3D11_SDK_VERSION으로 설정합니다.
														, &m_pd3dDevice				// 만들어진 Direct3D 장치를 반환합니다.
														, &nd3dFeatureLevel			// 만들어진 장치의 기능 수준을 반환합니다.
														, &m_pd3dDeviceContext		// 장치 직접 컨텍스트를 반환합니다.
													)
							)
				)
				break;
		}

		// 모든 지원 가능한 드라이버에서 생성이 실패하면 프로그램을 종료합니다.
		if (!m_pd3dDevice)
		{
			MessageBox(m_hWnd, TEXT("지원 가능한 그래픽 사양이 없습니다."), TEXT("프로그램 구동 실패"), MB_OK);
			return(false);
		}

		// DXGI Factory 인스턴스를 DXGIFactory에서 받습니다.
		IDXGIFactory1 *pdxgiFactory = NULL;
		if (FAILED(hResult = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void **)&pdxgiFactory)))
		{
			MessageBox(m_hWnd, TEXT("DXGIFactory에서의 객체 생성이 실패했습니다. 프로그램을 종료합니다."), TEXT("프로그램 구동 실패"), MB_OK);
			return(false);
		}
		// DXGI Device 를 받습니다.
		IDXGIDevice *pdxgiDevice = NULL;
		if (FAILED(hResult = m_pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&pdxgiDevice)))
		{
			MessageBox(m_hWnd, TEXT("DXGI Device 객체를 반환받지 못했습니다. 프로그램을 종료합니다."), TEXT("프로그램 구동 실패"), MB_OK);
			return(false);
		}
		//  SwapChain 을 생성합니다
		if (FAILED(hResult = pdxgiFactory->CreateSwapChain(pdxgiDevice, &dxgiSwapChainDesc, &m_pdxgiSwapChain)))
		{
			MessageBox(m_hWnd, TEXT("SwapChain 인스턴스 생성이 실패했습니다. 프로그램을 종료합니다."), TEXT("프로그램 구동 실패"), MB_OK);
			return(false);
		}

		// 할당받은 COM 객체를 반환합니다.
		if (pdxgiDevice) pdxgiDevice->Release();
		if (pdxgiFactory) pdxgiFactory->Release();

		// render target과 depth-stencil buffer 생성
		// TODO:

		return(true);

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
