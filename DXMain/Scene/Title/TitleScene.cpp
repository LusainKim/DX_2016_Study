//-----------------------------------------------------------------------------
// File: Scene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Framework\Framework.h"
#include "header\LusWrite.h"

#include "Objects\Base\Mesh.h"
#include "Objects\Base\Object.h"
#include "Objects\Base\Shader.h"

#include "TitleScene.h"

CTitleScene::CTitleScene()
{

}

CTitleScene::~CTitleScene()
{
	ReleaseObjects();
}

D2D_RECT_F CreateRect(D2D_POINT_2F sp, D2D_POINT_2F ep)
{
	FLOAT left = min(sp.x, ep.x);
	FLOAT top = min(sp.y, ep.y);
	FLOAT right = max(sp.x, ep.x);
	FLOAT bottom = max(sp.y, ep.y);
	return RectF(left, top, right, bottom);
}

bool CTitleScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	const float m_DPI = 0.25f;
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		m_LusWrite.Click(Point2F(LOWORD(lParam), HIWORD(lParam)));
		if (!m_LusWrite.IsActive())
		{
			
			GetCursorPos(&m_ptCursorPos);
			SetCapture(hWnd);
			ShowCursor(FALSE);
			m_ptDragStart = Point2F(LOWORD(lParam), HIWORD(lParam));
			m_bDrag = true;
			m_bDragTracking = true;
		}
		break;

	case WM_RBUTTONDOWN:
		m_ptDragStart = Point2F(LOWORD(lParam), HIWORD(lParam));
		m_bDrag = true;
		break;

	case WM_MOUSEMOVE:
		if (m_bDrag)
		{
			m_ptDragEnd = Point2F(LOWORD(lParam), HIWORD(lParam));
			
			if(m_bDragTracking)
			{ 
				auto fPitch		= m_DPI * static_cast<float>(m_ptDragEnd.y - m_ptDragStart.y);
				auto fYaw		= m_DPI * static_cast<float>(m_ptDragEnd.x - m_ptDragStart.x);
			
				m_Camera.Rotate(fPitch, fYaw, 0.f);
				m_ptDragStart = m_ptDragEnd;
			}
			
		}
		break;

	case WM_LBUTTONUP:
		if (m_bDragTracking)
		{
			m_bDragTracking = false;
			m_ptDragEnd = Point2F(LOWORD(lParam), HIWORD(lParam));
			
			auto fPitch		= m_DPI * static_cast<float>(m_ptDragEnd.y - m_ptDragStart.y);
			auto fYaw		= m_DPI * static_cast<float>(m_ptDragEnd.x - m_ptDragStart.x);

			ReleaseCapture();
			SetCursorPos(m_ptCursorPos.x, m_ptCursorPos.y);
			int nCount = ShowCursor(TRUE);
			while (nCount < 0) nCount = ShowCursor(TRUE);
			while (nCount > 0) nCount = ShowCursor(FALSE);
			m_Camera.Rotate(fPitch, fYaw, 0.f);
		}
		m_bDrag = false;
		break;

	case WM_RBUTTONUP:
		m_ptDragEnd = Point2F(LOWORD(lParam), HIWORD(lParam));
		m_bDrag = false;
		m_LusWrite.Resize(CreateRect(m_ptDragStart, m_ptDragEnd));
		break;

	default:
		break;
	}

	return(true);
}

bool CTitleScene::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SIZE:
		{
			auto pd3dDeviceContext = reinterpret_cast<ID3D11DeviceContext*>(wParam);

			auto m_nWndClientWidth = static_cast<LONG>(LOWORD(lParam));
			auto m_nWndClientHeight = static_cast<LONG>(HIWORD(lParam));
			m_Camera.SetViewport(pd3dDeviceContext, 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.0f, 1.0f);
		}
		return false;

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
	case WM_IME_CHAR:
	case WM_IME_STARTCOMPOSITION:
	case WM_IME_COMPOSITION:
	case WM_IME_COMPOSITIONFULL:
	case WM_IME_CONTROL:
	case WM_IME_ENDCOMPOSITION:
		if (!m_LusWrite.OnProcessingWindowMessage(hWnd, nMessageID, wParam, lParam) && !m_LusWrite.IsActive())
			OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		return false;
		break;

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MOUSEMOVE:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;

	default:
		return true;
	}

	return true;
}

