#pragma once
#include "afxwin.h"


// CMapTool ��ȭ �����Դϴ�.

class CMapTool : public CDialog
{
	DECLARE_DYNAMIC(CMapTool)

public:
	CMapTool(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CMapTool();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAPTOOL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnLbnSelchangeTileLst();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedLoad();

private:
	void SetHorizontalScroll();

public:					// control
	CListBox	m_ListBox;
	CStatic		m_PictureCtrl;

public:
	// CImage: MFC���� �����ϴ� �̹��� Ŭ����. jpg, gif, png, bmp�� ����.
	map<CString, CImage*>	m_MapImg;	
	int						m_iDrawID;

};
