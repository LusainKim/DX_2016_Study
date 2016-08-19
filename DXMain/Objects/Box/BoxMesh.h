#pragma once

#include "Objects\Base\Mesh.h"
#include "HLSL\PositionColor\BindingBufferStructs.h"

class CBoxMesh : public CMesh {

public:

	CBoxMesh(ID3D11Device* pd3dDevice, float fWidth = 0.5f, float fHeight = 0.5f, float fDepth = 0.5f);
	
protected:

	ID3D11Buffer				*	m_pd3dVertexPositionColorBuffer { nullptr }	;
	vector<VertexPositionColor>		m_vVertexPositionColor						;

};