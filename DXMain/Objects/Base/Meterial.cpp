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
ID3D11Buffer *CTextureBase::m_pd3dcbTextureMatrix = nullptr;

CTexture::CTexture(int nTextures, int nSamplers, int nTextureStartSlot, int nSamplerStartSlot)
{
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

void CTexture::SetTexture(int nIndex, ID3D11ShaderResourceView *pd3dsrvTexture)
{
	if (m_ppd3dsrvTextures[nIndex]) m_ppd3dsrvTextures[nIndex]->Release();
	m_ppd3dsrvTextures[nIndex] = pd3dsrvTexture;
	if (pd3dsrvTexture) pd3dsrvTexture->AddRef();
}

void CTexture::SetSampler(int nIndex, ID3D11SamplerState *pd3dSamplerState)
{
	if (m_ppd3dSamplerStates[nIndex]) m_ppd3dSamplerStates[nIndex]->Release();
	m_ppd3dSamplerStates[nIndex] = pd3dSamplerState;
	if (pd3dSamplerState) pd3dSamplerState->AddRef();
}

void CTexture::UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext)
{
	pd3dDeviceContext->PSSetShaderResources(	  m_nTextureStartSlot
												, static_cast<UINT>(m_ppd3dsrvTextures.size())
												, &(m_ppd3dsrvTextures[0]));

	pd3dDeviceContext->PSSetSamplers(			  m_nSamplerStartSlot
												, static_cast<UINT>(m_ppd3dSamplerStates.size())
												, &(m_ppd3dSamplerStates[0]));
}

void CTexture::UpdateTextureShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, int nIndex, int nSlot)
{
	pd3dDeviceContext->PSSetShaderResources(nSlot, 1, &m_ppd3dsrvTextures[nIndex]);
}

void CTexture::UpdateSamplerShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, int nIndex, int nSlot)
{
	pd3dDeviceContext->PSSetSamplers(nSlot, 1, &m_ppd3dSamplerStates[nIndex]);
}

void CTextureBase::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	CD3D11_BUFFER_DESC d3dBufferDesc(sizeof(XMMATRIX), D3D11_BIND_CONSTANT_BUFFER);
	pd3dDevice->CreateBuffer(&d3dBufferDesc, NULL, &m_pd3dcbTextureMatrix);
}

void CTextureBase::ReleaseShaderVariables()
{
	if (m_pd3dcbTextureMatrix) m_pd3dcbTextureMatrix->Release();
}

// for animation texture
void CTextureBase::UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, XMMATRIX *pd3dxmtxTexture)
{
	return;

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

ID3D11ShaderResourceView *CTextureBase::CreateRandomTexture1DSRV(ID3D11Device *pd3dDevice)
{
	auto RANDOMCOLOR = [] (float min, float max)
	{ return static_cast<float>(rand() / static_cast<float>(RAND_MAX)) * (max - min) + min; };

	XMCOLOR RV[1024];
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
	ID3D11Texture1D *pd3dTexture;
	// Texture1D를 제작
	pd3dDevice->CreateTexture1D(&d3dTextureDesc, &d3dSubresourceData, &pd3dTexture);
	ID3D11ShaderResourceView *pd3dsrvTexture;
	//쉐이더 리소스 뷰 제작
	pd3dDevice->CreateShaderResourceView(pd3dTexture, NULL, &pd3dsrvTexture);
	pd3dTexture->Release();
	return(pd3dsrvTexture);
}



///////////////////////////////////////////////////////////////////////////
//
CTextureDrawable::~CTextureDrawable()
{
	SafeRelease(m_pd2dRenderTarget);
	SafeRelease(m_pd3dTex2dDrawable);
	SafeRelease(m_pd3dsrvTexture);
	SafeRelease(m_pwicFactory);
}

void CTextureDrawable::SetTexture(int nIndex, ID3D11ShaderResourceView * pd3dsrvTexture)
{
}

void CTextureDrawable::SetSampler(int nIndex, ID3D11SamplerState * pd3dSamplerState)
{
}

void CTextureDrawable::UpdateShaderVariable(ID3D11DeviceContext * pd3dDeviceContext)
{
}

void CTextureDrawable::UpdateTextureShaderVariable(ID3D11DeviceContext * pd3dDeviceContext, int nIndex, int nSlot)
{
	Assert_CTextureDrawable(nIndex, nSlot);
}

void CTextureDrawable::UpdateSamplerShaderVariable(ID3D11DeviceContext * pd3dDeviceContext, int nIndex, int nSlot)
{
	Assert_CTextureDrawable(nIndex, nSlot);



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

void CMaterial::UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext)
{
//	작업중
//	if (m_pColors) CObject::UpdateShaderVariable(pd3dDeviceContext, m_pColors.get());
	if (m_pTexture) m_pTexture->UpdateShaderVariable(pd3dDeviceContext);
}
