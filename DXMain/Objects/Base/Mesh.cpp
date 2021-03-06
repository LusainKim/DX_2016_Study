#include "stdafx.h"
#include "Mesh.h"

CMesh::~CMesh()
{
//	for (auto p : m_vVertexBuffers)
//		SafeRelease(p);
}

bool RayIntersectTriangle(	  XMVECTOR xmvOrigin
							, XMVECTOR xmvDirection
							, XMVECTOR xmvP0
							, XMVECTOR xmvP1
							, XMVECTOR xmvP2
							, float *pfU
							, float *pfV
							, float *pfRayToTriangle
)
{
	XMVECTOR xmvEdge1 = xmvP1 - xmvP0;
	XMVECTOR xmvEdge2 = xmvP2 - xmvP0;
	XMVECTOR xmvP, xmvQ;
	
	xmvP = XMVector3Cross(xmvDirection, xmvEdge2);
	
	XMVECTOR xmvA = XMVector3Dot(xmvEdge1, xmvP);

	if (XMVector3Equal(xmvA, XMVectorZero())) return(false);

	float f = 1.0f / XMVectorGetX(xmvA);

	XMVECTOR d3dxvP0ToOrigin = xmvOrigin - xmvP0;
	*pfU = f * XMVectorGetX(XMVector3Dot(d3dxvP0ToOrigin, xmvP));
	if ((*pfU < 0.0f) || (*pfU > 1.0f)) return(false);
	xmvQ = XMVector3Cross(d3dxvP0ToOrigin, xmvEdge1);

	*pfV = f * XMVectorGetX(XMVector3Dot(xmvDirection, xmvQ));
	if ((*pfV < 0.0f) || ((*pfU + *pfV) > 1.0f)) return(false);
	*pfRayToTriangle = f * XMVectorGetX(XMVector3Dot(xmvEdge2, xmvQ));
	
	return(*pfRayToTriangle >= 0.0f);
}

/// <summary>
/// 각 인자를 배열에 추가합니다.
/// </summary>
///	<param name = "pd3dBuffer"> 버텍스 버퍼입니다. </param>
///	<param name = "nVertexStrides"> 버텍스 버퍼의 크기입니다. </param>
///	<param name = "nVertexOffset"> 버텍스 버퍼의 오프셋입니다. </param>
/// <remarks> 세 개의 인자는 모두 연관되어야 합니다. </remarks>
/// <returns> 이 함수는 값을 반환하지 않습니다. </returns>
void CMesh::AssembleToVertexBuffers(ID3D11Buffer * pd3dBuffer, UINT nVertexStrides, UINT nVertexOffset)
{
	m_vVertexBuffers.push_back(pd3dBuffer);
	m_vVertexStrides.push_back(nVertexStrides);
	m_vVertexOffset.push_back(nVertexOffset);
}

/// <summary>
/// 각 인자를 배열에 추가합니다.
/// </summary>
///	<param name = "pd3dBuffer"> 버퍼 정보의 배열입니다. </param>
/// <remarks> </remarks>
/// <returns> 이 함수는 값을 반환하지 않습니다. </returns>
void CMesh::AssembleToVertexBuffers(vector<BufferInfo> vBufferInfo)
{

	m_vVertexBuffers.reserve(m_vVertexBuffers.size() + vBufferInfo.size());
	m_vVertexStrides.reserve(m_vVertexStrides.size() + vBufferInfo.size());
	m_vVertexOffset.reserve(m_vVertexOffset.size() + vBufferInfo.size());

	for (auto p : vBufferInfo)
	{
		m_vVertexBuffers.push_back(p.m_pVertexBuffer);
		m_vVertexStrides.push_back(p.m_nVertexStrides);
		m_vVertexOffset.push_back(p.m_nVertexOffset);
	}
}

