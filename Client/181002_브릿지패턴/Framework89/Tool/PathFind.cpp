// PathFind.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "PathFind.h"
#include "FileInfo.h"
#include "afxdialogex.h"


// CPathFind 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPathFind, CDialog)

CPathFind::CPathFind(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_PATHFIND, pParent)
{

}

CPathFind::~CPathFind()
{
	for_each(m_ImgInfoLst.begin(), m_ImgInfoLst.end(), SafeDelete<IMGPATHINFO*>);
	m_ImgInfoLst.clear();
}

void CPathFind::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
}


BEGIN_MESSAGE_MAP(CPathFind, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CPathFind::OnBnClickedSave)
	ON_BN_CLICKED(IDC_BUTTON6, &CPathFind::OnBnClickedLoad)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// CPathFind 메시지 처리기입니다.

void CPathFind::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialog::OnDropFiles(hDropInfo);

	UpdateData(TRUE);

	int iCount = DragQueryFile(hDropInfo, -1, nullptr, 0);

	TCHAR szFullPath[MAX_STR] = L"";

	for (int i = 0; i < iCount; ++i)
	{
		DragQueryFile(hDropInfo, i, szFullPath, MAX_STR);		
		CFileInfo::DirInfoExtraction(szFullPath, m_ImgInfoLst);
	}

	wstring wstrCombined = L"";
	TCHAR szCount[MIN_STR] = L"";

	for (auto& pImgPathInfo : m_ImgInfoLst)
	{
		// iCount를 szCount로 변환(10진수).
		_itow_s(pImgPathInfo->iCount, szCount, 10);

		wstrCombined = pImgPathInfo->wstrObjKey + L"|" + pImgPathInfo->wstrStateKey + L"|"
			+ szCount + L"|" + pImgPathInfo->wstrPath;

		m_ListBox.AddString(wstrCombined.c_str());
	}

	SetHorizontalScroll();

	UpdateData(FALSE);
}

void CPathFind::OnBnClickedSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	
	// CFileDialog: 파일 열기 혹은 저장 작업에 필요한 대화상자를 생성하는 클래스.
	CFileDialog Dlg(FALSE, L"txt", L"제목없음.txt", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Txt Files(*.txt)|*.txt||", this);

	TCHAR szCurPath[MAX_STR] = L"";

	// GetCurrentDirectory: 현재 작업경로를 얻어옴.
	GetCurrentDirectory(MAX_STR, szCurPath);

	// PathRemoveFileSpec: 현재 경로에서 파일명을 제거.
	// 단, 제거할 파일명이 없다면 말단 폴더명을 제거한다.
	PathRemoveFileSpec(szCurPath);
	lstrcat(szCurPath, L"\\Data");

	// lpstrInitialDir: 대화상자를 열었을 때 보일 기본 경로.
	Dlg.m_ofn.lpstrInitialDir = szCurPath;	// 절대경로!

	if (IDOK == Dlg.DoModal())
	{
		// C++ 파일 입출력
		wofstream fout;
		fout.open(Dlg.GetPathName().GetString());

		if (fout.fail())
			return;

		wstring wstrCombined = L"";
		TCHAR szCount[MIN_STR] = L"";

		for (auto& pImgPathInfo : m_ImgInfoLst)
		{
			// iCount를 szCount로 변환(10진수).
			_itow_s(pImgPathInfo->iCount, szCount, 10);

			wstrCombined = pImgPathInfo->wstrObjKey + L"|" + pImgPathInfo->wstrStateKey + L"|"
				+ szCount + L"|" + pImgPathInfo->wstrPath;

			fout << wstrCombined << endl;
		}

		fout.close();
	}

	UpdateData(FALSE);

	// WinExec: 윈도우의 기본 응용프로그램을 실행.
	WinExec("notepad.exe ../Data/ImgPathInfo.txt", SW_SHOW);
}


void CPathFind::OnBnClickedLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	// CFileDialog: 파일 열기 혹은 저장 작업에 필요한 대화상자를 생성하는 클래스.
	CFileDialog Dlg(TRUE, L"txt", L"제목없음.txt", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Txt Files(*.txt)|*.txt||", this);

	TCHAR szCurPath[MAX_STR] = L"";

	// GetCurrentDirectory: 현재 작업경로를 얻어옴.
	GetCurrentDirectory(MAX_STR, szCurPath);

	// PathRemoveFileSpec: 현재 경로에서 파일명을 제거.
	// 단, 제거할 파일명이 없다면 말단 폴더명을 제거한다.
	PathRemoveFileSpec(szCurPath);
	lstrcat(szCurPath, L"\\Data");

	// lpstrInitialDir: 대화상자를 열었을 때 보일 기본 경로.
	Dlg.m_ofn.lpstrInitialDir = szCurPath;	// 절대경로!

	if (IDOK == Dlg.DoModal())
	{
		// C++ 파일 입출력
		wifstream fin;
		fin.open(Dlg.GetPathName().GetString());

		if (fin.fail())
			return;

		m_ListBox.ResetContent();

		for_each(m_ImgInfoLst.begin(), m_ImgInfoLst.end(), SafeDelete<IMGPATHINFO*>);
		m_ImgInfoLst.clear();

		wstring wstrCombined = L"";
		TCHAR szBuf[MAX_STR] = L"";

		IMGPATHINFO* pImgPathInfo = nullptr;

		while (true)
		{
			wstrCombined = L"";
			pImgPathInfo = new IMGPATHINFO;

			fin.getline(szBuf, MAX_STR, '|');
			wstrCombined += wstring(szBuf) += L"|";
			pImgPathInfo->wstrObjKey = szBuf;

			fin.getline(szBuf, MAX_STR, '|');
			wstrCombined += wstring(szBuf) += L"|";
			pImgPathInfo->wstrStateKey = szBuf;

			fin.getline(szBuf, MAX_STR, '|');
			wstrCombined += wstring(szBuf) += L"|";
			pImgPathInfo->iCount = _wtoi(szBuf);

			fin.getline(szBuf, MAX_STR);
			wstrCombined += szBuf;
			pImgPathInfo->wstrPath = szBuf;		

			if (fin.eof())
			{
				SafeDelete(pImgPathInfo);
				break;
			}

			m_ListBox.AddString(wstrCombined.c_str());
			m_ImgInfoLst.push_back(pImgPathInfo);
		}

		fin.close();
	}

	SetHorizontalScroll();

	UpdateData(FALSE);
}

void CPathFind::SetHorizontalScroll()
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
