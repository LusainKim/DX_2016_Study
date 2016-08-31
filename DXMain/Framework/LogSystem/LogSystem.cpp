#include "stdafx.h"
#include "LogSystem.h"
#include "Objects\Base\Object.h"

bool CLogSystem::m_bCreated = false;
CMeasureTimeElapsedObserver* CLogSystem::CTimeElapsedObs = nullptr;

CLogSystem::~CLogSystem()
{
	m_bCreated = false;
	if (m_hWnd) DestroyWindow(m_hWnd);
	ReleaseBackBufferResources();
}

bool CLogSystem::Initialize(HWND hParentWnd, HINSTANCE hInstance)
{
	if (m_bCreated) return false;

	if (CTimeElapsedObs == nullptr) CTimeElapsedObs = new CMeasureTimeElapsedObserver();

	MyRegisterClass(hInstance);

	if(!CreateLogWindow(hParentWnd, hInstance)) return false;

	CreateBackBuffer();

	// Run Draw Timer
	SetTimer(m_hWnd, WT_DEBUG_DRWACALL, m_TickFrequency, NULL);



	return m_bCreated = true;
}

void CLogSystem::Draw(HDC hdc)
{
	FillRect(m_hDCBackBuffer, &m_rcClient, static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));

	GetTimeElapsedObserver()->Draw(m_hDCBackBuffer, RECT { 10, 10, m_rcClient.right - 10, m_rcClient.bottom });


	BitBlt(hdc, 0, 0, m_rcClient.right, m_rcClient.bottom, m_hDCBackBuffer, 0, 0, SRCCOPY);
}

ATOM CLogSystem::MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style =
		  CS_HREDRAW 	// Ŭ���̾�Ʈ�� �ʺ� �����ϸ�, ��ü �����츦 �ٽ� �׸��� �Ѵ�.
		| CS_VREDRAW	// Ŭ���̾�Ʈ�� ���̸� �����ϸ�, ��ü �����츦 �ٽ� �׸��� �Ѵ�.
	//	| CS_DBLCLKS	// �ش� �����쿡�� ����Ŭ���� ����ؾ� �Ѵٸ� �߰��ؾ� �Ѵ�.
		;
	wcex.lpfnWndProc = CLogSystem::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;	//	�ش� ���α׷��� �ν��Ͻ� �ڵ�.

//	MAKEINTERSOURCE : Make Inter Source. ���α׷� ���ο� �ִ� ���ҽ��� �ε����� ��ȯ�ϴ� ��ũ��.
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = m_szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	return RegisterClassEx(&wcex);
}