bool CTitleScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	float fMoveUnit = 0.25f;
	float fRotateUnit = 5.f;
	switch (nMessageID)
	{
	case WM_KEYDOWN:

		switch (wParam)
		{

		case VK_LEFT:
			m_Camera.Rotate(0.f, -fRotateUnit, 0.f);
			break;

		case VK_RIGHT:
			m_Camera.Rotate(0.f, fRotateUnit, 0.f);
			break;

		case VK_UP:										    
			m_Camera.Rotate(-fRotateUnit, 0.f, 0.f);
			break;

		case VK_DOWN:									    
			m_Camera.Rotate(fRotateUnit, 0.f, 0.f);
			break;

		case 'A':
			m_Camera.MoveStrafe(-fMoveUnit);
			break;
		case 'W':
			m_Camera.MoveForward(+fMoveUnit);
			break;
		case 'D':
			m_Camera.MoveStrafe(+fMoveUnit);
			break;
		case 'S':
			m_Camera.MoveForward(-fMoveUnit);
			break;
		case 'Z':
			m_Camera.Move(XMVectorSet(0.f, +fMoveUnit, 0.f, 0.f));
//			m_Camera.MoveRise(+fMoveUnit);
			break;
		case 'X':
			m_Camera.Move(XMVectorSet(0.f, -fMoveUnit, 0.f, 0.f));
//			m_Camera.MoveRise(-fMoveUnit);
			break;

		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		}
		
		break;

	default:

		if (!m_LusWrite.IsActive())
		{
			HIMC hIMC;
			hIMC = ImmGetContext(hWnd);
			ImmSetConversionStatus(hIMC, IME_CMODE_ALPHANUMERIC, IME_SMODE_NONE);
			ImmReleaseContext(hWnd, hIMC);
		}
		break;
	}
	return(true);
}

void CTitleScene::BuildObjects(wstring Tag, HWND hWnd, CDirectXFramework *pMasterFramework)
{
	CScene::BuildObjects(Tag, hWnd, pMasterFramework);


}

void CTitleScene::BuildObjecsFromD3D11Devices(ID3D11Device * pd3dDevice, ID3D11DeviceContext * pd3dDeviceContext)
{
	shared_ptr<CMesh> pMesh = make_shared<CBoxMesh>(pd3dDevice);
	shared_ptr<CShader> pShader = make_shared<CBoxShader>();
	
	pShader->CreateVertexShaderAndInputLayout(LOADSCOPATH("PositionColorVertexShader.cso"), pd3dDevice);
	pShader->CreatePixelShader(LOADSCOPATH("PositionColorPixelShader.cso"), pd3dDevice);

	m_BoxObjects.EnquireShaderAndMesh(move(pShader), move(pMesh));
	m_BoxObjects.AddObject(XMVectorSet(0.f, 0.f, 0.f, 1.f));

	RECT m_rcClient;
	GetWindowRect(m_hWnd, &m_rcClient);

	// 시선은 y축을 따라 업 벡터가 있는 상태로 점 (0,-0.1,0)를 보며 (0,0.7,1.5)에 있습니다.
	static const XMVECTORF32 eye = { 0.0f, 0.7f, -5.5f, 0.0f };
	static const XMVECTORF32 at = { 0.0f, -0.1f, 0.0f, 0.0f };
	static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

	m_Camera.SetPosition(eye);

	m_Camera.GenerateProjectionMatrix(0.01f, 100.0f, static_cast<float>(m_rcClient.right / float(m_rcClient.bottom)), 60.0f);
	m_Camera.SetViewport(pd3dDeviceContext, 0, 0, m_rcClient.right, m_rcClient.bottom, 0.0f, 1.0f);

}

void CTitleScene::BuildObjecsFromD2D1Devices(ID2D1Device2 * pd2dDevice, ID2D1DeviceContext2 * pd2dDeviceContext, IDWriteFactory3 * pdwFactory)
{
	pd2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_pd2dsbrDefault);

	CreateLusWrite(pd2dDeviceContext, pdwFactory, Point2F(50, 100), RectF(0, 0, 200, 150), 15, TEXT("Arial"));
}

void CTitleScene::ReleaseObjects()
{
	SafeRelease(m_pd2dsbrDefault);
	m_LusWrite.ReleaseObjects();
}

void CTitleScene::Update2D(float fTimeElapsed)
{
	m_LusWrite.Update(fTimeElapsed);
}

void CTitleScene::Update3D(float fTimeElapsed)
{
	m_Camera.GenerateViewMatrix();
//	m_Camera.GenerateViewMatrix(XMVectorSet(0.0f, -0.8f, -1.5f, 0.f));
	m_BoxObjects.Update(fTimeElapsed);
}

void CTitleScene::AnimateObjects(float fTimeElapsed)
{
	Update2D(fTimeElapsed);
	Update3D(fTimeElapsed);

}

void CTitleScene::Render2D(ID2D1DeviceContext2 *pd2dDeviceContext)
{

		Matrix3x2F screenTranslation { Matrix3x2F::Identity() };
		pd2dDeviceContext->SetTransform(screenTranslation);

		auto oldColor = m_pd2dsbrDefault->GetColor();
		m_pd2dsbrDefault->SetColor(ColorF(ColorF::LightPink));
		m_pd2dsbrDefault->SetOpacity(0.25f);

		float fMargin = 3.5f;

		pd2dDeviceContext->FillRectangle(m_LusWrite.GetRect(fMargin), m_pd2dsbrDefault);

		m_pd2dsbrDefault->SetColor(oldColor);
		m_pd2dsbrDefault->SetOpacity(1.0f);
		pd2dDeviceContext->DrawRectangle(m_LusWrite.GetRect(fMargin), m_pd2dsbrDefault);

		m_LusWrite.Draw(pd2dDeviceContext);

}

void CTitleScene::Render3D(ID3D11DeviceContext *pd3dDeviceContext)
{
	m_Camera.UpdateShaderVariable(pd3dDeviceContext);
	m_BoxObjects.Render(pd3dDeviceContext);
}
