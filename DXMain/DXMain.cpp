// DXMain.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
// ��� �÷��� : Windows 10
// ��� �÷��� ���� : 10.0.10586.0 ���� �ۼ�(�������������� �ܰ踦 ������ �մϴ�)

#include "stdafx.h"
#include "DXMain.h"

// D3D11
#include <d3d11_2.h>
#include <dxgi1_3.h>

// D2D1
#include <d2d1_3.h>
#include <dwrite_3.h>
#include <d2d1_2helper.h>
#include <wincodec.h>

// DirectX Math
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

// C++11
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>

using namespace std;
using namespace std::chrono;

using namespace D2D1;
using namespace DirectX;
using namespace DirectX::PackedVector;

template<typename Ty, size_t N>
constexpr size_t GetArraySize(Ty (&)[N]) noexcept
{
	return N;
}


class CDirectXFramework {

private:

	ID3D11Device			*	m_pd3dDevice				{ nullptr }	;
	IDXGISwapChain			*	m_pdxgiSwapChain			{ nullptr }	;

	ID3D11RenderTargetView	*	m_pd3dRenderTargetView		{ nullptr }	;
	ID3D11DeviceContext		*	m_pd3dDeviceContext			{ nullptr }	;

	HINSTANCE					m_hInstance					{ NULL }	;
	HWND						m_hWnd						{ NULL }	;
	RECT						m_rcClient								;

	// Rendering �߰� �κ� : Depth-Stencil �� ���� ������ �������� ť�갡 ��µ��� �ʴ´�.
	ID3D11Texture2D				*m_pd3dDepthStencilBuffer	{ nullptr }	;
	ID3D11DepthStencilView		*m_pd3dDepthStencilView		{ nullptr }	;

	D3D11_VIEWPORT				m_screenViewport			{ NULL }	;
	

public:

	CDirectXFramework() = default;
	~CDirectXFramework()
	{
		// Release ��ü
		if (m_pd3dDevice) m_pd3dDevice->Release();
		if (m_pdxgiSwapChain) m_pdxgiSwapChain->Release();
		if (m_pd3dRenderTargetView) m_pd3dRenderTargetView->Release();
		if (m_pd3dDeviceContext) m_pd3dDeviceContext->Release();

		// Rendering �߰� �κ� : Depth-Stencil �� ���� ������ �������� ť�갡 ��µ��� �ʴ´�.
		if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
		if (m_pd3dDepthStencilView) m_pd3dDepthStencilView->Release();

		// Rendering ���õ�, ���̴� ���� COM ��ü�� ������ ����
		ReleaseShaderResources();

		// D2D Resource ����
		ReleaseD2DResources();
	}

	void Initialize(HINSTANCE hInstance, HWND hWnd)
	{
		m_hInstance = hInstance;
		m_hWnd = hWnd;
		GetClientRect(m_hWnd, &m_rcClient);

		CreateD3D11Deivce();
	}

