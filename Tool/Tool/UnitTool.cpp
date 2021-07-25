// UnitTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "UnitTool.h"
#include "afxdialogex.h"
#include "Management.h"
#include "GameObject.h"
#include "Layer.h"


// CUnitTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CUnitTool, CDialogEx)

CUnitTool::CUnitTool(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_UNITTOOL, pParent)
	, m_strX_Pos(_T("0"))
	, m_strY_Pos(_T("0"))
	, m_strZ_Pos(_T("0"))
	, m_strX_Scale(_T("1"))
	, m_strY_Scale(_T("1"))
	, m_strZ_Scale(_T("1"))
	, m_strX_Rot(_T(""))
	, m_strY_Rot(_T(""))
	, m_strZ_Rot(_T(""))
	, m_strCameraSpeed(_T(""))
{

}

CUnitTool::~CUnitTool()
{
}

void CUnitTool::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strX_Pos);
	DDX_Text(pDX, IDC_EDIT2, m_strY_Pos);
	DDX_Text(pDX, IDC_EDIT3, m_strZ_Pos);
	DDX_Text(pDX, IDC_EDIT4, m_strX_Scale);
	DDX_Text(pDX, IDC_EDIT5, m_strY_Scale);
	DDX_Text(pDX, IDC_EDIT6, m_strZ_Scale);
	DDX_Control(pDX, IDC_LIST1, m_ListBox_Static);
	DDX_Control(pDX, IDC_LIST2, m_ListBox_Dynamic);
	DDX_Text(pDX, IDC_EDIT7, m_strX_Rot);
	DDX_Text(pDX, IDC_EDIT8, m_strY_Rot);
	DDX_Text(pDX, IDC_EDIT9, m_strZ_Rot);
	DDX_Text(pDX, IDC_EDIT12, m_strCameraSpeed);
}


BEGIN_MESSAGE_MAP(CUnitTool, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CUnitTool::OnBnClickedApply)
	ON_LBN_SELCHANGE(IDC_LIST1, &CUnitTool::OnLbnSelchangeStatic_Mesh)
	ON_BN_CLICKED(IDC_BUTTON2, &CUnitTool::OnBnClickedSave)
	ON_BN_CLICKED(IDC_BUTTON3, &CUnitTool::OnBnClickedLoad)
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BUTTON6, &CUnitTool::OnBnClickedUpdate)
	ON_LBN_SELCHANGE(IDC_LIST2, &CUnitTool::OnLbnSelchangeDynamic_Mesh)
	ON_BN_CLICKED(IDC_BUTTON11, &CUnitTool::OnBnClickedCameraApply)
END_MESSAGE_MAP()


void CUnitTool::OnBnClickedApply()
{
	UpdateData(TRUE);
	m_fX_Pos = (_float)_ttof(m_strX_Pos);
	m_fY_Pos = (_float)_ttof(m_strY_Pos);
	m_fZ_Pos = (_float)_ttof(m_strZ_Pos);
	m_fX_Scale = (_float)_ttof(m_strX_Scale);
	m_fY_Scale = (_float)_ttof(m_strY_Scale);
	m_fZ_Scale = (_float)_ttof(m_strZ_Scale);
	m_fX_Rot = (_float)_ttof(m_strX_Rot);
	m_fY_Rot = (_float)_ttof(m_strY_Rot);
	m_fZ_Rot = (_float)_ttof(m_strZ_Rot);


	CManagement*	pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();

	map<const _tchar*, CLayer*>	mapTemp = *pManagement->Get_MapLayer(SCENE_LOGO);

	for (auto& iter : mapTemp)
	{
		auto& iter1 = iter.second->GetGameObjectList()->begin();
		for (; iter1 != iter.second->GetGameObjectList()->end(); ++iter1)
		{
			if ((*iter1)->GetIsPick())
			{
				CTransform* pTransform = (CTransform*)(*iter1)->Get_ComponentPointer(L"Com_Transform");
				pTransform->SetUp_Speed(5.f, D3DXToRadian(90.f));
				pTransform->Set_Rotate(_vec3(m_fX_Rot, m_fY_Rot, m_fZ_Rot));


				pTransform->Set_StateInfo(STATE_POSITION, &_vec3(m_fX_Pos, m_fY_Pos, m_fZ_Pos));
				pTransform->Scaling(m_fX_Scale, m_fY_Scale, m_fZ_Scale);
				pTransform->Set_Add_PosY(m_fY_Pos);
			}
		}
	}

	Safe_Release(pManagement);
	UpdateData(FALSE);
}


