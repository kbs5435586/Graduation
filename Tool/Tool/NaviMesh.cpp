// NaviMesh.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "NaviMesh.h"
#include "afxdialogex.h"
#include "Management.h"
#include "Cell.h"

// CNaviMesh 대화 상자입니다.

IMPLEMENT_DYNAMIC(CNaviMesh, CDialogEx)

CNaviMesh::CNaviMesh(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_NAVIMESH, pParent)
{

}

CNaviMesh::~CNaviMesh()
{
}

void CNaviMesh::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CNaviMesh, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CNaviMesh::OnBnClickedApply)
	ON_BN_CLICKED(IDC_BUTTON4, &CNaviMesh::OnBnClickedSave)
	ON_BN_CLICKED(IDC_BUTTON5, &CNaviMesh::OnBnClickedLoad)
END_MESSAGE_MAP()


// CNaviMesh 메시지 처리기입니다.


void CNaviMesh::OnBnClickedApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CNaviMesh::OnBnClickedSave()
{
	UpdateData(TRUE);
	CFileDialog Dlg(FALSE, L"dat", L"제목없음.dat", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Data Files(*.dat)|*.dat||", this);

	TCHAR szCurPath[MAX_STR] = L"";

	GetCurrentDirectory(MAX_STR, szCurPath);

	PathRemoveFileSpec(szCurPath);
	lstrcat(szCurPath, L"\\Data");

	Dlg.m_ofn.lpstrInitialDir = szCurPath;

	if (IDOK == Dlg.DoModal())
	{
		CString FilePath = Dlg.GetPathName();

		HANDLE hFile = CreateFile(FilePath.GetString(), GENERIC_WRITE, 0, 0,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
			return;

		DWORD dwByte = 0;
		CManagement* pManagement = CManagement::GetInstance();
		if (nullptr == pManagement)
			return;
		pManagement->AddRef();

		CNavi* pNavi = (CNavi*)pManagement->Get_ComponentPointer(SCENE_LOGO, L"Layer_Terrain", L"Com_Navi");

		auto& iter = pNavi->GetTotalPos().begin();

		for (; iter != pNavi->GetTotalPos().end(); ++iter)
			WriteFile(hFile, *iter, sizeof(_vec3), &dwByte, nullptr);

		Safe_Release(pManagement);	
		CloseHandle(hFile);
	}

	UpdateData(FALSE);
}


void CNaviMesh::OnBnClickedLoad()
{
	UpdateData(TRUE);
	CFileDialog Dlg(TRUE, L"dat", L"제목없음.dat", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Data Files(*.dat)|*.dat||", this);

	TCHAR szCurPath[MAX_STR] = L"";

	GetCurrentDirectory(MAX_STR, szCurPath);

	PathRemoveFileSpec(szCurPath);
	lstrcat(szCurPath, L"\\Data");

	Dlg.m_ofn.lpstrInitialDir = szCurPath;
	if (IDOK == Dlg.DoModal())
	{
		CString FilePath = Dlg.GetPathName();

		HANDLE hFile = CreateFile(FilePath.GetString(), GENERIC_READ, 0, 0,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
			return;
		_vec3		vPoint = {};
		DWORD dwByte = 0;

		CManagement* pManagement = CManagement::GetInstance();
		if (nullptr == pManagement)
			return;
		pManagement->AddRef();
		CNavi* pNavi = (CNavi*)pManagement->Get_ComponentPointer(SCENE_LOGO, L"Layer_Terrain", L"Com_Navi");

		pNavi->GetTotalPos().clear();
		pNavi->GetTotalPos().shrink_to_fit();
		while (TRUE)
		{
			ReadFile(hFile, &vPoint, sizeof(_vec3), &dwByte, nullptr);
			if (dwByte == 0)
				break;
			pNavi->GetTotalPos().push_back(vPoint);
		}

		pNavi->Set_CellVector(pNavi->GetTotalPos());
		Safe_Release(pManagement);
		CloseHandle(hFile);
	}

	UpdateData(FALSE);
}
