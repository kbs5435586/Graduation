// TerrainTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "TerrainTool.h"
#include "afxdialogex.h"
#include "Management.h"
#include "Layer.h"
#include "GameObject.h"

// CTerrainTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTerrainTool, CDialogEx)

CTerrainTool::CTerrainTool(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TERRAINTOOL, pParent)
	, m_strInterval(_T("0"))
	, m_strSide(_T("0"))
	, m_strCross(_T("0"))
	, m_strComponentTag(_T(""))
{

}

CTerrainTool::~CTerrainTool()
{

}

void CTerrainTool::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT3, m_strInterval);
	DDX_Text(pDX, IDC_EDIT1, m_strSide);
	DDX_Text(pDX, IDC_EDIT2, m_strCross);
	DDX_Text(pDX, IDC_EDIT11, m_strComponentTag);
	DDX_Control(pDX, IDC_LIST1, m_ListBox_Terrain_Component);
}

BEGIN_MESSAGE_MAP(CTerrainTool, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CTerrainTool::OnBnClickedApply)
	ON_LBN_SELCHANGE(IDC_LIST1, &CTerrainTool::OnLbnSelchangeTerrain_ComponentList)
	ON_BN_CLICKED(IDC_BUTTON6, &CTerrainTool::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_BUTTON9, &CTerrainTool::OnBnClickedSave)
	ON_BN_CLICKED(IDC_BUTTON10, &CTerrainTool::OnBnClickedLoad)
END_MESSAGE_MAP()

void CTerrainTool::OnBnClickedApply()
{
	m_IsApply = true;

	//m_vecTerrain_Component_Tag[m_iComponent_Tag_Idx];
}

BOOL CTerrainTool::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	return TRUE; 
}

void CTerrainTool::OnLbnSelchangeTerrain_ComponentList()
{
	UpdateData(TRUE);
	_int iSelect = m_ListBox_Terrain_Component.GetCurSel();
	if (iSelect == -1)
		return;
	if (nullptr != m_pOutput_Component_tag)
	{
		delete[] m_pOutput_Component_tag;
		m_pOutput_Component_tag = nullptr;
	}
	
	CString		pTemp;
	m_ListBox_Terrain_Component.GetText(iSelect, pTemp);

	int iLength = pTemp.GetLength();
	m_pOutput_Component_tag = new _tchar[iLength + 1];
	lstrcpy(m_pOutput_Component_tag, pTemp.GetBuffer());

// 	auto& iter_find = m_mapMapinfo.find(m_pOutput_Component_tag);
// 	if (iter_find == m_mapMapinfo.end())
// 		return;
// 
// 	m_strCross.Format(_T("% f"), (*iter_find).second.m_fSizeX);
// 	m_strSide.Format(_T("% f"), (*iter_find).second.m_fSizeZ);
// 	m_strInterval.Format(_T("% f"), (*iter_find).second.m_fInterval);
	UpdateData(FALSE);
}

void CTerrainTool::OnBnClickedAdd()
{
	UpdateData(TRUE);
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();


	m_fInterval = (_float)_ttof(m_strInterval);
	m_fSizeX = (_float)_ttof(m_strCross);
	m_fSizeZ = (_float)_ttof(m_strSide);

	_int	iLength = m_strComponentTag.GetLength();
	_tchar* pComponent_Tag = new _tchar[iLength+1];
	if (nullptr == pComponent_Tag)
		return;
	lstrcpy(pComponent_Tag, m_strComponentTag.GetBuffer());

	CGameObject* pGameObject = pManagement->Get_GameObject(SCENE_LOGO, L"Layer_Terrain", 0);
	if (nullptr == pGameObject)
		return;

	m_pGraphic_Device = pGameObject->Get_Device();
	if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, pComponent_Tag, CBuffer_Terrain::Create(m_pGraphic_Device, m_fSizeX, m_fSizeZ, m_fInterval))))
		return;


	//m_mapMapinfo.insert({pComponent_Tag, MAPINFO(m_fSizeX, m_fSizeZ, m_fInterval)});
	MAPINFO* pInfo = new MAPINFO(pComponent_Tag, m_fSizeX, m_fSizeZ, m_fInterval);
	m_vecMapInfo.push_back(pInfo);
	m_ListBox_Terrain_Component.AddString(pComponent_Tag);


	Safe_Release(pManagement);
	UpdateData(FALSE);
}

void CTerrainTool::OnBnClickedSave()
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
		pManagement->AddRef();

		for (auto& iter : m_vecMapInfo)
		{
			_float	fSizeX = iter->m_fSizeX;
			_float	fSizeZ = iter->m_fSizeZ;
			_float	fInterval = iter->m_fInterval;
			TCHAR*	pName = iter->m_strKey;
			_int	iLength = lstrlen(pName) + 1;


			WriteFile(hFile, &iLength, sizeof(_int), &dwByte, nullptr);
			WriteFile(hFile, pName, sizeof(TCHAR)*iLength, &dwByte, nullptr);
			WriteFile(hFile, &fSizeX, sizeof(_float), &dwByte, nullptr);
			WriteFile(hFile, &fSizeZ, sizeof(_float), &dwByte, nullptr);
			WriteFile(hFile, &fInterval, sizeof(_float), &dwByte, nullptr);
		}

		
		Safe_Release(pManagement);
		CloseHandle(hFile);
	}

	UpdateData(FALSE);
}

void CTerrainTool::OnBnClickedLoad()
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
		m_ListBox_Terrain_Component.ResetContent();

		CManagement* pManagement = CManagement::GetInstance();
		if (nullptr == pManagement)
			return;
		pManagement->AddRef();

		CString FilePath = Dlg.GetPathName();

		HANDLE hFile = CreateFile(FilePath.GetString(), GENERIC_READ, 0, 0,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
			return;
		TCHAR szName[MAX_STR] = L"";

		DWORD dwByte = 0;
		


		_float		fSizeX = 0.f;
		_float		fSizeZ = 0.f;
		_float		fInterval = 0.f;
		_int		iLength = 0;
		while (TRUE)
		{
			ReadFile(hFile, &iLength, sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, szName, sizeof(TCHAR)*iLength, &dwByte, nullptr);
			ReadFile(hFile, &fSizeX, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &fSizeZ, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &fInterval, sizeof(_float), &dwByte, nullptr);

			if (0 == dwByte)
				break;

		
			_int	iLength_ = lstrlen(szName);
			_tchar* pComponent_Tag = new _tchar[iLength_ + 1];
			if (nullptr == pComponent_Tag)
				return;
			lstrcpy(pComponent_Tag, szName);

			MAPINFO* pInfo = new MAPINFO(pComponent_Tag, fSizeX, fSizeZ, fInterval);
			m_vecMapInfo.push_back(pInfo);
			m_ListBox_Terrain_Component.AddString(pComponent_Tag);

			CGameObject* pGameObject = pManagement->Get_GameObject(SCENE_LOGO, L"Layer_Terrain", 0);
			if (nullptr == pGameObject)
			{
				CloseHandle(hFile);
				return;
			}
			if (FAILED(pManagement->Add_Prototype_Component(SCENE_LOGO, pComponent_Tag, CBuffer_Terrain::Create(pGameObject->Get_Device(), fSizeX, fSizeZ, fInterval))))
			{
				CloseHandle(hFile);
				return;
			} 
		}
		Safe_Release(pManagement);
		CloseHandle(hFile);
	}

	UpdateData(FALSE);
}