BOOL CUnitTool::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	_tchar* strName = L"";
	auto& iter_find = m_mapGameObject.find(strName);

	if (iter_find != m_mapGameObject.end())
		return FALSE;

	for (_int i = 0; i < 47; ++i)
	{

		if (i == 0)
		{
			strName = L"MountainRock01";
		}
		else if (i == 1)
		{
			strName = L"MountainRock01_A";
		}
		else if (i == 2)
		{
			strName = L"MountainRock01_B";
		}
		else if (i == 3)
		{
			strName = L"MountainRock01_C";
		}
		else if (i == 4)
		{
			strName = L"MountainRock01_D";
		}
		else if (i == 5)
		{
			strName = L"MountainRock02";
		}
		else if (i == 6)
		{
			strName = L"MountainRock02_A";
		}
		else if (i == 7)
		{
			strName = L"MountainRock02_B";
		}
		else if (i == 8)
		{
			strName = L"MountainRock03";
		}
		else if (i == 9)
		{
			strName = L"tree_1";
		}
		else if (i == 10)
		{
			strName = L"rpgpp_lt_tree_01";
		}
		else if (i == 11)
		{
			strName = L"rpgpp_lt_tree_02";
		}
		else if (i == 12)
		{
			strName = L"rpgpp_lt_tree_pine_01";
		}
		else if (i == 13)
		{
			strName = L"rpgpp_lt_flower_01";
		}
		else if (i == 14)
		{
			strName = L"rpgpp_lt_flower_02";
		}
		else if (i == 15)
		{
			strName = L"rpgpp_lt_flower_03";
		}
		else if (i == 16)
		{
			strName = L"rpgpp_lt_plant_01";
		}
		else if (i == 17)
		{
			strName = L"rpgpp_lt_plant_02";
		}
		else if (i == 18)
		{
			strName = L"rpgpp_lt_rock_01";
		}
		else if (i == 19)
		{
			strName = L"rpgpp_lt_rock_02";
		}
		else if (i == 20)
		{
			strName = L"rpgpp_lt_rock_03";
		}
		else if (i == 21)
		{
			strName = L"rpgpp_lt_rock_small_01";
		}
		else if (i == 22)
		{
			strName = L"rpgpp_lt_rock_small_02";
		}
		else if (i == 23)
		{
			strName = L"rpgpp_lt_bush_01";
		}
		else if (i == 24)
		{
			strName = L"rpgpp_lt_bush_02";
		}
		else if (i == 25)
		{
			strName = L"build_barracks_01_low";
		}
		else if (i == 26)
		{
			strName = L"build_barracks_single_01_low";
		}
		else if (i == 27)
		{
			strName = L"build_big_storage_01_low";
		}
		else if (i == 28)
		{
			strName = L"build_bighouse_01_low";
		}
		else if (i == 29)
		{
			strName = L"build_bighouse_02_low";
		}
		else if (i == 30)
		{
			strName = L"build_blacksmith_01_low";
		}
		else if (i == 31)
		{
			strName = L"build_boat_01_low";
		}
		else if (i == 32)
		{
			strName = L"build_gate_01";
		}
		else if (i == 33)
		{
			strName = L"build_small_house_01_low";
		}
		else if (i == 34)
		{
			strName = L"build_small_house_straw_roof_01_low";
		}
		else if (i == 35)
		{
			strName = L"build_small_house_tall_roof_01_low";
		}
		else if (i == 36)
		{
			strName = L"build_storage_01_low";
		}
		else if (i == 37)
		{
			strName = L"build_tower_01";
		}
		else if (i == 38)
		{
			strName = L"build_wall_corner_01_low";
		}
		else if (i == 39)
		{
			strName = L"Rampart_Wall_10M0";
		}
		else if (i == 40)
		{
			strName = L"Rampart_Gate_Small_A0";
		}
		else if (i == 41)
		{
			strName = L"RoundTower_Small_8M0";
		}
		else if (i == 42)
		{
			strName = L"Base_RoundTower_Medium0";
		}
		else if (i == 43)
		{
			strName = L"Tower_Top_Round_Medium0";
		}
		else if (i == 44)
		{
			strName = L"Tower_Top_Round_Medium1";
		}
		else if (i == 45)
		{
			strName = L"Tower_Top_Round_Medium2";
		}
		else if (i == 46)
		{
			strName = L"Castle";
		}

		m_mapGameObject.insert({ strName, i });
		m_ListBox_Static.AddString(strName);
	}


	return TRUE;
}