	bool CreateD3D11Deivce()
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
			dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
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
			return SUCCEEDED(D3D11CreateDevice(nullptr
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

		// �Ҵ���� COM ��ü�� ��ȯ�մϴ�.
		if (pdxgiDevice) pdxgiDevice->Release();
		if (pdxgiFactory) pdxgiFactory->Release();

		// render target�� depth-stencil buffer ����
		if(!CreateRenderTargetView())
		{
			MessageBox(m_hWnd, TEXT("RenderTarget�̳� Depth-Stencil ���� ������ �����߽��ϴ�. ���α׷��� �����մϴ�."), TEXT("���α׷� ���� ����"), MB_OK);
			return(false);
		}


		return true;
	}

	bool CreateRenderTargetView()
	{

		// Result Handle �Դϴ�. ��ġ�� ���������� ���������� �˻��մϴ�.
		HRESULT hResult = S_OK; 
		
		// render target �� �����ϱ� ���� back buffer �� SwapChain ���� ��û�մϴ�.
		ID3D11Texture2D *pd3dBackBuffer{ nullptr };

		if (FAILED(hResult = m_pdxgiSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pd3dBackBuffer))) return(false);

		// ��ȯ���� ���۸� ����Ͽ� render target view �� �����մϴ�.
		if (FAILED(hResult = m_pd3dDevice->CreateRenderTargetView(pd3dBackBuffer, NULL, &m_pd3dRenderTargetView))) return(false);

		// back buffer �� ��ȯ�մϴ�.
		if (pd3dBackBuffer) pd3dBackBuffer->Release();



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
		m_screenViewport = D3D11_VIEWPORT{
			  static_cast<FLOAT>(0.0f)
			, static_cast<FLOAT>(0.0f)
			, static_cast<FLOAT>(m_rcClient.right)
			, static_cast<FLOAT>(m_rcClient.bottom)
			, static_cast<FLOAT>(0.0f)
			, static_cast<FLOAT>(1.0f)
		};

		// RS�� ����Ʈ ����
		m_pd3dDeviceContext->RSSetViewports(1, &m_screenViewport);


		// Rendering Resource ����
		CreateRenderingResource();

		// Direct2D : RenderTarget���� 2DBackBuffer�� ȹ���մϴ�.
		return(CreateD2DBackBuffer());

//		return true;
	}

	std::chrono::system_clock::time_point m_current_time = chrono::system_clock::now();
	std::chrono::duration<double> m_timeElapsed; // �ð��� �󸶳� ������
	double m_fps = 0.f;

	using FLOAT4 = float[4];

	void FrameAdvance()
	{
		// Get tick
		m_timeElapsed = chrono::system_clock::now() - m_current_time;
#if 1
		if (m_timeElapsed.count() > 0.016f)
#else
		if (true)
#endif
		{
			m_current_time = chrono::system_clock::now();

			if (m_timeElapsed.count() > 0.0)
				m_fps = 1.0 / m_timeElapsed.count();
		}
		// �ִ� FPS �̸��� �ð��� ����ϸ� ���� ����
		else return;

		// �������� ��������� ������ Ȯ���ϱ� ���� Update �Լ�
		Update(static_cast<float>(m_timeElapsed.count()));
		Update2D();

		// OM�� RenderTarget �缳��
		m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTargetView, m_pd3dDepthStencilView);

		m_pd3dDeviceContext->ClearRenderTargetView(m_pd3dRenderTargetView, FLOAT4{0.0f, 0.5f, 0.8f, 1.0f});
		// Rendering �߰� �κ� : Depth-Stencil �� ���� ������ �������� ť�갡 ��µ��� �ʴ´�.
		m_pd3dDeviceContext->ClearDepthStencilView(m_pd3dDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		Render();
		Render2D();

		m_pdxgiSwapChain->Present(0, 0);
	}
	


	// Shader Code
private:
	
	vector<byte> ReadCSOFile(LPCTSTR path)
	{
		ifstream inputShaderCSO(path, ios::in | ios::binary);

		if (!inputShaderCSO.is_open())
		{
			MessageBox(m_hWnd, TEXT("Shader Load Fail!"), TEXT("���α׷� ���� ����"), MB_OK);
			return vector<byte>();
		}

		// size check;
		inputShaderCSO.seekg(0, fstream::end);
		size_t szData = inputShaderCSO.tellg();
		inputShaderCSO.seekg(0);

		vector<byte> retval;
		retval.resize(szData);
		
		inputShaderCSO.read(reinterpret_cast<char*>(&(retval[0])), szData);
		inputShaderCSO.close();

		return retval;
	}

	// MVP ��Ʈ������ ������ ���̴��� ������ �� ���Ǵ� ��� �����Դϴ�.
	struct ModelViewProjectionConstantBuffer
	{
		DirectX::XMFLOAT4X4 model;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
	};

