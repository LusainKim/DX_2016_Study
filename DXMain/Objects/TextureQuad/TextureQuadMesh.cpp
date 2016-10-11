#include "stdafx.h"
#include "TextureQuadMesh.h"

CTextureQuadMesh::CTextureQuadMesh(ID3D11Device * pd3dDevice, float fWidth, float fHeight)
	: CMesh()
{
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_vTexture2DQuadBuffer =
	{
		{ XMFLOAT3(+fWidth, +fHeight, 0.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(+fWidth, -fHeight, 0.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-fWidth, -fHeight, 0.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-fWidth, -fHeight, 0.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-fWidth, +fHeight, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(+fWidth, +fHeight, 0.0f), XMFLOAT2(1.0f, 0.0f) },
	};
	
	m_nVertices = static_cast<UINT>(m_vTexture2DQuadBuffer.size());
	
	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = &(m_vTexture2DQuadBuffer[0]);
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC vertexBufferDesc(
		  static_cast<UINT>(m_vTexture2DQuadBuffer.size() * sizeof(PositionTextureConstantBuffer))
		, D3D11_BIND_VERTEX_BUFFER);

	pd3dDevice->CreateBuffer(
		  &vertexBufferDesc
		, &vertexBufferData
		, &m_pd3dTexture2DQuadBuffer
	);
	AssembleToVertexBuffers(m_pd3dTexture2DQuadBuffer, sizeof(PositionTextureConstantBuffer), 0);

}

int CTextureQuadMesh::CheckRayIntersection(FXMVECTOR xmvRayPosition, FXMVECTOR xmvRayDirection, MESHINTERSECTINFO * pd3dxIntersectInfo)
{
	auto m_nIndices = static_cast<int>(m_vIndies.size());
	int nIntersections = 0;
	BYTE * pbPositions = reinterpret_cast<BYTE *>(&m_vTexture2DQuadBuffer[0]) + m_vVertexOffset[0];

	int nOffset = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? 3 : 1;
	int nPrimitives = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nVertices / 3) : (m_nVertices - 2);
	if (m_vIndies.size() > 0) nPrimitives = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nIndices / 3) : (m_nIndices - 2);

	XMFLOAT3 v0, v1, v2;
	float fuHitBaryCentric = 0.f;
	float fvHitBaryCentric = 0.f;
	float fHitDistance = 0.f;
	float fNearHitDistance = FLT_MAX;
	
	for (int i = 0; i < nPrimitives; i++)
	{
		v0 = reinterpret_cast<PositionTextureConstantBuffer*>(pbPositions + ((m_nIndices > 0) ? (m_vIndies[(i * nOffset) + 0]) : ((i * nOffset) + 0)) * m_vVertexStrides[0])->pos;
		v1 = reinterpret_cast<PositionTextureConstantBuffer*>(pbPositions + ((m_nIndices > 0) ? (m_vIndies[(i * nOffset) + 1]) : ((i * nOffset) + 1)) * m_vVertexStrides[0])->pos;
		v2 = reinterpret_cast<PositionTextureConstantBuffer*>(pbPositions + ((m_nIndices > 0) ? (m_vIndies[(i * nOffset) + 2]) : ((i * nOffset) + 2)) * m_vVertexStrides[0])->pos;

		XMVECTOR xmv[3] { XMLoadFloat3(&v0), XMLoadFloat3(&v1), XMLoadFloat3(&v2) };
		// 원본 함수
//		if (DirectX::TriangleTests::Intersects(xmvRayPosition, XMVector4Normalize(xmvRayDirection), xmv[0], xmv[1], xmv[2], fHitDistance))
//		if (::RayIntersectTriangle(xmvRayPosition, XMVector4Normalize(xmvRayDirection), xmv[0], xmv[1], xmv[2], &fuHitBaryCentric, &fvHitBaryCentric, &fHitDistance))

		auto timeElapsed = [&] (auto d) {

		#if USE_DEBUG_WINDOW
			auto du = chrono::duration_cast<chrono::nanoseconds>(d).count();

			Event_MeasureTimeElapsed event(string("RayIntersectTriangle"), du);
			CLogSystem::PropagateNotification(nullptr, &event);
		#endif
		};
		// 시간 측정 함수
//		if (MeasureFunctionTimeElapsed(timeElapsed, RayIntersectTriangle, xmvRayPosition, XMVector4Normalize(xmvRayDirection), xmv[0], xmv[1], xmv[2], &fuHitBaryCentric, &fvHitBaryCentric, &fHitDistance))
		if (MeasureFunctionTimeElapsed(timeElapsed
			, static_cast<bool(XM_CALLCONV*)(FXMVECTOR, FXMVECTOR, FXMVECTOR, GXMVECTOR, HXMVECTOR, float&)>(DirectX::TriangleTests::Intersects)
			, xmvRayPosition, XMVector4Normalize(xmvRayDirection), xmv[0], xmv[1], xmv[2], fHitDistance)
			)
		{
			if (fHitDistance < fNearHitDistance)
			{
				fNearHitDistance = fHitDistance;
				if (pd3dxIntersectInfo)
				{
					pd3dxIntersectInfo->m_dwFaceIndex = i;
					pd3dxIntersectInfo->m_fU = fuHitBaryCentric;
					pd3dxIntersectInfo->m_fV = fvHitBaryCentric;
					pd3dxIntersectInfo->m_fDistance = fHitDistance;
				}
			}
			nIntersections++;
		}
	}

	return(nIntersections);
}