void CUnitTool::Update_Mode()
{

}

void CUnitTool::OnLbnSelchangeStatic_Mesh()
{
	m_IsStatic = true;
	TCHAR* strName = L"";
	int iSelect = m_ListBox_Static.GetCurSel();

	if (iSelect == -1)
		return;

	if (iSelect == 0)
	{
		m_pComponentTag = L"Component_StaticMesh_MountainRocks01";
		m_pLayerTag = L"Layer_MountainRock01";
	}
	else if (iSelect == 1)
	{
		m_pComponentTag = L"Component_StaticMesh_MountainRocks01_A";
		m_pLayerTag = L"Layer_MountainRock01_A";
	}
	else if (iSelect == 2)
	{
		m_pComponentTag = L"Component_StaticMesh_MountainRocks01_B";
		m_pLayerTag = L"Layer_MountainRock01_B";
	}
	else if (iSelect == 3)
	{
		m_pComponentTag = L"Component_StaticMesh_MountainRocks01_C";
		m_pLayerTag = L"Layer_MountainRock01_C";
	}
	else if (iSelect == 4)
	{
		m_pComponentTag = L"Component_StaticMesh_MountainRocks01_D";
		m_pLayerTag = L"Layer_MountainRock01_D";
	}
	else if (iSelect == 5)
	{
		m_pComponentTag = L"Component_StaticMesh_MountainRocks02";
		m_pLayerTag = L"Layer_MountainRock02";
	}
	else if (iSelect == 6)
	{
		m_pComponentTag = L"Component_StaticMesh_MountainRocks02_A";
		m_pLayerTag = L"Layer_MountainRock02_A";
	}
	else if (iSelect == 7)
	{
		m_pComponentTag = L"Component_StaticMesh_MountainRocks02_B";
		m_pLayerTag = L"Layer_MountainRock02_B";
	}
	else if (iSelect == 8)
	{
		m_pComponentTag = L"Component_StaticMesh_MountainRocks03";
		m_pLayerTag = L"Layer_MountainRock03";
	}
	else if (iSelect == 9)
	{
		m_pComponentTag = L"Component_StaticMesh_tree_1";
		m_pLayerTag = L"Layer_tree_1";
	}
	else if (iSelect == 10)
	{
		m_pComponentTag = L"Component_StaticMesh_rpgpp_lt_tree_01";
		m_pLayerTag = L"Layer_rpgpp_lt_tree_01";
	}
	else if (iSelect == 11)
	{
		m_pComponentTag = L"Component_StaticMesh_rpgpp_lt_tree_02";
		m_pLayerTag = L"Layer_rpgpp_lt_tree_02";
	}
	else if (iSelect == 12)
	{
		m_pComponentTag = L"Component_StaticMesh_rpgpp_lt_tree_pine_01";
		m_pLayerTag = L"Layer_rpgpp_lt_tree_pine_01";
	}
	else if (iSelect == 13)
	{
		m_pComponentTag = L"Component_StaticMesh_rpgpp_lt_flower_01";
		m_pLayerTag = L"Layer_rpgpp_lt_flower_01";
	}
	else if (iSelect == 14)
	{
		m_pComponentTag = L"Component_StaticMesh_rpgpp_lt_flower_02";
		m_pLayerTag = L"Layer_rpgpp_lt_flower_02";
	}
	else if (iSelect == 15)
	{
		m_pComponentTag = L"Component_StaticMesh_rpgpp_lt_flower_03";
		m_pLayerTag = L"Layer_rpgpp_lt_flower_03";
	}
	else if (iSelect == 16)
	{
		m_pComponentTag = L"Component_StaticMesh_rpgpp_lt_plant_01";
		m_pLayerTag = L"Layer_rpgpp_lt_plant_01";
	}
	else if (iSelect == 17)
	{
		m_pComponentTag = L"Component_StaticMesh_rpgpp_lt_plant_02";
		m_pLayerTag = L"Layer_rpgpp_lt_plant_02";
	}
	else if (iSelect == 18)
	{
		m_pComponentTag = L"Component_StaticMesh_rpgpp_lt_rock_01";
		m_pLayerTag = L"Layer_rpgpp_lt_rock_01";
	}
	else if (iSelect == 19)
	{
		m_pComponentTag = L"Component_StaticMesh_rpgpp_lt_rock_02";
		m_pLayerTag = L"Layer_rpgpp_lt_rock_02";
	}
	else if (iSelect == 20)
	{
		m_pComponentTag = L"Component_StaticMesh_rpgpp_lt_rock_03";
		m_pLayerTag = L"Layer_rpgpp_lt_rock_03";
	}
	else if (iSelect == 21)
	{
		m_pComponentTag = L"Component_StaticMesh_rpgpp_lt_rock_small_01";
		m_pLayerTag = L"Layer_rpgpp_lt_rock_small_01";
	}
	else if (iSelect == 22)
	{
		m_pComponentTag = L"Component_StaticMesh_rpgpp_lt_rock_small_02";
		m_pLayerTag = L"Layer_rpgpp_lt_rock_small_02";
	}
	else if (iSelect == 23)
	{
		m_pComponentTag = L"Component_StaticMesh_rpgpp_lt_bush_01";
		m_pLayerTag = L"Layer_rpgpp_lt_bush_01";
	}
	else if (iSelect == 24)
	{
		m_pComponentTag = L"Component_StaticMesh_rpgpp_lt_bush_02";
		m_pLayerTag = L"Layer_rpgpp_lt_bush_02";
	}
	else if (iSelect == 25)
	{
		m_pComponentTag = L"Component_StaticMesh_build_barracks_01_low";
		m_pLayerTag = L"Layer_build_barracks_01_low";
	}
	else if (iSelect == 26)
	{
		m_pComponentTag = L"Component_StaticMesh_build_barracks_single_01_low";
		m_pLayerTag = L"Layer_build_barracks_single_01_low";
	}
	else if (iSelect == 27)
	{
		m_pComponentTag = L"Component_StaticMesh_build_big_storage_01_low";
		m_pLayerTag = L"Layer_build_big_storage_01_low";
	}
	else if (iSelect == 28)
	{
		m_pComponentTag = L"Component_StaticMesh_build_bighouse_01_low";
		m_pLayerTag = L"Layer_build_bighouse_01_low";
	}
	else if (iSelect == 29)
	{
		m_pComponentTag = L"Component_StaticMesh_build_bighouse_02_low";
		m_pLayerTag = L"Layer_build_bighouse_02_low";
	}
	else if (iSelect == 30)
	{
		m_pComponentTag = L"Component_StaticMesh_build_blacksmith_01_low";
		m_pLayerTag = L"Layer_build_blacksmith_01_low";
	}
	else if (iSelect == 31)
	{
		m_pComponentTag = L"Component_StaticMesh_build_boat_01_low";
		m_pLayerTag = L"Layer_build_boat_01_low";
	}
	else if (iSelect == 32)
	{
		m_pComponentTag = L"Component_StaticMesh_build_gate_01";
		m_pLayerTag = L"Layer_build_gate_01";
	}
	else if (iSelect == 33)
	{
		m_pComponentTag = L"Component_StaticMesh_build_small_house_01_low";
		m_pLayerTag = L"Layer_build_small_house_01_low";
	}
	else if (iSelect == 34)
	{
		m_pComponentTag = L"Component_StaticMesh_build_small_house_straw_roof_01_low";
		m_pLayerTag = L"Layer_build_small_house_straw_roof_01_low";
	}
	else if (iSelect == 35)
	{
		m_pComponentTag = L"Component_StaticMesh_build_small_house_tall_roof_01_low";
		m_pLayerTag = L"Layer_build_small_house_tall_roof_01_low";
	}
	else if (iSelect == 36)
	{
		m_pComponentTag = L"Component_StaticMesh_build_storage_01_low";
		m_pLayerTag = L"Layer_build_storage_01_low";
	}
	else if (iSelect == 37)
	{
		m_pComponentTag = L"Component_StaticMesh_build_tower_01";
		m_pLayerTag = L"Layer_build_tower_01";
	}
	else if (iSelect == 38)
	{
		m_pComponentTag = L"Component_StaticMesh_build_wall_corner_01_low";
		m_pLayerTag = L"Layer_build_wall_corner_01_low";
	}
	else if (iSelect == 39)
	{
		m_pComponentTag = L"Component_StaticMesh_Rampart_Wall_10M0";
		m_pLayerTag = L"Layer_Rampart_Wall_10M0";
	}
	else if (iSelect == 40)
	{
		m_pComponentTag = L"Component_StaticMesh_Rampart_Gate_Small_A0";
		m_pLayerTag = L"Layer_Rampart_Gate_Small_A0";
	}
	else if (iSelect == 41)
	{
		m_pComponentTag = L"Component_StaticMesh_build_RoundTower_Small_8M0";
		m_pLayerTag = L"Layer_RoundTower_Small_8M0";
	}
	else if (iSelect == 42)
	{
		m_pComponentTag = L"Component_StaticMesh_Base_RoundTower_Medium0";
		m_pLayerTag = L"Layer_Base_RoundTower_Medium0";
	}
	else if (iSelect == 43)
	{
		m_pComponentTag = L"Component_StaticMesh_Tower_Top_Round_Medium0";
		m_pLayerTag = L"Layer_Tower_Top_Round_Medium0";
	}
	else if (iSelect == 44)
	{
		m_pComponentTag = L"Component_StaticMesh_Tower_Top_Round_Medium1";
		m_pLayerTag = L"Layer_Tower_Top_Round_Medium1";
	}
	else if (iSelect == 45)
	{
		m_pComponentTag = L"Component_StaticMesh_Tower_Top_Round_Medium2";
		m_pLayerTag = L"Layer_Tower_Top_Round_Medium2";
	}
	else if (iSelect == 46)
	{
		m_pComponentTag = L"Component_StaticMesh_Castle";
		m_pLayerTag = L"Layer_Castle";
	}


	m_pGameObjectTag = L"GameObject_Temp";
	m_IsCheckListBox = true;
	m_gLayerTag = m_pLayerTag;
}

