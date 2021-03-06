#include "stdafx.h"
#include "HLSL\BindingBufferStructs.h"
#include "Camera.h"

ID3D11Buffer* CCamera::m_pd3dcbCamera = nullptr;

string& operator+(string& str, XMFLOAT4A xmf)
{
	str += "x : " + to_string(xmf.x) + ", ";
	str += "y : " + to_string(xmf.y) + ", ";
	str += "z : " + to_string(xmf.z);
	return str;
}

inline string to_string(XMFLOAT4A xmf)
{
	string str;
	str += "x : " + to_string(xmf.x) + ", ";
	str += "y : " + to_string(xmf.y) + ", ";
	str += "z : " + to_string(xmf.z);
	return str;
}

void CCamera::Rotate(float fPitch, float fYaw, float fRoll)
{
	if(fabs(m_fPitch + fPitch) < 90.f)
		m_fPitch += fPitch;
	m_fYaw += fYaw;
	m_fRoll += fRoll;

	XMVECTOR xmvRight	= XMVectorSet(1.f, 0.f, 0.f, 0.f);
	XMVECTOR xmvUp		= XMVectorSet(0.f, 1.f, 0.f, 0.f);
	XMVECTOR xmvLook	= XMVectorSet(0.f, 0.f, 1.f, 0.f);

	XMMATRIX mtxRotate = XMMatrixIdentity();

	mtxRotate = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(m_fPitch));
	xmvLook		= XMVector3TransformNormal(xmvLook	, mtxRotate);
	xmvUp		= XMVector3TransformNormal(xmvUp	, mtxRotate);
	xmvRight	= XMVector3TransformNormal(xmvRight	, mtxRotate);
	
	mtxRotate = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_fYaw));
	xmvLook		= XMVector3TransformNormal(xmvLook	, mtxRotate);
	xmvUp		= XMVector3TransformNormal(xmvUp	, mtxRotate);
	xmvRight	= XMVector3TransformNormal(xmvRight	, mtxRotate);
	
	mtxRotate = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(m_fRoll));

	XMVECTOR xmvPosition = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	xmvPosition = XMVector3TransformCoord(xmvPosition, mtxRotate);
	xmvPosition += XMLoadFloat4A(&m_xmfPosition);

	xmvLook		= XMVector3TransformNormal(xmvLook	, mtxRotate);
	xmvUp		= XMVector3TransformNormal(xmvUp	, mtxRotate);
	xmvRight	= XMVector3TransformNormal(xmvRight	, mtxRotate);

	XMStoreFloat4A(&m_xmfLook		, XMVector3Normalize(xmvLook		)		);
	XMStoreFloat4A(&m_xmfUp			, XMVector3Normalize(xmvUp			)		);
	XMStoreFloat4A(&m_xmfRight		, XMVector3Normalize(xmvRight		)		);
	XMStoreFloat4A(&m_xmfPosition	,					(xmvPosition	)		);

#if USE_DEBUG_WINDOW
	Event_CutsomLogger eventLook		(string("m_fPitch"),	to_string(m_fPitch));
	Event_CutsomLogger eventUp			(string("m_fYaw"),		to_string(m_fYaw));
	Event_CutsomLogger eventRight		(string("m_fRoll"),		to_string(m_fRoll));
	Event_CutsomLogger eventPosition	(string("position"),	to_string(m_xmfPosition));
	CLogSystem::PropagateNotification(nullptr, &eventLook);
	CLogSystem::PropagateNotification(nullptr, &eventUp);
	CLogSystem::PropagateNotification(nullptr, &eventRight);
	CLogSystem::PropagateNotification(nullptr, &eventPosition);
#endif

	GenerateViewMatrix();
}

void CCamera::SetViewport(ID3D11DeviceContext * pd3dDeviceContext, LONG xTopLeft, LONG yTopLeft, LONG nWidth, LONG nHeight, float fMinZ, float fMaxZ)
{
	m_d3dViewport.TopLeftX =	static_cast<float>(xTopLeft);
	m_d3dViewport.TopLeftY =	static_cast<float>(yTopLeft);
	m_d3dViewport.Width =		static_cast<float>(nWidth);
	m_d3dViewport.Height =		static_cast<float>(nHeight);
	m_d3dViewport.MinDepth = fMinZ;
	m_d3dViewport.MaxDepth = fMaxZ;

	SetViewport(pd3dDeviceContext);
}

void CCamera::GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle)
{
	m_xmmtxProjection = XMMatrixPerspectiveFovLH(	  XMConvertToRadians(fFOVAngle)
													, fAspectRatio
													, fNearPlaneDistance
													, fFarPlaneDistance
	);
}

