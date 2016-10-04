#include "stdafx.h"
#include "Objects\Base\Object.h"
#include "TextureQuadObject.h"
#include "Camera\Camera.h"

CTextureQuadObject::CTextureQuadObject(ID3D11Device * pd3dDevice, float width, float height)
	: m_Mesh(pd3dDevice, width, height)
{
	m_Shader.CreateVertexShaderAndInputLayout(LOADSCOPATH("Texture2DQuadVertexShader.cso"), pd3dDevice);
	m_Shader.CreatePixelShader(LOADSCOPATH("Texture2DQuadPixelShader.cso"), pd3dDevice);

	CreateBlendState(pd3dDevice);
	CreateRasterizerState(pd3dDevice, D3D11_CULL_NONE);
}

void CTextureQuadObject::Update(float fTimeElapsed, const XMMATRIX& xmmtxObject, FXMVECTOR xmvPosition, CCamera* pCamera)
{
	m_xmmtxLocal = xmmtxObject;
	m_xmmtxLocal.r[3] += xmvPosition;

	if (pCamera)
	{
		XMVECTOR xmvCameraLook	= XMLoadFloat4A(&pCamera->GetLookVector());
		XMVECTOR xmvCameraUp	= XMLoadFloat4A(&pCamera->GetUpVector());
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

void CTextureQuadObject::Render(ID3D11DeviceContext * pd3dDeviceContext, CCamera* pCamera)
{
	// Shader Set
	m_Shader.Render(pd3dDeviceContext);
	m_pTextureDrawable->UpdateTextureShaderVariable(pd3dDeviceContext);

	CObject::OnPrepareRender(pd3dDeviceContext);
	
	// Draw
	m_Mesh.Render(pd3dDeviceContext);

	// Release States
	CObject::OnFinishRender(pd3dDeviceContext);
}
