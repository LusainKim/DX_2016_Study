// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �Ǵ� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#pragma warning(disable:4100)
#pragma warning(disable:4324)

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����:
#include <windows.h>

// C ��Ÿ�� ��� �����Դϴ�.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <assert.h>

// D3D11
#include <d3d11_2.h>
#include <dxgi1_3.h>

// D2D1
#include <d2d1_3.h>
#include <dwrite_3.h>
#include <d2d1_2helper.h>
#include <wincodec.h>

// DirectX Math
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

// C++11
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <algorithm>
#include <functional>
#include <memory>
#include <vector>
#include <list>
#include <deque>
#include <map>
#include <set>

using namespace std;
using namespace std::chrono;

using namespace D2D1;
using namespace DirectX;
using namespace DirectX::PackedVector;

// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.
#include "header\D2D_Operator.h"
#include "header\LusWrite.h"



// TODO: ���α׷� ���ݿ� �ʿ��� �Լ��� ���⿡�� �����մϴ�.
template<typename Ty, size_t N>
inline constexpr size_t GetArraySize(Ty(&)[N]) noexcept { return N; }


template <typename Frmwk>
inline Frmwk GetUserDataPtr(HWND hWnd) noexcept
{
	return reinterpret_cast<Frmwk>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
}

inline void SetUserDataPtr(HWND hWnd, LPVOID ptr) noexcept
{
	LONG_PTR result = ::SetWindowLongPtr(
		hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(ptr));

}

/// <summary>
/// �����ϰ� COM ��ü�� 1ȸ �����մϴ�. ������ �� ��ü�� ������ �ִ� �����͸� nullptr�� �ʱ�ȭ�մϴ�.
/// </summary>
///	<param name = "obj"> ���� �� �ʱ�ȭ�� COM ��ü�Դϴ�. ������ COM ��ü���߸� �մϴ�. </param>
/// <remarks> obj�� �ݵ��� COM ��ü���߸� �մϴ�. <remarks>
/// <returns> ��ȯ���� �������� �ʽ��ϴ�. </returns>
template<typename Ty>
inline void SafeRelease(Ty& obj) { if (obj != nullptr) { obj->Release(); obj = nullptr; } }

/// <summary>
/// HRESULT�� S_FALSE �� ��� �Էµ� �޽����� �Է��� �޽��� �ڽ��� ����ϸ� true�� ��ȯ�մϴ�.
/// </summary>
///	<param name = "hr"> ���� HRSULT ��ü�Դϴ�. S_FALSE�̸� �޽��� �ڽ��� ����մϴ�. </param>
///	<param name = "msg"> �޽����Դϴ�. ���� �����մϴ�. </param>
///	<param name = "captionMsg"> ĸ�� �����Դϴ�. ���� �����մϴ�. </param>
/// <returns> HRESULT ���� S_FALSE�� ��� true�� ��ȯ�մϴ�. </returns>
inline bool FailureMessage(HRESULT hr, LPCTSTR msg = TEXT(""), LPCTSTR captionMsg = TEXT("Error!"))
{
	if (FAILED(hr))
	{
		MessageBox(NULL, msg, captionMsg, MB_OK);
		return true;
	}
	return false;
}

// TODO: ���α׷��� �ʿ��� ��ó����� ���⿡�� �����մϴ�.

#define CLIENT_WIDTH	800
#define CLIENT_HEIGHT	600

#define CLIENT_MINIMUM_WIDTH	600
#define CLIENT_MINIMUM_HEIGHT	480


#ifdef _DEBUG
#define LOADSCOPATH(path) TEXT(##path)
#else
#define LOADSCOPATH(path) TEXT("..\\x64\\Release\\"##path)
#endif

// ĸ�� FPS ��� ���� -------------------
// �׻� ĸ�ǿ� FPS�� ���		(0 : ��Ȱ�� | 1 : Ȱ��)
#define USE_CAPTIONFPS_ALWAYS	 1
#define USE_TITLE_STRING		 0
#define USE_LIMIT_MAX_FPS		 0


#if USE_CAPTIONFPS_ALWAYS
#define SHOW_CAPTIONFPS 
#elif _DEBUG	// Debug������ �׻� ����
#define SHOW_CAPTIONFPS 
#endif

#if defined(SHOW_CAPTIONFPS)
#define MAX_UPDATE_FPS 1.0 / 3.0
#endif

// �ִ� FPS
#if _DEBUG
#define MAX_FPS 0.0
#elif USE_LIMIT_MAX_FPS
#define MAX_FPS 1.0 / 60.0
#else
#define MAX_FPS 0.0
#endif

#define TITLE_MAX_LENGTH 64

#if USE_TITLE_STRING
#define TITLESTRING	TEXT("Warp Framework")
#endif

