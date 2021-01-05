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

	for (_int i = 0; i < 78; ++i)
	{
		if (i == 0)
		{
			strName = L"Cliff0";
		}
		else if (i == 1)
		{
			strName = L"Cliff1";
		}
		else if (i == 2)
		{
			strName = L"Cliff2";
		}
		else if (i == 3)
		{
			strName = L"Cliff3";
		}
		else if (i == 4)
		{
			strName = L"Cliff4";
		}
		else if (i == 5)
		{
			strName = L"Fence0";
		}
		else if (i == 6)
		{
			strName = L"Fence1";
		}
		else if (i == 7)
		{
			strName = L"Fence2";
		}
		else if (i == 8)
		{
			strName = L"Fence3";
		}
		else if (i == 9)
		{
			strName = L"Fence4";
		}
		else if (i == 10)
		{
			strName = L"Fence5";
		}
		else if (i == 11)
		{
			strName = L"Fence6";
		}
		else if (i == 12)
		{
			strName = L"Fence7";
		}
		else if (i == 13)
		{
			strName = L"Flower0";
		}
		else if (i == 14)
		{
			strName = L"Flower1";
		}
		else if (i == 15)
		{
			strName = L"Flower2";
		}
		else if (i == 16)
		{
			strName = L"Flower3";
		}
		else if (i == 17)
		{
			strName = L"Flower4";
		}
		else if (i == 18)
		{
			strName = L"Flower5";
		}
		else if (i == 19)
		{
			strName = L"Flower6";
		}
		else if (i == 20)
		{
			strName = L"Flower7";
		}
		else if (i == 21)
		{
			strName = L"Flower8";
		}
		else if (i == 22)
		{
			strName = L"Flower9";
		}
		else if (i == 23)
		{
			strName = L"Flower10";
		}
		else if (i == 24)
		{
			strName = L"Grass0";
		}
		else if (i == 25)
		{
			strName = L"Grass1";
		}
		else if (i == 26)
		{
			strName = L"Grass2";
		}
		else if (i == 27)
		{
			strName = L"Grass3";
		}
		else if (i == 28)
		{
			strName = L"Grass4";
		}
		else if (i == 29)
		{
			strName = L"Grass5";
		}
		else if (i == 30)
		{
			strName = L"Grass6";
		}
		else if (i == 31)
		{
			strName = L"Grass7";
		}
		else if (i == 32)
		{
			strName = L"Grass8";
		}
		else if (i == 33)
		{
			strName = L"Grass9";
		}
		else if (i == 34)
		{
			strName = L"Grass10";
		}
		else if (i == 35)
		{
			strName = L"Grass11";
		}
		else if (i == 36)
		{
			strName = L"Grass12";
		}
		else if (i == 37)
		{
			strName = L"Grass13";
		}
		else if (i == 38)
		{
			strName = L"Grass14";
		}
		else if (i == 39)
		{
			strName = L"Grass15";
		}
		else if (i == 40)
		{
			strName = L"Grass16";
		}
		else if (i == 41)
		{
			strName = L"Grass17";
		}
		else if (i == 42)
		{
			strName = L"Grass18";
		}
		else if (i == 43)
		{
			strName = L"Grass19";
		}
		else if (i == 44)
		{
			strName = L"Hill0";
		}
		else if (i == 45)
		{
			strName = L"Hill1";
		}
		else if (i == 46)
		{
			strName = L"Hill2";
		}
		else if (i == 47)
		{
			strName = L"House0";
		}
		else if (i == 48)
		{
			strName = L"Rock0";
		}
		else if (i == 49)
		{
			strName = L"Rock1";
		}
		else if (i == 50)
		{
			strName = L"Rock2";
		}
		else if (i == 51)
		{
			strName = L"Rock3";
		}
		else if (i == 52)
		{
			strName = L"Rock4";
		}
		else if (i == 53)
		{
			strName = L"Rock5";
		}
		else if (i == 54)
		{
			strName = L"Rock6";
		}
		else if (i == 55)
		{
			strName = L"Tree0";
		}
		else if (i == 56)
		{
			strName = L"Tree1";
		}
		else if (i == 57)
		{
			strName = L"Tree2";
		}
		else if (i == 58)
		{
			strName = L"Tree3";
		}
		else if (i == 59)
		{
			strName = L"Tree4";
		}
		else if (i == 60)
		{
			strName = L"Tree5";
		}
		else if (i == 61)
		{
			strName = L"Tree6";
		}
		else if (i == 62)
		{
			strName = L"Tree7";
		}
		else if (i == 63)
		{
			strName = L"Tree8";
		}
		else if (i == 64)
		{
			strName = L"Wall0";
		}
		else if (i == 65)
		{
			strName = L"Wall1";
		}
		else if (i == 66)
		{
			strName = L"Wall2";
		}
		else if (i == 67)
		{
			strName = L"Wall3";
		}
		else if (i == 68)
		{
			strName = L"Wall4";
		}
		else if (i == 69)
		{
			strName = L"Wall5";
		}
		else if (i == 70)
		{
			strName = L"Wall6";
		}
		else if (i == 71)
		{
			strName = L"Wall7";
		}
		else if (i == 72)
		{
			strName = L"Wall8";
		}
		else if (i == 73)
		{
			strName = L"Wall9";
		}
		else if (i == 74)
		{
			strName = L"Wall10";
		}
		else if (i == 75)
		{
			strName = L"SkHouse0";
		}
		else if (i == 76)
		{
			strName = L"SkHouse1";
		}
		else if (i == 77)
		{
			strName = L"SkHouse2";
		}
		m_mapGameObject.insert({ strName, i });
		m_ListBox_Static.AddString(strName);
	}

	for (_uint i = 0; i < 12; ++i)
	{
		_int iAnimate_num = 0;
		if (i == 0)
		{
			map<_int, _tchar*> mapTemp;
			strName = L"PoPori";
			iAnimate_num = 44;
			for (int i = 0; i < iAnimate_num; ++i)
			{
				if (i == 0)
				{

					mapTemp.insert({ i, L"JUMP_ENDLAND" });
				}
				else if (i == 1)
				{
					mapTemp.insert({ i, L"JUMP_END" });
				}
				else if (i == 2)
				{
					mapTemp.insert({ i, L"JUMP_LOOP" });
				}
				else if (i == 3)
				{
					mapTemp.insert({ i, L"JUMP_START" });
				}
				else if (i == 4)
				{
					mapTemp.insert({ i, L"PL_WAIT" });
				}
				else if (i == 5)
				{
					mapTemp.insert({ i, L"PL_RUN" });
				}
				else if (i == 6)
				{
					mapTemp.insert({ i, L"OUT_WEAPON" });
				}
				else if (i == 7)
				{
					mapTemp.insert({ i, L"IN_WEAPON" });
				}
				else if (i == 8)
				{
					mapTemp.insert({ i, L"PL_COM7" });
				}
				else if (i == 9)
				{
					mapTemp.insert({ i, L"PL_COM6R" });
				}
				else if (i == 10)
				{
					mapTemp.insert({ i, L"PL_COM6" });
				}
				else if (i == 11)
				{
					mapTemp.insert({ i, L"PL_COM5R" });
				}
				else if (i == 12)
				{
					mapTemp.insert({ i, L"PL_COM5" });
				}
				else if (i == 13)
				{
					mapTemp.insert({ i, L"PL_COM4R" });
				}
				else if (i == 14)
				{
					mapTemp.insert({ i, L"PL_COM4" });
				}
				else if (i == 15)
				{
					mapTemp.insert({ i, L"PL_COM3R" });
				}
				else if (i == 16)
				{
					mapTemp.insert({ i, L"PL_COM3" });
				}
				else if (i == 17)
				{
					mapTemp.insert({ i, L"PL_COM2R" });
				}
				else if (i == 18)
				{
					mapTemp.insert({ i, L"PL_COM2" });
				}
				else if (i == 19)
				{
					mapTemp.insert({ i, L"PL_COM1R" });
				}
				else if (i == 20)
				{
					mapTemp.insert({ i, L"PL_COM1" });
				}
				else if (i == 21)
				{
					mapTemp.insert({ i, L"PL_BACKJUMP" });
				}
				else if (i == 22)
				{
					mapTemp.insert({ i, L"PL_BACKJUMP_END" });
				}
				else if (i == 23)
				{
					mapTemp.insert({ i, L"ASS_MOVE_LOOP" });
				}
				else if (i == 24)
				{
					mapTemp.insert({ i, L"ASS_MOVE_END" });
				}
				else if (i == 25)
				{
					mapTemp.insert({ i, L"ASS_MOVE_STOP" });
				}
				else if (i == 26)
				{
					mapTemp.insert({ i, L"DASH_START" });
				}
				else if (i == 27)
				{
					mapTemp.insert({ i, L"DASH_END" });
				}
				else if (i == 28)
				{
					mapTemp.insert({ i, L"ROT_SHIRAKEN_START" });
				}
				else if (i == 29)
				{
					mapTemp.insert({ i, L"ROT_SHIRAKEN_LOOP" });
				}
				else if (i == 30)
				{
					mapTemp.insert({ i, L"ROT_SHIRAKEN_END" });
				}
				else if (i == 31)
				{
					mapTemp.insert({ i, L"FIRE_START" });
				}
				else if (i == 32)
				{
					mapTemp.insert({ i, L"FIRE_READY" });
				}
				else if (i == 33)
				{
					mapTemp.insert({ i, L"FIRE_SHOT" });
				}
				else if (i == 34)
				{
					mapTemp.insert({ i, L"FIRE_END" });
				}
				else if (i == 35)
				{
					mapTemp.insert({ i, L"TWIN_START" });
				}
				else if (i == 36)
				{
					mapTemp.insert({ i, L"TWIN_SHOT" });
				}
				else if (i == 37)
				{
					mapTemp.insert({ i, L"TWIN_R" });
				}
				else if (i == 38)
				{
					mapTemp.insert({ i, L"CALL_ELEC" });
				}
				else if (i == 39)
				{
					mapTemp.insert({ i, L"CALL_ELEC_LOOP" });
				}
				else if (i == 40)
				{
					mapTemp.insert({ i, L"CALL_ELEC_END" });
				}
				else if (i == 41)
				{
					mapTemp.insert({ i, L"B_REACT_START" });
				}
				else if (i == 42)
				{
					mapTemp.insert({ i, L"B_REACT_LOOP" });
				}
				else if (i == 43)
				{
					mapTemp.insert({ i, L"B_REACT_END" });
				}
			}
			m_mapDynamic.insert({ strName, mapTemp });
		}
		else if (i == 1)
		{
			map<_int, _tchar*> mapTemp;
			strName = L"Beatle";
			iAnimate_num = 9;
			for (int i = 0; i < iAnimate_num; ++i)
			{
				if (i == 0)
				{
					mapTemp.insert({ i, L"WAIT" });
				}
				else if (i == 1)
				{
					mapTemp.insert({ i, L"WALK" });
				}
				else if (i == 2)
				{
					mapTemp.insert({ i, L"ATT1" });
				}
				else if (i == 3)
				{
					mapTemp.insert({ i, L"FLINCH" });
				}
				else if (i == 4)
				{
					mapTemp.insert({ i, L"DEATH" });
				}
				else if (i == 5)
				{
					mapTemp.insert({ i, L"DEATHWAIT" });
				}
				else if (i == 6)
				{
					mapTemp.insert({ i, L"FLY_START" });
				}
				else if (i == 7)
				{
					mapTemp.insert({ i, L"FLYING" });
				}
				else if (i == 8)
				{
					mapTemp.insert({ i, L"FLY_END" });
				}
			}
			m_mapDynamic.insert({ strName, mapTemp });
		}
		else if (i == 2)
		{
			map<_int, _tchar*> mapTemp;
			strName = L"Bee";
			iAnimate_num = 8;
			for (int i = 0; i < iAnimate_num; ++i)
			{
				if (i == 0)
				{
					mapTemp.insert({ i, L"IDLE" });
				}
				else if (i == 1)
				{
					mapTemp.insert({ i, L"WALK" });
				}
				else if (i == 2)
				{
					mapTemp.insert({ i, L"RUN" });
				}
				else if (i == 3)
				{
					mapTemp.insert({ i, L"ATT" });
				}
				else if (i == 4)
				{
					mapTemp.insert({ i, L"DEATH" });
				}
				else if (i == 5)
				{
					mapTemp.insert({ i, L"DEATH_WAIT" });
				}
				else if (i == 6)
				{
					mapTemp.insert({ i, L"WAIT" });
				}
			}
			m_mapDynamic.insert({ strName, mapTemp });
		}
		else if (i == 3)
		{
			map<_int, _tchar*> mapTemp;
			strName = L"Mantis";
			iAnimate_num = 12;
			for (int i = 0; i < iAnimate_num; ++i)
			{
				if (i == 0)
				{
					mapTemp.insert({ i, L"IDLE" });
				}
				else if (i == 1)
				{
					mapTemp.insert({ i, L"WAIT" });
				}
				else if (i == 2)
				{
					mapTemp.insert({ i, L"WALK" });
				}
				else if (i == 3)
				{
					mapTemp.insert({ i, L"RUN" });
				}
				else if (i == 4)
				{
					mapTemp.insert({ i, L"ATT1" });
				}
				else if (i == 5)
				{
					mapTemp.insert({ i, L"ATT2_1" });
				}
				else if (i == 6)
				{
					mapTemp.insert({ i, L"ATT2_2" });
				}
				else if (i == 7)
				{
					mapTemp.insert({ i, L"ATT2_3" });
				}
				else if (i == 8)
				{
					mapTemp.insert({ i, L"ATT3" });
				}
				else if (i == 9)
				{
					mapTemp.insert({ i, L"ATT4" });
				}
				else if (i == 10)
				{
					mapTemp.insert({ i, L"DEATH" });
				}
				else if (i == 11)
				{
					mapTemp.insert({ i, L"DEATH_WAIT" });
				}
			}
			m_mapDynamic.insert({ strName, mapTemp });
		}
		else if (i == 4)
		{
			map<_int, _tchar*> mapTemp;
			strName = L"Monkey";
			iAnimate_num = 9;
			for (int i = 0; i < iAnimate_num; ++i)
			{
				if (i == 0)
				{
					mapTemp.insert({ i, L"WAIT" });
				}
				else if (i == 1)
				{
					mapTemp.insert({ i, L"WALK" });
				}
				else if (i == 2)
				{
					mapTemp.insert({ i, L"RUN" });
				}
				else if (i == 3)
				{
					mapTemp.insert({ i, L"BananaSwing" });
				}
				else if (i == 4)
				{
					mapTemp.insert({ i, L"ATT1" });
				}
				else if (i == 5)
				{
					mapTemp.insert({ i, L"DEATH" });
				}
				else if (i == 6)
				{
					mapTemp.insert({ i, L"IDLE1" });
				}
				else if (i == 7)
				{
					mapTemp.insert({ i, L"IDLE2" });
				}
				else if (i == 8)
				{
					mapTemp.insert({ i, L"IDLE3" });
				}
			}
			m_mapDynamic.insert({ strName, mapTemp });
		}
		else if (i == 5)
		{
			map<_int, _tchar*> mapTemp;
			strName = L"NPC";
			iAnimate_num = 15;
			for (int i = 0; i < iAnimate_num; ++i)
			{
				if (i == 0)
				{
					mapTemp.insert({ i, L"NONE" });
				}
				else if (i == 1)
				{
					mapTemp.insert({ i, L"IDLE" });
				}
				else if (i == 2)
				{
					mapTemp.insert({ i, L"WALK" });
				}
				else if (i == 3)
				{
					mapTemp.insert({ i, L"RUN" });
				}
				else if (i == 4)
				{
					mapTemp.insert({ i, L"CHARGE" });
				}
				else if (i == 5)
				{
					mapTemp.insert({ i, L"COBAT_IDLE" });
				}
				else if (i == 6)
				{
					mapTemp.insert({ i, L"COBAT_WALK" });
				}
				else if (i == 7)
				{
					mapTemp.insert({ i, L"ATT_A" });
				}
				else if (i == 8)
				{
					mapTemp.insert({ i, L"ATT_B" });
				}
				else if (i == 9)
				{
					mapTemp.insert({ i, L"DAMAGED" });
				}
				else if (i == 10)
				{
					mapTemp.insert({ i, L"DEATH_A" });
				}
				else if (i == 11)
				{
					mapTemp.insert({ i, L"DEATH_B" });
				}
				else if (i == 12)
				{
					mapTemp.insert({ i, L"CAST_A" });
				}
				else if (i == 13)
				{
					mapTemp.insert({ i, L"CAST_B" });
				}
				else if (i == 14)
				{
					mapTemp.insert({ i, L"CAST_LOAD" });
				}
				m_mapDynamic.insert({ strName, mapTemp });
			}
		}
		else if (i == 6)
		{
			map<_int, _tchar*> mapTemp;
			strName = L"Tower";
			iAnimate_num = 3;
			for (int i = 0; i < iAnimate_num; ++i)
			{
				if (i == 0)
				{
					mapTemp.insert({ i, L"WAIT" });
				}
				else if (i == 1)
				{
					mapTemp.insert({ i, L"DEATH" });
				}
				else if (i == 2)
				{
					mapTemp.insert({ i, L"DEATH_WAIT" });
				}
				m_mapDynamic.insert({ strName, mapTemp });
			}
		}
		else if (i == 7)
		{
			map<_int, _tchar*> mapTemp;
			strName = L"Vergos";
			iAnimate_num = 30;
			for (int i = 0; i < iAnimate_num; ++i)
			{
				if (i == 0)
				{
					mapTemp.insert({ i, L"IDLE" });
				}
				else if (i == 1)
				{
					mapTemp.insert({ i, L"ROUND_ATT_START" });
				}
				else if (i == 2)
				{
					mapTemp.insert({ i, L"ROUND_ATT_END" });
				}
				else if (i == 3)
				{
					mapTemp.insert({ i, L"ROUND_ATT_R" });
				}
				else if (i == 4)
				{
					mapTemp.insert({ i, L"ROUND_ATT2_START" });
				}
				else if (i == 5)
				{
					mapTemp.insert({ i, L"ROUND_ATT2_END" });
				}
				else if (i == 6)
				{
					mapTemp.insert({ i, L"ROUND_ATT2_R" });
				}
				else if (i == 7)
				{
					mapTemp.insert({ i, L"ROUND_MOVE1(Right_Hand)" });
				}
				else if (i == 8)
				{
					mapTemp.insert({ i, L"ROUND_MOVE2(Left_Hand)" });
				}
				else if (i == 9)
				{
					mapTemp.insert({ i, L"ROUND_MOVE3(Right_Hand)" });
				}
				else if (i == 10)
				{
					mapTemp.insert({ i, L"ROUND_MOVE4(Left_Hand)" });
				}
				else if (i == 11)
				{
					mapTemp.insert({ i, L"SPON1" });
				}
				else if (i == 12)
				{
					mapTemp.insert({ i, L"SPON2	" });
				}
				else if (i == 13)
				{
					mapTemp.insert({ i, L"ROAR_STAR	" });
				}
				else if (i == 14)
				{
					mapTemp.insert({ i, L"ROAR_LOOP	" });
				}
				else if (i == 15)
				{
					mapTemp.insert({ i, L"ROAR_END	" });
				}
				else if (i == 16)
				{
					mapTemp.insert({ i, L"FLY_STAR	" });
				}
				else if (i == 17)
				{
					mapTemp.insert({ i, L"FLY_FIRE_STAR	" });
				}
				else if (i == 18)
				{
					mapTemp.insert({ i, L"FLY_FIRE_LOOP	" });
				}
				else if (i == 19)
				{
					mapTemp.insert({ i, L"FLY_FIRE_END	" });
				}
				else if (i == 20)
				{
					mapTemp.insert({ i, L"FLY_END" });
				}
				else if (i == 21)
				{
					mapTemp.insert({ i, L"FLY_LOOP" });
				}
				else if (i == 22)
				{
					mapTemp.insert({ i, L"R_IN_ATT_START" });
				}
				else if (i == 23)
				{
					mapTemp.insert({ i, L"R_IN_ATT_LOOP" });
				}
				else if (i == 24)
				{
					mapTemp.insert({ i, L"R_IN_ATT_R" });
				}
				else if (i == 25)
				{
					mapTemp.insert({ i, L"L_IN_ATT_START" });
				}
				else if (i == 26)
				{
					mapTemp.insert({ i, L"L_IN_ATT_LOOP" });
				}
				else if (i == 27)
				{
					mapTemp.insert({ i, L"L_IN_ATT_R" });
				}
				else if (i == 28)
				{
					mapTemp.insert({ i, L"DEATH	" });
				}
				else if (i == 29)
				{
					mapTemp.insert({ i, L"DEATH_WAIT" });
				}
				m_mapDynamic.insert({ strName, mapTemp });
			}

		}
		else if (i == 8)
		{
			strName = L"UD_Worker";

		}
		else if (i == 9)
		{
			strName = L"UD_LightCarvarly";
		}
		else if (i == 10)
		{
			strName = L"UD_HeavyInfantry";
		}
		m_mapGameObject.insert({ strName, i });

		m_ListBox_Dynamic.AddString(strName);
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
	//Cliff
	if (iSelect == 0)
	{
		m_pLayerTag = L"Layer_Cliff0";
		m_pGameObjectTag = L"GameObject_Cliff0";
	}
	else if (iSelect == 1)
	{
		m_pLayerTag = L"Layer_Cliff1";
		m_pGameObjectTag = L"GameObject_Cliff1";
	}
	else if (iSelect == 2)
	{
		m_pLayerTag = L"Layer_Cliff2";
		m_pGameObjectTag = L"GameObject_Cliff2";
	}
	else if (iSelect == 3)
	{
		m_pLayerTag = L"Layer_Cliff3";
		m_pGameObjectTag = L"GameObject_Cliff3";
	}
	else if (iSelect == 4)
	{
		m_pLayerTag = L"Layer_Cliff4";
		m_pGameObjectTag = L"GameObject_Cliff4";
	}
	//Fence
	else if (iSelect == 5)
	{
		m_pLayerTag = L"Layer_Fence0";
		m_pGameObjectTag = L"GameObject_Fence0";
	}
	else if (iSelect == 6)
	{
		m_pLayerTag = L"Layer_Fence1";
		m_pGameObjectTag = L"GameObject_Fence1";
	}
	else if (iSelect == 7)
	{
		m_pLayerTag = L"Layer_Fence2";
		m_pGameObjectTag = L"GameObject_Fence2";
	}
	else if (iSelect == 8)
	{
		m_pLayerTag = L"Layer_Fence3";
		m_pGameObjectTag = L"GameObject_Fence3";
	}
	else if (iSelect == 9)
	{
		m_pLayerTag = L"Layer_Fence4";
		m_pGameObjectTag = L"GameObject_Fence4";
	}
	else if (iSelect == 10)
	{
		m_pLayerTag = L"Layer_Fence5";
		m_pGameObjectTag = L"GameObject_Fence5";
	}
	else if (iSelect == 11)
	{
		m_pLayerTag = L"Layer_Fence6";
		m_pGameObjectTag = L"GameObject_Fence6";
	}
	else if (iSelect == 12)
	{
		m_pLayerTag = L"Layer_Fence7";
		m_pGameObjectTag = L"GameObject_Fence7";
	}
	//Flower
	else if (iSelect == 13)
	{
		m_pLayerTag = L"Layer_Flower0";
		m_pGameObjectTag = L"GameObject_Flower0";
	}
	else if (iSelect == 14)
	{
		m_pLayerTag = L"Layer_Flower1";
		m_pGameObjectTag = L"GameObject_Flower1";
	}
	else if (iSelect == 15)
	{
		m_pLayerTag = L"Layer_Flower2";
		m_pGameObjectTag = L"GameObject_Flower2";
	}
	else if (iSelect == 16)
	{
		m_pLayerTag = L"Layer_Flower3";
		m_pGameObjectTag = L"GameObject_Flower3";
	}
	else if (iSelect == 17)
	{
		m_pLayerTag = L"Layer_Flower4";
		m_pGameObjectTag = L"GameObject_Flower4";
	}
	else if (iSelect == 18)
	{
		m_pLayerTag = L"Layer_Flower5";
		m_pGameObjectTag = L"GameObject_Flower5";
	}
	else if (iSelect == 19)
	{
		m_pLayerTag = L"Layer_Flower6";
		m_pGameObjectTag = L"GameObject_Flower6";

	}
	else if (iSelect == 20)
	{
		m_pLayerTag = L"Layer_Flower7";
		m_pGameObjectTag = L"GameObject_Flower7";
	}
	else if (iSelect == 21)
	{
		m_pLayerTag = L"Layer_Flower8";
		m_pGameObjectTag = L"GameObject_Flower8";
	}
	else if (iSelect == 22)
	{
		m_pLayerTag = L"Layer_Flower9";
		m_pGameObjectTag = L"GameObject_Flower9";
	}
	else if (iSelect == 23)
	{
		m_pLayerTag = L"Layer_Flower10";
		m_pGameObjectTag = L"GameObject_Flower10";
	}
	//Grass
	else if (iSelect == 24)
	{
		m_pLayerTag = L"Layer_Grass0";
		m_pGameObjectTag = L"GameObject_Grass0";
	}
	else if (iSelect == 25)
	{
		m_pLayerTag = L"Layer_Grass1";
		m_pGameObjectTag = L"GameObject_Grass1";
	}
	else if (iSelect == 26)
	{
		m_pLayerTag = L"Layer_Grass2";
		m_pGameObjectTag = L"GameObject_Grass2";
	}
	else if (iSelect == 27)
	{
		m_pLayerTag = L"Layer_Grass3";
		m_pGameObjectTag = L"GameObject_Grass3";
	}
	else if (iSelect == 28)
	{
		m_pLayerTag = L"Layer_Grass4";
		m_pGameObjectTag = L"GameObject_Grass4";
	}
	else if (iSelect == 29)
	{
		m_pLayerTag = L"Layer_Grass5";
		m_pGameObjectTag = L"GameObject_Grass5";
	}
	else if (iSelect == 30)
	{
		m_pLayerTag = L"Layer_Grass6";
		m_pGameObjectTag = L"GameObject_Grass6";
	}
	else if (iSelect == 31)
	{
		m_pLayerTag = L"Layer_Grass7";
		m_pGameObjectTag = L"GameObject_Grass7";
	}
	else if (iSelect == 32)
	{
		m_pLayerTag = L"Layer_Grass8";
		m_pGameObjectTag = L"GameObject_Grass8";
	}
	else if (iSelect == 33)
	{
		m_pLayerTag = L"Layer_Grass9";
		m_pGameObjectTag = L"GameObject_Grass9";
	}
	else if (iSelect == 34)
	{
		m_pLayerTag = L"Layer_Grass10";
		m_pGameObjectTag = L"GameObject_Grass10";
	}
	else if (iSelect == 35)
	{
		m_pLayerTag = L"Layer_Grass11";
		m_pGameObjectTag = L"GameObject_Grass11";
	}
	else if (iSelect == 36)
	{
		m_pLayerTag = L"Layer_Grass12";
		m_pGameObjectTag = L"GameObject_Grass12";
	}
	else if (iSelect == 37)
	{
		m_pLayerTag = L"Layer_Grass13";
		m_pGameObjectTag = L"GameObject_Grass13";
	}
	else if (iSelect == 38)
	{
		m_pLayerTag = L"Layer_Grass14";
		m_pGameObjectTag = L"GameObject_Grass14";
	}
	else if (iSelect == 39)
	{
		m_pLayerTag = L"Layer_Grass15";
		m_pGameObjectTag = L"GameObject_Grass15";
	}
	else if (iSelect == 40)
	{
		m_pLayerTag = L"Layer_Grass16";
		m_pGameObjectTag = L"GameObject_Grass16";
	}
	else if (iSelect == 41)
	{
		m_pLayerTag = L"Layer_Grass17";
		m_pGameObjectTag = L"GameObject_Grass17";
	}
	else if (iSelect == 42)
	{
		m_pLayerTag = L"Layer_Grass18";
		m_pGameObjectTag = L"GameObject_Grass18";
	}
	else if (iSelect == 43)
	{
		m_pLayerTag = L"Layer_Grass19";
		m_pGameObjectTag = L"GameObject_Grass19";
	}
	//Hill
	else if (iSelect == 44)
	{
		m_pLayerTag = L"Layer_Hill0";
		m_pGameObjectTag = L"GameObject_Hill0";
	}
	else if (iSelect == 45)
	{
		m_pLayerTag = L"Layer_Hill1";
		m_pGameObjectTag = L"GameObject_Hill1";
	}
	else if (iSelect == 46)
	{
		m_pLayerTag = L"Layer_Hill2";
		m_pGameObjectTag = L"GameObject_Hill2";
	}
	//House
	else if (iSelect == 47)
	{
		m_pLayerTag = L"Layer_House0";
		m_pGameObjectTag = L"GameObject_House0";
	}
	//Rock
	else if (iSelect == 48)
	{
		m_pLayerTag = L"Layer_Rock0";
		m_pGameObjectTag = L"GameObject_Rock0";
	}
	else if (iSelect == 49)
	{
		m_pLayerTag = L"Layer_Rock1";
		m_pGameObjectTag = L"GameObject_Rock1";
	}
	else if (iSelect == 50)
	{
		m_pLayerTag = L"Layer_Rock2";
		m_pGameObjectTag = L"GameObject_Rock2";
	}
	else if (iSelect == 51)
	{
		m_pLayerTag = L"Layer_Rock3";
		m_pGameObjectTag = L"GameObject_Rock3";
	}
	else if (iSelect == 52)
	{
		m_pLayerTag = L"Layer_Rock4";
		m_pGameObjectTag = L"GameObject_Rock4";
	}
	else if (iSelect == 53)
	{
		m_pLayerTag = L"Layer_Rock5";
		m_pGameObjectTag = L"GameObject_Rock5";
	}
	else if (iSelect == 54)
	{
		m_pLayerTag = L"Layer_Rock6";
		m_pGameObjectTag = L"GameObject_Rock6";
	}
	//Tree
	else if (iSelect == 55)
	{
		m_pLayerTag = L"Layer_Tree0";
		m_pGameObjectTag = L"GameObject_Tree0";
	}
	else if (iSelect == 56)
	{
		m_pLayerTag = L"Layer_Tree1";
		m_pGameObjectTag = L"GameObject_Tree1";
	}
	else if (iSelect == 57)
	{
		m_pLayerTag = L"Layer_Tree2";
		m_pGameObjectTag = L"GameObject_Tree2";
	}
	else if (iSelect == 58)
	{
		m_pLayerTag = L"Layer_Tree3";
		m_pGameObjectTag = L"GameObject_Tree3";
	}
	else if (iSelect == 59)
	{
		m_pLayerTag = L"Layer_Tree4";
		m_pGameObjectTag = L"GameObject_Tree4";
	}
	else if (iSelect == 60)
	{
		m_pLayerTag = L"Layer_Tree5";
		m_pGameObjectTag = L"GameObject_Tree5";
	}
	else if (iSelect == 61)
	{
		m_pLayerTag = L"Layer_Tree6";
		m_pGameObjectTag = L"GameObject_Tree6";
	}
	else if (iSelect == 62)
	{
		m_pLayerTag = L"Layer_Tree7";
		m_pGameObjectTag = L"GameObject_Tree7";
	}
	else if (iSelect == 63)
	{
		m_pLayerTag = L"Layer_Tree8";
		m_pGameObjectTag = L"GameObject_Tree8";
	}
	//Wall
	else if (iSelect == 64)
	{
		m_pLayerTag = L"Layer_Wall0";
		m_pGameObjectTag = L"GameObject_Wall0";
	}
	else if (iSelect == 65)
	{
		m_pLayerTag = L"Layer_Wall1";
		m_pGameObjectTag = L"GameObject_Wall1";
	}
	else if (iSelect == 66)
	{
		m_pLayerTag = L"Layer_Wall2";
		m_pGameObjectTag = L"GameObject_Wall2";
	}
	else if (iSelect == 67)
	{
		m_pLayerTag = L"Layer_Wall3";
		m_pGameObjectTag = L"GameObject_Wall3";
	}
	else if (iSelect == 68)
	{
		m_pLayerTag = L"Layer_Wall4";
		m_pGameObjectTag = L"GameObject_Wall4";
	}
	else if (iSelect == 69)
	{
		m_pLayerTag = L"Layer_Wall5";
		m_pGameObjectTag = L"GameObject_Wall5";
	}
	else if (iSelect == 70)
	{
		m_pLayerTag = L"Layer_Wall6";
		m_pGameObjectTag = L"GameObject_Wall6";
	}
	else if (iSelect == 71)
	{
		m_pLayerTag = L"Layer_Wall7";
		m_pGameObjectTag = L"GameObject_Wall7";
	}
	else if (iSelect == 72)
	{
		m_pLayerTag = L"Layer_Wall8";
		m_pGameObjectTag = L"GameObject_Wall8";
	}
	else if (iSelect == 73)
	{
		m_pLayerTag = L"Layer_Wall9";
		m_pGameObjectTag = L"GameObject_Wall9";
	}
	else if (iSelect == 74)
	{
		m_pLayerTag = L"Layer_Wall10";
		m_pGameObjectTag = L"GameObject_Wall10";
	}
	else if (iSelect == 75)
	{
		m_pLayerTag = L"Layer_SkHouse0";
		m_pGameObjectTag = L"GameObject_SkHouse0";
	}
	else if (iSelect == 76)
	{
		m_pLayerTag = L"Layer_SkHouse1";
		m_pGameObjectTag = L"GameObject_SkHouse1";
	}
	else if (iSelect == 77)
	{
		m_pLayerTag = L"Layer_SkHouse2";
		m_pGameObjectTag = L"GameObject_SkHouse2";
	}
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

		DWORD dwByte = 0;
		DWORD dwByte_Size = 0;
		int iLength = 0;
		int iLength_Layer = 0;

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

				_tchar* pName = new _tchar[iLength + 1];
				ZeroMemory(pName, iLength + 1);

				_tchar* pLayerTag = new _tchar[iLength_Layer + 1];
				ZeroMemory(pLayerTag, iLength_Layer + 1);

				lstrcpy(pName, szName);
				lstrcpy(pLayerTag, szLayerTag);

				if (FAILED(CManagement::GetInstance()->Add_GameObjectToLayer(pName, SCENE_LOGO, pLayerTag)))
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
