#include "stdafx.h"
#include "BoxMesh.h"

CBoxMesh::CBoxMesh(ID3D11Device * pd3dDevice, float fWidth, float fHeight, float fDepth)
	: CMesh()
{
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	m_vVertexPositionColor =
	{
		{XMFLOAT3(-fWidth, +fHeight, -fDepth), XMFLOAT3(0.0f, 1.0f, 0.0f)},
		{XMFLOAT3(+fWidth, +fHeight, -fDepth), XMFLOAT3(1.0f, 1.0f, 0.0f)},
		{XMFLOAT3(+fWidth, +fHeight, +fDepth), XMFLOAT3(1.0f, 1.0f, 1.0f)},
		{XMFLOAT3(-fWidth, +fHeight, +fDepth), XMFLOAT3(0.0f, 1.0f, 1.0f)},
		{XMFLOAT3(-fWidth, -fHeight, -fDepth), XMFLOAT3(0.0f, 0.0f, 0.0f)},
		{XMFLOAT3(+fWidth, -fHeight, -fDepth), XMFLOAT3(1.0f, 0.0f, 0.0f)},
		{XMFLOAT3(+fWidth, -fHeight, +fDepth), XMFLOAT3(1.0f, 0.0f, 1.0f)},
		{XMFLOAT3(-fWidth, -fHeight, +fDepth), XMFLOAT3(0.0f, 0.0f, 1.0f)},
	};
	
	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = &(m_vVertexPositionColor[0]);
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC vertexBufferDesc(
		  static_cast<UINT>(m_vVertexPositionColor.size() * sizeof(VertexPositionColor))
		, D3D11_BIND_VERTEX_BUFFER);

	pd3dDevice->CreateBuffer(
		  &vertexBufferDesc
		, &vertexBufferData
		, &m_pd3dVertexPositionColorBuffer
	);
	AssembleToVertexBuffers(m_pd3dVertexPositionColorBuffer, sizeof(VertexPositionColor), 0);

	m_vIndies =
	{
		  5
		, 6
		, 4
		, 7
		, 0
		, 3
		, 1
		, 2
		, 2
		, 3
		, 3
		, 7
		, 2
		, 6
		, 1
		, 5
		, 0
		, 4
	};

	// 메시 인덱스를 위한 인덱스 버퍼 생성
	D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
	indexBufferData.pSysMem = &(m_vIndies[0]);
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC indexBufferDesc(
		  static_cast<UINT>(m_vIndies.size() * sizeof(decltype(m_vIndies)::value_type))
		, D3D11_BIND_INDEX_BUFFER);

	pd3dDevice->CreateBuffer(
		  &indexBufferDesc
		, &indexBufferData
		, &m_pIndexBuffer
	);

}
