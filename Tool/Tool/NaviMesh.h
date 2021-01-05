#pragma once


// CNaviMesh 대화 상자입니다.

class CNaviMesh : public CDialogEx
{
	DECLARE_DYNAMIC(CNaviMesh)

public:
	CNaviMesh(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CNaviMesh();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NAVIMESH };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedApply();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedLoad();
};
