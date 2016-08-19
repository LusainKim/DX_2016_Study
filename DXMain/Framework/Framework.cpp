#include "stdafx.h"
#include "HLSL\PositionColor\BindingBufferStructs.h"
#include "Objects\Base\Object.h"
#include "Scene\Title\TitleScene.h"

#include "Framework.h"

CDirectXFramework::CDirectXFramework()
{
}

CDirectXFramework::~CDirectXFramework()
{
	ReleaseObjects();
}

bool CDirectXFramework::Initialize(HINSTANCE hInstance, HWND hWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hWnd;
	GetClientRect(m_hWnd, &m_rcClient);

	// Ÿ�̸� �ʱ�ȭ
	m_LastUpdate_time = chrono::system_clock::now();
	m_current_time = chrono::system_clock::now();

	m_fps = 0.0;
	m_dCumulativefps = 0.0;
	m_nCumulativefpsCount = 0;

	// Ŭ������ ������ ���ν��� ����
	::SetUserDataPtr(m_hWnd, this);

	// ĸ�� ����
#if defined(TITLESTRING)
	lstrcpy(m_CaptionTitle, TITLESTRING);
#else
	GetWindowText(m_hWnd, m_CaptionTitle, TITLE_MAX_LENGTH);
#endif
#if defined(SHOW_CAPTIONFPS)
	lstrcat(m_CaptionTitle, TEXT(" ("));
#endif
	m_TitleLength = lstrlen(m_CaptionTitle);
	SetWindowText(m_hWnd, m_CaptionTitle);

	if (!CreateD3D11Deivce()) return false;

	BuildScene();

	return true;
}

void CDirectXFramework::ReleaseObjects()
{
	// D3D11 Device Resources
	ReleaseD3D11Devices();
	
	// D2D Resource ����
	ReleaseD2DResources();
}