	// �������� �����͸� ������ ���̴��� ������ �� ���˴ϴ�.
	struct VertexPositionColor
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 color;
	};
	
	ID3D11Buffer						*	m_vertexBuffer			{ nullptr }	;
	ID3D11Buffer						*	m_indexBuffer			{ nullptr }	;
	ID3D11Buffer						*	m_constantBuffer		{ nullptr }	;

	ID3D11VertexShader					*	m_vertexShader			{ nullptr } ;
	ID3D11PixelShader					*	m_pixelShader			{ nullptr } ;

	ID3D11InputLayout					*	m_inputLayout			{ nullptr }	;
	ID3D11RasterizerState				*	m_pd3dRasterizerState	{ nullptr }	;

	ModelViewProjectionConstantBuffer		m_constantBufferData	{         }	;

	UINT									m_indexCount			{    0    }	;

	void ReleaseShaderResources()
	{
		if (m_vertexBuffer)			m_vertexBuffer->Release();
		if (m_indexBuffer)			m_indexBuffer->Release();
		if (m_constantBuffer)		m_constantBuffer->Release();

		if (m_vertexShader)			m_vertexShader->Release();
		if (m_pixelShader)			m_pixelShader->Release();

		if (m_inputLayout)			m_inputLayout->Release();
		if (m_pd3dRasterizerState)	m_pd3dRasterizerState->Release();
	}

