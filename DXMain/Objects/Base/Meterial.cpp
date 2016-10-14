#include "stdafx.h"
#include "Meterial.h"
#include "Object.h"

using cbTextureIndex = int[4];

///////////////////////////////////////////////////////////////////////////
//
CMaterialColors::CMaterialColors()
{
	m_xmcAmbient = XMCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_xmcDiffuse = XMCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_xmcSpecular = XMCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_xmcEmissive = XMCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
}

CMaterialColors::~CMaterialColors()
{
}



///////////////////////////////////////////////////////////////////////////
//
ID3D11ShaderResourceView *CTextureBase::CreateRandomTexture1DSRV(ID3D11Device *pd3dDevice)
{
	auto RANDOMCOLOR = [] (float min, float max)
	{ return static_cast<float>(rand() / static_cast<float>(RAND_MAX)) * (max - min) + min; };

	XMCOLOR RV[1024] {};
	for (int i = 0; i < 1024; i++) RV[i] = XMCOLOR(RANDOMCOLOR(-1.0, 1.0f), RANDOMCOLOR(-1.0, 1.0f), RANDOMCOLOR(-1.0, 1.0f), RANDOMCOLOR(-1.0, 1.0f));
	D3D11_SUBRESOURCE_DATA d3dSubresourceData;
	d3dSubresourceData.pSysMem = RV;
	d3dSubresourceData.SysMemPitch = sizeof(XMCOLOR) * 1024;
	d3dSubresourceData.SysMemSlicePitch = 0;
	D3D11_TEXTURE1D_DESC d3dTextureDesc;
	ZeroMemory(&d3dTextureDesc, sizeof(D3D11_TEXTURE1D_DESC));
	d3dTextureDesc.Width = 1024;
	d3dTextureDesc.MipLevels = 1;
	//포맷을 RGBA
	d3dTextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	d3dTextureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	d3dTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	d3dTextureDesc.ArraySize = 1;
	ID3D11Texture1D *pd3dTexture = nullptr;
	// Texture1D를 제작
	pd3dDevice->CreateTexture1D(&d3dTextureDesc, &d3dSubresourceData, &pd3dTexture);

	if (!pd3dTexture) return nullptr;

	ID3D11ShaderResourceView *pd3dsrvTexture;
	//쉐이더 리소스 뷰 제작
	pd3dDevice->CreateShaderResourceView(pd3dTexture, NULL, &pd3dsrvTexture);
	pd3dTexture->Release();
	return(pd3dsrvTexture);
}



///////////////////////////////////////////////////////////////////////////
//
ID3D11Buffer *CTexture::m_pd3dcbTextureMatrix = nullptr;

CTexture::CTexture(int nTextures, int nSamplers, int nTextureStartSlot, int nSamplerStartSlot)
{
	m_texType = TextureType::File;

	m_ppd3dsrvTextures.resize(nTextures, nullptr);
	m_nTextureStartSlot = nTextureStartSlot;

	m_ppd3dSamplerStates.resize(nSamplers, nullptr);
	m_nSamplerStartSlot = nSamplerStartSlot;
}

CTexture::~CTexture()
{
	for (auto p : m_ppd3dsrvTextures) SafeRelease(p);
	for (auto p : m_ppd3dSamplerStates) SafeRelease(p);
}

// Texture 연결
void CTexture::SetTexture(int nIndex, ID3D11ShaderResourceView *pd3dsrvTexture)
{
	assert(nIndex < m_ppd3dsrvTextures.size() && "Index가 허용 범위를 넘었습니다!");
	
	if (pd3dsrvTexture) pd3dsrvTexture->AddRef();
	
	if (nIndex == m_ppd3dsrvTextures.size()) { m_ppd3dsrvTextures.push_back(pd3dsrvTexture); return; }

	if (m_ppd3dsrvTextures[nIndex]) m_ppd3dsrvTextures[nIndex]->Release();
	m_ppd3dsrvTextures[nIndex] = pd3dsrvTexture;
}

// Sampler에 연결
void CTexture::SetSampler(int nIndex, ID3D11SamplerState *pd3dSamplerState)
{
	assert(nIndex < m_ppd3dSamplerStates.size() && "Index가 허용 범위를 넘었습니다!");

	if (pd3dSamplerState) pd3dSamplerState->AddRef();

	if (nIndex == m_ppd3dSamplerStates.size()) { m_ppd3dSamplerStates.push_back(pd3dSamplerState); return; }

	if (m_ppd3dSamplerStates[nIndex]) m_ppd3dSamplerStates[nIndex]->Release();
	m_ppd3dSamplerStates[nIndex] = pd3dSamplerState;
}

