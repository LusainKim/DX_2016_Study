#pragma once

class CMesh {

public:

	struct BufferInfo {
		// ���� �����Ͱ� ����� �����Դϴ�.
		ID3D11Buffer*	m_pVertexBuffer { nullptr };
		// ���� ũ���Դϴ�.
		UINT			m_nVertexStrides { 0 };
		// ������ ������(offset)�Դϴ�.
		// offset : ���� ���ۿ��� ������ ���Ǵ� ���� ������ ��ġ. ���� : byte
		UINT			m_nVertexOffset { 0 };
	};

public:

	CMesh() = default;
	virtual ~CMesh();

	virtual void OnPrepareRender(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void Render(ID3D11DeviceContext* pd3dDeviceContext);
	virtual void RenderInstanced(ID3D11DeviceContext *pd3dDeviceContext, int nInstances, int nStartInstance);
	
	void AssembleToVertexBuffers(ID3D11Buffer* pd3dBuffer, UINT nVertexStrides, UINT nVertexOffset);
	void AssembleToVertexBuffers(vector<BufferInfo> vBufferInfo);

protected:

	// ���� �����Ͱ� ����� �����Դϴ�.
	// �迭 ��� ������ m_vVertexStrides �� m_vVertexOffset �� ���� �ֽ��ϴ�.
	vector<ID3D11Buffer*>			m_vVertexBuffers		{ }			;

	ID3D11Buffer				*	m_pIndexBuffer			{ nullptr }	;
	// �ε��� ������ �������Դϴ�.
	UINT							m_nIndexOffset			{ 0 }		;
	// �ε��� �迭�Դϴ�.
	vector<unsigned short>			m_vIndies				{}			;

	// �迭 ��Ұ� ���� ���ؽ� ������ ũ���Դϴ�.
	vector<UINT>					m_vVertexStrides		{}			;
	// �迭 ��Ұ� ���� ���ؽ� ������ ������(offset)�Դϴ�.
	// offset : ���� ���ۿ��� ������ ���Ǵ� ���� ������ ��ġ. ���� : byte
	vector<UINT>					m_vVertexOffset			{}			;

	// ���� �����Դϴ�.
	UINT							m_nVertices				{ 0 }		;
	// m_vIndies �� type �� ���� �����˴ϴ�.
	DXGI_FORMAT						m_dxgiIndexFormat		{ DXGI_FORMAT_R16_UINT }					;
	
	D3D11_PRIMITIVE_TOPOLOGY		m_d3dPrimitiveTopology	{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST }	;

	UINT							m_nStartIndex			{ 0 }		;
	UINT							m_nStartVertex			{ 0 }		;
	UINT							m_nBaseVertex			{ 0 }		;
	
};