void CCamera::GenerateViewMatrix()
{
	XMVECTOR eyePos = XMLoadFloat4A(&m_xmfPosition);
	XMVECTOR lookvec = XMLoadFloat4A(&m_xmfLook);
	XMVECTOR upvec = XMLoadFloat4A(&m_xmfUp);

	m_xmmtxView = XMMatrixLookToLH(eyePos, lookvec, upvec);
}

void CCamera::CreateShaderVariables(ID3D11Device * pd3dDevice)
{
	ReleaseShaderVariables();

	CD3D11_BUFFER_DESC bd(sizeof(CameraConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	pd3dDevice->CreateBuffer(&bd, nullptr, &m_pd3dcbCamera);
}

void CCamera::UpdateAnimationTextureShaderVariable(ID3D11DeviceContext * pd3dDeviceContext, FXMMATRIX pxmmtxView, FXMMATRIX pxmmtxProjection)
{
	CameraConstantBuffer cbuffer;

	XMStoreFloat4x4A(&cbuffer.projection, XMMatrixTranspose(pxmmtxProjection));
	XMStoreFloat4x4A(&cbuffer.view, XMMatrixTranspose(pxmmtxView));
	
	// 그래픽 장치로 보낼 수 있도록 상수 버퍼를 준비합니다.
	pd3dDeviceContext->UpdateSubresource(	  m_pd3dcbCamera
											, 0
											, NULL
											, &cbuffer
											, 0
											, 0
	);

	pd3dDeviceContext->VSSetConstantBuffers(0, 1, &m_pd3dcbCamera);

}

bool CCamera::IsInFrustum(FXMVECTOR xmvMinimum, FXMVECTOR xmvMaximum)
{
	XMFLOAT4A xmfMinimum;
	XMFLOAT4A xmfMaximum;
	XMStoreFloat4A(&xmfMinimum, xmvMinimum);
	XMStoreFloat4A(&xmfMaximum, xmvMaximum);

	return IsInFrustum(xmfMinimum, xmfMaximum);
}

bool CCamera::IsInFrustum(const XMFLOAT4A & xmfMinimum, const XMFLOAT4A & xmfMaximum)
{
	XMVECTOR frustum[6];
	m_xnaFrustumPlanes.GetPlanes(
		&frustum[4], &frustum[5], &frustum[1], &frustum[0], &frustum[2], &frustum[3]
	);
	XMFLOAT4A d3dxvNearPoint, d3dxvNormal;
	for (int i = 0; i < 6; i++)
	{
		XMStoreFloat4A(&d3dxvNormal, frustum[i]);
		if (d3dxvNormal.x >= 0.0f)
		{
			if (d3dxvNormal.y >= 0.0f)
			{
				if (d3dxvNormal.z >= 0.0f)
				{
					d3dxvNearPoint.x = xmfMinimum.x; 
					d3dxvNearPoint.y = xmfMinimum.y;
					d3dxvNearPoint.z = xmfMinimum.z;
				}
				else
				{
					d3dxvNearPoint.x = xmfMinimum.x; 
					d3dxvNearPoint.y = xmfMinimum.y; 
					d3dxvNearPoint.z =	xmfMaximum.z;
				}
			}
			else
			{
				if (d3dxvNormal.z >= 0.0f)
				{
					d3dxvNearPoint.x = xmfMinimum.x;
					d3dxvNearPoint.y =	xmfMaximum.y;
					d3dxvNearPoint.z = xmfMinimum.z;
				}
				else
				{
					d3dxvNearPoint.x = xmfMinimum.x;
					d3dxvNearPoint.y =	xmfMaximum.y; 
					d3dxvNearPoint.z =	xmfMaximum.z;
				}
			}
		}
		else
		{
			if (d3dxvNormal.y >= 0.0f)
			{
				if (d3dxvNormal.z >= 0.0f)
				{
					d3dxvNearPoint.x =	xmfMaximum.x; 
					d3dxvNearPoint.y = xmfMinimum.y; 
					d3dxvNearPoint.z = xmfMinimum.z;
				}
				else
				{
					d3dxvNearPoint.x =	xmfMaximum.x; 
					d3dxvNearPoint.y = xmfMinimum.y; 
					d3dxvNearPoint.z =	xmfMaximum.z;
				}
			}
			else
			{
				if (d3dxvNormal.z >= 0.0f)
				{
					d3dxvNearPoint.x =	xmfMaximum.x; 
					d3dxvNearPoint.y =	xmfMaximum.y; 
					d3dxvNearPoint.z = xmfMinimum.z;
				}
				else
				{
					d3dxvNearPoint.x =	xmfMaximum.x;
					d3dxvNearPoint.y =	xmfMaximum.y; 
					d3dxvNearPoint.z =	xmfMaximum.z;
				}
			}
		}

		XMVECTOR xmvec = XMLoadFloat4A(&d3dxvNearPoint);
		if (XMVectorGetX(XMPlaneDotCoord(frustum[i], xmvec)) > 0.0f) return(false);
	}
	return(true);
}
