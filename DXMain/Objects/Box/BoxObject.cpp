#include "stdafx.h"
#include "Objects\Base\Object.h"
#include "BoxObject.h"

void CBoxObject::Update(float fTimeElapsed)
{
	float radiansPerSecond = XMConvertToRadians(45);
	double totalRotation = fTimeElapsed * radiansPerSecond;
	float radians = static_cast<float>(fmod(totalRotation, XM_2PI));
	Rotate(radians);
}

void CBoxObject::Render2D(ID2D1RenderTarget * pd2dRenderTarget, IDWriteTextFormat* pdwTextFormat, ID2D1SolidColorBrush * pd2dsbrFont)
{

}

void CBoxObject::Rotate(float radians)
{
	// 업데이트된 모델 매트릭스를 셰이더에 전달하도록 준비합니다.
	m_xmmtxLocal = XMMatrixTranspose(XMMatrixRotationY(radians)) * m_xmmtxLocal;
}

CBoxObjectComponent::~CBoxObjectComponent()
{
}

void CBoxObjectComponent::AddObject(XMVECTOR xmvec)
{
	unique_ptr<BaseObject> uniqueObj = make_unique<CBoxObject>();
	auto pObj = reinterpret_cast<BoxObject*>(uniqueObj.get());

	pObj->SetPosition(xmvec);
	CObjectComponent::AddObject(move(uniqueObj));
}

void CBoxObjectComponent::Update(float fTimeElapsed)
{
	for (auto &p : m_lstObjects)
	{
		m_ObjectMeterial.UpdateTextureShaderVariable(nullptr, p.get());
		p->Update(fTimeElapsed);
	}
}

void CBoxObjectComponent::Render(ID3D11DeviceContext * pd3dDeviceContext)
{
	// Shader Set
	m_pShader->Render(pd3dDeviceContext);

	for (auto& p : m_lstObjects)
	{
		// Set States
		p->Render(pd3dDeviceContext);

		// Draw
		m_pMesh->Render(pd3dDeviceContext);

		// Release States
		p->OnFinishRender(pd3dDeviceContext);
	}

	for (auto& p : m_lstObjects)
	{
		m_TextureObjectDrawHP->Update(0.f, p->GetLocalMatrix(), XMVectorSet(0, 0.9f, 0, 0));
		m_TextureObjectDrawHP->Render(pd3dDeviceContext);
	}
}

void CBoxObjectComponent::BuildComponent(ID3D11Device * pd3dDevice, ID2D1Factory * pd2dFactory, IDWriteFactory * pdwFactory, IWICImagingFactory * pwicFactory)
{
	m_TextureObjectDrawHP = make_unique<CTextureQuadObject>(pd3dDevice, 1.f, 0.5f);
//	shared_ptr<CTextureBase> pTex2DDrawable = make_shared<CTextureDrawHP>(pwicFactory, pdwFactory, pd3dDevice, pd2dFactory, 100, 40);
//
//	m_ObjectMeterial.m_pTexture = pTex2DDrawable;
	m_TextureObjectDrawHP->CreateDrawableTexture<CTextureDrawHP>(pwicFactory, pdwFactory, pd3dDevice, pd2dFactory, 200, 100);

}
