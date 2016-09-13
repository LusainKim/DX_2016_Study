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
	/// PickRay ��ġ�� PickRay ������ �����ϴ� �Լ��Դϴ�.
	/// </summary>
	///	<param name = "pd3dDevice"> ��ü ������ �ʿ��� D3D Factory �Դϴ�. </param>
	///	<param name = "pd2dFactory"> ��ü ������ �ʿ��� D2D Factory �Դϴ�. </param>
	///	<param name = "ppd3dTexture2D"> ��ȯ�Ǵ� Texture2D ��ü�Դϴ�. </param>
	///	<param name = "ppd2dRenderTarget"> ��ȯ�Ǵ� Texture2D�� RenderTarget�Դϴ�. ���⿡ �׸�����. </param>
	///	<param name = "width"> �ؽ�ó�� �����Դϴ�. </param>
	///	<param name = "height"> �ؽ�ó�� �����Դϴ�. </param>
	///	<param name = "format"> DXGI Format�Դϴ�. �⺻������ DXGI_FORMAT_B8G8R8A8_UNORM �� �����˴ϴ�. </param>
	/// <remarks> ���� : https://msdn.microsoft.com/ko-kr/library/windows/desktop/dd370966(v=vs.85).aspx </remarks>
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
			// ȭ���� �ػ󵵸� ����ϴ�.
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

// 2�� �߻�Ŭ���� : ���� �������ϴ� ������ ���� Concrete class �ʿ�
class CTextureDrawable : public CTextureBase
{
public:

#define Assert_CTextureDrawable(nIndex, nSlot) \
assert(		(nIndex == 0 && nSlot == 0) \
		&& "CTextureRenderable�� ������ �ϳ��� indext�� slot�� �����ϴ�!")

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
