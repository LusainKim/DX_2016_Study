#pragma once

class CMesh {

public:

	struct BufferInfo {
		// 실제 데이터가 저장된 버퍼입니다.
		ID3D11Buffer*	m_pVertexBuffer { nullptr };
		// 버퍼 크기입니다.
		UINT			m_nVertexStrides { 0 };
		// 버퍼의 오프셋(offset)입니다.
		// offset : 정점 버퍼에서 실제로 사용되는 시작 정점의 위치. 단위 : byte
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

	// 실제 데이터가 저장된 버퍼입니다.
	// 배열 요소 단위로 m_vVertexStrides 와 m_vVertexOffset 과 관계 있습니다.
	vector<ID3D11Buffer*>			m_vVertexBuffers		{ }			;

	ID3D11Buffer				*	m_pIndexBuffer			{ nullptr }	;
	// 인덱스 버퍼의 오프셋입니다.
	UINT							m_nIndexOffset			{ 0 }		;
	// 인덱스 배열입니다.
	vector<unsigned short>			m_vIndies				{}			;

	// 배열 요소가 같은 버텍스 버퍼의 크기입니다.
	vector<UINT>					m_vVertexStrides		{}			;
	// 배열 요소가 같은 버텍스 버퍼의 오프셋(offset)입니다.
	// offset : 정점 버퍼에서 실제로 사용되는 시작 정점의 위치. 단위 : byte
	vector<UINT>					m_vVertexOffset			{}			;

	// 정점 개수입니다.
	UINT							m_nVertices				{ 0 }		;
	// m_vIndies 의 type 에 따라 결정됩니다.
	DXGI_FORMAT						m_dxgiIndexFormat		{ DXGI_FORMAT_R16_UINT }					;
	
	D3D11_PRIMITIVE_TOPOLOGY		m_d3dPrimitiveTopology	{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST }	;

	UINT							m_nStartIndex			{ 0 }		;
	UINT							m_nStartVertex			{ 0 }		;
	UINT							m_nBaseVertex			{ 0 }		;
	
};