public:

	void CreateRenderingResource()
	{
		XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovRH(
			  // FOV Y �� : Ŭ ���� �ָ����� �� �� �ִ�.
			  60.0f * XM_PI / 180.0f
			  // ��Ⱦ��
			, static_cast<float>(m_rcClient.right / float(m_rcClient.bottom))
			  // �ּ� �Ÿ�
			, 0.01f
			  // �ִ� �Ÿ�
			, 100.0f
		);

		// ���� 4x4 ���
		XMMATRIX orientationMatrix(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
			);

		// projection �� �� �Է�
		XMStoreFloat4x4(
			&m_constantBufferData.projection,
			XMMatrixTranspose(perspectiveMatrix * orientationMatrix)
		);

		// �ü��� y���� ���� �� ���Ͱ� �ִ� ���·� �� (0,-0.1,0)�� ���� (0,0.7,1.5)�� �ֽ��ϴ�.
		static const XMVECTORF32 eye = { 0.0f, 0.7f, 1.5f, 0.0f };
		static const XMVECTORF32 at = { 0.0f, -0.1f, 0.0f, 0.0f };
		static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

		// ���� �� �Է�.
		XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up)));
		XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixRotationY(0.f)));

		CreateShader();
		CreateMesh();

	}

	void CreateShader()
	{
		// ������ hlsl�� cso �ε� �� VS�� IL ���� : ���� ��Ƽ������� ������ �ؾ� �� �κ�
		auto loadVS = ReadCSOFile(TEXT("VertexShader.cso"));
		{
			m_pd3dDevice->CreateVertexShader(
				  &(loadVS[0])
				, loadVS.size()
				, nullptr
				, &m_vertexShader
			);
	
			static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
			m_pd3dDevice->CreateInputLayout(
				  vertexDesc
				, static_cast<UINT>(GetArraySize(vertexDesc))
				, &(loadVS[0])
				, loadVS.size()
				, &m_inputLayout
			);
		}
	
		// ������ hlsl�� cso �ε� �� PS ���� : ���� ��Ƽ������� ������ �ؾ� �� �κ�
		auto loadPS = ReadCSOFile(TEXT("PixelShader.cso"));
		{
			m_pd3dDevice->CreatePixelShader(
				  &(loadPS[0])
				, loadPS.size()
				, nullptr
				, &m_pixelShader
			);
	
		}
		
		// ��� ���� ����. �̿� �ٸ� ������� ����/������ ��� ���� ����� ���� ����. �߰� ���ΰ� �ʿ�
		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);

		m_pd3dDevice->CreateBuffer(
			  &constantBufferDesc
			, nullptr
			, &m_constantBuffer
		);

		// Create RasterizerState
		D3D11_RASTERIZER_DESC d3dRasterizerDesc;
		ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
		d3dRasterizerDesc.CullMode = D3D11_CULL_NONE;
		d3dRasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
		m_pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);

	}

	void CreateMesh()
	{
		
		// �޽� �������� �ε��մϴ�. �� �������� ��ġ �� ���� ������ �ֽ��ϴ�.
		static const VertexPositionColor cubeVertices[] = 
		{
			{XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
			{XMFLOAT3(-0.5f, -0.5f, +0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f)},
			{XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f)},
			{XMFLOAT3(-0.5f,  0.5f, +0.5f), XMFLOAT3(0.0f, 1.0f, 1.0f)},
			{XMFLOAT3( 0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f)},
			{XMFLOAT3( 0.5f, -0.5f, +0.5f), XMFLOAT3(1.0f, 0.0f, 1.0f)},
			{XMFLOAT3( 0.5f,  0.5f, -0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f)},
			{XMFLOAT3( 0.5f,  0.5f, +0.5f), XMFLOAT3(1.0f, 1.0f, 1.0f)},
		};

		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		vertexBufferData.pSysMem = cubeVertices;	// VertexPositionColor * GetArraySize(cubeVertices)
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(cubeVertices), D3D11_BIND_VERTEX_BUFFER);

		m_pd3dDevice->CreateBuffer(
			  &vertexBufferDesc
			, &vertexBufferData
			, &m_vertexBuffer
		);

		// �޽� �ε����� �ε��մϴ�. �� �ε����� 3�� ���ڴ�
		// ȭ�鿡 �������� �ﰢ���� ��Ÿ���ϴ�.
		// ���� ��� 0,2,1�� �ε����� �������� �ǹ��մϴ�.
		// 0, 2 �� 1�� �������� �� �޽��� ù ��° �ﰢ����
		// �������� �ǹ��մϴ�.
		static const unsigned short cubeIndices [] =
		{
			0,2,1, // -x
			1,2,3,

			4,5,6, // +x
			5,7,6,

			0,1,5, // -y
			0,5,4,

			2,6,7, // +y
			2,7,3,

			0,4,6, // -z
			0,6,2,

			1,3,7, // +z
			1,7,5,
		};

		// �޽� �ε��� ����
		m_indexCount = static_cast<UINT>(GetArraySize(cubeIndices));
		// �޽� �ε����� ���� �ε��� ���� ����
		D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
		indexBufferData.pSysMem = cubeIndices;
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(cubeIndices), D3D11_BIND_INDEX_BUFFER);

		m_pd3dDevice->CreateBuffer(
			  &indexBufferDesc
			, &indexBufferData
			, &m_indexBuffer
		);

	}

	void Render()
	{
		// �׷��� ��ġ�� ���� �� �ֵ��� ��� ���۸� �غ��մϴ�.
		m_pd3dDeviceContext->UpdateSubresource(   m_constantBuffer
												, 0
												, NULL
												, &m_constantBufferData
												, 0
												, 0
		);

		// �� �������� VertexPositionColor ����ü�� �� �ν��Ͻ��Դϴ�.
		UINT stride = sizeof(VertexPositionColor);
		UINT offset = 0;

		// ������ ���۸� �����մϴ�.
		m_pd3dDeviceContext->IASetVertexBuffers(	  0
													, 1
													, &m_vertexBuffer
													, &stride
													, &offset
		);

		// �ε��� ���۸� �����մϴ�.
		m_pd3dDeviceContext->IASetIndexBuffer(	  m_indexBuffer
												, DXGI_FORMAT_R16_UINT // �� �ε����� �ϳ��� ��ȣ ���� 16��Ʈ ����(Short)�Դϴ�.
												, 0
		);

		m_pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Input Layout�� �����մϴ�.
		m_pd3dDeviceContext->IASetInputLayout(m_inputLayout);

		// ������ ���̴��� �����մϴ�.
		m_pd3dDeviceContext->VSSetShader(	  m_vertexShader
											, nullptr
											, 0
		);

		// ��� ���۸� �׷��� ��ġ�� �����ϴ�.
		m_pd3dDeviceContext->VSSetConstantBuffers(	  0
													, 1
													, &m_constantBuffer
		);

		// �����Ͷ����� ���¸� �����մϴ�.
		m_pd3dDeviceContext->RSSetState(m_pd3dRasterizerState);

		// �ȼ� ���̴��� �����մϴ�.
		m_pd3dDeviceContext->PSSetShader(	  m_pixelShader
											, nullptr
											, 0
		);

		// ��ü�� �׸��ϴ�.
		m_pd3dDeviceContext->DrawIndexed(	  m_indexCount
											, 0
											, 0
		);

	}



	void Update(float fTimeElapsed)
	{
		static float fTotalTime = 0.f;
		fTotalTime += fTimeElapsed;
		float radiansPerSecond = XMConvertToRadians(45);
		double totalRotation = fTotalTime * radiansPerSecond;
		float radians = static_cast<float>(fmod(totalRotation, XM_2PI));
		Rotate(radians);
	}

	void Rotate(float radians)
	{
		// ������Ʈ�� �� ��Ʈ������ ���̴��� �����ϵ��� �غ��մϴ�.
		XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixRotationY(radians)));
	}



	// 2D code
