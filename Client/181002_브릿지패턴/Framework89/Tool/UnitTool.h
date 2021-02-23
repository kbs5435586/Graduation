#pragma once
#include "afxwin.h"


// CUnitTool ��ȭ �����Դϴ�.

class CUnitTool : public CDialog
{
	DECLARE_DYNAMIC(CUnitTool)

public:
	CUnitTool(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CUnitTool();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UNITTOOL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
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
	// �Է� ���ڿ�
	CString m_strInput;	
	// ��� ���ڿ�
	CString m_strOutput;

	// ĳ���� �̸�
	CString m_strName;
	// ĳ���� ���ݷ�
	int m_iAtt;
	// ĳ���� ����
	int m_iDef;

	// ���ڿ� �˻�
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