void CUnitTool::OnBnClickedSave()
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

		DWORD dwByte_Size = 0;
		DWORD dwByte = 0;

		CManagement* pManagement = CManagement::GetInstance();
		pManagement->AddRef();

		for (int i = 0; i < m_ListBox_Static.GetCount(); i++)
		{
			CString str, strLayer;
			strLayer = L"Layer_";

			m_ListBox_Static.GetText(i, str);
			strLayer += str;

			if (CManagement::GetInstance()->Get_ObjectList(SCENE_LOGO, strLayer) == nullptr)
				continue;

			list<CGameObject*> ObjList = *CManagement::GetInstance()->Get_ObjectList(SCENE_LOGO, strLayer);
			auto iter = ObjList.begin();

			int size = ObjList.size();
			WriteFile(hFile, &size, sizeof(int), &dwByte_Size, nullptr);

			for (iter; iter != ObjList.end(); iter++)
			{
				CComponent* pMesh = (*iter)->Get_ComponentPointer(L"Com_Mesh");

				CTransform* pTransform = nullptr;
				_matrix mat = {};
				_float  fAdd_PosY = 0.f;
				TCHAR* pNames = pMesh->Get_ObjectName();
				TCHAR* pLayerTag = (TCHAR*)(LPCTSTR)(strLayer);
				TCHAR* pComTag = (*iter)->Get_Component_Tag();

				int iLength = lstrlen(pMesh->Get_ObjectName()) + 1;
				int iLength_Layer = lstrlen(strLayer) + 1;
				int iLength_ComTag = lstrlen(pComTag) + 1;

				WriteFile(hFile, &iLength, sizeof(int), &dwByte, nullptr);
				WriteFile(hFile, pNames, sizeof(TCHAR)*iLength, &dwByte, nullptr);

				WriteFile(hFile, &iLength_Layer, sizeof(int), &dwByte, nullptr);
				WriteFile(hFile, pLayerTag, sizeof(TCHAR)*iLength_Layer, &dwByte, nullptr);

				WriteFile(hFile, &iLength_ComTag, sizeof(int), &dwByte, nullptr);
				WriteFile(hFile, pComTag, sizeof(TCHAR)*iLength_ComTag, &dwByte, nullptr);


				pTransform = (CTransform*)(*iter)->Get_ComponentPointer(L"Com_Transform");
				mat = pTransform->Get_Matrix();
				WriteFile(hFile, mat, sizeof(_matrix), &dwByte, nullptr);
				fAdd_PosY = pTransform->Get_Add_PosY();
				WriteFile(hFile, (void*)&fAdd_PosY, sizeof(_float), &dwByte, nullptr);


			}
		}

		for (int i = 0; i < m_ListBox_Dynamic.GetCount(); ++i)
		{
			CString str, strLayer;
			strLayer += L"Layer_";
			m_ListBox_Dynamic.GetText(i, str);
			strLayer += str;

			if (CManagement::GetInstance()->Get_ObjectList(SCENE_LOGO, strLayer) == nullptr)
				continue;

			list<CGameObject*>		ObjList = *CManagement::GetInstance()->Get_ObjectList(SCENE_LOGO, strLayer);
			auto iter = ObjList.begin();
			int size = ObjList.size();
			WriteFile(hFile, &size, sizeof(int), &dwByte_Size, nullptr);

			for (; iter != ObjList.end(); ++iter)
			{
				CComponent* pMesh = (*iter)->Get_ComponentPointer(L"Com_Mesh");

				CTransform* pTransform = nullptr;
				_matrix mat = {};
				_float  fAdd_PosY = 0.f;
				TCHAR* pNames = pMesh->Get_ObjectName();
				TCHAR* pLayerTag = (TCHAR*)(LPCTSTR)(strLayer);

				int iLength = lstrlen(pMesh->Get_ObjectName()) + 1;
				int iLength_Layer = lstrlen(strLayer) + 1;

				WriteFile(hFile, &iLength, sizeof(int), &dwByte, nullptr);
				WriteFile(hFile, pNames, sizeof(TCHAR)*iLength, &dwByte, nullptr);

				WriteFile(hFile, &iLength_Layer, sizeof(int), &dwByte, nullptr);
				WriteFile(hFile, pLayerTag, sizeof(TCHAR)*iLength_Layer, &dwByte, nullptr);

				pTransform = (CTransform*)(*iter)->Get_ComponentPointer(L"Com_Transform");
				mat = pTransform->Get_Matrix();
				WriteFile(hFile, mat, sizeof(_matrix), &dwByte, nullptr);
				fAdd_PosY = pTransform->Get_Add_PosY();
				WriteFile(hFile, (void*)&fAdd_PosY, sizeof(_float), &dwByte, nullptr);
			}

		}

		Safe_Release(pManagement);
		CloseHandle(hFile);
	}

	UpdateData(FALSE);
}

