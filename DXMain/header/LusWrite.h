#pragma once
#pragma comment(lib, "imm32")

class LusWrite {
public:

	BOOL MyGetImeMode(HWND hEdit)
	{
		HIMC hImc;
		DWORD Con, Sen;

		hImc = ImmGetContext(hEdit);
		ImmGetConversionStatus(hImc, &Con, &Sen);
		ImmReleaseContext(hEdit, hImc);
		if (Con & IME_CMODE_NATIVE) {
			return TRUE;
		}
		else {
			return FALSE;
		}
	}

	void MySetImeMode(HWND hEdit, BOOL bHan)
	{
		HIMC hImc;
		hImc = ImmGetContext(hEdit);

		if (bHan == TRUE) {
			ImmSetConversionStatus(hImc, IME_CMODE_NATIVE, IME_SMODE_NONE);
		}
		else {
			ImmSetConversionStatus(hImc, 0, IME_SMODE_NONE);
		}

		ImmReleaseContext(hEdit, hImc);
	}

	template<typename Val> inline
		Val sideStore(Val min, Val value, Val max)
	{
		Val return_val = value;
		if (min > value) return_val = min;
		if (value > max) return_val = max;
		return return_val;
	}

protected:
	D2D_POINT_2F				m_ptHeader;
	D2D_RECT_F					m_rcBoxSize;

	bool						m_bComp = false;
	vector<wstring>				m_strMsg { wstring() };
	size_t						m_iRow = 0;
	size_t						m_iColumn = 0;

	float						m_fFontHeight = 0;

	HWND						m_hWnd = NULL;

	IDWriteFactory			*	m_pdwFactory = nullptr;
	IDWriteTextFormat		*	m_pdwTextFormat = nullptr;
	IDWriteTextLayout		*	m_pdwTextLayout = nullptr;
	ID2D1SolidColorBrush	*	m_pd2d1sbrText = nullptr;
	ID2D1SolidColorBrush	*	m_pd2d1sbrCaret = nullptr;

	D2D_RECT_F					m_rcCaret;
	float						m_fTick = 0;
	float						m_fSwapCaretState = 0.3f;
	bool						m_bCaretState = false;

	bool						m_bActive = false;

	BOOL						m_bOpenStatus = 0;

public:

	LusWrite() = default;

	virtual ~LusWrite()
	{
	}

	void ReleaseObjects()
	{
		if (m_pdwFactory) { m_pdwFactory->Release(); m_pdwFactory = nullptr; };
		if (m_pdwTextFormat) { m_pdwTextFormat->Release(); m_pdwTextFormat = nullptr; };
		if (m_pdwTextLayout) { m_pdwTextLayout->Release(); m_pdwTextLayout = nullptr; };
		if (m_pd2d1sbrText) { m_pd2d1sbrText->Release(); m_pd2d1sbrText = nullptr; };
		if (m_pd2d1sbrCaret) { m_pd2d1sbrCaret->Release(); m_pd2d1sbrCaret = nullptr; };

	}

	bool IsActive() const { return m_bActive; }

	void Initialize(HWND hWnd, D2D_POINT_2F pt, D2D_RECT_F rcSize, ID2D1DeviceContext* m_pd2d1DC, IDWriteFactory* pdwFactory)
	{
		m_hWnd = hWnd;
		m_ptHeader = pt;
		m_rcBoxSize = rcSize;
		MySetImeMode(m_hWnd, TRUE);

		if (m_pdwFactory)m_pdwFactory->Release();
		m_pdwFactory = pdwFactory;
		if (m_pdwFactory) m_pdwFactory->AddRef();
		if (m_pd2d1DC)
		{
			m_pd2d1DC->AddRef();
			m_pd2d1DC->CreateSolidColorBrush(ColorF(ColorF::White), &m_pd2d1sbrText);

			auto Color = (ColorF::White - ColorF::White);
			m_pd2d1DC->CreateSolidColorBrush(ColorF(Color, 0.3f), &m_pd2d1sbrCaret);
			m_pd2d1DC->Release();
		}
	}

	void ChangeTextColor(ColorF color) { m_pd2d1sbrText->SetColor(color); }
	void ChangeTextOpacity(FLOAT opacity) { m_pd2d1sbrText->SetOpacity(opacity); }

