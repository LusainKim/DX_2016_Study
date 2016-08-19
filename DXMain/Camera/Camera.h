#pragma once

class CCamera {
private:

	// 각 행렬의 인자입니다.

	
	XMFLOAT4A						m_xmvRight				{1.f, 0.f, 0.f, 0.f}	;	// 1 행 : Right 방향입니다.
	XMFLOAT4A						m_xmvUp					{0.f, 1.f, 0.f, 0.f}	;	// 2 행 : Up 방향입니다.
	XMFLOAT4A						m_xmvLook				{0.f, 0.f, 1.f, 0.f}	;	// 3 행 : Look 방향입니다.
	XMFLOAT4A						m_xmvPosition			{0.f, 0.f, 0.f, 1.f}	;	// 4 행 : 동차좌표계 / Position 입니다.

	float           				m_fPitch				{ 0 }	;
	float           				m_fRoll					{ 0 }	;
	float           				m_fYaw					{ 0 }	;

	XMFLOAT4A						m_xmvOffset				{0.f, 0.f, 0.f, 0.f}	;
	float           				m_fTimeLag				{ 0 }	;

	D3D11_VIEWPORT					m_d3dViewport			{ 0 }	;

	XMMATRIX						m_xmmtxView						;
	XMMATRIX						m_xmmtxProjection				;

	BoundingFrustum					m_pd3dxFrustumPlanes			;	//World Coordinates          

	
	static ID3D11Buffer				*m_pd3dcbCamera					;		// 모든 카메라는 하나의 Static Buffer를 사용합니다.

public:

	CCamera() = default;

	XMFLOAT4A& GetRightVector()		{ return(m_xmvRight); }
	XMFLOAT4A& GetUpVector()		{ return(m_xmvUp); }
	XMFLOAT4A& GetLookVector()		{ return(m_xmvLook); }
	XMFLOAT4A& GetPosition()		{ return(m_xmvPosition); }
	void SetPosition(const XMFLOAT4A& xmfPosition) { m_xmvPosition = xmfPosition; }
	void SetPosition(FXMVECTOR xmvPosition) { XMStoreFloat4A(&m_xmvPosition, xmvPosition); }

	float& GetPitch()	{ return(m_fPitch); }
	float& GetRoll()	{ return(m_fRoll); }
	float& GetYaw()		{ return(m_fYaw); }


	void SetOffset(const XMFLOAT4A& xmfOffset)
	{
		m_xmvOffset = xmfOffset;
		XMStoreFloat4A(&m_xmvPosition, XMLoadFloat4A(&m_xmvPosition) + XMLoadFloat4A(&xmfOffset));
	}

	void SetOffset(FXMVECTOR xmvOffset)
	{
		XMStoreFloat4A(&m_xmvOffset, xmvOffset);
		XMStoreFloat4A(&m_xmvPosition, xmvOffset + XMLoadFloat4A(&m_xmvPosition));
	}
	
	XMFLOAT4A& GetOffset() { return(m_xmvOffset); }


	void SetTimeLag(float fTimeLag) { m_fTimeLag = fTimeLag; }
	float GetTimeLag() { return(m_fTimeLag); }


	virtual void Move(FXMVECTOR xmvShift) { XMStoreFloat4A(&m_xmvPosition, xmvShift + XMLoadFloat4A(&m_xmvPosition)); }
	virtual void Move(const XMFLOAT4A& xmfShift) { XMStoreFloat4A(&m_xmvPosition, XMLoadFloat4A(&m_xmvPosition) + XMLoadFloat4A(&xmfShift)); }

	virtual void MoveForward(float fSpeed)
	{
		XMVECTOR xmvLook = fSpeed * XMLoadFloat4A(&m_xmvLook);
		XMStoreFloat4A(&m_xmvPosition, xmvLook + XMLoadFloat4A(&m_xmvPosition));
	}

	virtual void MoveStrafe(float fSpeed)
	{
		XMVECTOR xmvRight = fSpeed * XMLoadFloat4A(&m_xmvRight);
		XMStoreFloat4A(&m_xmvPosition, xmvRight + XMLoadFloat4A(&m_xmvPosition));
	}

	virtual void MoveRise(float fSpeed)
	{
		XMVECTOR xmvUp = fSpeed * XMLoadFloat4A(&m_xmvUp);
		XMStoreFloat4A(&m_xmvPosition, xmvUp + XMLoadFloat4A(&m_xmvPosition));
	}

	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);

	virtual void Update(float fTimeElapsed) { }

	void SetViewport(ID3D11DeviceContext *pd3dDeviceContext	, LONG xTopLeft
															, LONG yTopLeft
															, LONG nWidth
															, LONG nHeight
															, float fMinZ
															, float fMaxZ);
	

	void SetViewport(ID3D11DeviceContext *pd3dDeviceContext) { if (pd3dDeviceContext) pd3dDeviceContext->RSSetViewports(1, &m_d3dViewport); }

	void GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle);

	void GenerateViewMatrix();

	void CalculateFrustumPlanes()
	{
		XMMATRIX mtxViewProjection = m_xmmtxView * m_xmmtxProjection;
		BoundingFrustum::CreateFromMatrix(m_pd3dxFrustumPlanes, mtxViewProjection);
	}

	void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext)
	{
		UpdateShaderVariable(pd3dDeviceContext, m_xmmtxView, m_xmmtxProjection);
	}

	static void CreateShaderVariables(ID3D11Device *pd3dDevice);

	static void ReleaseShaderVariables()
	{
		if (m_pd3dcbCamera) m_pd3dcbCamera->Release();
		m_pd3dcbCamera = nullptr;
	}

	static void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, FXMMATRIX pxmmtxView, FXMMATRIX pxmmtxProjection);
	

	bool IsInFrustum(FXMVECTOR xmvMinimum, FXMVECTOR xmvMaximum);

	bool IsInFrustum(const XMFLOAT4A& xmfMinimum, const XMFLOAT4A& xmfMaximum);
	
};
