#pragma once
#include "TextureQuadMesh.h"
#include "Objects\Base\Shader.h"
#include "Objects\Base\Meterial.h"

class CTextureQuadObject : public CObject {

	using TextureShader = PositionTextureConstantBuffer;

public:

	CTextureQuadObject(ID3D11Device* pd3dDevice, float width, float height);
	virtual ~CTextureQuadObject() = default;

	virtual void Update(float fTimeElapsed, CObject* obj);
	virtual void Render(ID3D11DeviceContext * pd3dDeviceContext, class CCamera* pCamera = nullptr);
	virtual void SetRenderingPosition(const XMMATRIX& xmmtxObject, FXMVECTOR xmvPosition = XMVectorZero(), class CCamera* pCamera = nullptr);

	template<typename TextureType, typename... Arg>
	void CreateDrawableTexture(Arg&&... args)
	{
		static_assert(is_base_of<CTextureDrawable, TextureType>::value,
			"texture type is to be used only 'CTextureDrawable'!!");

		m_pTexture = make_unique<TextureType>(args...);
		m_bDrawableTexture = true;
	}

	template<typename TextureType, typename... Arg>
	void CreateTextureFromFile(ID3D11Device * pd3dDevice, LPCTSTR path, Arg&&... args)
	{
		static_assert(is_base_of<CTexture, TextureType>::value,
			"texture type is to be used only 'CTexture'!!");

		m_pTexture = make_unique<TextureType>(args...);

		auto pTexture = reinterpret_cast<TextureType*>(m_pTexture.get());

		ComPtr<ID3D11SamplerState> pd3dSamplerState;
		D3D11_SAMPLER_DESC d3dSamplerDesc;
		ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
		d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		d3dSamplerDesc.MinLOD = 0;
		d3dSamplerDesc.MaxLOD = 0;

		pd3dDevice->CreateSamplerState(&d3dSamplerDesc, pd3dSamplerState.GetAddressOf());
		pTexture->SetSampler(0, pd3dSamplerState.Get());

		ComPtr<ID3D11ShaderResourceView> srv;
		CreateWICTextureFromFile(pd3dDevice, path, nullptr, srv.GetAddressOf());

		pTexture->SetTexture(0, srv.Get());
		m_bDrawableTexture = false;
	}

protected:
	
	virtual void Render2D(CObject* obj) { dynamic_cast<CTextureDrawable*>(m_pTexture.get())->Render2D(obj); }

	CShader<TextureShader>			m_Shader						;
	CTextureQuadMesh				m_Mesh							;

	unique_ptr<CTextureBase>		m_pTexture						;
	bool							m_bDrawableTexture	{ false }	;
};