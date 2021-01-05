#pragma once
#include "afxwin.h"


// CUI 대화 상자입니다.

class CUI : public CDialogEx
{
	DECLARE_DYNAMIC(CUI)

public:
	CUI(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CUI();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UI };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCreate_Buffer();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedLoad();
	afx_msg void OnBnClickedApply();
	afx_msg void OnLbnSelchangeListBox();
	afx_msg void OnBnClickedUp();
	afx_msg void OnBnClickedLeft();
	afx_msg void OnBnClickedDown();
	afx_msg void OnBnClickedRight();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnInitDialog();
public:
	CListBox m_ListBox_Texture;
	CButton m_Radio_UI[4];
	CString m_strSizeX;
	CString m_strSIzeY;
public:
	vector<const _tchar*>		m_vecTextureName;
	_int						m_iTextureIdx = 0;
	const _tchar*				m_pTextureName;
	_bool						m_IsChangeTexture = false;
	_tchar*						m_pComponentTag = L"";
	TCHAR						szComponentTag[MAX_STR] = L"";
public:
	_bool						m_IsCreateUI = false;
	_bool						m_IsMove = false;
public:
	_bool						m_IsMove_Side = false;
	_bool						m_IsMove_Cross = false;
public:
	_float						m_fCrossCnt = 0.f;
	_float						m_fSideCnt = 0.f;

};
