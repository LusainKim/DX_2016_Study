#include "stdafx.h"
#include "Objects\Base\Object.h"
#include "TextureQuadObject.h"
#include "Camera\Camera.h"

CTextureQuadObject::CTextureQuadObject(ID3D11Device * pd3dDevice, float width, float height)
	: m_Mesh(pd3dDevice, width, height)
{
	m_Shader.CreateVertexShaderAndInputLayout(pd3dDevice);
	m_Shader.CreatePixelShader(pd3dDevice);

	CreateBlendState(pd3dDevice);
	CreateRasterizerState(pd3dDevice, D3D11_CULL_NONE);
}

void CTextureQuadObject::Update(float fTimeElapsed, CObject* obj)
{

	// Update Rendering
	if (m_bDrawableTexture)
		Render2D(obj);
}

void CTextureQuadObject::Render(ID3D11DeviceContext * pd3dDeviceContext, CCamera* pCamera)
{
	// Shader Set
	m_Shader.Render(pd3dDeviceContext);
	m_pTexture->UpdateTextureShaderVariable(pd3dDeviceContext);

	CObject::OnPrepareRender(pd3dDeviceContext);
	
	// Draw
	m_Mesh.Render(pd3dDeviceContext);

	// Release States
	CObject::OnFinishRender(pd3dDeviceContext);
}

void CTextureQuadObject::SetRenderingPosition(const XMMATRIX & xmmtxObject, FXMVECTOR xmvPosition, CCamera * pCamera)
{
	m_xmmtxLocal = xmmtxObject;
	m_xmmtxLocal.r[3] += xmvPosition;

	if (pCamera)
	{
		XMFLOAT4A xmfLook	= pCamera->GetLookVector();
		XMFLOAT4A xmfUp		= pCamera->GetUpVector();

		XMVECTOR xmvCameraLook	= XMLoadFloat4A(&xmfLook);
		XMVECTOR xmvCameraUp	= XMLoadFloat4A(&xmfUp);
		XMVECTOR xmvLook		= XMVector3Normalize(-xmvCameraLook);
		XMVECTOR xmvRight		= XMVector3Cross(xmvLook, xmvCameraUp);

//		m_xmmtxLocal._11 = d3dxvRight.x;	m_xmmtxLocal._12 = d3dxvRight.y;	m_xmmtxLocal._13 = d3dxvRight.z;
//		m_xmmtxLocal._21 = 0.f;				m_xmmtxLocal._22 = 1.f;				m_xmmtxLocal._23 = 0.f;
//		m_xmmtxLocal._31 = d3dxvLook.x;		m_xmmtxLocal._32 = d3dxvLook.y;		m_xmmtxLocal._33 = d3dxvLook.z;

		m_xmmtxLocal.r[0] = xmvRight;
		m_xmmtxLocal.r[1] = xmvCameraUp;
		m_xmmtxLocal.r[2] = xmvLook;

	}
}
