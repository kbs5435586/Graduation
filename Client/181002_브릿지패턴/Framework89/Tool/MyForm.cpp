// MyForm.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Tool.h"
#include "MyForm.h"


// CMyForm

IMPLEMENT_DYNCREATE(CMyForm, CFormView)

CMyForm::CMyForm()
	: CFormView(IDD_MYFORM)
{

}

CMyForm::~CMyForm()
{
}

void CMyForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMyForm, CFormView)
	ON_BN_CLICKED(IDC_BUTTON1, &CMyForm::OnBnClickedUnitTool)
	ON_BN_CLICKED(IDC_BUTTON6, &CMyForm::OnBnClickedMapTool)
	ON_BN_CLICKED(IDC_BUTTON7, &CMyForm::OnBnClickedPopUp)
	ON_BN_CLICKED(IDC_BUTTON8, &CMyForm::OnBnClickedPathFind)
END_MESSAGE_MAP()


// CMyForm �����Դϴ�.

#ifdef _DEBUG
void CMyForm::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CMyForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMyForm �޽��� ó�����Դϴ�.


void CMyForm::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	// CreatePointFont(��Ʈũ��, �۾�ü)
	m_Font.CreatePointFont(180, L"�ü�");

	// GetDlgItem: ���̾�α׿� ��ġ�� ������Ʈ�� ID���� ���� ������ �Լ�.
	GetDlgItem(IDC_BUTTON1)->SetFont(&m_Font);
	GetDlgItem(IDC_BUTTON6)->SetFont(&m_Font);
	GetDlgItem(IDC_BUTTON7)->SetFont(&m_Font);
	GetDlgItem(IDC_BUTTON8)->SetFont(&m_Font);
}


void CMyForm::OnBnClickedUnitTool()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//AfxMessageBox(L"UnitTool!!");

	// GetSafeHwnd: ���� ������ ���̾�α��� �ڵ��� ��ȯ.
	// ���� �����Ǿ����� ���� ���̾�α״� nulltr�� ��ȯ�Ѵ�.
	if (nullptr == m_UnitTool.GetSafeHwnd())
	{
		// �ش� ID�� �´� ���̾�α� ����.
		m_UnitTool.Create(IDD_UNITTOOL);
	}
	

	// â������� ���
	m_UnitTool.ShowWindow(SW_SHOW);
}


void CMyForm::OnBnClickedMapTool()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (nullptr == m_MapTool.GetSafeHwnd())
	{
		// �ش� ID�� �´� ���̾�α� ����.
		m_MapTool.Create(IDD_MAPTOOL);
	}


	// â������� ���
	m_MapTool.ShowWindow(SW_SHOW);
}


void CMyForm::OnBnClickedPopUp()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (nullptr == m_Popup.GetSafeHwnd())
	{
		// �ش� ID�� �´� ���̾�α� ����.
		m_Popup.Create(IDD_POPUPWND);
	}


	// â������� ���
	m_Popup.ShowWindow(SW_SHOW);
}


void CMyForm::OnBnClickedPathFind()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (nullptr == m_PathFind.GetSafeHwnd())
	{
		// �ش� ID�� �´� ���̾�α� ����.
		m_PathFind.Create(IDD_PATHFIND);
	}


	// â������� ���
	m_PathFind.ShowWindow(SW_SHOW);
}