bool CLogSystem::CreateLogWindow(HWND hParentWnd, HINSTANCE hInstance)
{
	//	������ ��Ÿ��
	DWORD dwStyle =
		  WS_OVERLAPPED 	// ����Ʈ ������. Ÿ��Ʋ �ٿ� ũ�� ������ �ȵǴ� ��輱�� ������. �ƹ��� ��Ÿ�ϵ� ���� ������ �� ��Ÿ���� ����ȴ�.
		| WS_CAPTION 		// Ÿ��Ʋ �ٸ� ���� �����츦 ����� WS_BORDER ��Ÿ���� �����Ѵ�.
//		| WS_SYSMENU		// �ý��� �޴��� ���� �����츦 �����.
//		| WS_MINIMIZEBOX	// �ּ�ȭ ��ư�� �����.
//		| WS_BORDER			// �ܼ����� �� ��輱�� ����� ũ�� ������ �� �� ����.
		| WS_THICKFRAME		// ũ�� ������ ������ �β��� ��輱�� ������. WS_BORDER�� ���� ����� �� ����.
		;					// �߰��� �ʿ��� ������ ��Ÿ���� http://www.soen.kr/lecture/win32api/reference/Function/CreateWindow.htm ����.

	//	����ũ�� ������ ������
	RECT rcParentWindow;
	GetWindowRect(hParentWnd, &rcParentWindow);

	//	Ŭ���̾�Ʈ ������
	m_rcClient.left = m_rcClient.top = 0;
	m_rcClient.right = DEBUG_CLIENT_WIDTH;
	m_rcClient.bottom = DEBUG_CLIENT_HEIGHT;
	//	������ ����� ������ �߰��Ǵ�(ĸ��, �ܰ��� ��) ũ�⸦ ����.
	AdjustWindowRect(&m_rcClient, dwStyle, FALSE);
	
	m_rcClient.right -= m_rcClient.left;
	m_rcClient.bottom -= m_rcClient.top;
	m_rcClient.left = m_rcClient.top = 0;

	//	Ŭ���̾�Ʈ ������ǥ(left, top)
	//	�θ� Ŭ���̾�Ʈ�� �»�ܿ� ��ġ�ϵ��� ����
	POINT ptClientWorld;
	ptClientWorld.x = rcParentWindow.right + m_ptStartPosition.x;
	ptClientWorld.y = rcParentWindow.top + m_ptStartPosition.y;

	//	������ ����
	m_hWnd = CreateWindow(
		  m_szWindowClass		//	������ Ŭ���� ��
		, m_szTitle				//	ĸ�� ǥ�� ���ڿ�
		, dwStyle				//	������ ��Ÿ��
		, ptClientWorld.x		//	�θ� ������ ��� ������ ������ǥ : x
		, ptClientWorld.y		//	�θ� ������ ��� ������ ������ǥ : y
		, m_rcClient.right		//	������ ������ : width
		, m_rcClient.bottom		//	������ ������ : height
		, hParentWnd			//	�θ� ������.
		, NULL					//	�޴� �ڵ�
		, hInstance				//	�ν��Ͻ� �ڵ�
		, NULL					//	�߰� �Ķ���� : NULL
	);

	if (!m_hWnd)
	{
		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf, 0, NULL);
		MessageBox(NULL, (LPCTSTR) lpMsgBuf, L"Create Debug Window Fail", MB_ICONERROR);
		LocalFree(lpMsgBuf);
		return false;
	}

	// Ŭ������ ������ ���ν��� ����
	::SetUserDataPtr(m_hWnd, this);

	//	������ ǥ��
	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);
	return true;
}

void CLogSystem::CreateBackBuffer()
{
	ReleaseBackBufferResources();

	HDC hdc = GetDC(m_hWnd);
	{
		GetClientRect(m_hWnd, &m_rcClient);

		m_hDCBackBuffer = CreateCompatibleDC(hdc);
		m_hbmpBackBufffer = CreateCompatibleBitmap(hdc, m_rcClient.right, m_rcClient.bottom);

		SelectObject(m_hDCBackBuffer, m_hbmpBackBufffer);

		InvalidateRect(m_hWnd, NULL, FALSE);
	}
	ReleaseDC(m_hWnd, hdc);
}

void CLogSystem::ReleaseBackBufferResources()
{
	if (m_hDCBackBuffer)
	{
		SelectObject(m_hDCBackBuffer, static_cast<HBITMAP>(NULL));
		DeleteDC(m_hDCBackBuffer);
		m_hDCBackBuffer = NULL;
	}
	if (m_hbmpBackBufffer)
	{
		DeleteObject(m_hbmpBackBufffer);
		m_hbmpBackBufffer = NULL;
	}
}

LRESULT CLogSystem::WndProc(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	CLogSystem* self = ::GetUserDataPtr<CLogSystem*>(hWnd);

	if (!self)
		return ::DefWindowProc(hWnd, nMessageID, wParam, lParam);

	switch (nMessageID)
	{
	case WM_CREATE:
		break;

	case WM_SYSKEYDOWN:
		switch (wParam)
		{
		case VK_F4:
			return 0;
		}
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_SPACE:
			self->GetTimeElapsedObserver()->RenewalViewList();
			break;
		}
		break;

	case WM_TIMER:
		switch (wParam)
		{
		case WT_DEBUG_DRWACALL:
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		}
		break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = ::BeginPaint(hWnd, &ps);
		self->Draw(hdc);
		::EndPaint(hWnd, &ps);
	}
	break;

	default:
		return DefWindowProc(hWnd, nMessageID, wParam, lParam);
	}
	
	return 0;
}

