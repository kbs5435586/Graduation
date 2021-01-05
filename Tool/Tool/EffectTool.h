#pragma once
#include "afxwin.h"


// CEffectTool 대화 상자입니다.

class CEffectTool : public CDialogEx
{
	DECLARE_DYNAMIC(CEffectTool)

public:
	CEffectTool(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CEffectTool();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EFFECTTOOL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLbnSelchangeEffectTexture();
	afx_msg void OnBnClickedApply();
	afx_msg void OnBnClickedUpdate();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnBnClickedCreate();
	virtual BOOL OnInitDialog();
public:
	CListBox m_ListBox_Effect_Texture;
	CString m_strMoveX;
	CString m_strMoveY;
	CString m_strMoveZ;
	CString m_strRotateX;
	CString m_strRotateY;
	CString m_strRotateZ;
public:
	map<const _tchar*, _int>		m_mapEffectMesh;

};
