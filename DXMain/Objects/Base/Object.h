#pragma once

class CObject {

public:

	CObject();
	CObject(FXMVECTOR xmvPosition);
	virtual ~CObject();
	UINT GetID() const { return m_ID; }

	static void CreateModelMatrixBuffer(ID3D11Device *pd3dDevice);
	static void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, FXMMATRIX pd3dxmtxWorld);

	void CreateRasterizerState(ID3D11Device* pd3dDevice, D3D11_CULL_MODE cull = D3D11_CULL_BACK, D3D11_FILL_MODE fill = D3D11_FILL_SOLID);
	void CreateRasterizerState(ID3D11Device* pd3dDevice, D3D11_RASTERIZER_DESC rasterizerDesc);

	void SetPosition(float x, float y, float z) { m_xmmtxLocal.r[3] = XMVectorSet(x, y, z, 1); }

	void SetPosition(FXMVECTOR xmPosition) { m_xmmtxLocal.r[3] = xmPosition; }

	void MoveStrafe(float fDistance);

	void MoveUp(float fDistance);

	void MoveForward(float fDistance);

	void Rotate(float fPitch, float fYaw, float fRoll);

	void Rotate(FXMVECTOR pxmAxis, float fAngle);

	XMVECTOR GetPosition() { return m_xmmtxLocal.r[3]; }

	XMVECTOR GetRight() { return XMVector3Normalize(m_xmmtxLocal.r[0]); }

	XMVECTOR GetUp() { return XMVector3Normalize(m_xmmtxLocal.r[1]); }

	XMVECTOR GetLook() { return XMVector3Normalize(m_xmmtxLocal.r[2]); }

	virtual void OnPrepareRender(ID3D11DeviceContext* pd3dDeviceContext);
	virtual void OnFinishRender(ID3D11DeviceContext* pd3dDeviceContext);
	virtual void Render(ID3D11DeviceContext* pd3dDeviceContext);

	virtual void Update(float fTimeElapsed) {}

protected:

	UINT						m_ID					{ 0 }		;
	XMMATRIX					m_xmmtxLocal			{}			;

	static ID3D11Buffer		*	m_pd3dcbWorldMatrix					;

	ID3D11RasterizerState1	*	m_pd3dRasterizerState	{ nullptr }	;
};



class CShader;
class CMesh;

class CObjectComponent {

protected:
	
	using BaseShader = CShader;
	using BaseMesh = CMesh;

	using BaseObject = CObject;

public:

	CObjectComponent() = default;
	virtual ~CObjectComponent();

	void EnquireShaderAndMesh(shared_ptr<BaseShader> pShader, shared_ptr<BaseMesh> pMesh);

	virtual void AddObject(unique_ptr<BaseObject>&& obj);

	template<typename Fn>
	void RemoveObject(Fn&& func) { m_lstObjects.remove_if(func); }

	virtual void OnPrepareRender(ID3D11DeviceContext* pd3dDeviceContext);
	virtual void Render(ID3D11DeviceContext* pd3dDeviceContext);

	virtual void Update(float fTimeElapsed);

protected:

	shared_ptr<CShader>				m_pShader		{ nullptr }	;
	shared_ptr<CMesh>				m_pMesh			{ nullptr }	;

	list<unique_ptr<CObject>>		m_lstObjects	{}			;

};