inline void CMeasureTimeElapsedObserver::RenewalViewList()
{
	string filename = "MeasureTimeElapse.log";

	ifstream ifs(filename, ios::in);
	if (!ifs.is_open())
	{
		ifs.close();

		ofstream ofs(filename, ios::out);
		auto D = MessageBox(NULL, TEXT("view list ������ �����ϴ�. ���� �ۼ��մϴ�. ��� �ð� ����� ����մϱ�?"), TEXT("����"), MB_YESNO);
		if (D == IDYES)
		{
			for (auto s : m_mapFunctionTimeElapsed)
				ofs << s.first << endl;

		}
		ofs.close();
		return RenewalViewList();
	}

	while (!ifs.eof())
	{
		string str;
		ifs >> str;

		if (str.length() > 0)
			m_vViewStrings.emplace_back(move(str));
	}
	ifs.close();
}

void CMeasureTimeElapsedObserver::Draw(HDC hdc, RECT rcDrawArea)
{
	POINT ptStart { rcDrawArea.left, rcDrawArea.top };

	int iHeight = 0;
	for (auto p : m_vViewStrings)
	{
		const size_t newsizew = strlen(p.c_str()) + 1;
		size_t convertedChars = 0;
		wstring wstr;
		wstr.resize(newsizew);
		mbstowcs_s(&convertedChars, &(wstr[0]), newsizew, p.c_str(), _TRUNCATE);
		if (wstr.back() == 0) wstr.pop_back();

		long long maxcount = 0;

		for (auto q : m_mapFunctionTimeElapsed[p])
			maxcount += q;

		wstring soutput = wstr + L" : " + ((m_mapFunctionTimeElapsed[p].size() > 0) ?
			FixValue(maxcount / m_mapFunctionTimeElapsed[p].size()) : L"0")
			+ L" / " + FixValue(m_mapFunctionTimeElapsed[p].size()) + L" ȸ";

		RECT rc { rcDrawArea.left, rcDrawArea.top + iHeight, rcDrawArea.right, rcDrawArea.bottom };

		iHeight += DrawText(hdc, soutput.c_str(), -1, &rc, DT_LEFT | DT_SINGLELINE);
	}

}

void CCustomLoggerObserver::RenewalViewList()
{
	string filename = "CustomLogger.log";

	ifstream ifs(filename, ios::in);
	if (!ifs.is_open())
	{
		ifs.close();

		ofstream ofs(filename, ios::out);
		auto D = MessageBox(NULL, TEXT("view list ������ �����ϴ�. ���� �ۼ��մϴ�. ��� �ΰŸ� ����մϱ�?"), TEXT("����"), MB_YESNO);
		if (D == IDYES)
		{
			for (auto s : m_mapStringLogger)
				ofs << s.first << endl;

		}
		ofs.close();
		return RenewalViewList();
	}

	while (!ifs.eof())
	{
		string str;
		ifs >> str;

		if (str.length() > 0)
			m_vViewStrings.emplace_back(move(str));
	}
	ifs.close();
}

void CCustomLoggerObserver::Draw(HDC hdc, RECT rcDrawArea)
{
	POINT ptStart { rcDrawArea.left, rcDrawArea.top };

	int iHeight = 0;
	for (auto p : m_vViewStrings)
	{
		string str = p + " : " + m_mapStringLogger[p];

		const size_t newsizew = strlen(str.c_str()) + 1;
		size_t convertedChars = 0;
		wstring wstr;
		wstr.resize(newsizew);
		mbstowcs_s(&convertedChars, &(wstr[0]), newsizew, str.c_str(), _TRUNCATE);
		if (wstr.back() == 0) wstr.pop_back();

		RECT rc { rcDrawArea.left, rcDrawArea.top + iHeight, rcDrawArea.right, rcDrawArea.bottom };

		iHeight += DrawText(hdc, wstr.c_str(), -1, &rc, DT_LEFT | DT_SINGLELINE);
	}
}
