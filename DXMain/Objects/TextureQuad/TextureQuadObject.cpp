#include "stdafx.h"
#include "Objects\Base\Object.h"
#include "TextureQuadObject.h"

CTextureQuadObject::CTextureQuadObject(ID3D11Device * pd3dDevice, float width, float height)
	: m_Mesh(pd3dDevice, width, height)
{
	m_Shader.CreateVertexShaderAndInputLayout(LOADSCOPATH("Texture2DQuadVertexShader.cso"), pd3dDevice);
	m_Shader.CreatePixelShader(LOADSCOPATH("Texture2DQuadPixelShader.cso"), pd3dDevice);
}

void CTextureQuadObject::Update(float fTimeElapsed, const XMMATRIX& xmmtxObject, FXMVECTOR xmvPosition)
{
	m_pTextureDrawable->Render2D(this);
	m_xmmtxLocal = xmmtxObject;
	m_xmmtxLocal.r[3] += xmvPosition;

}

void CTextureQuadObject::Render(ID3D11DeviceContext * pd3dDeviceContext)
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