bool CDirectXFramework::CreateD3D11Deivce()
{
	// Swap Chain Description ����ü
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	#pragma region [DXGI_SWAP_CHAIN_DESC �ʱ�ȭ]
	// ����ü ����
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
	// BufferCount : �ĸ� ������ ���� ����
	dxgiSwapChainDesc.BufferCount = 2;
		
	// BufferDesc : �ĸ� ������ ���÷��� ������ ����
	{
		// Width : ������ ���� ũ�⸦ �ȼ��� ����
		dxgiSwapChainDesc.BufferDesc.Width = m_rcClient.right;
		// Height : ������ ���� ũ�⸦ �ȼ��� ����
		dxgiSwapChainDesc.BufferDesc.Height = m_rcClient.bottom;
		// Format : �ĸ� ���� �ȼ� ����
		/// DirectX 11-1(Chap 01)-Device, p.49 ����
		dxgiSwapChainDesc.BufferDesc.Format = m_dxgiFormatCurrentSelect;
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
	//	DXGI_SWAP_EFFECT_DISCARD		(0) : ���� ������ ���
	//	DXGI_SWAP_EFFECT_SEQUENTIAL		(1) : ���� ����
	// DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL	(2) : Flip ���� ����
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	
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
	auto szFeatureLevel = static_cast<UINT>(GetArraySize(d3dFeatureLevels));

	// ���������� ���� ��ġ�� ��� ������ �����ϱ� ���� �����Դϴ�.
	D3D_FEATURE_LEVEL nd3dFeatureLevel = D3D_FEATURE_LEVEL_11_1;

	// Result Handle �Դϴ�. ��ġ�� ���������� ���������� �˻��մϴ�.
	HRESULT hResult = S_OK;

	// �� �÷��״� API �⺻���� �ٸ� �� ä�� ������ ǥ�鿡 ���� ������
	// �߰��մϴ�. Direct2D���� ȣȯ���� ���� �ʿ��մϴ�.
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)

	auto IsSDKLayerAvailable = []() -> bool {
		return SUCCEEDED(D3D11CreateDevice(	  nullptr
											, D3D_DRIVER_TYPE_NULL			// ���� �ϵ���� ��ġ�� ���� �ʿ䰡 �����ϴ�.
											, 0
											, D3D11_CREATE_DEVICE_DEBUG		// SDK ���̾ Ȯ���ϼ���.
											, nullptr						// ��� ��� ������ ����˴ϴ�.
											, 0
											, D3D11_SDK_VERSION				// Windows ����� ���� ��� �׻� �� ���� D3D11_SDK_VERSION���� �����մϴ�.
											, nullptr						// D3D ��ġ ������ ������ �ʿ䰡 �����ϴ�.
											, nullptr						// ��� ������ �� �ʿ䰡 �����ϴ�.
											, nullptr						// D3D ��ġ ���ؽ�Ʈ ������ ������ �ʿ䰡 �����ϴ�.
		));
	};

	// SDK ���̾� ������ Ȯ���ϼ���.
	if (IsSDKLayerAvailable())
	{
		// ������Ʈ�� ����� ���� ���� ��쿡�� �� �÷��װ� �ִ� SDK ���̾ ���� ������� ����Ͻʽÿ�.
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
	}
#endif

	for (D3D_DRIVER_TYPE &p : d3dDriverTypes)
	{
		if (SUCCEEDED(hResult = D3D11CreateDevice(	  NULL						// �⺻ ����͸� ����Ϸ��� nullptr�� �����մϴ�.
													, p							// �ϵ���� �׷��� ����̹��� ����Ͽ� ��ġ�� ����ϴ�.
													, 0							// ����̹��� D3D_DRIVER_TYPE_SOFTWARE�� �ƴ� ��� 0�̾�� �մϴ�.
													, creationFlags				// ����� �� Direct2D ȣȯ�� �÷��׸� �����մϴ�.
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

	// DXGI Device �� �޽��ϴ�.
	IDXGIDevice3 *pdxgiDevice = NULL;
	if (FAILED(hResult = m_pd3dDevice->QueryInterface(__uuidof(IDXGIDevice3), (LPVOID*)&pdxgiDevice)))
	{
		MessageBox(m_hWnd, TEXT("DXGI Device ��ü�� ��ȯ���� ���߽��ϴ�. ���α׷��� �����մϴ�."), TEXT("���α׷� ���� ����"), MB_OK);
		return(false);
	}
	// DXGI Factory �ν��Ͻ��� DXGIFactory���� �޽��ϴ�.
	IDXGIFactory3 *pdxgiFactory = NULL;

	UINT udxgiFlag = 0;
#ifdef _DEBUG
	udxgiFlag |= DXGI_CREATE_FACTORY_DEBUG;
#endif
	if (FAILED(hResult = CreateDXGIFactory2(udxgiFlag, __uuidof(IDXGIFactory3), (LPVOID*)&pdxgiFactory)))
	{
		MessageBox(m_hWnd, TEXT("DXGIFactory������ ��ü ������ �����߽��ϴ�. ���α׷��� �����մϴ�."), TEXT("���α׷� ���� ����"), MB_OK);
		return(false);
	}
	//  SwapChain �� �����մϴ�
	if (FAILED(hResult = pdxgiFactory->CreateSwapChain(pdxgiDevice, &dxgiSwapChainDesc, &m_pdxgiSwapChain)))
	{
		MessageBox(m_hWnd, TEXT("SwapChain �ν��Ͻ� ������ �����߽��ϴ�. ���α׷��� �����մϴ�."), TEXT("���α׷� ���� ����"), MB_OK);
		return(false);
	}

	// Direct2D : Direct2D �ν��Ͻ��� �����մϴ�.
	if (!CreateD2D1Device(pdxgiDevice))
	{
		MessageBox(m_hWnd, TEXT("Direct2D �ν��Ͻ� ������ �����߽��ϴ�. ���α׷��� �����մϴ�."), TEXT("���α׷� ���� ����"), MB_OK);
		return(false);
	}

//	pdxgiDevice->SetMaximumFrameLatency(1);
	// �Ҵ���� COM ��ü�� ��ȯ�մϴ�.
	SafeRelease(pdxgiDevice);
	SafeRelease(pdxgiFactory);

	// render target�� depth-stencil buffer ����
	if(!CreateRenderTargetDepthStencilView())
	{
		MessageBox(m_hWnd, TEXT("RenderTarget�̳� Depth-Stencil ���� ������ �����߽��ϴ�. ���α׷��� �����մϴ�."), TEXT("���α׷� ���� ����"), MB_OK);
		return(false);
	}
	
	// Direct2D : RenderTarget���� 2DBackBuffer�� ȹ���մϴ�.
	CreateD2DBackBuffer();
	return true;
}

bool CDirectXFramework::CreateRenderTargetDepthStencilView()
{


	// Result Handle �Դϴ�. ��ġ�� ���������� ���������� �˻��մϴ�.
	HRESULT hResult = S_OK; 
		
	// render target �� �����ϱ� ���� back buffer �� SwapChain ���� ��û�մϴ�.
	ID3D11Texture2D *pd3dBackBuffer{ nullptr };

	if (FAILED(hResult = m_pdxgiSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pd3dBackBuffer))) return(false);

	// ��ȯ���� ���۸� ����Ͽ� render target view �� �����մϴ�.
	if (FAILED(hResult = m_pd3dDevice->CreateRenderTargetView(pd3dBackBuffer, NULL, &m_pd3dRenderTargetView))) return(false);

	// back buffer �� ��ȯ�մϴ�.
	SafeRelease(pd3dBackBuffer);



	// Rendering �߰� �κ� : Depth-Stencil �� ���� ������ �������� ť�갡 ��µ��� �ʴ´�.
	{
		// depth stencil "texture" �� �����մϴ�.
		D3D11_TEXTURE2D_DESC d3dDepthStencilBufferDesc;
	
		// �޸𸮴� 0���� �ʱ�ȭ�մϴ�.
		ZeroMemory(&d3dDepthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));



		// Width : texture �� �ʺ��Դϴ�.
		d3dDepthStencilBufferDesc.Width = m_rcClient.right;
		// Height : texture �� �����Դϴ�.
		d3dDepthStencilBufferDesc.Height = m_rcClient.bottom;
		// MipLevels : texture �ִ� MipMap Level ��. 
		//				���� ���ø� �ؽ�ó : 1
		//				�ִ� �Ӹ� ���� : 0
		d3dDepthStencilBufferDesc.MipLevels = 1;
		// ArraySize :texture �迭�� texture ����. (�迭�� �ƴϸ� 1)
		d3dDepthStencilBufferDesc.ArraySize = 1;
		// Format : texture �ȼ� ����
		d3dDepthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

		// SampleDesc : ���� ���ø��� ǰ���� ����
		// CheckMultisampleQualityLevels �Լ��� ����Ͽ� ���� ���ø� ���� ���θ� Ȯ���� �ڿ� �� ����
		{

			// Count : �ȼ� �� ���� ����
			//	1  : ���� ���ø��� ���� ����
			//	2~ : �ش� ����ŭ�� ���� ���ø�
			d3dDepthStencilBufferDesc.SampleDesc.Count = 1;
			// Quality : ǰ�� ����
			// 0 : ���� ���ø��� ���� ����
			d3dDepthStencilBufferDesc.SampleDesc.Quality = 0;
		}

		// Usage : texture �� �а� ���� ����� ���� ����
		d3dDepthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		// BindFlags : ���������� �ܰ� ��� ������ ������ ����
		d3dDepthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		// MiscFlags : ���ҽ��� ���� �߰� ���� ����. ������� ������ 0.
		d3dDepthStencilBufferDesc.MiscFlags = 0;
		// CPUAccessFlags : CPU�� ���۸� ����� �� �ִ� ����. ������� ������ 0.
		d3dDepthStencilBufferDesc.CPUAccessFlags = 0;

		//	�⺻ ����-���ٽ� ������ ���� ���۸� �������� �Ѵ�.
		if (FAILED(hResult = m_pd3dDevice->CreateTexture2D(&d3dDepthStencilBufferDesc, NULL, &m_pd3dDepthStencilBuffer))) return(false);

		// Create the depth stencil view
		D3D11_DEPTH_STENCIL_VIEW_DESC d3dDepthStencilViewDesc;
		ZeroMemory(&d3dDepthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		d3dDepthStencilViewDesc.Format = d3dDepthStencilBufferDesc.Format;

		d3dDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		d3dDepthStencilViewDesc.Texture2D.MipSlice = 0;

		if (FAILED(hResult = m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, &d3dDepthStencilViewDesc, &m_pd3dDepthStencilView))) return(false);


	}

	//	DXGI_FRAME_STATISTICS p;
	//	m_pdxgiSwapChain->GetFrameStatistics(&p);


	// ��ü â�� ������� �ϱ� ���� 3D ������ ����Ʈ�� �����մϴ�.

	return true;
}

void CDirectXFramework::ReleaseD3D11Devices()
{
	// Release ��ü
	SafeRelease(m_pd3dDevice);
	SafeRelease(m_pdxgiSwapChain);
	SafeRelease(m_pd3dRenderTargetView);
	SafeRelease(m_pd3dDeviceContext);

	// Rendering �߰� �κ� : Depth-Stencil �� ���� ������ �������� ť�갡 ��µ��� �ʴ´�.
	SafeRelease(m_pd3dDepthStencilBuffer);
	SafeRelease(m_pd3dDepthStencilView);
}






void CDirectXFramework::Render()
{
	// OM�� RenderTarget �缳��
	m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTargetView, m_pd3dDepthStencilView);

	m_pd3dDeviceContext->ClearRenderTargetView(m_pd3dRenderTargetView, FLOAT4 { 0.0f, 0.5f, 0.8f, 1.0f });
	// Rendering �߰� �κ� : Depth-Stencil �� ���� ������ �������� ť�갡 ��µ��� �ʴ´�.
	m_pd3dDeviceContext->ClearDepthStencilView(m_pd3dDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


	if (m_pCurrentScene) m_pCurrentScene->Render3D(m_pd3dDeviceContext);

	Render2D();
	
	m_pdxgiSwapChain->Present(0, 0);
}

inline void CDirectXFramework::Update(float fTimeElapsed)
{
	if (m_pCurrentScene) m_pCurrentScene->AnimateObjects(fTimeElapsed);
}



inline void CDirectXFramework::ReleaseD2DResources()
{
	SafeRelease(m_pd2dDevice);
	SafeRelease(m_pd2dFactory);
	SafeRelease(m_pd2dDeviceContext);
	SafeRelease(m_pdwFactory);
	SafeRelease(m_wicFactory);
	SafeRelease(m_pd2dBmpBackBuffer);
	SafeRelease(m_pd2dStateBlock);
}

bool CDirectXFramework::CreateD2D1Device(IDXGIDevice3 * pdxgiDevice)
{
	if (!pdxgiDevice) return (false);

	HRESULT hResult = S_OK;

	// Direct2D ���ҽ��� �ʱ�ȭ�մϴ�.
	D2D1_FACTORY_OPTIONS options;
	ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));

#if defined(_DEBUG)
	// ������Ʈ�� ����� ���� ���� ��� SDK ���̾ ���� Direct2D ������� ����մϴ�.
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

	// Direct2D ���͸��� �ʱ�ȭ�մϴ�.
	if(FAILED(hResult = D2D1CreateFactory(	  D2D1_FACTORY_TYPE_SINGLE_THREADED
											, __uuidof(ID2D1Factory3)
											, &options
											, reinterpret_cast<LPVOID*>(&m_pd2dFactory)
	))) goto ReleaseDXGI;

	// DirectWrite ���͸��� �ʱ�ȭ�մϴ�.
	if (FAILED(hResult = DWriteCreateFactory(	  DWRITE_FACTORY_TYPE_SHARED
												, __uuidof(IDWriteFactory3)
												, reinterpret_cast<IUnknown**>(&m_pdwFactory)
	))) goto ReleaseDXGI;

	// COM Library�� �ʱ�ȭ�մϴ�.
	// UWP���� ��� �� ���ư����� ���⼱ �� ���ư���.
	if (FAILED(hResult = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED))) goto ReleaseDXGI;

	// WIC(Windows Imaging Component) ���͸��� �ʱ�ȭ�մϴ�.
	if (FAILED(hResult = CoCreateInstance(	  CLSID_WICImagingFactory
											, nullptr
											, CLSCTX_INPROC_SERVER
											, IID_PPV_ARGS(&m_wicFactory)
	))) goto ReleaseDXGI;

	m_pd2dFactory->CreateDevice(pdxgiDevice, &m_pd2dDevice);
	m_pd2dFactory->CreateDrawingStateBlock(&m_pd2dStateBlock);

	// Direct2D DC�� �����մϴ�.
	m_pd2dDevice->CreateDeviceContext(
		  D2D1_DEVICE_CONTEXT_OPTIONS_NONE
		, &m_pd2dDeviceContext
	);

