#pragma once

struct Event 
{
protected:
	USHORT	ID;

public:

	bool CheckType(USHORT id) const { return ID == id; }

	enum : USHORT {
		  unknown = 0
		, MeasureTimeElapsed
		, CustomLogger
	//	, ...
		, count
	};

};

class CObserver {
	
protected:

	friend class CLogSystem;
	CObserver() = default;

public:

	virtual ~CObserver() {}
	virtual void onNotify(const class CObject* obj, Event* _event) = 0;
};

struct Event_MeasureTimeElapsed
	: public Event
{
	Event_MeasureTimeElapsed() { ID = MeasureTimeElapsed; }
	Event_MeasureTimeElapsed(string str, long long te)
		: Event_MeasureTimeElapsed()
	{
		strTag = str;
		llTimeElapsed = te;
	}

	string		strTag;
	long long	llTimeElapsed = 0LL;
};

class CMeasureTimeElapsedObserver
	: public CObserver
{
	using MyEvent = Event_MeasureTimeElapsed;

private:

	map<string, vector<long long>>	m_mapFunctionTimeElapsed;
	vector<string>					m_vViewStrings;

public:

	CMeasureTimeElapsedObserver() 
	{
		RenewalViewList();
	}
	virtual ~CMeasureTimeElapsedObserver() {}

	void RenewalViewList();

	virtual void onNotify(const class CObject* obj, Event* _event)
	{
		if (!_event->CheckType(Event::MeasureTimeElapsed)) return;
		auto ThisEvent = static_cast<MyEvent*>(_event);
		m_mapFunctionTimeElapsed[ThisEvent->strTag].emplace_back(ThisEvent->llTimeElapsed);
	}

	void Draw(HDC hdc, RECT rcDrawArea);

};

struct Event_CutsomLogger
	: public Event
{
	Event_CutsomLogger() { ID = CustomLogger; }
	Event_CutsomLogger(string str, string log)
		: Event_CutsomLogger()
	{
		strTag = str;
		strLog = log;
	}

	string		strTag;
	string		strLog;
};

class CCustomLoggerObserver
	: public CObserver
{
	using MyEvent = Event_CutsomLogger;

private:

	map<string, string>				m_mapStringLogger;
	vector<string>					m_vViewStrings;

public:

	CCustomLoggerObserver()
	{
		RenewalViewList();
	}
	virtual ~CCustomLoggerObserver() {}

	void RenewalViewList();

	virtual void onNotify(const class CObject* obj, Event* _event)
	{
		if (!_event->CheckType(Event::CustomLogger)) return;
		auto ThisEvent = static_cast<MyEvent*>(_event);
		m_mapStringLogger[ThisEvent->strTag] = ThisEvent->strLog;
	}

	void Draw(HDC hdc, RECT rcDrawArea);

};




class CLogSystem {
	
private:
#define WT_DEBUG_DRWACALL 0

public:

	CLogSystem() = default;
	virtual ~CLogSystem();

	bool Initialize(HWND hParentWnd, HINSTANCE hInstance);

	void Draw(HDC hdc);

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

protected:

	ATOM MyRegisterClass(HINSTANCE hInstance);
	bool CreateLogWindow(HWND hParentWnd, HINSTANCE hInstance);
	void CreateBackBuffer();
	void ReleaseBackBufferResources();

	static bool		m_bCreated;

	HWND			m_hWnd				{ nullptr };
	LPCTSTR			m_szWindowClass		{ TEXT("CLogSystem") };
	LPCTSTR			m_szTitle			{ TEXT("Log Window") };
	POINT			m_ptStartPosition	{ 20,20	};


	RECT			m_rcClient					;

	HDC				m_hDCBackBuffer		{ NULL };
	HBITMAP			m_hbmpBackBufffer	{ NULL };


	LONG			m_TickFrequency		{ 100 }	;

protected:

	static CMeasureTimeElapsedObserver	*CTimeElapsedObs	;

public:

	static auto* GetTimeElapsedObserver() { return CTimeElapsedObs; }

};