// DXMain.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
// 대상 플랫폼 : Windows 10
// 대상 플랫폼 버전 : 10.0.10586.0 기준 작성(하위버전에서는 단계를 내려야 합니다)

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

	// Rendering 추가 부분 : Depth-Stencil 을 하지 않으면 정상적인 큐브가 출력되지 않는다.
	ID3D11Texture2D				*m_pd3dDepthStencilBuffer	{ nullptr }	;
	ID3D11DepthStencilView		*m_pd3dDepthStencilView		{ nullptr }	;

	D3D11_VIEWPORT				m_screenViewport			{ NULL }	;
	

public:

	CDirectXFramework() = default;
	~CDirectXFramework()
	{
		// Release 객체
		if (m_pd3dDevice) m_pd3dDevice->Release();
		if (m_pdxgiSwapChain) m_pdxgiSwapChain->Release();
		if (m_pd3dRenderTargetView) m_pd3dRenderTargetView->Release();
		if (m_pd3dDeviceContext) m_pd3dDeviceContext->Release();

		// Rendering 추가 부분 : Depth-Stencil 을 하지 않으면 정상적인 큐브가 출력되지 않는다.
		if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
		if (m_pd3dDepthStencilView) m_pd3dDepthStencilView->Release();

		// Rendering 관련된, 세이더 등의 COM 객체는 별도로 해제
		ReleaseShaderResources();

		// D2D Resource 해제
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
		// Swap Chain Description 구조체
		DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
		#pragma region [DXGI_SWAP_CHAIN_DESC 초기화]
		// 구조체 비우기
		::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
		// BufferCount : 후면 버퍼의 수를 지정
		dxgiSwapChainDesc.BufferCount = 2;
		
		// BufferDesc : 후면 버퍼의 디스플레이 형식을 지정
		{
			// Width : 버퍼의 가로 크기를 픽셀로 지정
			dxgiSwapChainDesc.BufferDesc.Width = m_rcClient.right;
			// Height : 버퍼의 세로 크기를 픽셀로 지정
			dxgiSwapChainDesc.BufferDesc.Height = m_rcClient.bottom;
			// Format : 후면 버퍼 픽셀 형식
			/// DirectX 11-1(Chap 01)-Device, p.49 참조
			dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
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
		//	DXGI_SWAP_EFFECT_DISCARD		(0) : 버퍼 내용을 폐기
		//	DXGI_SWAP_EFFECT_SEQUENTIAL		(1) : 순차 복사
		// DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL	(2) : Flip 순차 복사
		dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		
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
		auto szFeatureLevel = static_cast<UINT>(GetArraySize(d3dFeatureLevels));

		// 최종적으로 사용될 장치의 기능 수준을 저장하기 위한 변수입니다.
		D3D_FEATURE_LEVEL nd3dFeatureLevel = D3D_FEATURE_LEVEL_11_1;

		// Result Handle 입니다. 장치가 성공적으로 생성도는지 검사합니다.
		HRESULT hResult = S_OK;

		// 이 플래그는 API 기본값과 다른 색 채널 순서의 표면에 대한 지원을
		// 추가합니다. Direct2D와의 호환성을 위해 필요합니다.
		UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

	#if defined(_DEBUG)

		auto IsSDKLayerAvailable = []() -> bool {
			return SUCCEEDED(D3D11CreateDevice(nullptr
				, D3D_DRIVER_TYPE_NULL			// 실제 하드웨어 장치를 만들 필요가 없습니다.
				, 0
				, D3D11_CREATE_DEVICE_DEBUG		// SDK 레이어를 확인하세요.
				, nullptr						// 모든 기능 수준이 적용됩니다.
				, 0
				, D3D11_SDK_VERSION				// Windows 스토어 앱의 경우 항상 이 값을 D3D11_SDK_VERSION으로 설정합니다.
				, nullptr						// D3D 장치 참조를 보관할 필요가 없습니다.
				, nullptr						// 기능 수준을 알 필요가 없습니다.
				, nullptr						// D3D 장치 컨텍스트 참조를 보관할 필요가 없습니다.
			));
		};

		// SDK 레이어 지원을 확인하세요.
		if (IsSDKLayerAvailable())
		{
			// 프로젝트가 디버그 빌드 중인 경우에는 이 플래그가 있는 SDK 레이어를 통해 디버깅을 사용하십시오.
			creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
		}
	#endif

		for (D3D_DRIVER_TYPE &p : d3dDriverTypes)
		{
			if (SUCCEEDED(hResult = D3D11CreateDevice(	  NULL						// 기본 어댑터를 사용하려면 nullptr을 지정합니다.
														, p							// 하드웨어 그래픽 드라이버를 사용하여 장치를 만듭니다.
														, 0							// 드라이버가 D3D_DRIVER_TYPE_SOFTWARE가 아닌 경우 0이어야 합니다.
														, creationFlags				// 디버그 및 Direct2D 호환성 플래그를 설정합니다.
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

		// DXGI Device 를 받습니다.
		IDXGIDevice3 *pdxgiDevice = NULL;
		if (FAILED(hResult = m_pd3dDevice->QueryInterface(__uuidof(IDXGIDevice3), (LPVOID*)&pdxgiDevice)))
		{
			MessageBox(m_hWnd, TEXT("DXGI Device 객체를 반환받지 못했습니다. 프로그램을 종료합니다."), TEXT("프로그램 구동 실패"), MB_OK);
			return(false);
		}
		// DXGI Factory 인스턴스를 DXGIFactory에서 받습니다.
		IDXGIFactory3 *pdxgiFactory = NULL;

		UINT udxgiFlag = 0;
#ifdef _DEBUG
		udxgiFlag |= DXGI_CREATE_FACTORY_DEBUG;
#endif
		if (FAILED(hResult = CreateDXGIFactory2(udxgiFlag, __uuidof(IDXGIFactory3), (LPVOID*)&pdxgiFactory)))
		{
			MessageBox(m_hWnd, TEXT("DXGIFactory에서의 객체 생성이 실패했습니다. 프로그램을 종료합니다."), TEXT("프로그램 구동 실패"), MB_OK);
			return(false);
		}
		//  SwapChain 을 생성합니다
		if (FAILED(hResult = pdxgiFactory->CreateSwapChain(pdxgiDevice, &dxgiSwapChainDesc, &m_pdxgiSwapChain)))
		{
			MessageBox(m_hWnd, TEXT("SwapChain 인스턴스 생성이 실패했습니다. 프로그램을 종료합니다."), TEXT("프로그램 구동 실패"), MB_OK);
			return(false);
		}

		// Direct2D : Direct2D 인스턴스를 생성합니다.
		if (!CreateD2D1Device(pdxgiDevice))
		{
			MessageBox(m_hWnd, TEXT("Direct2D 인스턴스 생성이 실패했습니다. 프로그램을 종료합니다."), TEXT("프로그램 구동 실패"), MB_OK);
			return(false);
		}

		// 할당받은 COM 객체를 반환합니다.
		if (pdxgiDevice) pdxgiDevice->Release();
		if (pdxgiFactory) pdxgiFactory->Release();

		// render target과 depth-stencil buffer 생성
		if(!CreateRenderTargetView())
		{
			MessageBox(m_hWnd, TEXT("RenderTarget이나 Depth-Stencil 버퍼 생성이 실패했습니다. 프로그램을 종료합니다."), TEXT("프로그램 구동 실패"), MB_OK);
			return(false);
		}


		return true;
	}

	bool CreateRenderTargetView()
	{

		// Result Handle 입니다. 장치가 성공적으로 생성도는지 검사합니다.
		HRESULT hResult = S_OK; 
		
		// render target 을 생성하기 위한 back buffer 를 SwapChain 에게 요청합니다.
		ID3D11Texture2D *pd3dBackBuffer{ nullptr };

		if (FAILED(hResult = m_pdxgiSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pd3dBackBuffer))) return(false);

		// 반환받은 버퍼를 사용하여 render target view 를 생성합니다.
		if (FAILED(hResult = m_pd3dDevice->CreateRenderTargetView(pd3dBackBuffer, NULL, &m_pd3dRenderTargetView))) return(false);

		// back buffer 를 반환합니다.
		if (pd3dBackBuffer) pd3dBackBuffer->Release();



		// Rendering 추가 부분 : Depth-Stencil 을 하지 않으면 정상적인 큐브가 출력되지 않는다.
		{
			// depth stencil "texture" 를 생성합니다.
			D3D11_TEXTURE2D_DESC d3dDepthStencilBufferDesc;
	
			// 메모리는 0으로 초기화합니다.
			ZeroMemory(&d3dDepthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));



			// Width : texture 의 너비입니다.
			d3dDepthStencilBufferDesc.Width = m_rcClient.right;
			// Height : texture 의 높이입니다.
			d3dDepthStencilBufferDesc.Height = m_rcClient.bottom;
			// MipLevels : texture 최대 MipMap Level 수. 
			//				다중 샘플링 텍스처 : 1
			//				최대 밉맵 레벨 : 0
			d3dDepthStencilBufferDesc.MipLevels = 1;
			// ArraySize :texture 배열의 texture 개수. (배열이 아니면 1)
			d3dDepthStencilBufferDesc.ArraySize = 1;
			// Format : texture 픽셀 형식
			d3dDepthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

			// SampleDesc : 다중 샘플링의 품질을 지정
			// CheckMultisampleQualityLevels 함수를 사용하여 다중 샘플링 가능 여부를 확인한 뒤에 값 변경
			{

				// Count : 픽셀 당 샘플 개수
				//	1  : 다중 샘플링을 하지 않음
				//	2~ : 해당 수만큼의 다중 샘플링
				d3dDepthStencilBufferDesc.SampleDesc.Count = 1;
				// Quality : 품질 레벨
				// 0 : 다중 샘플링을 하지 않음
				d3dDepthStencilBufferDesc.SampleDesc.Quality = 0;
			}

			// Usage : texture 를 읽고 쓰는 방법에 대한 정의
			d3dDepthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			// BindFlags : 파이프라인 단계 어디에 연결할 것인지 정의
			d3dDepthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			// MiscFlags : 리소스에 대한 추가 선택 사항. 사용하지 않으면 0.
			d3dDepthStencilBufferDesc.MiscFlags = 0;
			// CPUAccessFlags : CPU가 버퍼를 사용할 수 있는 유형. 사용하지 않으면 0.
			d3dDepthStencilBufferDesc.CPUAccessFlags = 0;

			//	기본 깊이-스텐실 버퍼의 깊이 버퍼를 읽으려고 한다.
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



		// 전체 창을 대상으로 하기 위한 3D 렌더링 뷰포트를 설정합니다.
		m_screenViewport = D3D11_VIEWPORT{
			  static_cast<FLOAT>(0.0f)
			, static_cast<FLOAT>(0.0f)
			, static_cast<FLOAT>(m_rcClient.right)
			, static_cast<FLOAT>(m_rcClient.bottom)
			, static_cast<FLOAT>(0.0f)
			, static_cast<FLOAT>(1.0f)
		};

		// RS에 뷰포트 연결
		m_pd3dDeviceContext->RSSetViewports(1, &m_screenViewport);


		// Rendering Resource 생성
		CreateRenderingResource();

		// Direct2D : RenderTarget에서 2DBackBuffer를 획득합니다.
		return(CreateD2DBackBuffer());

//		return true;
	}

	std::chrono::system_clock::time_point m_current_time = chrono::system_clock::now();
	std::chrono::duration<double> m_timeElapsed; // 시간이 얼마나 지났나
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
		// 최대 FPS 미만의 시간이 경과하면 진행 생략
		else return;

		// 지속적인 상수버퍼의 갱신을 확인하기 위한 Update 함수
		Update(static_cast<float>(m_timeElapsed.count()));
		Update2D();

		// OM에 RenderTarget 재설정
		m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTargetView, m_pd3dDepthStencilView);

		m_pd3dDeviceContext->ClearRenderTargetView(m_pd3dRenderTargetView, FLOAT4{0.0f, 0.5f, 0.8f, 1.0f});
		// Rendering 추가 부분 : Depth-Stencil 을 하지 않으면 정상적인 큐브가 출력되지 않는다.
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
			MessageBox(m_hWnd, TEXT("Shader Load Fail!"), TEXT("프로그램 구동 실패"), MB_OK);
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

	// MVP 매트릭스를 꼭짓점 셰이더로 보내는 데 사용되는 상수 버퍼입니다.
	struct ModelViewProjectionConstantBuffer
	{
		DirectX::XMFLOAT4X4 model;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
	};

	// 꼭짓점별 데이터를 꼭짓점 셰이더로 보내는 데 사용됩니다.
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
			  // FOV Y 값 : 클 수록 멀리까지 볼 수 있다.
			  60.0f * XM_PI / 180.0f
			  // 종횡비
			, static_cast<float>(m_rcClient.right / float(m_rcClient.bottom))
			  // 최소 거리
			, 0.01f
			  // 최대 거리
			, 100.0f
		);

		// 단위 4x4 행렬
		XMMATRIX orientationMatrix(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
			);

		// projection 에 값 입력
		XMStoreFloat4x4(
			&m_constantBufferData.projection,
			XMMatrixTranspose(perspectiveMatrix * orientationMatrix)
		);

		// 시선은 y축을 따라 업 벡터가 있는 상태로 점 (0,-0.1,0)를 보며 (0,0.7,1.5)에 있습니다.
		static const XMVECTORF32 eye = { 0.0f, 0.7f, 1.5f, 0.0f };
		static const XMVECTORF32 at = { 0.0f, -0.1f, 0.0f, 0.0f };
		static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

		// 각자 각 입력.
		XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up)));
		XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixRotationY(0.f)));

		CreateShader();
		CreateMesh();

	}

	void CreateShader()
	{
		// 번역된 hlsl인 cso 로딩 후 VS와 IL 생성 : 추후 멀티스레드로 구현을 해야 할 부분
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
	
		// 번역된 hlsl인 cso 로딩 후 PS 생성 : 추후 멀티스레드로 구현을 해야 할 부분
		auto loadPS = ReadCSOFile(TEXT("PixelShader.cso"));
		{
			m_pd3dDevice->CreatePixelShader(
				  &(loadPS[0])
				, loadPS.size()
				, nullptr
				, &m_pixelShader
			);
	
		}
		
		// 상수 버퍼 생성. 이와 다른 방식으로 선언/구현할 경우 정상 출력이 되지 않음. 추가 공부가 필요
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
		
		// 메시 꼭짓점을 로드합니다. 각 꼭짓점은 위치 및 색을 가지고 있습니다.
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

		// 메시 인덱스를 로드합니다. 각 인덱스의 3개 숫자는
		// 화면에 렌더링할 삼각형을 나타냅니다.
		// 예를 들어 0,2,1은 인덱스로 꼭짓점을 의미합니다.
		// 0, 2 및 1인 꼭짓점이 이 메시의 첫 번째 삼각형을
		// 구성함을 의미합니다.
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

		// 메시 인덱스 개수
		m_indexCount = static_cast<UINT>(GetArraySize(cubeIndices));
		// 메시 인덱스를 위한 인덱스 버퍼 생성
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
		// 그래픽 장치로 보낼 수 있도록 상수 버퍼를 준비합니다.
		m_pd3dDeviceContext->UpdateSubresource(   m_constantBuffer
												, 0
												, NULL
												, &m_constantBufferData
												, 0
												, 0
		);

		// 각 꼭짓점은 VertexPositionColor 구조체의 한 인스턴스입니다.
		UINT stride = sizeof(VertexPositionColor);
		UINT offset = 0;

		// 꼭지점 버퍼를 연결합니다.
		m_pd3dDeviceContext->IASetVertexBuffers(	  0
													, 1
													, &m_vertexBuffer
													, &stride
													, &offset
		);

		// 인덱스 버퍼를 연결합니다.
		m_pd3dDeviceContext->IASetIndexBuffer(	  m_indexBuffer
												, DXGI_FORMAT_R16_UINT // 각 인덱스는 하나의 부호 없는 16비트 정수(Short)입니다.
												, 0
		);

		m_pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Input Layout을 연결합니다.
		m_pd3dDeviceContext->IASetInputLayout(m_inputLayout);

		// 꼭짓점 셰이더를 연결합니다.
		m_pd3dDeviceContext->VSSetShader(	  m_vertexShader
											, nullptr
											, 0
		);

		// 상수 버퍼를 그래픽 장치에 보냅니다.
		m_pd3dDeviceContext->VSSetConstantBuffers(	  0
													, 1
													, &m_constantBuffer
		);

		// 레스터라이저 상태를 연결합니다.
		m_pd3dDeviceContext->RSSetState(m_pd3dRasterizerState);

		// 픽셀 셰이더를 연결합니다.
		m_pd3dDeviceContext->PSSetShader(	  m_pixelShader
											, nullptr
											, 0
		);

		// 개체를 그립니다.
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
		// 업데이트된 모델 매트릭스를 셰이더에 전달하도록 준비합니다.
		XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixRotationY(radians)));
	}



	// 2D code