ReleaseDXGI:
	return (!FAILED(hResult));
}

bool CDirectXFramework::CreateD2DBackBuffer()

{
	// ȭ���� �ػ󵵸� ����ϴ�.
	float fdpiX, fdpiY;
	m_pd2dFactory->GetDesktopDpi(&fdpiX, &fdpiY);
	
	HRESULT hResult = S_OK;

	// ���� ü�� �� ���ۿ� ����� Direct2D ���
	// ��Ʈ���� ����� �̸� ���� ������� �����մϴ�.
	D2D1_BITMAP_PROPERTIES1 bitmapProperties =
		BitmapProperties1(	  D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW
				, PixelFormat(m_dxgiFormatCurrentSelect, D2D1_ALPHA_MODE_PREMULTIPLIED)
				, fdpiX
				, fdpiY
		);

	// DXGI ǥ���� ����ϴ�.
	IDXGISurface2 *dxgiBackBuffer;
	if (FAILED(hResult = m_pdxgiSwapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer)))) return false;
	if (FAILED(hResult = m_pd2dDeviceContext->CreateBitmapFromDxgiSurface(	  dxgiBackBuffer
																			, &bitmapProperties
																			, &m_pd2dBmpBackBuffer
	))) return false;

	// ǥ���� �� ����ϸ� ��ȯ�մϴ�.
	SafeRelease(dxgiBackBuffer);

	m_pd2dDeviceContext->SetTarget(m_pd2dBmpBackBuffer);
	m_pd2dDeviceContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

	return true;
}

