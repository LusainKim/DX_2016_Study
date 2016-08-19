#include "stdafx.h"
#include "Mesh.h"

CMesh::~CMesh()
{
	SafeRelease(m_pIndexBuffer);
	for (auto p : m_vVertexBuffers)
		if (p) p->Release();
}

/// <summary>
/// �� ���ڸ� �迭�� �߰��մϴ�.
/// </summary>
///	<param name = "pd3dBuffer"> ���ؽ� �����Դϴ�. </param>
///	<param name = "nVertexStrides"> ���ؽ� ������ ũ���Դϴ�. </param>
///	<param name = "nVertexOffset"> ���ؽ� ������ �������Դϴ�. </param>
/// <remarks> �� ���� ���ڴ� ��� �����Ǿ�� �մϴ�. </remarks>
/// <returns> �� �Լ��� ���� ��ȯ���� �ʽ��ϴ�. </returns>
void CMesh::AssembleToVertexBuffers(ID3D11Buffer * pd3dBuffer, UINT nVertexStrides, UINT nVertexOffset)
{
	m_vVertexBuffers.push_back(pd3dBuffer);
	m_vVertexStrides.push_back(nVertexStrides);
	m_vVertexOffset.push_back(nVertexOffset);
}

/// <summary>
/// �� ���ڸ� �迭�� �߰��մϴ�.
/// </summary>
///	<param name = "pd3dBuffer"> ���� ������ �迭�Դϴ�. </param>
/// <remarks> </remarks>
/// <returns> �� �Լ��� ���� ��ȯ���� �ʽ��ϴ�. </returns>
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
	// �Է������⿡ VertexBuffer �� �����մϴ�.
	pd3dDeviceContext->IASetVertexBuffers(	  0
											, static_cast<UINT>(m_vVertexBuffers.size())
											, &(m_vVertexBuffers[0])
											, &(m_vVertexStrides[0])
											, &(m_vVertexOffset[0])
	);
	
	// �Է������⿡ IndexBuffer�� �����մϴ�.
	pd3dDeviceContext->IASetIndexBuffer(	  m_pIndexBuffer
											, m_dxgiIndexFormat
											, m_nIndexOffset
	);
	
	// �Է������⿡ �̹��� ����� Primitive Topology �� �����մϴ�.
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

	//��ü���� �ν��Ͻ����� �������Ѵ�. 
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