// 모든 SRV와 Sampler를 연결
void CTexture::UpdateTextureShaderVariable(ID3D11DeviceContext *pd3dDeviceContext)
{
	pd3dDeviceContext->PSSetShaderResources(	  m_nTextureStartSlot
												, static_cast<UINT>(m_ppd3dsrvTextures.size())
												, &(m_ppd3dsrvTextures[0]));

	pd3dDeviceContext->PSSetSamplers(			  m_nSamplerStartSlot
												, static_cast<UINT>(m_ppd3dSamplerStates.size())
												, &(m_ppd3dSamplerStates[0]));
}

// SRV 하나 연결
void CTexture::UpdateTextureShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, int nIndex, int nSlot)
{
	pd3dDeviceContext->PSSetShaderResources(nSlot, 1, &m_ppd3dsrvTextures[nIndex]);
}

// Sampler 하나 연결
void CTexture::UpdateSamplerShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, int nIndex, int nSlot)
{
	pd3dDeviceContext->PSSetSamplers(nSlot, 1, &m_ppd3dSamplerStates[nIndex]);
}

// for animation texture
void CTexture::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	CD3D11_BUFFER_DESC d3dBufferDesc(sizeof(XMMATRIX), D3D11_BIND_CONSTANT_BUFFER);
	pd3dDevice->CreateBuffer(&d3dBufferDesc, NULL, &m_pd3dcbTextureMatrix);
}

// for animation texture
void CTexture::ReleaseShaderVariables()
{
	if (m_pd3dcbTextureMatrix) m_pd3dcbTextureMatrix->Release();
}

// for animation texture
void CTexture::UpdateAnimationTextureShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, XMMATRIX *pd3dxmtxTexture)
{
	// 그래픽 장치로 보낼 수 있도록 상수 버퍼를 준비합니다.
	pd3dDeviceContext->UpdateSubresource(	  m_pd3dcbTextureMatrix
											, 0
											, NULL
											, pd3dxmtxTexture
											, 0
											, 0
	);

	auto VS_CB_SLOT_TEXTURE_MATRIX = 2;
	pd3dDeviceContext->VSSetConstantBuffers(VS_CB_SLOT_TEXTURE_MATRIX, 1, &m_pd3dcbTextureMatrix);
}

///////////////////////////////////////////////////////////////////////////
//
CTextureDrawable::CTextureDrawable(IWICImagingFactory* pwicFactory, IDWriteFactory *pdwFactory, ID3D11Device* pd3dDevice, ID2D1Factory* pd2dDevice, UINT width, UINT height, DXGI_FORMAT format)
{
	m_texType = TextureType::Render;
	
	m_pwicFactory = pwicFactory;
	if (m_pwicFactory) m_pwicFactory->AddRef();

	m_pdwFactory = pdwFactory;
	if (m_pdwFactory) m_pdwFactory->AddRef();

	Create2DTexture(pd3dDevice, pd2dDevice, &m_pd3dTex2dDrawable, &m_pd2dRenderTarget, width, height, format);
	
	D3D11_SAMPLER_DESC d3dSamplerDesc;
	ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = 0;

	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &m_pd3dSamplerState);
	pd3dDevice->CreateShaderResourceView(m_pd3dTex2dDrawable, NULL, &m_pd3dsrvTexture);
}

CTextureDrawable::~CTextureDrawable()
{
	SafeRelease(m_pd2dRenderTarget);
	SafeRelease(m_pd3dTex2dDrawable);
	SafeRelease(m_pd3dsrvTexture);
	SafeRelease(m_pwicFactory);
	SafeRelease(m_pdwFactory);
	SafeRelease(m_pd3dSamplerState);
}

void CTextureDrawable::SetSampler(ID3D11SamplerState * pd3dSamplerState)
{
	SafeRelease(m_pd3dSamplerState);
	m_pd3dSamplerState = pd3dSamplerState;
	pd3dSamplerState->AddRef();
}

void CTextureDrawable::UpdateTextureShaderVariable(ID3D11DeviceContext * pd3dDeviceContext)
{
	pd3dDeviceContext->PSSetShaderResources(	  10
												, 1
												, &m_pd3dsrvTexture);

	pd3dDeviceContext->PSSetSamplers(			  10
												, 1
												, &m_pd3dSamplerState);

}



