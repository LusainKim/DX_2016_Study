#pragma once
#include "TextureQuadMesh.h"
#include "TextureQuadShader.h"
#include "Objects\Base\Meterial.h"

class CTextureQuadObject : public CObject {

public:

	CTextureQuadObject(ID3D11Device* pd3dDevice, float width, float height);
	virtual ~CTextureQuadObject() = default;

	virtual void Update(float fTimeElapsed, const XMMATRIX& xmmtxObject, FXMVECTOR xmvPosition);
	virtual void Render(ID3D11DeviceContext * pd3dDeviceContext);

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