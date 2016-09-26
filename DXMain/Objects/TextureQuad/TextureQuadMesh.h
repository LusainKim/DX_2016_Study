#pragma once

#include "Objects\Base\Mesh.h"
#include "HLSL\BindingBufferStructs.h"

class CTexture2DQuad : public CMesh {

public:

	CTexture2DQuad(ID3D11Device* pd3dDevice, float fWidth = 0.5f, float fHeight = 0.5f);
	~CTexture2DQuad() = default;

	virtual int CheckRayIntersection(FXMVECTOR xmvRayPosition, FXMVECTOR xmvRayDirection, MESHINTERSECTINFO * pd3dxIntersectInfo);

protected:

	ID3D11Buffer						*	m_pd3dTexture2DQuadBuffer { nullptr };
	vector<Texture2DQuadConstantBuffer>		m_vTexture2DQuadBuffer;

};