///////////////////////////////////////////////////////////////////////////
//
CTextureDrawHP::CTextureDrawHP(IWICImagingFactory* pwicFactory, IDWriteFactory *pdwFactory, ID3D11Device* pd3dDevice, ID2D1Factory* pd2dFactory, UINT width, UINT height, DXGI_FORMAT format)
	: CTextureDrawable(pwicFactory, pdwFactory, pd3dDevice, pd2dFactory, width, height, format)
{
	m_pd2dRenderTarget->CreateSolidColorBrush(ColorF(ColorF::RoyalBlue), &m_pd2dsbrFont);
	m_pd2dRenderTarget->CreateSolidColorBrush(ColorF(ColorF::DimGray), &m_pd2dsbrHPBar);
	m_pd2dRenderTarget->CreateSolidColorBrush(ColorF(ColorF::Crimson), &m_pd2dsbrHPGage);
	m_pd2dsbrHPBar->SetOpacity(0.5f);
	m_pd2dsbrHPGage->SetOpacity(0.6f);

	m_pdwFactory->CreateTextFormat(	  L"Arial"
									, nullptr
									, DWRITE_FONT_WEIGHT_NORMAL
									, DWRITE_FONT_STYLE_NORMAL
									, DWRITE_FONT_STRETCH_NORMAL
									, width * 0.15f
									, L"ko-kr"
									, reinterpret_cast<IDWriteTextFormat**>(&m_pdwTextFormat)
	);

	m_pdwTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	m_pdwTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	float fWidth = static_cast<float>(width);
	float fHeight = static_cast<float>(height);
	m_rcHPBar = RectF(fWidth * 0.25f, fHeight * 0.4f, fWidth * 0.95f, fHeight * 0.6f);
	m_rcWriteID = RectF(0, 0, fWidth * 0.25f, fHeight);
}

CTextureDrawHP::~CTextureDrawHP()
{
	SafeRelease(m_pd2dsbrHPBar);
	SafeRelease(m_pd2dsbrFont);
	SafeRelease(m_pd2dsbrHPGage);
}

#include "Objects\Box\BoxObject.h"

void CTextureDrawHP::Render2D(CObject * obj)
{
	auto Object = static_cast<CBoxObject*>(obj);
	assert(Object && "Error!");

	wstring strID = to_wstring(Object->GetID());

	m_pd2dRenderTarget->BeginDraw();
	m_pd2dRenderTarget->Clear(ColorF(0.f,0.f,0.f,0.f));
	
	m_pd2dRenderTarget->DrawText(	  strID.c_str(), static_cast<UINT>(strID.length())
									, m_pdwTextFormat, m_rcWriteID, m_pd2dsbrFont);

	m_pd2dRenderTarget->FillRectangle(m_rcHPBar, m_pd2dsbrHPBar);
	
	D2D_RECT_F rcCurrent = m_rcHPBar;
	float fCurrentPBar = (m_rcHPBar.right - m_rcHPBar.left) * 0.7f;
	rcCurrent.right = rcCurrent.left + fCurrentPBar;

	m_pd2dRenderTarget->FillRectangle(rcCurrent, m_pd2dsbrHPGage);

	m_pd2dRenderTarget->EndDraw();
}



///////////////////////////////////////////////////////////////////////////
//
CMaterial::CMaterial(shared_ptr<CMaterialColors> pColors)
{
	m_pColors = pColors;
}

CMaterial::~CMaterial()
{
}

void CMaterial::SetTexture(shared_ptr<CTexture> pTexture)
{
	m_pTexture = pTexture;
}

void CMaterial::UpdateTextureShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, CObject* obj)
{
	if (!m_pTexture) return;
	//	작업중
	//	if (m_pColors) CObject::UpdateShaderVariable(pd3dDeviceContext, m_pColors.get());
	switch (m_pTexture->GetTextureType())
	{
	case CTextureBase::TextureType::File:
		{
			auto pTexture = static_cast<CTexture*>(m_pTexture.get());
			if (pTexture) pTexture->UpdateTextureShaderVariable(pd3dDeviceContext);
		}
		break;
	case CTextureBase::TextureType::Render:
		{
			auto pTexture = static_cast<CTextureDrawable*>(m_pTexture.get());
			if (pTexture) pTexture->Render2D(obj);
		}
		break;
	default:
		assert(!"Error!");
	}


}
