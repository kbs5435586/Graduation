// MapTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "MapTool.h"
#include "afxdialogex.h"
#include "FileInfo.h"

#include "MainFrm.h"
#include "ToolView.h"
#include "MiniView.h"
#include "Terrain.h"


// CMapTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMapTool, CDialog)

CMapTool::CMapTool(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_MAPTOOL, pParent), m_iDrawID(0)
{

}

CMapTool::~CMapTool()
{
	for_each(m_MapImg.begin(), m_MapImg.end(), 
		[](auto& Mypair)
	{
		SafeDelete(Mypair.second);
	});

	m_MapImg.clear();
}

void CMapTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_PICTURE, m_PictureCtrl);
}


BEGIN_MESSAGE_MAP(CMapTool, CDialog)
	ON_WM_DROPFILES()
	ON_LBN_SELCHANGE(IDC_LIST1, &CMapTool::OnLbnSelchangeTileLst)
	ON_BN_CLICKED(IDC_BUTTON1, &CMapTool::OnBnClickedSave)
	ON_BN_CLICKED(IDC_BUTTON6, &CMapTool::OnBnClickedLoad)
END_MESSAGE_MAP()


// CMapTool 메시지 처리기입니다.


// HDROP: 드롭된 파일들의 정보를 갖고 있는 핸들.
void CMapTool::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CDialog::OnDropFiles(hDropInfo);

	TCHAR szFilePath[MAX_STR] = L"";
	TCHAR szFileName[MAX_STR] = L"";

	// DragQueryFile의 두번째 인자가 -1일 경우 드롭된 파일들의 개수를 반환.
	int iCount = DragQueryFile(hDropInfo, -1, nullptr, 0);

	for (int i = 0; i < iCount; ++i)
	{
		// DragQueryFile(HDROP, iIndex, 파일경로, 버퍼길이)
		// 드롭된 파일들 중 iIndex번째의 파일경로를 얻어옴.
		DragQueryFile(hDropInfo, i, szFilePath, MAX_STR);

		// 절대경로 -> 상대경로 변환
		CString strRelative = CFileInfo::ConvertRelativePath(szFilePath);

		// PathFindFileName: 경로 상에서 "파일명.확장자"를 추려내는 함수.
		CString strFileName = PathFindFileName(strRelative);

		lstrcpy(szFileName, strFileName.GetString());

		// PathRemoveExtension: 현재 문자열에서 확장자를 잘라내는 함수.
		PathRemoveExtension(szFileName);

		auto& iter_find = m_MapImg.find(szFileName);

		if (m_MapImg.end() == iter_find)
		{
			CImage* pImg = new CImage;
			pImg->Load(strRelative);

			m_MapImg.insert({ szFileName, pImg });
			m_ListBox.AddString(szFileName);
		}		
	}

	SetHorizontalScroll();
}

void CMapTool::OnLbnSelchangeTileLst()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	int iSelect = m_ListBox.GetCurSel();

	if (-1 == iSelect)
		return;

	CString strFileName = L"";
	m_ListBox.GetText(iSelect, strFileName);

	auto& iter_find = m_MapImg.find(strFileName);

	if (m_MapImg.end() == iter_find)
		return;

	CDC* pDC = this->GetDC();
	

	//RECT rc = { };
	//m_PictureCtrl.GetWindowRect(&rc);
	//ScreenToClient(&rc);	
	//
	////m_PictureCtrl.Invalidate(TRUE);
	////iter_find->second->TransparentBlt(pDC->m_hDC, rc, RGB(255, 255, 255));
	////iter_find->second->Draw(pDC->m_hDC, rc);

	//this->ReleaseDC(pDC);

	m_PictureCtrl.SetBitmap(*(iter_find->second));

	int i = 0;

	for (; i < strFileName.GetLength(); ++i)
	{
		// isdigit: 현재 검사하는 문자가 글자인지 숫자인지 판별.
		if (isdigit(strFileName[i]))
			break;
	}

	// CString::Delete(start, count): start부터 count만큼의 문자열을 잘라냄.
	strFileName.Delete(0, i);

	// _tstoi: 숫자 문자열을 int형으로 변환.
	m_iDrawID = _tstoi(strFileName.GetString());

	UpdateData(FALSE);
}

void CMapTool::OnBnClickedSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	CFileDialog Dlg(FALSE, L"dat", L"제목없음.dat", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Data Files(*.dat)|*.dat||", this);

	TCHAR szCurPath[MAX_STR] = L"";

	GetCurrentDirectory(MAX_STR, szCurPath);
	PathRemoveFileSpec(szCurPath);
	lstrcat(szCurPath, L"\\Data");

	Dlg.m_ofn.lpstrInitialDir = szCurPath;	// 절대경로!

	if (IDOK == Dlg.DoModal())
	{
		CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
		CToolView* pMainView = dynamic_cast<CToolView*>(pMainFrame->m_MainSplt.GetPane(0, 1));		
		CTerrain* pTerrain = pMainView->m_pTerrain;

		CString FilePath = Dlg.GetPathName();
		pTerrain->SaveData(FilePath.GetString());
	}

	UpdateData(FALSE);
}


void CMapTool::OnBnClickedLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	CFileDialog Dlg(TRUE, L"dat", L"제목없음.dat", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Data Files(*.dat)|*.dat||", this);

	TCHAR szCurPath[MAX_STR] = L"";

	GetCurrentDirectory(MAX_STR, szCurPath);
	PathRemoveFileSpec(szCurPath);
	lstrcat(szCurPath, L"\\Data");

	Dlg.m_ofn.lpstrInitialDir = szCurPath;	// 절대경로!

	if (IDOK == Dlg.DoModal())
	{
		CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
		CToolView* pMainView = dynamic_cast<CToolView*>(pMainFrame->m_MainSplt.GetPane(0, 1));
		CMiniView* pMiniView = dynamic_cast<CMiniView*>(pMainFrame->m_SecondSplt.GetPane(0, 0));
		CTerrain* pTerrain = pMainView->m_pTerrain;

		CString FilePath = Dlg.GetPathName();
		pTerrain->LoadData(FilePath.GetString());

		pMainView->Invalidate(FALSE);
		pMiniView->Invalidate(FALSE);
	}

	UpdateData(FALSE);
}

void CMapTool::SetHorizontalScroll()
{
	CString strName = L"";
	CSize	size;	

	int iCX = 0;

	CDC* pDC = m_ListBox.GetDC();

	// GetCount: 리스트박스가 가진 목록의 총 개수를 반환.
	for (int i = 0; i < m_ListBox.GetCount(); ++i)
	{
		m_ListBox.GetText(i, strName);

		// GetTextExtent: 현재 문자열의 길이를 픽셀 단위로 변환.
		size = pDC->GetTextExtent(strName);

		if (size.cx > iCX)
			iCX = size.cx;
	}

	m_ListBox.ReleaseDC(pDC);

	// GetHorizontalExtent: 현재 리스트박스가 가로로 스크롤 할 수 있는 최대 범위를 얻어옴.
	if (iCX > m_ListBox.GetHorizontalExtent())
		m_ListBox.SetHorizontalExtent(iCX);
}