void CUnitTool::OnBnClickedLoad()
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

		_matrix	mat;
		_float  fAdd_PosY = 0.f;
		D3DXMatrixIdentity(&mat);
		TCHAR szName[MAX_STR] = L"";
		TCHAR szLayerTag[MAX_STR] = L"";
		TCHAR szComTag[MAX_STR] = L"";

		DWORD dwByte = 0;
		DWORD dwByte_Size = 0;
		int iLength = 0;
		int iLength_Layer = 0;
		int iLength_Com = 0;
		while (TRUE)
		{
			int iSize = 0;
			ReadFile(hFile, &iSize, sizeof(int), &dwByte_Size, nullptr);
			if (dwByte_Size == 0)
				break;

			for (int i = 0; i < iSize; i++)
			{
				ReadFile(hFile, &iLength, sizeof(int), &dwByte, nullptr);
				ReadFile(hFile, szName, sizeof(TCHAR)*iLength, &dwByte, nullptr);

				ReadFile(hFile, &iLength_Layer, sizeof(int), &dwByte, nullptr);
				ReadFile(hFile, szLayerTag, sizeof(TCHAR)*iLength_Layer, &dwByte, nullptr);
				
				ReadFile(hFile, &iLength_Com, sizeof(int), &dwByte, nullptr);
				ReadFile(hFile, szComTag, sizeof(TCHAR)*iLength_Com, &dwByte, nullptr);

				_tchar* pName = new _tchar[iLength + 1];
				ZeroMemory(pName, iLength + 1);

				_tchar* pLayerTag = new _tchar[iLength_Layer + 1];
				ZeroMemory(pLayerTag, iLength_Layer + 1);

				_tchar* pComrTag = new _tchar[iLength_Com + 1];
				ZeroMemory(pComrTag, iLength_Com + 1);

				lstrcpy(pName, szName);
				lstrcpy(pLayerTag, szLayerTag);
				lstrcpy(pComrTag, szComTag);

				if (FAILED(CManagement::GetInstance()->Add_GameObjectToLayer(L"GameObject_Temp", SCENE_LOGO, pLayerTag, pComrTag)))
					return;

				ReadFile(hFile, mat, sizeof(_matrix), &dwByte, nullptr);
				CTransform* pTransform = (CTransform*)CManagement::GetInstance()->Get_BackObject(SCENE_LOGO, szLayerTag)->Get_ComponentPointer(L"Com_Transform");
				pTransform->Set_Matrix(mat);

				ReadFile(hFile, (void*)&fAdd_PosY, sizeof(_float), &dwByte, nullptr);
				pTransform->Set_Add_PosY(fAdd_PosY);
			}
		}

		CloseHandle(hFile);
	}

	UpdateData(FALSE);
}

