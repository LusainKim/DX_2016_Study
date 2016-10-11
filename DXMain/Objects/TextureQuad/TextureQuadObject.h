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
	virtual void SetRenderingPosition(const XMMATRIX& xmmtxObject, FXMVECTOR xmvPosition, class CCamera* pCamera = nullptr);

	template<typename TextureType, typename... Arg>
	void CreateDrawableTexture(Arg&&... args)
	{
		static_assert(is_base_of<CTextureDrawable, TextureType>::value,
			"texture type is to be used only 'CTextureDrawable'!!");

		m_pTextureDrawable = make_unique<TextureType>(args...);
		
	}

protected:
	
	virtual void Render2D(CObject* obj) { m_pTextureDrawable->Render2D(obj); }

	CShader<TextureShader>			m_Shader;
	CTextureQuadMesh				m_Mesh;

	unique_ptr<CTextureDrawable>	m_pTextureDrawable;

};