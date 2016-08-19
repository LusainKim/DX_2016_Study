//-----------------------------------------------------------------------------
// File: Scene.h
//-----------------------------------------------------------------------------

#pragma once

class CDirectXFramework;

class CScene
{
public:

	CScene();
	virtual ~CScene();

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

	bool FindByTag(std::wstring Tag);
	std::wstring GetTag() { return m_wsTag; }
protected:
	std::wstring					m_wsTag;
	HWND							m_hWnd;
	CDirectXFramework				*m_pMasterFramework;
};

