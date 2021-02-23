// MapTool.cpp : ���� �����Դϴ�.
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


// CMapTool ��ȭ �����Դϴ�.

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


// CMapTool �޽��� ó�����Դϴ�.


// HDROP: ��ӵ� ���ϵ��� ������ ���� �ִ� �ڵ�.
void CMapTool::OnDropFiles(HDROP hDropInfo)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	CDialog::OnDropFiles(hDropInfo);

	TCHAR szFilePath[MAX_STR] = L"";
	TCHAR szFileName[MAX_STR] = L"";

	// DragQueryFile�� �ι�° ���ڰ� -1�� ��� ��ӵ� ���ϵ��� ������ ��ȯ.
	int iCount = DragQueryFile(hDropInfo, -1, nullptr, 0);

	for (int i = 0; i < iCount; ++i)
	{
		// DragQueryFile(HDROP, iIndex, ���ϰ��, ���۱���)
		// ��ӵ� ���ϵ� �� iIndex��°�� ���ϰ�θ� ����.
		DragQueryFile(hDropInfo, i, szFilePath, MAX_STR);

		// ������ -> ����� ��ȯ
		CString strRelative = CFileInfo::ConvertRelativePath(szFilePath);

		// PathFindFileName: ��� �󿡼� "���ϸ�.Ȯ����"�� �߷����� �Լ�.
		CString strFileName = PathFindFileName(strRelative);

		lstrcpy(szFileName, strFileName.GetString());

		// PathRemoveExtension: ���� ���ڿ����� Ȯ���ڸ� �߶󳻴� �Լ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
		// isdigit: ���� �˻��ϴ� ���ڰ� �������� �������� �Ǻ�.
		if (isdigit(strFileName[i]))
			break;
	}

	// CString::Delete(start, count): start���� count��ŭ�� ���ڿ��� �߶�.
	strFileName.Delete(0, i);

	// _tstoi: ���� ���ڿ��� int������ ��ȯ.
	m_iDrawID = _tstoi(strFileName.GetString());

	UpdateData(FALSE);
}

void CMapTool::OnBnClickedSave()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);

	CFileDialog Dlg(FALSE, L"dat", L"�������.dat", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Data Files(*.dat)|*.dat||", this);

	TCHAR szCurPath[MAX_STR] = L"";

	GetCurrentDirectory(MAX_STR, szCurPath);
	PathRemoveFileSpec(szCurPath);
	lstrcat(szCurPath, L"\\Data");

	Dlg.m_ofn.lpstrInitialDir = szCurPath;	// ������!

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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);

	CFileDialog Dlg(TRUE, L"dat", L"�������.dat", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Data Files(*.dat)|*.dat||", this);

	TCHAR szCurPath[MAX_STR] = L"";

	GetCurrentDirectory(MAX_STR, szCurPath);
	PathRemoveFileSpec(szCurPath);
	lstrcat(szCurPath, L"\\Data");

	Dlg.m_ofn.lpstrInitialDir = szCurPath;	// ������!

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

	// GetCount: ����Ʈ�ڽ��� ���� ����� �� ������ ��ȯ.
	for (int i = 0; i < m_ListBox.GetCount(); ++i)
	{
		m_ListBox.GetText(i, strName);

		// GetTextExtent: ���� ���ڿ��� ���̸� �ȼ� ������ ��ȯ.
		size = pDC->GetTextExtent(strName);

		if (size.cx > iCX)
			iCX = size.cx;
	}

	m_ListBox.ReleaseDC(pDC);

	// GetHorizontalExtent: ���� ����Ʈ�ڽ��� ���η� ��ũ�� �� �� �ִ� �ִ� ������ ����.
	if (iCX > m_ListBox.GetHorizontalExtent())
		m_ListBox.SetHorizontalExtent(iCX);
}