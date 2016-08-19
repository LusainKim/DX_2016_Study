//-----------------------------------------------------------------------------
// File: Scene.h
//-----------------------------------------------------------------------------

#pragma once

#include "Scene\Scene.h"
#include "Camera\Camera.h"
#include "Objects\Box\BoxMesh.h"
#include "Objects\Box\BoxShader.h"
#include "Objects\Box\BoxObject.h"

class CTitleScene : public CScene
{
public:

	CTitleScene();
	virtual ~CTitleScene();

	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void BuildObjects(std::wstring Tag, HWND hWnd, CDirectXFramework *pMasterFramework);
	virtual void BuildObjecsFromD3D11Devices(ID3D11Device* pd3dDevice, ID3D11DeviceContext *pd3dDeviceContext);
	virtual void BuildObjecsFromD2D1Devices(ID2D1Device2* pd2dDevice, ID2D1DeviceContext2 * pd2dDeviceContext, IDWriteFactory3 *pdwFactory);
	virtual void ReleaseObjects();

	virtual void Update2D(float fTimeElapsed);
	virtual void Update3D(float fTimeElapsed);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void Render2D(ID2D1DeviceContext2 *pd2dDeviceContext);
	virtual void Render3D(ID3D11DeviceContext *pd3dDeviceContext);

	void CreateLusWrite(ID2D1DeviceContext2 * pd2dDeviceContext, IDWriteFactory3 * pdwFactory, D2D_POINT_2F pt, D2D_RECT_F rc, float FontSize, LPTSTR fontName)
	{
		m_LusWrite.Initialize(m_hWnd, pt, rc, pd2dDeviceContext, pdwFactory);
		m_LusWrite.MakeFont(FontSize, fontName);
	}

protected:

	LusWrite							m_LusWrite						;
	ID2D1SolidColorBrush			*	m_pd2dsbrDefault	{ nullptr }	;

	CCamera								m_Camera						;
	CBoxObjectComponent					m_BoxObjects					;

	D2D_POINT_2F						m_ptDragStart					; 
	D2D_POINT_2F						m_ptDragEnd						;

	POINT								m_ptCursorPos					;
	
	bool								m_bDragTracking		{ false }	;
	bool								m_bDrag				{ false }	;
};