inline void CDirectXFramework::Render2D()
{
	m_pd2dDeviceContext->SaveDrawingState(m_pd2dStateBlock);

	m_pd2dDeviceContext->BeginDraw();
	{

		if (m_pCurrentScene) m_pCurrentScene->Render2D(m_pd2dDeviceContext);

	}
	m_pd2dDeviceContext->EndDraw();

	m_pd2dDeviceContext->RestoreDrawingState(m_pd2dStateBlock);
}



void CDirectXFramework::BuildScene()
{
	CObject::CreateModelMatrixBuffer(m_pd3dDevice);
	CCamera::CreateShaderVariables(m_pd3dDevice);
	auto pNewScene = make_unique<CTitleScene>();



	pNewScene->BuildObjects(TEXT("Title"), m_hWnd, this);
	pNewScene->BuildObjecsFromD3D11Devices(m_pd3dDevice, m_pd3dDeviceContext);
	pNewScene->BuildObjecsFromD2D1Devices(m_pd2dDevice, m_pd2dDeviceContext, m_pdwFactory);

	m_lstScenes.push_back(move(pNewScene));
	m_pCurrentScene = m_lstScenes.back().get();
}

bool CDirectXFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		break;
	case WM_RBUTTONDOWN:
		break;
	case WM_MOUSEMOVE:
		break;
	case WM_LBUTTONUP:
		break;
	case WM_RBUTTONUP:
		break;
	default:
		break;
	}

	return(false);
}