private:

	ID2D1Device2					*	m_pd2dDevice		{ nullptr }	;
	ID2D1Factory3					*	m_pd2dFactory		{ nullptr }	;
	ID2D1DeviceContext2				*	m_pd2dDeviceContext	{ nullptr }	;
	IDWriteFactory3					*	m_pdwFactory		{ nullptr }	;
	IWICImagingFactory2				*	m_wicFactory		{ nullptr }	;
	
	// 3D SwapChain에서 RenderTarget을 얻기 위한 BackBuffer 입니다.
	ID2D1Bitmap1					*	m_pd2dBmpBackBuffer { nullptr }	;

	wstring								m_strText			{         }	;
	ID2D1SolidColorBrush			*	m_pd2dsbrText		{ nullptr }	;
	IDWriteTextLayout3				*	m_pdwTextLayout		{ nullptr }	;
	IDWriteTextFormat2				*	m_pdwTextFormat		{ nullptr }	;
	// 이전까지의 Drawing 상태를 저장합니다.
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

		// Direct2D 리소스를 초기화합니다.
		D2D1_FACTORY_OPTIONS options;
		ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));

	#if defined(_DEBUG)
		// 프로젝트가 디버그 빌드 중인 경우 SDK 레이어를 통해 Direct2D 디버깅을 사용합니다.
		options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
	#endif

		// Direct2D 팩터리를 초기화합니다.
		if(FAILED(hResult = D2D1CreateFactory(	  D2D1_FACTORY_TYPE_SINGLE_THREADED
												, __uuidof(ID2D1Factory3)
												, &options
												, reinterpret_cast<LPVOID*>(&m_pd2dFactory)
		))) goto ReleaseDXGI;

		// DirectWrite 팩터리를 초기화합니다.
		if (FAILED(hResult = DWriteCreateFactory(	  DWRITE_FACTORY_TYPE_SHARED
													, __uuidof(IDWriteFactory3)
													, reinterpret_cast<IUnknown**>(&m_pdwFactory)
		))) goto ReleaseDXGI;

		// COM Library를 초기화합니다.
		CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

		// WIC(Windows Imaging Component) 팩터리를 초기화합니다.
		if (FAILED(hResult = CoCreateInstance(	  CLSID_WICImagingFactory
												, nullptr
												, CLSCTX_INPROC_SERVER
												, IID_PPV_ARGS(&m_wicFactory)
		))) goto ReleaseDXGI;

		m_pd2dFactory->CreateDevice(pdxgiDevice, &m_pd2dDevice);

		// Direct2D DC를 생성합니다.
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
		// 화면의 해상도를 얻습니다.
		float fdpiX, fdpiY;
		m_pd2dFactory->GetDesktopDpi(&fdpiX, &fdpiY);
		
		HRESULT hResult = S_OK;

		// 스왑 체인 백 버퍼에 연결된 Direct2D 대상
		// 비트맵을 만들고 이를 현재 대상으로 설정합니다.
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

   framework.Initialize(hInst, hWnd);

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
		//	HDC hdc = 
				BeginPaint(hWnd, &ps);
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
