#pragma once
#include "BoxMesh.h"
#include "BoxShader.h"
#include "Objects\Base\Meterial.h"

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

protected:

	using BoxShader = CBoxShader;
	using BoxMesh = CBoxMesh;

	using BoxObject = CBoxObject;

public:

	CBoxObjectComponent() = default;
	virtual ~CBoxObjectComponent();

	void AddObject(XMVECTOR xmvec);

	virtual void Update(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext* pd3dDeviceContext);

	virtual void BuildComponent(	  ID3D11Device * pd3dDevice
									, ID2D1Factory * pd2dFactory = nullptr
									, IDWriteFactory * pdwFactory = nullptr
									, IWICImagingFactory * pwicFactory = nullptr);

protected:

	CMaterial					m_ObjectMeterial					;
};
