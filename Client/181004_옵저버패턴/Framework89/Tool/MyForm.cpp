// MyForm.cpp : 구현 파일입니다.
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


// CMyForm 진단입니다.

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


// CMyForm 메시지 처리기입니다.


void CMyForm::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	// CreatePointFont(폰트크기, 글씨체)
	m_Font.CreatePointFont(180, L"궁서");

	// GetDlgItem: 다이얼로그에 배치된 오브젝트를 ID값에 따라 얻어오는 함수.
	GetDlgItem(IDC_BUTTON1)->SetFont(&m_Font);
	GetDlgItem(IDC_BUTTON6)->SetFont(&m_Font);
	GetDlgItem(IDC_BUTTON7)->SetFont(&m_Font);
	GetDlgItem(IDC_BUTTON8)->SetFont(&m_Font);
}


void CMyForm::OnBnClickedUnitTool()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//AfxMessageBox(L"UnitTool!!");

	// GetSafeHwnd: 현재 생성된 다이얼로그의 핸들을 반환.
	// 아직 생성되어있지 않은 다이얼로그는 nulltr을 반환한다.
	if (nullptr == m_UnitTool.GetSafeHwnd())
	{
		// 해당 ID에 맞는 다이얼로그 생성.
		m_UnitTool.Create(IDD_UNITTOOL);
	}
	

	// 창모양으로 출력
	m_UnitTool.ShowWindow(SW_SHOW);
}


void CMyForm::OnBnClickedMapTool()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (nullptr == m_MapTool.GetSafeHwnd())
	{
		// 해당 ID에 맞는 다이얼로그 생성.
		m_MapTool.Create(IDD_MAPTOOL);
	}


	// 창모양으로 출력
	m_MapTool.ShowWindow(SW_SHOW);
}


void CMyForm::OnBnClickedPopUp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (nullptr == m_Popup.GetSafeHwnd())
	{
		// 해당 ID에 맞는 다이얼로그 생성.
		m_Popup.Create(IDD_POPUPWND);
	}


	// 창모양으로 출력
	m_Popup.ShowWindow(SW_SHOW);
}


void CMyForm::OnBnClickedPathFind()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (nullptr == m_PathFind.GetSafeHwnd())
	{
		// 해당 ID에 맞는 다이얼로그 생성.
		m_PathFind.Create(IDD_PATHFIND);
	}


	// 창모양으로 출력
	m_PathFind.ShowWindow(SW_SHOW);
}
