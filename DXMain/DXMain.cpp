// DXMain.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
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
		// Release ��ü
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
		// Swap Chain Description ����ü
		DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
		#pragma region [DXGI_SWAP_CHAIN_DESC �ʱ�ȭ]
		// ����ü ����
		::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
		// BufferCount : �ĸ� ������ ���� ����
		dxgiSwapChainDesc.BufferCount = 1;
		
		// BufferDesc : �ĸ� ������ ���÷��� ������ ����
		{
			// Width : ������ ���� ũ�⸦ �ȼ��� ����
			dxgiSwapChainDesc.BufferDesc.Width = m_rcClient.right;
			// Height : ������ ���� ũ�⸦ �ȼ��� ����
			dxgiSwapChainDesc.BufferDesc.Height = m_rcClient.bottom;
			// Format : �ĸ� ���� �ȼ� ����
			/// DirectX 11-1(Chap 01)-Device, p.49 ����
			dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			// RefreshRate : ȭ�� ���� ������ Hz ������ ����
			{
				// Denominator : �и�
				dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
				// Numerator : ����
				dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
			}
			// ScanlineOrdering : scan line �׸��� ��� ����(�⺻ 0)
			//	DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED        (0) : ��ĵ ���� ������ �������� ����	
			//	DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE        (1) : ���α׷��ú� ��ĵ ���� ���� ����
			//	DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST  (2) : ���� �ʵ�� �̹��� ����
			//	DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST  (3) : ���� �ʵ�� �̹��� ����
			dxgiSwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		}

		// BufferUsage : �ĸ� ���ۿ� ���� ǥ�� ��� ��İ� CPU�� ���� ��� ����
		//	DXGI_USAGE_SHADER_INPUT				: shader �� �Է����� ���
		//	DXGI_USAGE_RENDER_TARGET_OUTPUT		: render target���� ���
		//	DXGI_USAGE_BACK_BUFFER         		: back buffer �� ���
		//	DXGI_USAGE_SHARED              		: ���� ����
		//	DXGI_USAGE_READ_ONLY           		: �б� ����
		//	DXGI_USAGE_DISCARD_ON_PRESENT  		: DXGI ���� ���� ���(����ڰ� ������� ����)
		//	DXGI_USAGE_UNORDERED_ACCESS    		: ������ȭ ����
		dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		// OutputWindow : ��� �������� �ڵ��� ����(�ݵ�� ��ȿ�ؾ� ��)
		dxgiSwapChainDesc.OutputWindow = m_hWnd;

		// SampleDesc : ���� ���ø��� ǰ���� ����
		// CheckMultisampleQualityLevels �Լ��� ����Ͽ� ���� ���ø� ���� ���θ� Ȯ���� �ڿ� �� ����
		{
			// Count : �ȼ� �� ���� ����
			//	1  : ���� ���ø��� ���� ����
			//	2~ : �ش� ����ŭ�� ���� ���ø�
			dxgiSwapChainDesc.SampleDesc.Count = 1;
			// Quality : ǰ�� ����
			// 0 : ���� ���ø��� ���� ����
			dxgiSwapChainDesc.SampleDesc.Quality = 0;
		}

		// Windowed : ������ ��� �Ǵ� ��ü ȭ�� ��� ���� ~ TRUE  | â ��� 
		//												   ~ FALSE | ��ü ȭ��
		dxgiSwapChainDesc.Windowed = TRUE;

		// Flags : Swap Chain ���ۿ� ���� ���� ������ ����
		//	DXGI_SWAP_CHAIN_FLAG_NONPREROTATED		(1) : ��ü ȭ�� ��忡�� ���� ������ ������ ȭ������ �ű� �� �ڵ� ȸ������ ����
		//	DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH	(2) : ���� ���α׷��� ���÷��� ��带 ������ �� ����
		//	DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE		(4) : ���� ���α׷��� GDI �� ������ �� �� ����. �ĸ� ���ۿ� GetDC() ��� ����
		dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH/*2*/;

		// SwapEffect : Swaping�� ó���ϴ� ���û����� ����(�⺻ : 0)
		//	DXGI_SWAP_EFFECT_DISCARD	(0) : ���� ������ ���
		//	DXGI_SWAP_EFFECT_SEQUENTIAL	(1) : ���� ����
		dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		
		#pragma endregion


		// �����ϴ� �ϵ���� �׷��� ����̹��� �����մϴ�.
		D3D_DRIVER_TYPE d3dDriverTypes[]
		{
			// �ϵ���� ����̹�, Direct3D�� ����� �ִ��� �ϵ����� ���� 
			// �ϵ���������� �������� �ʴ� ����� ����Ʈ��� ���� ����
			  D3D_DRIVER_TYPE_HARDWARE
			// DirectX SDK���� �����ϴ� ������ ����Ʈ���� ����̹�
			// Ư�� ���� 9_1���� Ư�� ���� 10.1���� ����
			// - ������ ����Ʈ���� ����̹�(�����Ͷ�����)�� �ʿ����
			// - ����Ʈ���� ����̹��̸鼭 �׷��� �ϵ������ �ִ� ������ ����
			//   Direct3D 11�� �������� �ʴ� �׷��� �ϵ������� ���
			, D3D_DRIVER_TYPE_WARP
			// ���� ����̹�(��� Direct3D Ư���� �����ϴ� ����Ʈ���� ����̹�)
			// DirectX SDK���� �ӵ����ٴ� ��Ȯ���� ���� ����
			// CPU�� ����Ͽ� �����Ͷ����� ���� 
			// Direct3D 11�� ��� Ư�� �������� �� ����̹��� ����� �� ����
			// ���� ���α׷��� �׽�Ʈ�� ������� ���� �ַ� ���
			, D3D_DRIVER_TYPE_REFERENCE
		};

		// �� �迭�� �� ���� ���α׷����� �����ϴ� DirectX �ϵ���� ��� ���� ������ �����մϴ�.
		// ������ �����ؾ� �մϴ�.
		// ������ ���� ���α׷��� �ʿ��� �ּ� ��� ������ �����ؾ� �մϴ�.
		// ������ �������� ���� ��� ��� ���� ���α׷��� 9.1�� �����ϴ� ������ ���ֵ˴ϴ�.
		D3D_FEATURE_LEVEL d3dFeatureLevels[]
		{
			  D3D_FEATURE_LEVEL_11_1
			, D3D_FEATURE_LEVEL_11_0
			, D3D_FEATURE_LEVEL_10_1
			, D3D_FEATURE_LEVEL_10_0
		};
		// ���� ������ ��� �ϵ���� ��� ���� ���� ���� ���մϴ�.
		size_t szFeatureLevel = GetArraySize(d3dFeatureLevels);

		// ���������� ���� ��ġ�� ��� ������ �����ϱ� ���� �����Դϴ�.
		D3D_FEATURE_LEVEL nd3dFeatureLevel = D3D_FEATURE_LEVEL_11_1;

		// Result Handle �Դϴ�. ��ġ�� ���������� ���������� �˻��մϴ�.
		HRESULT hResult = S_OK;

		for (D3D_DRIVER_TYPE &p : d3dDriverTypes)
		{
			if (SUCCEEDED(hResult = D3D11CreateDevice(	  NULL						// �⺻ ����͸� ����Ϸ��� nullptr�� �����մϴ�.
														, p							// �ϵ���� �׷��� ����̹��� ����Ͽ� ��ġ�� ����ϴ�.
														, 0							// ����̹��� D3D_DRIVER_TYPE_SOFTWARE�� �ƴ� ��� 0�̾�� �մϴ�.
														, 0							// ����� �� Direct2D ȣȯ�� �÷��׸� �����մϴ�.
														, d3dFeatureLevels			// �� ���� ���α׷��� ������ �� �ִ� ��� ���� ����Դϴ�.
														, szFeatureLevel			// �� ����� ũ���Դϴ�.
														, D3D11_SDK_VERSION			// Windows ����� ���� ��� �׻� �� ���� D3D11_SDK_VERSION���� �����մϴ�.
														, &m_pd3dDevice				// ������� Direct3D ��ġ�� ��ȯ�մϴ�.
														, &nd3dFeatureLevel			// ������� ��ġ�� ��� ������ ��ȯ�մϴ�.
														, &m_pd3dDeviceContext		// ��ġ ���� ���ؽ�Ʈ�� ��ȯ�մϴ�.
													)
							)
				)
				break;
		}

		// ��� ���� ������ ����̹����� ������ �����ϸ� ���α׷��� �����մϴ�.
		if (!m_pd3dDevice)
		{
			MessageBox(m_hWnd, TEXT("���� ������ �׷��� ����� �����ϴ�."), TEXT("���α׷� ���� ����"), MB_OK);
			return(false);
		}

		// DXGI Factory �ν��Ͻ��� DXGIFactory���� �޽��ϴ�.
		IDXGIFactory1 *pdxgiFactory = NULL;
		if (FAILED(hResult = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void **)&pdxgiFactory)))
		{
			MessageBox(m_hWnd, TEXT("DXGIFactory������ ��ü ������ �����߽��ϴ�. ���α׷��� �����մϴ�."), TEXT("���α׷� ���� ����"), MB_OK);
			return(false);
		}
		// DXGI Device �� �޽��ϴ�.
		IDXGIDevice *pdxgiDevice = NULL;
		if (FAILED(hResult = m_pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&pdxgiDevice)))
		{
			MessageBox(m_hWnd, TEXT("DXGI Device ��ü�� ��ȯ���� ���߽��ϴ�. ���α׷��� �����մϴ�."), TEXT("���α׷� ���� ����"), MB_OK);
			return(false);
		}
		//  SwapChain �� �����մϴ�
		if (FAILED(hResult = pdxgiFactory->CreateSwapChain(pdxgiDevice, &dxgiSwapChainDesc, &m_pdxgiSwapChain)))
		{
			MessageBox(m_hWnd, TEXT("SwapChain �ν��Ͻ� ������ �����߽��ϴ�. ���α׷��� �����մϴ�."), TEXT("���α׷� ���� ����"), MB_OK);
			return(false);
		}

		// �Ҵ���� COM ��ü�� ��ȯ�մϴ�.
		if (pdxgiDevice) pdxgiDevice->Release();
		if (pdxgiFactory) pdxgiFactory->Release();

		// render target�� depth-stencil buffer ����
		// TODO:

		return(true);

	}

};