	void Resize(D2D_RECT_F rcSize)
	{
		m_ptHeader = Point2F(rcSize.left, rcSize.top);
		m_rcBoxSize = (-1 * m_ptHeader) + rcSize;

		ReBuildMessage();
		SetCaret();
	}

	void MakeFont(float FontSize, LPWSTR Fontname = L"Arial")
	{
		IDWriteTextFormat *pdwTextFormatTemp = nullptr;
		m_pdwFactory->CreateTextFormat(Fontname
			, nullptr
			, DWRITE_FONT_WEIGHT_NORMAL
			, DWRITE_FONT_STYLE_NORMAL
			, DWRITE_FONT_STRETCH_NORMAL
			, FontSize
			, L"ko-kr"
			, reinterpret_cast<IDWriteTextFormat**>(&pdwTextFormatTemp)
		);

		MakeFont(pdwTextFormatTemp);
		pdwTextFormatTemp->Release();
	}

	void MakeFont(IDWriteTextFormat* font)
	{
		if (m_pdwTextFormat) m_pdwTextFormat->Release();

		m_pdwTextFormat = font;
		if (m_pdwTextFormat) m_pdwTextFormat->AddRef();
		m_fFontHeight = m_pdwTextFormat->GetFontSize();
		ReBuildMessage();
		SetCaret();
	}

	void moveCursorColumn(int nPos)
	{
		m_iColumn = static_cast<size_t>(sideStore(0, nPos + static_cast<int>(m_iColumn), static_cast<int>(m_strMsg[m_iRow].length())));
	}
	void moveCursorRow(int nPos)
	{
		m_iRow = static_cast<size_t>(sideStore(0, nPos + static_cast<int>(m_iRow), static_cast<int>(m_strMsg.size()) - 1));
	}

	void Insert(LPWSTR str)
	{
		if (lstrlen(str) == 0) return;
		m_strMsg[m_iRow].insert(m_iColumn, str);
		ReBuildMessage();
	}

	void Delete(size_t nCount)
	{
		if (m_strMsg[m_iRow].length() < nCount)
			nCount = m_strMsg[m_iRow].length();
		if (m_iColumn > 0)
			m_strMsg[m_iRow].erase(m_iColumn - 1, nCount);
		ReBuildMessage();
	}

