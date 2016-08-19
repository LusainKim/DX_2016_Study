//-----------------------------------------------------------------------------
// File: Scene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Framework\Framework.h"
#include "Scene.h"

CScene::CScene()
{

}

CScene::~CScene()
{
	ReleaseObjects();
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		break;
	case WM_RBUTTONDOWN:
		break;
	case WM_MOUSEMOVE:
		break;
	case WM_LBUTTONUP:
		break;
	case WM_RBUTTONUP:
		break;
	default:
		break;
	}

	return(true);
}

bool CScene::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SIZE:
		break;
	default:
		return false;
	}

	return true;
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		break;
	default:
		break;
	}
	return(true);
}

void CScene::BuildObjects(wstring Tag, HWND hWnd, CDirectXFramework *pMasterFramework)
{
	m_wsTag = Tag;
	m_hWnd = hWnd;
	m_pMasterFramework = pMasterFramework;
}

void CScene::BuildObjecsFromD3D11Devices(ID3D11Device* pd3dDevice, ID3D11DeviceContext *pd3dDeviceContext)
{
}

void CScene::BuildObjecsFromD2D1Devices(ID2D1Device2 * pd2dDevice, ID2D1DeviceContext2 * pd2dDeviceContext, IDWriteFactory3 * pdwFactory)
{
}

void CScene::ReleaseObjects()
{
}

void CScene::Update2D(float fTimeElapsed)
{
}

void CScene::Update3D(float fTimeElapsed)
{
}

void CScene::AnimateObjects(float fTimeElapsed)
{

}

void CScene::Render2D(ID2D1DeviceContext2 * pd2dDeviceContext)
{
}

void CScene::Render3D(ID3D11DeviceContext *pd3dDeviceContext)
{

}

bool CScene::FindByTag(std::wstring Tag)
{
	return Tag == m_wsTag;
}
