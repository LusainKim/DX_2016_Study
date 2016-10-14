#pragma once

class CObject;

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

	enum class TextureType : byte {
		  none
		, Render
		, File
		, count
	};
	

protected:
	
	TextureType m_texType = TextureType::none;

public:
	
	TextureType GetTextureType() const { return m_texType; }

public:

	CTextureBase() = default;
	virtual ~CTextureBase() = default;
	
	virtual void UpdateTextureShaderVariable(ID3D11DeviceContext *pd3dDeviceContext) = 0;


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
						, PixelFormat(format, D2D1_ALPHA_MODE_PREMULTIPLIED)
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

	static ID3D11Buffer				*m_pd3dcbTextureMatrix;

	vector<ID3D11ShaderResourceView*>	m_ppd3dsrvTextures;
	int									m_nTextureStartSlot;

	vector<ID3D11SamplerState*>			m_ppd3dSamplerStates;
	int									m_nSamplerStartSlot;

public:

	virtual void SetTexture(int nIndex, ID3D11ShaderResourceView *pd3dsrvTexture);
	virtual void SetSampler(int nIndex, ID3D11SamplerState *pd3dSamplerState);
	virtual void UpdateTextureShaderVariable(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void UpdateTextureShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, int nIndex, int nSlot);
	virtual void UpdateSamplerShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, int nIndex, int nSlot);

	static void CreateShaderVariables(ID3D11Device *pd3dDevice);
	static void ReleaseShaderVariables();
	static void UpdateAnimationTextureShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, XMMATRIX *pd3dxmtxTexture);

};

// 2�� �߻�Ŭ���� : ���� �������ϴ� ������ ���� Concrete class �ʿ�
class CTextureDrawable : public CTextureBase
{
public:

	CTextureDrawable(	  IWICImagingFactory* pwicFactory
						, IDWriteFactory *pdwFactory
						, ID3D11Device* pd3dDevice
						, ID2D1Factory* pd2dFactory
						, UINT width
						, UINT height
						, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN
	);
	virtual ~CTextureDrawable();

protected:
	
	ID2D1RenderTarget			*	m_pd2dRenderTarget		{ nullptr }	;
	ID3D11Texture2D				*	m_pd3dTex2dDrawable		{ nullptr }	;

	IDWriteFactory				*	m_pdwFactory			{ nullptr }	;
	IWICImagingFactory			*	m_pwicFactory			{ nullptr }	;

	ID3D11ShaderResourceView	*	m_pd3dsrvTexture		{ nullptr }	;
	ID3D11SamplerState			*	m_pd3dSamplerState		{ nullptr }	;
public:

	virtual void SetSampler(ID3D11SamplerState *pd3dSamplerState);
	virtual void UpdateTextureShaderVariable(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void Render2D(CObject* obj) = 0;
};



class CTextureDrawHP : public CTextureDrawable
{
public:

	CTextureDrawHP(	  IWICImagingFactory* pwicFactory
					, IDWriteFactory *pdwFactory
					, ID3D11Device* pd3dDevice
					, ID2D1Factory* pd2dFactory
					, UINT width
					, UINT height
					
					, DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT
	);
	virtual ~CTextureDrawHP();

protected:

	ID2D1SolidColorBrush	*	m_pd2dsbrFont		{ nullptr };
	ID2D1SolidColorBrush	*	m_pd2dsbrHPBar		{ nullptr }	;
	ID2D1SolidColorBrush	*	m_pd2dsbrHPGage		{ nullptr }	;
	IDWriteTextFormat		*	m_pdwTextFormat		{ nullptr }	;

	D2D_RECT_F					m_rcHPBar;
	D2D_RECT_F					m_rcWriteID;

public:

	virtual void Render2D(CObject* obj);
};

class CMaterial
{
public:
	CMaterial(shared_ptr<CMaterialColors> pColors = nullptr);
	virtual ~CMaterial();

public:
	void SetTexture(shared_ptr<CTexture> pTexture);
	void UpdateTextureShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, CObject* obj);

	std::shared_ptr<CMaterialColors>	m_pColors;
	std::shared_ptr<CTextureBase>		m_pTexture;
};
