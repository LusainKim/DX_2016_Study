#pragma once

class CCamera {
private:

	// 각 행렬의 인자입니다.

	
	XMFLOAT4A						m_xmfRight				{1.f, 0.f, 0.f, 0.f}	;	// 1 행 : Right 방향입니다.
	XMFLOAT4A						m_xmfUp					{0.f, 1.f, 0.f, 0.f}	;	// 2 행 : Up 방향입니다.
	XMFLOAT4A						m_xmfLook				{0.f, 0.f, 1.f, 0.f}	;	// 3 행 : Look 방향입니다.
	XMFLOAT4A						m_xmfPosition			{0.f, 0.f, 0.f, 1.f}	;	// 4 행 : 동차좌표계 / Position 입니다.

	float           				m_fPitch				{ 0 }	;
	float           				m_fRoll					{ 0 }	;
	float           				m_fYaw					{ 0 }	;

	XMFLOAT4A						m_xmfOffset				{0.f, 0.f, 0.f, 0.f}	;
	float           				m_fTimeLag				{ 0 }	;

	D3D11_VIEWPORT					m_d3dViewport			{ 0 }	;

	XMMATRIX						m_xmmtxView						;
	XMMATRIX						m_xmmtxProjection				;

	BoundingFrustum					m_xnaFrustumPlanes				;	//World Coordinates          

	
	static ID3D11Buffer				*m_pd3dcbCamera					;		// 모든 카메라는 하나의 Static Buffer를 사용합니다.

public:

	CCamera() = default;

	XMFLOAT4A GetRightVector()		const { return(m_xmfRight); }
	XMFLOAT4A GetUpVector()			const { return(m_xmfUp); }
	XMFLOAT4A GetLookVector()		const { return(m_xmfLook); }
	XMFLOAT4A GetPosition()			const { return(m_xmfPosition); }

	XMMATRIX GetViewMatrix()		const { return(m_xmmtxView); }
	XMMATRIX GetProjectionMatrix()	const { return(m_xmmtxProjection); }
	D3D11_VIEWPORT GetViewport()	const { return(m_d3dViewport); }

	void SetPosition(const XMFLOAT4A& xmfPosition) { m_xmfPosition = xmfPosition; }
	void SetPosition(FXMVECTOR xmvPosition) { XMStoreFloat4A(&m_xmfPosition, xmvPosition); }

	float& GetPitch()	{ return(m_fPitch); }
	float& GetRoll()	{ return(m_fRoll); }
	float& GetYaw()		{ return(m_fYaw); }


	void SetOffset(const XMFLOAT4A& xmfOffset)
	{
		m_xmfOffset = xmfOffset;
		XMStoreFloat4A(&m_xmfPosition, XMLoadFloat4A(&m_xmfPosition) + XMLoadFloat4A(&xmfOffset));
	}

	void SetOffset(FXMVECTOR xmvOffset)
	{
		XMStoreFloat4A(&m_xmfOffset, xmvOffset);
		XMStoreFloat4A(&m_xmfPosition, xmvOffset + XMLoadFloat4A(&m_xmfPosition));
	}
	
	XMFLOAT4A& GetOffset() { return(m_xmfOffset); }


	void SetTimeLag(float fTimeLag) { m_fTimeLag = fTimeLag; }
	float GetTimeLag() { return(m_fTimeLag); }


	virtual void Move(FXMVECTOR xmvShift) { XMStoreFloat4A(&m_xmfPosition, xmvShift + XMLoadFloat4A(&m_xmfPosition)); }
	virtual void Move(const XMFLOAT4A& xmfShift) { XMStoreFloat4A(&m_xmfPosition, XMLoadFloat4A(&m_xmfPosition) + XMLoadFloat4A(&xmfShift)); }

	virtual void MoveForward(float fSpeed)
	{
		XMVECTOR xmvLook = fSpeed * XMLoadFloat4A(&m_xmfLook);
		XMStoreFloat4A(&m_xmfPosition, xmvLook + XMLoadFloat4A(&m_xmfPosition));
	}

	virtual void MoveStrafe(float fSpeed)
	{
		XMVECTOR xmvRight = fSpeed * XMLoadFloat4A(&m_xmfRight);
		XMStoreFloat4A(&m_xmfPosition, xmvRight + XMLoadFloat4A(&m_xmfPosition));
	}

	virtual void MoveRise(float fSpeed)
	{
		XMVECTOR xmvUp = fSpeed * XMLoadFloat4A(&m_xmfUp);
		XMStoreFloat4A(&m_xmfPosition, xmvUp + XMLoadFloat4A(&m_xmfPosition));
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
		BoundingFrustum::CreateFromMatrix(m_xnaFrustumPlanes, mtxViewProjection);
	}

	void UpdateAnimationTextureShaderVariable(ID3D11DeviceContext *pd3dDeviceContext)
	{
		UpdateAnimationTextureShaderVariable(pd3dDeviceContext, m_xmmtxView, m_xmmtxProjection);
	}

	static void CreateShaderVariables(ID3D11Device *pd3dDevice);

	static void ReleaseShaderVariables()
	{
		if (m_pd3dcbCamera) m_pd3dcbCamera->Release();
		m_pd3dcbCamera = nullptr;
	}

	static void UpdateAnimationTextureShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, FXMMATRIX pxmmtxView, FXMMATRIX pxmmtxProjection);
	

	bool IsInFrustum(FXMVECTOR xmvMinimum, FXMVECTOR xmvMaximum);

	bool IsInFrustum(const XMFLOAT4A& xmfMinimum, const XMFLOAT4A& xmfMaximum);
	
};
