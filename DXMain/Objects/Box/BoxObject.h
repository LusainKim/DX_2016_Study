#pragma once
#include "BoxMesh.h"
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
	
	virtual bool GetSelectedObject(CObject* obj)
	{
		BoxObject* pObj = dynamic_cast<BoxObject*>(obj);
		if (pObj == nullptr)
		{
			m_pSelectedObject = nullptr;
			return false;
		}
		m_pSelectedObject = obj;
		m_TextureObjectDrawHP->Update(0, obj);
		return true;
	}

protected:

	CObject							*	m_pSelectedObject		{ nullptr }	;

	CMaterial							m_ObjectMeterial					;
	unique_ptr<CTextureQuadObject>		m_TextureObjectDrawHP				;


	vector<unique_ptr<CTextureQuadObject>> m_vupDummy;
};
