// EffectTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "EffectTool.h"
#include "afxdialogex.h"
#include "Management.h"


// CEffectTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CEffectTool, CDialogEx)

CEffectTool::CEffectTool(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_EFFECTTOOL, pParent)
	, m_strMoveX(_T(""))
	, m_strMoveY(_T(""))
	, m_strMoveZ(_T(""))
	, m_strRotateX(_T(""))
	, m_strRotateY(_T(""))
	, m_strRotateZ(_T(""))
{

}

CEffectTool::~CEffectTool()
{
}

void CEffectTool::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_ListBox_Effect_Texture);
	DDX_Text(pDX, IDC_EDIT1, m_strMoveX);
	DDX_Text(pDX, IDC_EDIT2, m_strMoveY);
	DDX_Text(pDX, IDC_EDIT3, m_strMoveZ);
	DDX_Text(pDX, IDC_EDIT11, m_strRotateX);
	DDX_Text(pDX, IDC_EDIT13, m_strRotateY);
	DDX_Text(pDX, IDC_EDIT10, m_strRotateZ);
}


BEGIN_MESSAGE_MAP(CEffectTool, CDialogEx)
	ON_LBN_SELCHANGE(IDC_LIST2, &CEffectTool::OnLbnSelchangeEffectTexture)
	ON_BN_CLICKED(IDC_BUTTON9, &CEffectTool::OnBnClickedApply)
	ON_BN_CLICKED(IDC_BUTTON12, &CEffectTool::OnBnClickedUpdate)
	ON_BN_CLICKED(IDC_BUTTON13, &CEffectTool::OnBnClickedDelete)
	ON_BN_CLICKED(IDC_BUTTON11, &CEffectTool::OnBnClickedCreate)
END_MESSAGE_MAP()




void CEffectTool::OnLbnSelchangeEffectTexture()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


BOOL CEffectTool::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	_tchar*		strname = L"";
	auto& iter_find = m_mapEffectMesh.find(strname);
	if (iter_find != m_mapEffectMesh.end())
		return FALSE;


	return TRUE;  
				  
}


void CEffectTool::OnBnClickedApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CEffectTool::OnBnClickedUpdate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CEffectTool::OnBnClickedDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CEffectTool::OnBnClickedCreate()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;

	if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_EffectBuffer", SCENE_LOGO, L"Layer_EffectBuffer")))
		return ;

	Safe_Release(pManagement);
}
