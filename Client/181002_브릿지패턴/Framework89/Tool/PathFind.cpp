// PathFind.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Tool.h"
#include "PathFind.h"
#include "FileInfo.h"
#include "afxdialogex.h"


// CPathFind ��ȭ �����Դϴ�.

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


// CPathFind �޽��� ó�����Դϴ�.

void CPathFind::OnDropFiles(HDROP hDropInfo)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

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
		// iCount�� szCount�� ��ȯ(10����).
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);
	
	// CFileDialog: ���� ���� Ȥ�� ���� �۾��� �ʿ��� ��ȭ���ڸ� �����ϴ� Ŭ����.
	CFileDialog Dlg(FALSE, L"txt", L"�������.txt", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Txt Files(*.txt)|*.txt||", this);

	TCHAR szCurPath[MAX_STR] = L"";

	// GetCurrentDirectory: ���� �۾���θ� ����.
	GetCurrentDirectory(MAX_STR, szCurPath);

	// PathRemoveFileSpec: ���� ��ο��� ���ϸ��� ����.
	// ��, ������ ���ϸ��� ���ٸ� ���� �������� �����Ѵ�.
	PathRemoveFileSpec(szCurPath);
	lstrcat(szCurPath, L"\\Data");

	// lpstrInitialDir: ��ȭ���ڸ� ������ �� ���� �⺻ ���.
	Dlg.m_ofn.lpstrInitialDir = szCurPath;	// ������!

	if (IDOK == Dlg.DoModal())
	{
		// C++ ���� �����
		wofstream fout;
		fout.open(Dlg.GetPathName().GetString());

		if (fout.fail())
			return;

		wstring wstrCombined = L"";
		TCHAR szCount[MIN_STR] = L"";

		for (auto& pImgPathInfo : m_ImgInfoLst)
		{
			// iCount�� szCount�� ��ȯ(10����).
			_itow_s(pImgPathInfo->iCount, szCount, 10);

			wstrCombined = pImgPathInfo->wstrObjKey + L"|" + pImgPathInfo->wstrStateKey + L"|"
				+ szCount + L"|" + pImgPathInfo->wstrPath;

			fout << wstrCombined << endl;
		}

		fout.close();
	}

	UpdateData(FALSE);

	// WinExec: �������� �⺻ �������α׷��� ����.
	WinExec("notepad.exe ../Data/ImgPathInfo.txt", SW_SHOW);
}


void CPathFind::OnBnClickedLoad()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);

	// CFileDialog: ���� ���� Ȥ�� ���� �۾��� �ʿ��� ��ȭ���ڸ� �����ϴ� Ŭ����.
	CFileDialog Dlg(TRUE, L"txt", L"�������.txt", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Txt Files(*.txt)|*.txt||", this);

	TCHAR szCurPath[MAX_STR] = L"";

	// GetCurrentDirectory: ���� �۾���θ� ����.
	GetCurrentDirectory(MAX_STR, szCurPath);

	// PathRemoveFileSpec: ���� ��ο��� ���ϸ��� ����.
	// ��, ������ ���ϸ��� ���ٸ� ���� �������� �����Ѵ�.
	PathRemoveFileSpec(szCurPath);
	lstrcat(szCurPath, L"\\Data");

	// lpstrInitialDir: ��ȭ���ڸ� ������ �� ���� �⺻ ���.
	Dlg.m_ofn.lpstrInitialDir = szCurPath;	// ������!

	if (IDOK == Dlg.DoModal())
	{
		// C++ ���� �����
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
