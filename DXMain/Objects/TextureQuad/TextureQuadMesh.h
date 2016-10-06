#pragma once

#include "Objects\Base\Mesh.h"
#include "HLSL\BindingBufferStructs.h"

class CTextureQuadMesh : public CMesh {

public:

	CTextureQuadMesh(ID3D11Device* pd3dDevice, float fWidth = 0.5f, float fHeight = 0.5f);
	~CTextureQuadMesh() = default;

	virtual int CheckRayIntersection(FXMVECTOR xmvRayPosition, FXMVECTOR xmvRayDirection, MESHINTERSECTINFO * pd3dxIntersectInfo);

protected:

	ID3D11Buffer						*	m_pd3dTexture2DQuadBuffer { nullptr };
	vector<PositionTextureConstantBuffer>		m_vTexture2DQuadBuffer;

};