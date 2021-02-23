#pragma once
#include "afxwin.h"


// CUnitTool 대화 상자입니다.

class CUnitTool : public CDialog
{
	DECLARE_DYNAMIC(CUnitTool)

public:
	CUnitTool(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CUnitTool();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UNITTOOL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:	// massage function
	afx_msg void OnBnClickedPush();
	afx_msg void OnBnClickedAdd();
	afx_msg void OnLbnSelchangeListBox();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedLoad();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnEnChangeSearchName();

public:						// value
	// 입력 문자열
	CString m_strInput;	
	// 출력 문자열
	CString m_strOutput;

	// 캐릭터 이름
	CString m_strName;
	// 캐릭터 공격력
	int m_iAtt;
	// 캐릭터 방어력
	int m_iDef;

	// 문자열 검색
	CString m_strSearch;

public:						// control
	CListBox m_ListBox;
	CButton m_Radio[3];
	CButton m_CheckBox[3];
	CButton m_BTNPUSH;

public:						// user
	map<CString, UNITDATA*>	m_MapData;		
	HBITMAP					m_hBitmap;	
};
