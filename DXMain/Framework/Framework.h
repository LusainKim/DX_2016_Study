#pragma once

class CScene;


///	<remarks> 
///	Create �Լ��� ��ü�� �Ҵ� ���θ� �˻����� �ʽ��ϴ�. �Ҵ�� ��ü���� ��� �����Ǵ� �� �Ҵ���� ���� ���¿��� �մϴ�.
/// ��� �Ҵ���� ���� ��ü�� nullptr�� �����ؾ� �մϴ�.
///	</remarks>
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

	DXGI_FORMAT					m_dxgiFormatCurrentSelect { DXGI_FORMAT_R16G16B16A16_FLOAT }	;

public:

	CDirectXFramework();
	
	~CDirectXFramework();

	bool Initialize(HINSTANCE hInstance, HWND hWnd);

	void ReleaseObjects();

	bool CreateD3D11Deivce();

	bool CreateRenderTargetDepthStencilView();

	void ReleaseD3D11Devices();




	void Render();

	void Update(float fTimeElapsed);



	// 2D code
private:

	ID2D1Device2					*	m_pd2dDevice		{ nullptr }	;
	ID2D1Factory3					*	m_pd2dFactory		{ nullptr }	;
	ID2D1DeviceContext2				*	m_pd2dDeviceContext	{ nullptr }	;
	IDWriteFactory3					*	m_pdwFactory		{ nullptr }	;
	IWICImagingFactory2				*	m_wicFactory		{ nullptr }	;
	
	// 3D SwapChain���� RenderTarget�� ��� ���� BackBuffer �Դϴ�.
	ID2D1Bitmap1					*	m_pd2dBmpBackBuffer { nullptr }	;
	
	// ���������� Drawing ���¸� �����մϴ�.
	ID2D1DrawingStateBlock1			*	m_pd2dStateBlock	{ nullptr }	;


	void ReleaseD2DResources();

public:

	bool CreateD2D1Device(IDXGIDevice3* pdxgiDevice);

	bool CreateD2DBackBuffer();

	void Render2D();



// ���� ��ü�� ������ ����մϴ�.
private:

	std::chrono::system_clock::time_point m_current_time;
	std::chrono::duration<double> m_timeElapsed; // �ð��� �󸶳� ������

	double		m_fps = 0.0;

	TCHAR		m_CaptionTitle[TITLE_MAX_LENGTH];
	int			m_TitleLength;

	double		m_dCumulativefps = 0.0;
	int			m_nCumulativefpsCount = 0;

	std::chrono::system_clock::time_point m_LastUpdate_time;
	std::chrono::duration<double> m_UpdateElapsed; // �ð��� �󸶳� ������

	using FLOAT4 = float[4];

	std::list<std::unique_ptr<CScene>>		m_lstScenes;
	CScene								*	m_pCurrentScene	{ nullptr }	;
public:

	// �� ó���� ���� ��� true ��ȯ
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	// �� ó���� ���� ��� true ��ȯ
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	// �� ó���� ���� ��� true ��ȯ
	virtual LRESULT OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildScene();
	void FrameAdvance();

	// Framework ���� ȣ���ϴ� ������ ���ν����Դϴ�.
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};
