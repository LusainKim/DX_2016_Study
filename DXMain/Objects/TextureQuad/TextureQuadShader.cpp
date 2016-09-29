#include "stdafx.h"
#include "TextureQuadShader.h"

bool CTextureQuadShader::DefineInputElementDescriptions()
{
	m_vInputElementDescs.emplace_back(D3D11_INPUT_ELEMENT_DESC { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	m_vInputElementDescs.emplace_back(D3D11_INPUT_ELEMENT_DESC { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 });

	return true;
}
