// PopupWnd.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Tool.h"
#include "PopupWnd.h"
#include "MySheet.h"
#include "afxdialogex.h"


// CPopupWnd ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CPopupWnd, CDialog)

CPopupWnd::CPopupWnd(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_POPUPWND, pParent), m_pMySheet(nullptr)
{
	
}

CPopupWnd::~CPopupWnd()
{
	SafeDelete(m_pMySheet);
}

void CPopupWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPopupWnd, CDialog)
END_MESSAGE_MAP()


// CPopupWnd �޽��� ó�����Դϴ�.


BOOL CPopupWnd::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	if (nullptr == m_pMySheet)
	{
		m_pMySheet = new CMySheet;
		m_pMySheet->Create(this, WS_CHILD | WS_VISIBLE);
		m_pMySheet->MoveWindow(0, 0, WINCX, WINCY);
	}	

	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}
