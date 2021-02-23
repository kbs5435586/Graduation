#pragma once


// CPopupWnd 대화 상자입니다.

class CMySheet;
class CPopupWnd : public CDialog
{
	DECLARE_DYNAMIC(CPopupWnd)

public:
	CPopupWnd(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CPopupWnd();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_POPUPWND };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	CMySheet*	m_pMySheet;
	virtual BOOL OnInitDialog();
};
