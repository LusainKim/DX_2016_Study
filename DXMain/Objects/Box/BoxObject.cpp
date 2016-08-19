#include "stdafx.h"
#include "Objects\Base\Object.h"
#include "BoxMesh.h"
#include "BoxShader.h"
#include "BoxObject.h"

void CBoxObject::Update(float fTimeElapsed)
{
	float radiansPerSecond = XMConvertToRadians(45);
	double totalRotation = fTimeElapsed * radiansPerSecond;
	float radians = static_cast<float>(fmod(totalRotation, XM_2PI));
	Rotate(radians);
}

void CBoxObject::Rotate(float radians)
{
	// 업데이트된 모델 매트릭스를 셰이더에 전달하도록 준비합니다.
	m_xmmtxLocal = m_xmmtxLocal * XMMatrixTranspose(XMMatrixRotationY(radians));
}

void CBoxObjectComponent::AddObject(XMVECTOR xmvec)
{
	unique_ptr<BaseObject> uniqueObj = make_unique<CBoxObject>();
	auto pObj = reinterpret_cast<BoxObject*>(uniqueObj.get());

	pObj->SetPosition(xmvec);
	CObjectComponent::AddObject(move(uniqueObj));
}
