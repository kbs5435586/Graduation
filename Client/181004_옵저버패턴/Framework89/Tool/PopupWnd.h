#pragma once


// CPopupWnd ��ȭ �����Դϴ�.

class CMySheet;
class CPopupWnd : public CDialog
{
	DECLARE_DYNAMIC(CPopupWnd)

public:
	CPopupWnd(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CPopupWnd();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_POPUPWND };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:
	CMySheet*	m_pMySheet;
	virtual BOOL OnInitDialog();
};