// 정상적으로 동작하지 않습니다. 반드시 기본 포맷을 이용하여 적절한 Position 값을 대입해야 합니다.
int CMesh::CheckRayIntersection(FXMVECTOR xmvRayPosition, FXMVECTOR xmvRayDirection, MESHINTERSECTINFO * pd3dxIntersectInfo)
{
	vector<XMVECTOR> m_pd3dxvPositions;
	auto m_nIndices = static_cast<UINT>(m_vIndies.size());
	int nIntersections = 0;
	BYTE * pbPositions = reinterpret_cast<BYTE *>(&m_pd3dxvPositions[0]) + m_vVertexOffset[0];

	auto nOffset = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? 3 : 1;
	UINT nPrimitives = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nVertices / 3) : (m_nVertices - 2);
	if (m_vIndies.size() > 0) nPrimitives = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nIndices / 3) : (m_nIndices - 2);
	
	XMFLOAT4A v0, v1, v2;
	float fHitDistance, fNearHitDistance = FLT_MAX;
	for (UINT i = 0; i < nPrimitives; i++)
	{
		v0 = *(XMFLOAT4A *) (pbPositions + ((m_nIndices > 0) ? (m_vIndies[(i * nOffset) + 0]) : ((i * nOffset) + 0)) * m_vVertexStrides[0]);
		v1 = *(XMFLOAT4A *) (pbPositions + ((m_nIndices > 0) ? (m_vIndies[(i * nOffset) + 1]) : ((i * nOffset) + 1)) * m_vVertexStrides[0]);
		v2 = *(XMFLOAT4A *) (pbPositions + ((m_nIndices > 0) ? (m_vIndies[(i * nOffset) + 2]) : ((i * nOffset) + 2)) * m_vVertexStrides[0]);

		XMVECTOR xmv[3] { XMLoadFloat4A(&v0), XMLoadFloat4A(&v1), XMLoadFloat4A(&v2) };

		if (DirectX::TriangleTests::Intersects(xmvRayPosition, XMVector4Normalize(xmvRayDirection), xmv[0], xmv[1], xmv[2], fHitDistance))
		{
			if (fHitDistance < fNearHitDistance)
			{
				fNearHitDistance = fHitDistance;
				if (pd3dxIntersectInfo)
				{
					pd3dxIntersectInfo->m_dwFaceIndex = i;
					pd3dxIntersectInfo->m_fU = 0.f;
					pd3dxIntersectInfo->m_fV = 0.f;
					pd3dxIntersectInfo->m_fDistance = fHitDistance;
				}
			}
			nIntersections++;
		}
	}
	return(nIntersections);
}

void CMesh::OnPrepareRender(ID3D11DeviceContext * pd3dDeviceContext)
{
	// 입력조립기에 VertexBuffer 를 연결합니다.
	pd3dDeviceContext->IASetVertexBuffers(	  0
											, static_cast<UINT>(m_vVertexBuffers.size())
											, &(m_vVertexBuffers[0])
											, &(m_vVertexStrides[0])
											, &(m_vVertexOffset[0])
	);
	
	// 입력조립기에 IndexBuffer를 연결합니다.
	pd3dDeviceContext->IASetIndexBuffer(	  m_pIndexBuffer.Get()
											, m_dxgiIndexFormat
											, m_nIndexOffset
	);
	
	// 입력조립기에 이번에 사용할 Primitive Topology 를 연결합니다.
	pd3dDeviceContext->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
}

void CMesh::Render(ID3D11DeviceContext * pd3dDeviceContext)
{
	OnPrepareRender(pd3dDeviceContext);

	if (m_pIndexBuffer)
		pd3dDeviceContext->DrawIndexed(	  static_cast<UINT>(m_vIndies.size())
										, m_nStartIndex
										, m_nBaseVertex
		);
	else
		pd3dDeviceContext->Draw(	  m_nVertices
									, m_nStartVertex
		);
}

void CMesh::RenderInstanced(ID3D11DeviceContext * pd3dDeviceContext, int nInstances, int nStartInstance)
{
	OnPrepareRender(pd3dDeviceContext);

	//객체들의 인스턴스들을 렌더링한다. 
	if (m_pIndexBuffer)
		pd3dDeviceContext->DrawIndexedInstanced(	  static_cast<UINT>(m_vIndies.size())
													, nInstances
													, m_nStartIndex
													, m_nBaseVertex
													, nStartInstance
		);
	else
		pd3dDeviceContext->DrawInstanced(	  m_nVertices
											, nInstances
											, m_nStartVertex
											, nStartInstance
		);

}