private:

	ID2D1Device2					*	m_pd2dDevice		{ nullptr }	;
	ID2D1Factory3					*	m_pd2dFactory		{ nullptr }	;
	ID2D1DeviceContext2				*	m_pd2dDeviceContext	{ nullptr }	;
	IDWriteFactory3					*	m_pdwFactory		{ nullptr }	;
	IWICImagingFactory2				*	m_wicFactory		{ nullptr }	;
	
	// 3D SwapChain���� RenderTarget�� ��� ���� BackBuffer �Դϴ�.
	ID2D1Bitmap1					*	m_pd2dBmpBackBuffer { nullptr }	;

	wstring								m_strText			{         }	;
	ID2D1SolidColorBrush			*	m_pd2dsbrText		{ nullptr }	;
	IDWriteTextLayout3				*	m_pdwTextLayout		{ nullptr }	;
	IDWriteTextFormat2				*	m_pdwTextFormat		{ nullptr }	;
	// ���������� Drawing ���¸� �����մϴ�.
	ID2D1DrawingStateBlock1			*	m_pd2dStateBlock	{ nullptr }	;

	void ReleaseD2DResources()
	{
		if (m_pd2dDevice) m_pd2dDevice->Release();
		if (m_pd2dFactory) m_pd2dFactory->Release();
		if (m_pd2dDeviceContext) m_pd2dDeviceContext->Release();
		if (m_pdwFactory) m_pdwFactory->Release();
		if (m_wicFactory) m_wicFactory->Release();
		if (m_pd2dBmpBackBuffer) m_pd2dBmpBackBuffer->Release();
		if (m_pd2dsbrText) m_pd2dsbrText->Release();
		if (m_pdwTextLayout) m_pdwTextLayout->Release();
		if (m_pdwTextFormat) m_pdwTextFormat->Release();
		if (m_pd2dStateBlock) m_pd2dStateBlock->Release();
	}