	void ReBuildMessage()
	{
		if (m_pdwTextLayout) m_pdwTextLayout->Release();

		wstring str;
		for (auto& p : m_strMsg)
		{
			str += p;
			str += '\n';
		}
		str.pop_back();

		m_pdwFactory->CreateTextLayout(str.c_str()
			, static_cast<UINT>(str.length())
			, m_pdwTextFormat
			, m_rcBoxSize.right - m_rcBoxSize.left
			, m_rcBoxSize.bottom - m_rcBoxSize.top
			, reinterpret_cast<IDWriteTextLayout**>(&m_pdwTextLayout)
		);
		if (m_pdwTextLayout)
		{
			m_pdwTextLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_JUSTIFIED);
			m_pdwTextLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
		}
	}

	void Update(float fTimeElapsed)
	{
		m_fTick += fTimeElapsed;
		if (m_fTick > m_fSwapCaretState)
		{
			m_fTick -= m_fSwapCaretState;
			m_bCaretState = !m_bCaretState;
		}
	}

	void Click(D2D_POINT_2F pt)
	{
		RECT rcBox {
			  static_cast<LONG>(m_ptHeader.x + m_rcBoxSize.left)
			, static_cast<LONG>(m_ptHeader.y + m_rcBoxSize.top)
			, static_cast<LONG>(m_ptHeader.x + m_rcBoxSize.right)
			, static_cast<LONG>(m_ptHeader.y + m_rcBoxSize.bottom)
		};
		POINT ptCursor { static_cast<LONG>(pt.x), static_cast<LONG>(pt.y) };

		if (!PtInRect(&rcBox, ptCursor))
		{
			m_bActive = false;

			HIMC hIMC = ImmGetContext(m_hWnd);
			m_bOpenStatus = ImmGetOpenStatus(hIMC);
			// 무조건 영문 입력모드로 한다.
			ImmSetConversionStatus(hIMC, IME_CMODE_ALPHANUMERIC, IME_SMODE_NONE);
			ImmReleaseContext(m_hWnd, hIMC);

			return;
		}

		if (!m_bActive)
		{
			m_bActive = true;
			HIMC hIMC = ImmGetContext(m_hWnd);
			ImmSetConversionStatus(hIMC, m_bOpenStatus, IME_SMODE_NONE);
			ImmReleaseContext(m_hWnd, hIMC);
			return;

		}

		if (!m_pdwTextLayout) return;

		RECT rcCol;

		FLOAT caretX, caretY;
		DWRITE_HIT_TEST_METRICS dwHitTestMtx;

		m_pdwTextLayout->HitTestTextPosition(0, false, &caretX, &caretY, &dwHitTestMtx);

		if (pt.y <  m_ptHeader.y + caretY)
		{
			m_iColumn = 0;
			m_iRow = 0;
			SetCaret();
			return;
		}

		UINT32 stringLength = 0;

		int calcColumn = 0;
		int calcRow = 0;

		for (auto& p : m_strMsg)
		{
			calcColumn = 0;
			for (auto &q : p)
			{
				m_pdwTextLayout->HitTestTextPosition(stringLength++, false, &caretX, &caretY, &dwHitTestMtx);
				rcCol = RECT {
					static_cast<LONG>(m_ptHeader.x + caretX)
					, static_cast<LONG>(m_ptHeader.y + caretY)
					, static_cast<LONG>(m_ptHeader.x + caretX + dwHitTestMtx.width)
					, static_cast<LONG>(m_ptHeader.y + caretY + dwHitTestMtx.height)
				};

				if (PtInRect(&rcCol, ptCursor))
				{
					m_iRow = calcRow;
					m_iColumn = calcColumn;
					SetCaret();
					return;
				}
				calcColumn++;
			}

			if (p.length() == 0)
				caretY += m_fFontHeight;

			if (pt.y <  m_ptHeader.y + caretY + dwHitTestMtx.height)
			{
				m_iColumn = calcColumn;
				m_iRow = calcRow;
				SetCaret();
				return;
			}

			calcRow++;
			// '\n'
			stringLength++;
		}

		m_iRow = calcRow;
		m_iColumn = calcColumn;
		SetCaret();

	}

	void SetCaret()
	{
		if (!m_pdwTextLayout) return;

		UINT32 stringLength = 0;
		D2D_POINT_2F caret { 0.f,0.f };
		FLOAT caretX, caretY;
		DWRITE_HIT_TEST_METRICS dwHitTestMtx;

		for (size_t idx = 0; idx < m_iRow; ++idx)
			stringLength += static_cast<UINT32>(m_strMsg[idx].length() + 1);

		m_pdwTextLayout->HitTestTextPosition(static_cast<UINT32>(stringLength + (!m_bComp ? m_iColumn : m_iColumn - 1)), false, &caretX, &caretY, &dwHitTestMtx);
		caret.y += (dwHitTestMtx.top);
		caret.x = caretX;

		FLOAT caretWidth = (m_bComp ? static_cast<int>(max(2.5f, dwHitTestMtx.width)) : 2.5f);

		m_rcCaret = RectF(
			  m_ptHeader.x + caret.x
			, m_ptHeader.y + caret.y
			, m_ptHeader.x + caret.x + caretWidth
			, m_ptHeader.y + caret.y + dwHitTestMtx.height
		);

	}

	bool OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
	{
		if (!m_bActive) return false;

		switch (nMessageID)
		{
		case WM_IME_STARTCOMPOSITION:
			break;
		case WM_IME_COMPOSITION:
		{
			if (lParam & GCS_COMPSTR)
			{
				TCHAR szChar[2] = TEXT("");
				szChar[0] = static_cast<TCHAR>(wParam);
				szChar[1] = 0;
				if (wParam < 'A') szChar[0] = 0;

				if (m_bComp) {
					Delete(1);
					m_iColumn--;

				}
				if (lstrlen(szChar) == 0) {
					m_bComp = FALSE;
				}
				else {
					m_bComp = TRUE;
				}

				Insert(szChar);
				m_iColumn += lstrlen(szChar);

			}
			if (lParam & GCS_RESULTSTR)
			{
				HIMC hImc;
				int len;
				TCHAR *szComp;

				hImc = ImmGetContext(hWnd);
				len = ImmGetCompositionString(hImc, GCS_RESULTSTR, NULL, 0);
				szComp = new TCHAR[len];
				ImmGetCompositionString(hImc, GCS_RESULTSTR, szComp, len);
				szComp[len - 1] = 0;
				ImmReleaseContext(hWnd, hImc);
				if (m_bComp) {
					Delete(1);
					m_iColumn--;
				}
				Insert(szComp);
				m_iColumn += lstrlen(szComp);
				delete[] szComp;
				m_bComp = FALSE;

			}

			SetCaret();

			return (lParam & GCS_COMPSTR) ? false : true;

		}
		break;
		case WM_CHAR:
		{
			if (wParam == VK_BACK || wParam == VK_RETURN || wParam == VK_ESCAPE)
				break;

			TCHAR szChar[3];
			int i;

			szChar[0] = (BYTE) wParam;
			szChar[1] = 0;
			for (i = 0; i < LOWORD(lParam); i++) {
				Insert(szChar);
				m_iColumn += lstrlen(szChar);
			}

			m_bComp = FALSE;

			SetCaret();
		}
		break;

		case WM_KEYDOWN:

			switch (wParam)
			{
			case VK_LEFT:
				if (m_iColumn > 0)
					moveCursorColumn(-1);
				else if (m_iRow > 0)
				{
					moveCursorRow(-1);
					m_iColumn = m_strMsg[m_iRow].length();
				}
				SetCaret();

				break;
			case VK_RIGHT:
				if (m_strMsg[m_iRow].length() > m_iColumn)
					moveCursorColumn(1);
				else if (m_iRow < m_strMsg.size() - 1)
				{
					moveCursorRow(1);
					m_iColumn = 0;
				}
				SetCaret();

				break;
			case VK_DELETE:

				if (m_strMsg[m_iRow].length() > m_iColumn)
					moveCursorColumn(1);
				else if (m_iRow < m_strMsg.size() - 1)
				{
					moveCursorRow(1);
					m_iColumn = 0;
				}
				else break;

				if (m_iColumn == 0)
				{
					if (m_iRow > 0)
					{
						m_iColumn = m_strMsg[m_iRow - 1].length();
						m_strMsg[m_iRow - 1] += m_strMsg[m_iRow];
						m_strMsg.erase(m_strMsg.begin() + m_iRow);
						m_iRow--;
					}

				}
				else if (m_iColumn > 0)
				{
					Delete(1);
					m_iColumn--;
				}
				ReBuildMessage();
				SetCaret();

				break;

			case VK_BACK:
				if (m_iColumn == 0)
				{
					if (m_iRow > 0)
					{
						m_iColumn = m_strMsg[m_iRow - 1].length();
						m_strMsg[m_iRow - 1] += m_strMsg[m_iRow];
						m_strMsg.erase(m_strMsg.begin() + m_iRow);
						m_iRow--;
					}

				}
				else if (m_iColumn > 0)
				{
					Delete(1);
					m_iColumn--;
				}
				ReBuildMessage();
				SetCaret();

				break;

			case VK_END:
				m_iColumn = m_strMsg[m_iRow].length();
				SetCaret();
				break;

			case VK_HOME:
				m_iColumn = 0;
				SetCaret();
				break;

			case VK_ESCAPE:
				m_bComp = false;
				break;

			case VK_RETURN:
				m_strMsg.insert(m_strMsg.begin() + m_iRow + 1, wstring(m_strMsg[m_iRow].begin() + m_iColumn, m_strMsg[m_iRow].end()));
				m_strMsg[m_iRow].erase(m_iColumn);
				m_iRow++;
				m_iColumn = 0;
				ReBuildMessage();
				SetCaret();

				break;
			default:
				return false;
			}

			break;
		default:
			return false;
		}
		return true;
	}

	void Draw(ID2D1RenderTarget* pd2d1RenderTarget)
	{
		if (m_pdwTextLayout)
		{
			pd2d1RenderTarget->DrawTextLayout(m_ptHeader, m_pdwTextLayout, m_pd2d1sbrText, D2D1_DRAW_TEXT_OPTIONS_CLIP);
			if (m_bCaretState && m_bActive)
				pd2d1RenderTarget->FillRectangle(m_rcCaret, m_pd2d1sbrCaret);
		}
	}

	D2D_RECT_F GetRect(FLOAT margin = 0.f) const { return m_rcBoxSize + m_ptHeader + RectF(-margin, -margin, margin, margin); }
	void SetPosition(D2D_POINT_2F pt) { m_ptHeader = pt; }

};

