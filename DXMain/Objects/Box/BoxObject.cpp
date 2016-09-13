#include "stdafx.h"
#include "Objects\Base\Object.h"
#include "BoxMesh.h"
#include "BoxShader.h"
#include "BoxObject.h"
#include "Objects\Base\Meterial.h"


void CBoxObject::Update(float fTimeElapsed)
{
	float radiansPerSecond = XMConvertToRadians(45);
	double totalRotation = fTimeElapsed * radiansPerSecond;
	float radians = static_cast<float>(fmod(totalRotation, XM_2PI));
	Rotate(radians);
}

void CBoxObject::Render2D(ID2D1RenderTarget * pd2dRenderTarget, IDWriteTextFormat* pdwTextFormat, ID2D1SolidColorBrush * pd2dsbrFont)
{
	auto szRT = pd2dRenderTarget->GetSize();
	wstring strID = to_wstring(GetID());
	pd2dRenderTarget->BeginDraw();

	pd2dRenderTarget->DrawTextW(strID.c_str(), strID.length(), pdwTextFormat, RectF(0, 0, szRT.width, szRT.height), pd2dsbrFont);

	pd2dRenderTarget->EndDraw();
}

void CBoxObject::Rotate(float radians)
{
	// 업데이트된 모델 매트릭스를 셰이더에 전달하도록 준비합니다.
	m_xmmtxLocal = XMMatrixTranspose(XMMatrixRotationY(radians)) * m_xmmtxLocal;
}

CBoxObjectComponent::~CBoxObjectComponent()
{
	SafeRelease(m_pdwfID);
}

void CBoxObjectComponent::AddObject(XMVECTOR xmvec)
{
	unique_ptr<BaseObject> uniqueObj = make_unique<CBoxObject>();
	auto pObj = reinterpret_cast<BoxObject*>(uniqueObj.get());

	pObj->SetPosition(xmvec);
	CObjectComponent::AddObject(move(uniqueObj));
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
}

inline void CBoxObjectComponent::BuildComponent(ID3D11Device * pd3dDevice, ID2D1Factory * pd2dFactory, IDWriteFactory * pdwFactory, IWICImagingFactory * pwicFactory)
{
	assert(!(nullptr == pd3dDevice && nullptr == pd2dFactory && nullptr == pdwFactory) && "Device와 Factory는 활성화되어야 합니다!");

	if (nullptr != pwicFactory) 
	{ 
		m_pwicFactory = pwicFactory; 
		m_pwicFactory->AddRef();
	}
	
	SafeRelease(m_pd3dTex2dDrawID);
	SafeRelease(m_pd2dRenderTarget);

	CTexture::Create2DTexture(pd3dDevice, pd2dFactory, &m_pd3dTex2dDrawID, &m_pd2dRenderTarget, 30, 30, DXGI_FORMAT_R32G32B32A32_UINT);

	pdwFactory->CreateTextFormat(L"Arial"
		, nullptr
		, DWRITE_FONT_WEIGHT_NORMAL
		, DWRITE_FONT_STYLE_NORMAL
		, DWRITE_FONT_STRETCH_NORMAL
		, 12
		, L"ko-kr"
		, static_cast<IDWriteTextFormat**>(&m_pdwfID)
	);
	m_pd2dRenderTarget->CreateSolidColorBrush(ColorF(ColorF::Black), &m_pd2dsbrFont);
}
