#pragma once
#include "BoxMesh.h"
#include "Scene\Scene.h"
#include "Objects\Base\Object.h"
#include "Objects\Base\Shader.h"
#include "Objects\Base\Meterial.h"
#include "Objects\TextureQuad\TextureQuadObject.h"

class CBoxObject : public CObject {

public:

	CBoxObject() : CObject() {}
	virtual ~CBoxObject() = default;

	virtual void Update(float fTimeElapsed);
	virtual void Render2D(ID2D1RenderTarget* pd2dRenderTarget, IDWriteTextFormat* pdwTextFormat, ID2D1SolidColorBrush * pd2dsbrFont);
	void Rotate(float radians);

protected:

};



class CBoxObjectComponent : public CObjectComponent {

public:

	using BoxShader = CShader<PositionTextureConstantBuffer>;
	using BoxMesh = CTexturedBoxMesh;

	using BoxObject = CBoxObject;

public:

	CBoxObjectComponent() = default;
	virtual ~CBoxObjectComponent();

	void AddObject(XMVECTOR xmvec, ID3D11Device * pd3dDevice = nullptr);

	virtual void Update(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext* pd3dDeviceContext, class CCamera* pCamera = nullptr);

	virtual void BuildComponent(	  ID3D11Device			* pd3dDevice
									, ID2D1Factory			* pd2dFactory	= nullptr
									, IDWriteFactory		* pdwFactory	= nullptr
									, IWICImagingFactory	* pwicFactory	= nullptr);
	
	virtual bool GetSelectedObject(PickingObject obj)
	{
		BoxObject* pObj = dynamic_cast<BoxObject*>(obj.obj);
		if (pObj == nullptr)
		{
			m_pSelectedObject = nullptr;
			return false;
		}
		else
		{
			m_pSelectedObject = obj.obj;
			m_TextureObjectDrawHP->Update(0, obj.obj);

			// TODO : TextureObject 하나 더 만들어서 위치 표시
			auto face = static_cast<EBoxFace>(obj.info.m_dwFaceIndex >> 1);
			auto mtx = XMMatrixTranslation(0, 0, -0.6f);
			switch (face)
			{
			case EBoxFace::Front:
			{
				mtx *= (m_pSelectedObject->GetLocalMatrix());
				m_TextureObjectSelectedFace->SetRenderingPosition(mtx, XMVectorZero(), nullptr);
			}
				break;
			case EBoxFace::Down:
			{
				mtx = (mtx * XMMatrixTranspose(XMMatrixRotationX(XMConvertToRadians(90)))) * (m_pSelectedObject->GetLocalMatrix());
				m_TextureObjectSelectedFace->SetRenderingPosition(mtx, XMVectorZero(), nullptr);
			}
				break;
			case EBoxFace::Back:
			{
				mtx = (mtx * XMMatrixTranspose(XMMatrixRotationY(XMConvertToRadians(180)))) * (m_pSelectedObject->GetLocalMatrix());
				m_TextureObjectSelectedFace->SetRenderingPosition(mtx, XMVectorZero(), nullptr);
			}
				break;
			case EBoxFace::Up:
			{
				mtx = (mtx * XMMatrixTranspose(XMMatrixRotationX(XMConvertToRadians(-90)))) * (m_pSelectedObject->GetLocalMatrix());
				m_TextureObjectSelectedFace->SetRenderingPosition(mtx, XMVectorZero(), nullptr);
			}
				break;
			case EBoxFace::Left:
			{
				mtx = (mtx * XMMatrixTranspose(XMMatrixRotationY(XMConvertToRadians(-90)))) * (m_pSelectedObject->GetLocalMatrix());
				m_TextureObjectSelectedFace->SetRenderingPosition(mtx, XMVectorZero(), nullptr);
			}
				break;
			case EBoxFace::Right:
			{
				mtx = (mtx * XMMatrixTranspose(XMMatrixRotationY(XMConvertToRadians(90)))) * (m_pSelectedObject->GetLocalMatrix());
				m_TextureObjectSelectedFace->SetRenderingPosition(mtx, XMVectorZero(), nullptr);
			}
				break;
			}
		}


		return true;
	}

protected:

	CObject							*	m_pSelectedObject		{ nullptr }	;

	CMaterial							m_ObjectMeterial					;
	unique_ptr<CTextureQuadObject>		m_TextureObjectDrawHP				;
	unique_ptr<CTextureQuadObject>		m_TextureObjectSelectedFace			;


	vector<unique_ptr<CTextureQuadObject>> m_vupDummy;
};
