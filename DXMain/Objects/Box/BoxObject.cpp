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
	// ������Ʈ�� �� ��Ʈ������ ���̴��� �����ϵ��� �غ��մϴ�.
	m_xmmtxLocal = m_xmmtxLocal * XMMatrixTranspose(XMMatrixRotationY(radians));
}

void CBoxObjectComponent::AddObject(XMVECTOR xmvec)
{
	unique_ptr<BaseObject> uniqueObj = make_unique<CBoxObject>();
	auto pObj = reinterpret_cast<BoxObject*>(uniqueObj.get());

	pObj->SetPosition(xmvec);
	CObjectComponent::AddObject(move(uniqueObj));
}
