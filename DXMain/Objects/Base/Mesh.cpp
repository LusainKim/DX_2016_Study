#include "stdafx.h"
#include "Mesh.h"

CMesh::~CMesh()
{
	SafeRelease(m_pIndexBuffer);
	for (auto p : m_vVertexBuffers)
		if (p) p->Release();
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
	pd3dDeviceContext->IASetIndexBuffer(	  m_pIndexBuffer
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