#define MAX_LOADSTRING 100

// ���� ����:
HINSTANCE hInst;                                // ���� �ν��Ͻ��Դϴ�.
WCHAR szTitle[MAX_LOADSTRING];                  // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
WCHAR szWindowClass[MAX_LOADSTRING];            // �⺻ â Ŭ���� �̸��Դϴ�.

// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
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

    // TODO: ���⿡ �ڵ带 �Է��մϴ�.

    // ���� ���ڿ��� �ʱ�ȭ�մϴ�.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DXMAIN, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DXMAIN));

    MSG msg;

    // �⺻ �޽��� �����Դϴ�.
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
//  �Լ�: MyRegisterClass()
//
//  ����: â Ŭ������ ����մϴ�.
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
//   �Լ�: InitInstance(HINSTANCE, int)
//
//   ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
//   ����:
//
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

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
//  �Լ�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ����:  �� â�� �޽����� ó���մϴ�.
//
//  WM_COMMAND  - ���� ���α׷� �޴��� ó���մϴ�.
//  WM_PAINT    - �� â�� �׸��ϴ�.
//  WM_DESTROY  - ���� �޽����� �Խ��ϰ� ��ȯ�մϴ�.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // �޴� ������ ���� �м��մϴ�.
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
            // TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�.
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

// ���� ��ȭ ������ �޽��� ó�����Դϴ�.
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
