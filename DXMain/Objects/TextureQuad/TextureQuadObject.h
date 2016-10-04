#pragma once
#include "TextureQuadMesh.h"
#include "TextureQuadShader.h"
#include "Objects\Base\Meterial.h"

class CTextureQuadObject : public CObject {

public:

	CTextureQuadObject(ID3D11Device* pd3dDevice, float width, float height);
	virtual ~CTextureQuadObject() = default;

	virtual void Update(float fTimeElapsed, const XMMATRIX& xmmtxObject, FXMVECTOR xmvPosition, class CCamera* pCamera = nullptr);
	virtual void Render(ID3D11DeviceContext * pd3dDeviceContext, class CCamera* pCamera = nullptr);

	virtual void Render2D(CObject* obj) { m_pTextureDrawable->Render2D(obj); }
	
	template<typename TextureType, typename... Arg>
	void CreateDrawableTexture(Arg&&... args)
	{
		static_assert(is_base_of<CTextureDrawable, TextureType>::value,
			"texture type is to be used only 'CTextureDrawable'!!");

		m_pTextureDrawable = make_unique<TextureType>(args...);
		
	}

protected:

	CTextureQuadShader				m_Shader;
	CTextureQuadMesh				m_Mesh;

	unique_ptr<CTextureDrawable>	m_pTextureDrawable;

};