public:

	bool CreateD2D1Device(IDXGIDevice3* pdxgiDevice)
	{
		if(pdxgiDevice) pdxgiDevice->AddRef();
		else return (false);

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
		CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

		// WIC(Windows Imaging Component) ���͸��� �ʱ�ȭ�մϴ�.
		if (FAILED(hResult = CoCreateInstance(	  CLSID_WICImagingFactory
												, nullptr
												, CLSCTX_INPROC_SERVER
												, IID_PPV_ARGS(&m_wicFactory)
		))) goto ReleaseDXGI;

		m_pd2dFactory->CreateDevice(pdxgiDevice, &m_pd2dDevice);

		// Direct2D DC�� �����մϴ�.
		m_pd2dDevice->CreateDeviceContext(
			  D2D1_DEVICE_CONTEXT_OPTIONS_NONE
			, &m_pd2dDeviceContext
		);

	ReleaseDXGI:
		pdxgiDevice->Release();
		return (!FAILED(hResult));
	}

	bool CreateD2DBackBuffer()
	{
		// ȭ���� �ػ󵵸� ����ϴ�.
		float fdpiX, fdpiY;
		m_pd2dFactory->GetDesktopDpi(&fdpiX, &fdpiY);
		
		HRESULT hResult = S_OK;

		// ���� ü�� �� ���ۿ� ����� Direct2D ���
		// ��Ʈ���� ����� �̸� ���� ������� �����մϴ�.
		D2D1_BITMAP_PROPERTIES1 bitmapProperties =
			BitmapProperties1(	  D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW
					, PixelFormat(DXGI_FORMAT_R16G16B16A16_FLOAT, D2D1_ALPHA_MODE_PREMULTIPLIED)
					, fdpiX
					, fdpiY
			);

		IDXGISurface2 *dxgiBackBuffer;
		if (FAILED(hResult = m_pdxgiSwapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer)))) return false;
		if (FAILED(hResult = m_pd2dDeviceContext->CreateBitmapFromDxgiSurface(	  dxgiBackBuffer
																				, &bitmapProperties
																				, &m_pd2dBmpBackBuffer
		))) return false;

		m_pd2dDeviceContext->SetTarget(m_pd2dBmpBackBuffer);
		m_pd2dDeviceContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

		CreateDwriteDevice();

		return true;
	}

	void CreateDwriteDevice()
	{

		m_pdwFactory->CreateTextFormat(	  L"D2Coding"
										, nullptr
										, DWRITE_FONT_WEIGHT_LIGHT
										, DWRITE_FONT_STYLE_NORMAL
										, DWRITE_FONT_STRETCH_NORMAL
										, 32.0f
										, L"ko-kr"
										, reinterpret_cast<IDWriteTextFormat**>(&m_pdwTextFormat)
		);
		m_pdwTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		m_pd2dFactory->CreateDrawingStateBlock(&m_pd2dStateBlock);
		m_pd2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_pd2dsbrText);
	}

	void Update2D()
	{
		m_strText = L"Test Text";

		if (m_pdwTextLayout) m_pdwTextLayout->Release();

		m_pdwFactory->CreateTextLayout(   m_strText.c_str()
										, m_strText.length()
										, m_pdwTextFormat
										, 240.f
										, 50.f
										, reinterpret_cast<IDWriteTextLayout**>(&m_pdwTextLayout)
		);
	}

	void Render2D() 
	{
	//	m_pd2dDeviceContext->SaveDrawingState(m_pd2dStateBlock);

		m_pd2dDeviceContext->BeginDraw();
		{
			Matrix3x2F screenTranslation { Matrix3x2F::Identity() };
			m_pd2dDeviceContext->SetTransform(screenTranslation);

			m_pdwTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
			m_pd2dDeviceContext->DrawTextLayout(	  Point2F(0.f, 0.f)
													, m_pdwTextLayout
													, m_pd2dsbrText
			);

		}
		m_pd2dDeviceContext->EndDraw();

	//	m_pd2dDeviceContext->RestoreDrawingState(m_pd2dStateBlock);
	}

};



CDirectXFramework framework;

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
    while (true)
    {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			framework.FrameAdvance();
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

   framework.Initialize(hInst, hWnd);

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
		//	HDC hdc = 
				BeginPaint(hWnd, &ps);
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