void CUnitTool::OnMouseMove(UINT nFlags, CPoint point)
{

	CDialogEx::OnMouseMove(nFlags, point);
}

void CUnitTool::OnBnClickedUpdate()
{
	UpdateData(TRUE);
	CManagement*	pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();

	map<const _tchar*, CLayer*>	mapTemp = *pManagement->Get_MapLayer(SCENE_LOGO);

	for (auto& iter : mapTemp)
	{
		auto& iter1 = iter.second->GetGameObjectList()->begin();
		for (; iter1 != iter.second->GetGameObjectList()->end(); ++iter1)
		{
			if ((*iter1)->GetIsPick())
			{
				CTransform* pTransform = (CTransform*)(*iter1)->Get_ComponentPointer(L"Com_Transform");
				_vec3 vPos = _vec3
				(
					pTransform->Get_StateInfo(STATE_POSITION)->x,
					pTransform->Get_StateInfo(STATE_POSITION)->y,
					pTransform->Get_StateInfo(STATE_POSITION)->z
				);
				_vec3 vSize = pTransform->Get_Scale();
				_vec3 vRotate = pTransform->Get_Rotate();

				m_strX_Pos.Format(_T("%f"), vPos.x);
				m_strY_Pos.Format(_T("%f"), vPos.y);
				m_strZ_Pos.Format(_T("%f"), vPos.z);

				m_strX_Scale.Format(_T("%f"), vSize.x);
				m_strY_Scale.Format(_T("%f"), vSize.y);
				m_strZ_Scale.Format(_T("%f"), vSize.z);

				m_strX_Rot.Format(_T("%f"), vRotate.x);
				m_strY_Rot.Format(_T("%f"), vRotate.y);
				m_strZ_Rot.Format(_T("%f"), vRotate.z);
			}
		}
	}
	Safe_Release(pManagement);
	UpdateData(FALSE);
}

