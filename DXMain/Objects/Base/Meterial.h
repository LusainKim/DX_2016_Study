#pragma once

class CMaterialColors
{
public:
	CMaterialColors();
	virtual ~CMaterialColors();

public:
	XMCOLOR						m_xmcAmbient;
	XMCOLOR						m_xmcDiffuse;
	XMCOLOR						m_xmcSpecular; //(r,g,b,a=power)
	XMCOLOR						m_xmcEmissive;
};

class CTextureBase
{
public:
	CTextureBase() = default;
	virtual ~CTextureBase() = default;
	
	static ID3D11Buffer				*m_pd3dcbTextureMatrix;

	virtual void SetTexture(int nIndex, ID3D11ShaderResourceView *pd3dsrvTexture) = 0;
	virtual void SetSampler(int nIndex, ID3D11SamplerState *pd3dSamplerState) = 0;
	virtual void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext) = 0;
	virtual void UpdateTextureShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, int nIndex = 0, int nSlot = 0) = 0;
	virtual void UpdateSamplerShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, int nIndex = 0, int nSlot = 0) = 0;
	
	static void CreateShaderVariables(ID3D11Device *pd3dDevice);
	static void ReleaseShaderVariables();
	static void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, XMMATRIX *pd3dxmtxTexture);
	static ID3D11ShaderResourceView *CreateRandomTexture1DSRV(ID3D11Device *pd3dDevice);
	
	/// <summary>
	/// PickRay 위치와 PickRay 방향을 생성하는 함수입니다.
	/// </summary>
	///	<param name = "pd3dDevice"> 객체 생성에 필요한 D3D Factory 입니다. </param>
	///	<param name = "pd2dFactory"> 객체 생성에 필요한 D2D Factory 입니다. </param>
	///	<param name = "ppd3dTexture2D"> 반환되는 Texture2D 객체입니다. </param>
	///	<param name = "ppd2dRenderTarget"> 반환되는 Texture2D의 RenderTarget입니다. 여기에 그리세요. </param>
	///	<param name = "width"> 텍스처의 가로입니다. </param>
	///	<param name = "height"> 텍스처의 세로입니다. </param>
	///	<param name = "format"> DXGI Format입니다. 기본적으로 DXGI_FORMAT_B8G8R8A8_UNORM 이 제공됩니다. </param>
	/// <remarks> 참고 : https://msdn.microsoft.com/ko-kr/library/windows/desktop/dd370966(v=vs.85).aspx </remarks>
	static void Create2DTexture(	  ID3D11Device			*	pd3dDevice
									, ID2D1Factory			*	pd2dFactory
									, ID3D11Texture2D		**	ppd3dTexture2D
									, ID2D1RenderTarget		**	ppd2dRenderTarget
									, UINT						width
									, UINT						height
									, DXGI_FORMAT				format = DXGI_FORMAT_B8G8R8A8_UNORM
	)
	{
		HRESULT hr = S_OK;

		// First, use a Direct3D device to create a 2-D texture. 
		// The texture uses the D3D10_BIND_RENDER_TARGET and D3D10_BIND_SHADER_RESOURCE bind flags, 
		// and it uses the DXGI_FORMAT_B8G8R8A8_UNORM DXGI format, 
		// one of the DXGI formats supported by Direct2D.

		D3D11_TEXTURE2D_DESC texDesc;

		texDesc.ArraySize = 1;
		texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.Format = format;
		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.MipLevels = 1;
		texDesc.MiscFlags = 0;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;

		pd3dDevice->CreateTexture2D(&texDesc, NULL, ppd3dTexture2D);
		
		ID3D11Texture2D* pd3dTexture2D = (*ppd3dTexture2D);

		// Use the texture to obtain a DXGI surface.
		IDXGISurface *pDxgiSurface = NULL;
		hr = pd3dTexture2D->QueryInterface(&pDxgiSurface);

		// Use the surface with the CreateDxgiSurfaceRenderTarget method to obtain a Direct2D render target.

		if (SUCCEEDED(hr))
		{
			// 화면의 해상도를 얻습니다.
			float fdpiX, fdpiY;
			pd2dFactory->GetDesktopDpi(&fdpiX, &fdpiY);
			
			// Create a D2D render target which can draw into our offscreen D3D
			// surface. Given that we use a constant size for the texture.
			D2D1_RENDER_TARGET_PROPERTIES props =
				RenderTargetProperties(	D2D1_RENDER_TARGET_TYPE_DEFAULT
						, PixelFormat(	DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)
						, fdpiX
						, fdpiY
				);

			hr = pd2dFactory->CreateDxgiSurfaceRenderTarget(
				  pDxgiSurface
				, &props
				, ppd2dRenderTarget
			);
		}

	}

};

class CTexture : public CTextureBase
{
public:
	CTexture(int nTextures = 1, int nSamplers = 1, int nTextureStartSlot = 0, int nSamplerStartSlot = 0);
	virtual ~CTexture();

private:

	vector<ID3D11ShaderResourceView*>	m_ppd3dsrvTextures;
	int									m_nTextureStartSlot;

	vector<ID3D11SamplerState*>			m_ppd3dSamplerStates;
	int									m_nSamplerStartSlot;

public:

	virtual void SetTexture(int nIndex, ID3D11ShaderResourceView *pd3dsrvTexture);
	virtual void SetSampler(int nIndex, ID3D11SamplerState *pd3dSamplerState);
	virtual void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void UpdateTextureShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, int nIndex = 0, int nSlot = 0);
	virtual void UpdateSamplerShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, int nIndex = 0, int nSlot = 0);

};

// 2차 추상클래스 : 실제 랜더링하는 재료들을 가진 Concrete class 필요
class CTextureDrawable : public CTextureBase
{
public:

#define Assert_CTextureDrawable(nIndex, nSlot) \
assert(		(nIndex == 0 && nSlot == 0) \
		&& "CTextureRenderable는 무조건 하나의 indext와 slot을 가집니다!")

	CTextureDrawable() = default;
	virtual ~CTextureDrawable();

protected:
	

	ID2D1RenderTarget			*	m_pd2dRenderTarget		{ nullptr }	;
	ID3D11Texture2D				*	m_pd3dTex2dDrawable		{ nullptr }	;
	ID3D11ShaderResourceView	*	m_pd3dsrvTexture		{ nullptr }	;
	IWICImagingFactory			*	m_pwicFactory			{ nullptr }	;

public:

	virtual void SetTexture(int nIndex, ID3D11ShaderResourceView *pd3dsrvTexture);
	virtual void SetSampler(int nIndex, ID3D11SamplerState *pd3dSamplerState);
	virtual void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void UpdateTextureShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, int nIndex = 0, int nSlot = 0);
	virtual void UpdateSamplerShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, int nIndex = 0, int nSlot = 0);

};

class CMaterial
{
public:
	CMaterial(shared_ptr<CMaterialColors> pColors = nullptr);
	virtual ~CMaterial();

public:
	void SetTexture(shared_ptr<CTexture> pTexture);
	void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext);

	std::shared_ptr<CMaterialColors>	m_pColors;
	std::shared_ptr<CTexture>			m_pTexture;
};