LRESULT CDirectXFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	// ��ó�� ���� : ���� Scene���� ó���� ������ Ȯ��
	switch (nMessageID)
	{
	case WM_SIZE:
		{
			auto m_nWndClientWidth = static_cast<LONG>(LOWORD(lParam));
			auto m_nWndClientHeight = static_cast<LONG>(HIWORD(lParam));

			if (m_nWndClientWidth < CLIENT_MINIMUM_WIDTH)
				m_nWndClientWidth = CLIENT_MINIMUM_WIDTH;
				
			if (m_nWndClientHeight < CLIENT_MINIMUM_HEIGHT)
				m_nWndClientHeight = CLIENT_MINIMUM_HEIGHT;



			m_rcClient = RECT { 0, 0, m_nWndClientWidth, m_nWndClientHeight };

			m_pd3dDeviceContext->OMSetRenderTargets(0, NULL, NULL);

			// SwapChain�� ������ ��� ���ҽ����� �����ϰ�, 
			SafeRelease(m_pd3dRenderTargetView);
			SafeRelease(m_pd3dDepthStencilBuffer);
			SafeRelease(m_pd3dDepthStencilView);
			m_pd2dDeviceContext->SetTarget(nullptr);
			SafeRelease(m_pd2dBmpBackBuffer);

			// ���� ����
			HRESULT hr = m_pdxgiSwapChain->ResizeBuffers(2	, m_nWndClientWidth
															, m_nWndClientHeight
															, m_dxgiFormatCurrentSelect
															, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
			if (hr != S_OK) {

				if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
				{
					// �߰� ���� : ��ġ ����� ���� �ʿ�
					MessageBox(m_hWnd, TEXT("Device Lost! ���α׷��� �ٽ� ������ �ּ���."), TEXT("ResizeBuffer Fail!"), MB_OK);
				}
				else MessageBox(m_hWnd, TEXT("�� �� ���� ����! ���α׷��� �ٽ� ������ �ּ���."), TEXT("ResizeBuffer Fail!"), MB_OK);
				DestroyWindow(m_hWnd);
			}

			CreateRenderTargetDepthStencilView();
			CreateD2DBackBuffer();

			FrameAdvance();
			if (m_pCurrentScene)
				if (m_pCurrentScene->OnProcessingWindowMessage(hWnd, nMessageID, reinterpret_cast<WPARAM>(m_pd3dDeviceContext), lParam)) break;
				else return DefWindowProc(hWnd, nMessageID, wParam, lParam);
		}
		break;
	default:
		if (m_pCurrentScene)	if (m_pCurrentScene->OnProcessingWindowMessage(hWnd, nMessageID, wParam, lParam)) break;
								else return 0;
	}

	// ��ó�� ���� : Scene���� ó������ �ʰ� ���� �κ��� ó��
	switch (nMessageID)
	{
	case WM_GETMINMAXINFO:

	//	((MINMAXINFO*) lParam)->ptMaxTrackSize.x = 300;
	//	((MINMAXINFO*) lParam)->ptMaxTrackSize.y = 200;
		((MINMAXINFO*) lParam)->ptMinTrackSize.x = CLIENT_MINIMUM_WIDTH;
		((MINMAXINFO*) lParam)->ptMinTrackSize.y = CLIENT_MINIMUM_HEIGHT;

		return 0;

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MOUSEMOVE:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		return 0;
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		return 0;

	default:
		break;
	}

	return DefWindowProc(hWnd, nMessageID, wParam, lParam);
}