void CUnitTool::OnLbnSelchangeDynamic_Mesh()
{
	m_IsStatic = false;
	TCHAR* strName = L"";
	int iSelect = m_ListBox_Dynamic.GetCurSel();

	if (iSelect == -1)
		return;

	if (iSelect == 0)
	{
		m_pDynamicLayerTag = L"Layer_Popori";
		m_pDynamicObjectTag = L"GameObject_Popori";
	}
	else if (iSelect == 1)
	{
		m_pDynamicLayerTag = L"Layer_Beatle";
		m_pDynamicObjectTag = L"GameObject_Beatle";
	}
	else if (iSelect == 2)
	{
		m_pDynamicLayerTag = L"Layer_Bee";
		m_pDynamicObjectTag = L"GameObject_Bee";
	}
	else if (iSelect == 3)
	{
		m_pDynamicLayerTag = L"Layer_Mantis";
		m_pDynamicObjectTag = L"GameObject_Mantis";
	}
	else if (iSelect == 4)
	{
		m_pDynamicLayerTag = L"Layer_Monkey";
		m_pDynamicObjectTag = L"GameObject_Monkey";
	}
	else if (iSelect == 5)
	{
		m_pDynamicLayerTag = L"Layer_NPC";
		m_pDynamicObjectTag = L"GameObject_NPC";
	}
	else if (iSelect == 6)
	{
		m_pDynamicLayerTag = L"Layer_Tower";
		m_pDynamicObjectTag = L"GameObject_Tower";
	}
	else if (iSelect == 7)
	{
		m_pDynamicLayerTag = L"Layer_Vergos";
		m_pDynamicObjectTag = L"GameObject_Vergos";
	}
	else if (iSelect == 8)
	{
		m_pDynamicLayerTag = L"Layer_UD_Worker";
		m_pDynamicObjectTag = L"GameObject_UD_Worker";
	}
	else if (iSelect == 9)
	{
		m_pDynamicLayerTag = L"Layer_UD_LightCarvarly";
		m_pDynamicObjectTag = L"GameObject_UD_LightCarvarly";
	}
	else if (iSelect == 10)
	{
		m_pDynamicLayerTag = L"Layer_UD_HeavyInfantry";
		m_pDynamicObjectTag = L"GameObject_UD_HeavyInfantry";
	}
	m_IsCheckListBox = true;
	m_gLayerTag = m_pLayerTag;
}


void CUnitTool::OnBnClickedCameraApply()
{
	UpdateData(TRUE);
	CManagement* pManangement = CManagement::GetInstance();
	if (nullptr == pManangement)
		return;
	pManangement->AddRef();

	_float fCameraSpeed = (_float)_ttof(m_strCameraSpeed);

	CComponent* pComponent = pManangement->Get_ComponentPointer(SCENE_LOGO, L"Layer_Camera", L"Com_Transform");
	if (nullptr != pComponent)
	{
		dynamic_cast<CTransform*>(pComponent)->SetUp_Speed(fCameraSpeed, D3DXToRadian(360.f));
	}

	Safe_Release(pManangement);
	UpdateData(false);
}
