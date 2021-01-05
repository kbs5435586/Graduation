// AnimationTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "AnimationTool.h"
#include "afxdialogex.h"
#include "Layer.h"
#include "GameObject.h"
#include "Management.h"
#include "UnitTool.h"
#include "MainFrm.h"
#include "SecondWnd.h"


// CAnimationTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAnimationTool, CDialogEx)

CAnimationTool::CAnimationTool(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ANIMATIONTOOL, pParent)
	, m_strObjectName(_T(""))
	, m_iInput_Key(0)
{

}

CAnimationTool::~CAnimationTool()
{
}

void CAnimationTool::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strObjectName);
	DDX_Control(pDX, IDC_LIST1, m_ListBox_Picked);
	DDX_Control(pDX, IDC_LIST2, m_ListBox_New_Animation_Name);
	DDX_Text(pDX, IDC_EDIT10, m_iInput_Key);
}


BEGIN_MESSAGE_MAP(CAnimationTool, CDialogEx)
	ON_LBN_SELCHANGE(IDC_LIST1, &CAnimationTool::OnLbnSelchange_ListBox_Picked)
	ON_BN_CLICKED(IDC_BUTTON1, &CAnimationTool::OnBnClickedUpdate)
	ON_LBN_SELCHANGE(IDC_LIST2, &CAnimationTool::OnLbnSelchangeNew_Animation_Name)
	ON_BN_CLICKED(IDC_BUTTON8, &CAnimationTool::OnBnClickedAdd_Key)
	ON_BN_CLICKED(IDC_BUTTON7, &CAnimationTool::OnBnClickedAdd_Value)
	ON_BN_CLICKED(IDC_BUTTON3, &CAnimationTool::OnBnClickedPlay)
	ON_BN_CLICKED(IDC_BUTTON9, &CAnimationTool::OnBnClickedStop)
END_MESSAGE_MAP()



void CAnimationTool::OnLbnSelchange_ListBox_Picked()
{
	int iSelect = m_ListBox_Picked.GetCurSel();

	if (iSelect == -1)
		return;

	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CSecondWnd* pSecond = dynamic_cast<CSecondWnd*>(pMainFrm->m_MainSplt.GetPane(0, 1));


	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();

	CTransform* pTransform = (CTransform*)pManagement->Get_ComponentPointer(SCENE_LOGO, L"Layer_Terrain", L"Com_Transform");
	if (nullptr == pTransform)
		return;

	map<const _tchar*, CLayer*>	mapTemp = *pManagement->Get_MapLayer(SCENE_LOGO);
	for (auto& iter : mapTemp)
	{
		auto& iter1 = iter.second->GetGameObjectList()->begin();
		for (; iter1 != iter.second->GetGameObjectList()->end(); ++iter1)
		{
			if ((*iter1)->GetIsPick())
			{
				CComponent* pMesh = (*iter1)->Get_ComponentPointer(L"Com_Mesh");
				if (pMesh->Get_IsDyanmic())
				{
					(*iter1)->Set_AnimationNUm(iSelect);
				}
			}
		}
	}

	Safe_Release(pManagement);
}

void CAnimationTool::OnBnClickedUpdate()
{
	UpdateData(TRUE);
	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CSecondWnd* pSecond = dynamic_cast<CSecondWnd*>(pMainFrm->m_MainSplt.GetPane(0,1));
	

	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();

	CTransform* pTransform = (CTransform*)pManagement->Get_ComponentPointer(SCENE_LOGO, L"Layer_Terrain", L"Com_Transform");
	if (nullptr == pTransform)
		return;

	map<const _tchar*, CLayer*>	mapTemp = *pManagement->Get_MapLayer(SCENE_LOGO);
	for (auto& iter : mapTemp)
	{
		auto& iter1 = iter.second->GetGameObjectList()->begin();
		for (; iter1 != iter.second->GetGameObjectList()->end(); ++iter1)
		{
			if ((*iter1)->GetIsPick())
			{
				CComponent* pMesh =(*iter1)->Get_ComponentPointer(L"Com_Mesh");
				if (pMesh->Get_IsDyanmic())
				{	
					//pSecond->m_pUnitTool->m_mapDynamic;
					m_strObjectName = pMesh->Get_Object_FileName();
					for (auto& iter : pSecond->m_pUnitTool->m_mapDynamic)
					{
						if (iter.first == m_strObjectName)
						{
							for (auto& iter_Second : iter.second)
							{
								m_ListBox_Picked.AddString(iter_Second.second);
							}
						}
					}

				}
			}
		}
	}

	Safe_Release(pManagement);
	UpdateData(FALSE);
}


void CAnimationTool::OnLbnSelchangeNew_Animation_Name()
{
	
}


void CAnimationTool::OnBnClickedAdd_Key()
{
	UpdateData(TRUE);

	auto& iter_find = m_mapNewAnimation.find(m_iInput_Key);
	if (iter_find != m_mapNewAnimation.end())
		return;

	m_mapNewAnimation.insert({m_iInput_Key,vector<_int>()});

	UpdateData(FALSE);
}


void CAnimationTool::OnBnClickedAdd_Value()
{
	int iSelect = m_ListBox_Picked.GetCurSel();

	if (iSelect == -1)
		return;

	auto& iter_find = m_mapNewAnimation.find(m_iInput_Key);
	if (iter_find == m_mapNewAnimation.end())
		return;

	(*iter_find).second.push_back(iSelect);
}




void CAnimationTool::OnBnClickedPlay()
{
	IsPlay = true;
}


void CAnimationTool::OnBnClickedStop()
{
	IsPlay = false;
}