bool CDirectXFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		break;
	default:
		break;
	}
	return(false);
}


void CDirectXFramework::FrameAdvance()
{
	// Get tick
	m_timeElapsed = chrono::system_clock::now() - m_current_time;
	if (m_timeElapsed.count() > MAX_FPS)
	{
		m_current_time = chrono::system_clock::now();

		if (m_timeElapsed.count() > 0.0)
			m_fps = 1.0 / m_timeElapsed.count();
	}
	// �ִ� FPS �̸��� �ð��� ����ϸ� ���� ����
	else return;

	// ���� Frame���κ����� �ð�
	auto fTick = static_cast<float>(m_timeElapsed.count());

	// �������� ��������� ������ Ȯ���ϱ� ���� Update �Լ�
	Update(fTick);

	Render();



	// �����ӷ���Ʈ�� ����մϴ�.
#if defined(SHOW_CAPTIONFPS)

	m_dCumulativefps += m_fps;
	m_nCumulativefpsCount++;

	m_UpdateElapsed = chrono::system_clock::now() - m_LastUpdate_time;
	if (m_UpdateElapsed.count() > MAX_UPDATE_FPS)
		m_LastUpdate_time = chrono::system_clock::now();
	else return;

	double resultFps = m_dCumulativefps / static_cast<double>(m_nCumulativefpsCount);

	m_dCumulativefps = 0.0;
	m_nCumulativefpsCount = 0;

	_itow_s(static_cast<int>(resultFps + 0.1), m_CaptionTitle + m_TitleLength, TITLE_MAX_LENGTH - m_TitleLength, 10);
	wcscat_s(m_CaptionTitle + m_TitleLength, TITLE_MAX_LENGTH - m_TitleLength, TEXT(" FPS)"));
	SetWindowText(m_hWnd, m_CaptionTitle);
#endif

}

LRESULT CALLBACK CDirectXFramework::WndProc(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	CDirectXFramework* self = ::GetUserDataPtr<CDirectXFramework*>(hWnd);
	if (!self)
		return ::DefWindowProc(hWnd, nMessageID, wParam, lParam);

	switch (nMessageID)
	{
	case WM_CREATE:
		break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		::BeginPaint(hWnd, &ps);
		::EndPaint(hWnd, &ps);
	}
	break;

	case WM_DESTROY:
		::SetUserDataPtr(hWnd, NULL);
		::PostQuitMessage(0);
		break;

	default:
		return self->OnProcessingWindowMessage(hWnd, nMessageID, wParam, lParam);

	}
	return 0;
}
