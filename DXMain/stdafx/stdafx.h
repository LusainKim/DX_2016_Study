// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#pragma warning(disable:4100)
#pragma warning(disable:4324)

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

// C 런타임 헤더 파일입니다.
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

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include "header\D2D_Operator.h"
#include "header\LusWrite.h"



// TODO: 프로그램 전반에 필요한 함수는 여기에서 참조합니다.
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
/// 안전하게 COM 객체를 1회 해제합니다. 해제한 뒤 객체를 가지고 있던 포인터를 nullptr로 초기화합니다.
/// </summary>
///	<param name = "obj"> 해제 후 초기화할 COM 객체입니다. 오로지 COM 객체여야만 합니다. </param>
/// <remarks> obj는 반드지 COM 객체여야만 합니다. <remarks>
/// <returns> 반환값은 존재하지 않습니다. </returns>
template<typename Ty>
inline void SafeRelease(Ty& obj) { if (obj != nullptr) { obj->Release(); obj = nullptr; } }

/// <summary>
/// HRESULT가 S_FALSE 일 경우 입력된 메시지를 입력한 메시지 박스를 출력하며 true를 반환합니다.
/// </summary>
///	<param name = "hr"> 평가할 HRSULT 객체입니다. S_FALSE이면 메시지 박스를 출력합니다. </param>
///	<param name = "msg"> 메시지입니다. 생략 가능합니다. </param>
///	<param name = "captionMsg"> 캡션 문장입니다. 생략 가능합니다. </param>
/// <returns> HRESULT 값이 S_FALSE일 경우 true를 반환합니다. </returns>
inline bool FailureMessage(HRESULT hr, LPCTSTR msg = TEXT(""), LPCTSTR captionMsg = TEXT("Error!"))
{
	if (FAILED(hr))
	{
		MessageBox(NULL, msg, captionMsg, MB_OK);
		return true;
	}
	return false;
}

// TODO: 프로그램에 필요한 전처리기는 여기에서 참조합니다.

#define CLIENT_WIDTH	800
#define CLIENT_HEIGHT	600

#define CLIENT_MINIMUM_WIDTH	600
#define CLIENT_MINIMUM_HEIGHT	480


#ifdef _DEBUG
#define LOADSCOPATH(path) TEXT(##path)
#else
#define LOADSCOPATH(path) TEXT("..\\x64\\Release\\"##path)
#endif

// 캡션 FPS 출력 여부 -------------------
// 항상 캡션에 FPS를 출력		(0 : 비활성 | 1 : 활성)
#define USE_CAPTIONFPS_ALWAYS	 1
#define USE_TITLE_STRING		 0
#define USE_LIMIT_MAX_FPS		 0


#if USE_CAPTIONFPS_ALWAYS
#define SHOW_CAPTIONFPS 
#elif _DEBUG	// Debug에서는 항상 실행
#define SHOW_CAPTIONFPS 
#endif

#if defined(SHOW_CAPTIONFPS)
#define MAX_UPDATE_FPS 1.0 / 3.0
#endif

// 최대 FPS
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

