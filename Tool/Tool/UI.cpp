// UI.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "UI.h"
#include "afxdialogex.h"
#include "Management.h"
#include "Layer.h"


// CUI 대화 상자입니다.

IMPLEMENT_DYNAMIC(CUI, CDialogEx)

CUI::CUI(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_UI, pParent)
	, m_strSizeX(_T("100"))
	, m_strSIzeY(_T("100"))
{

}

CUI::~CUI()
{
}

void CUI::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox_Texture);
	DDX_Control(pDX, IDC_RADIO1, m_Radio_UI[0]);
	DDX_Control(pDX, IDC_RADIO2, m_Radio_UI[1]);
	DDX_Control(pDX, IDC_RADIO4, m_Radio_UI[2]);
	DDX_Control(pDX, IDC_RADIO3, m_Radio_UI[3]);
	DDX_Text(pDX, IDC_EDIT7, m_strSizeX);
	DDX_Text(pDX, IDC_EDIT8, m_strSIzeY);
}


BEGIN_MESSAGE_MAP(CUI, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CUI::OnBnClickedCreate_Buffer)
	ON_BN_CLICKED(IDC_BUTTON2, &CUI::OnBnClickedSave)
	ON_BN_CLICKED(IDC_BUTTON3, &CUI::OnBnClickedLoad)
	ON_BN_CLICKED(IDC_BUTTON6, &CUI::OnBnClickedApply)
	ON_LBN_SELCHANGE(IDC_LIST1, &CUI::OnLbnSelchangeListBox)
	ON_BN_CLICKED(IDC_BUTTON4, &CUI::OnBnClickedUp)
	ON_BN_CLICKED(IDC_BUTTON5, &CUI::OnBnClickedLeft)
	ON_BN_CLICKED(IDC_BUTTON7, &CUI::OnBnClickedDown)
	ON_BN_CLICKED(IDC_BUTTON9, &CUI::OnBnClickedRight)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CUI 메시지 처리기입니다.

void CUI::OnBnClickedCreate_Buffer()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();
	if (m_Radio_UI[0].GetCheck())
	{
		if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_HP", SCENE_LOGO, L"Layer_UI", (void*)&L"GameObject_UI_HP")))
			return;
	}
	else if (m_Radio_UI[1].GetCheck())
	{
		if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_MP", SCENE_LOGO, L"Layer_UI", (void*)&L"GameObject_UI_MP")))
			return;
	}
	else if (m_Radio_UI[2].GetCheck()) // HP_Back
	{
		if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_HP_Back", SCENE_LOGO, L"Layer_UI", (void*)&L"GameObject_UI_HP_Back")))
			return;
	}
	else if (m_Radio_UI[3].GetCheck()) // MP_Back
	{
		if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_MP_Back", SCENE_LOGO, L"Layer_UI", (void*)&L"GameObject_UI_MP_Back")))
			return;
	}
	m_IsCreateUI = true;
	Safe_Release(pManagement);
}


void CUI::OnBnClickedSave()
{
	// 저장해야할 정보: UI의 Position, Size, UI_Texture_Component_Tag, UI_GameObject_Tag
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

		for (auto& iter : *pManagement->Get_ObjectList(SCENE_LOGO, L"Layer_UI"))
		{
			if (!(*iter).GetIsUI())
				continue;

			//저장 목록
			CTexture*	pComponent = (CTexture*)(*iter).Get_ComponentPointer(L"Com_Texture");
			TCHAR		pComponent_Tag[MAX_STR] = {};
			lstrcpy(pComponent_Tag, pComponent->Get_ComponentTag());
			TCHAR		pGameObject_Tag[MAX_STR] = {};
			lstrcpy(pGameObject_Tag, (*iter).Get_GameObject_Tag());
			_float		fX = (*iter).Get_FX();
			_float		fY = (*iter).Get_FY();
			_float		fSizeX = (*iter).Get_SizeX();
			_float		fSizeY = (*iter).Get_SizeY();

			_int		iLength_Component_Tag = lstrlen(pComponent_Tag) + 1;
			_int		iLenght_GameObject_Tag = lstrlen(pGameObject_Tag) + 1;

			//Component_Tag
			WriteFile(hFile, &iLength_Component_Tag, sizeof(_int), &dwByte, nullptr);
			WriteFile(hFile, &pComponent_Tag, sizeof(TCHAR)*iLength_Component_Tag, &dwByte, nullptr);
			//GamObject_Tag
			WriteFile(hFile, &iLenght_GameObject_Tag, sizeof(_int), &dwByte, nullptr);
			WriteFile(hFile, &pGameObject_Tag, sizeof(TCHAR)*iLenght_GameObject_Tag, &dwByte, nullptr);
			//UI_Position
			WriteFile(hFile, &fX, sizeof(_float), &dwByte, nullptr);
			WriteFile(hFile, &fY, sizeof(_float), &dwByte, nullptr);
			//UI_Size
			WriteFile(hFile, &fSizeX, sizeof(_float), &dwByte, nullptr);
			WriteFile(hFile, &fSizeY, sizeof(_float), &dwByte, nullptr);
		}

		Safe_Release(pManagement);
		CloseHandle(hFile);
	}
	UpdateData(FALSE);
}


void CUI::OnBnClickedLoad()
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

		TCHAR szGameObject_Tag[MAX_STR] = L"";

		DWORD dwByte = 0;

		_int iLength_Component_Tag = 0;
		_int iLength_GameObject_Tag = 0;

		_float fX = 0.f;
		_float fY = 0.f;
		_float fSizeX = 0.f;
		_float fSizeY = 0.f;


		while (TRUE)
		{
			//Component_Tag
			ReadFile(hFile, (void*)&iLength_Component_Tag, sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, szComponentTag, sizeof(TCHAR)*iLength_Component_Tag, &dwByte, nullptr);
			//GameObject_Tag
			ReadFile(hFile, (void*)&iLength_GameObject_Tag, sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, szGameObject_Tag, sizeof(TCHAR)*iLength_GameObject_Tag, &dwByte, nullptr);
			//UI_Position
			ReadFile(hFile, (void*)&fX, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, (void*)&fY, sizeof(_float), &dwByte, nullptr);
			//UI_Size
			ReadFile(hFile, (void*)&fSizeX, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, (void*)&fSizeY, sizeof(_float), &dwByte, nullptr);

			if (dwByte == 0)
				break;

			
			if (FAILED(CManagement::GetInstance()->Add_GameObjectToLayer(szGameObject_Tag, SCENE_LOGO, L"Layer_UI")))
				return;

			CGameObject* pGameObject = CManagement::GetInstance()->Get_BackObject(SCENE_LOGO, L"Layer_UI");
			if (nullptr == pGameObject)
				return;
			pGameObject->Get_FX()=fX;
			pGameObject->Get_FY() = fY;
			pGameObject->Get_SizeX() = fSizeX;
			pGameObject->Get_SizeY() = fSizeY;
			m_pComponentTag = szComponentTag;
			pGameObject->Set_Component_Tag(szComponentTag);
			pGameObject->GetLoadUI_Texture() = true;

		}
		CloseHandle(hFile);
	}

	CGameObject* pGameObject = CManagement::GetInstance()->Get_BackObject(SCENE_LOGO, L"Layer_UI");
	int i = 0;
}

void CUI::OnBnClickedApply()
{
	UpdateData(TRUE);
	m_pTextureName = m_vecTextureName[m_iTextureIdx];
	m_IsChangeTexture = true;
	_float			fSizeX = 0.f;
	_float			fSizeY = 0.f;


	fSizeX = (_float)_tstof(m_strSizeX);
	fSizeY = (_float)_tstof(m_strSIzeY);
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();

	CGameObject* pGameObject = nullptr;
	if (m_Radio_UI[0].GetCheck())
	{
		pGameObject = pManagement->Get_GameObject(SCENE_LOGO, L"Layer_UI", 0);
		if (nullptr == pGameObject)
			return;
	}
	else if (m_Radio_UI[1].GetCheck())
	{
		pGameObject = pManagement->Get_GameObject(SCENE_LOGO, L"Layer_UI", 1);
		if (nullptr == pGameObject)
			return;		
	}
	else if (m_Radio_UI[2].GetCheck())
	{
		pGameObject = pManagement->Get_GameObject(SCENE_LOGO, L"Layer_UI", 2);
		if (nullptr == pGameObject)
			return;
	}
	else if (m_Radio_UI[3].GetCheck())
	{
		pGameObject = pManagement->Get_GameObject(SCENE_LOGO, L"Layer_UI", 3);
		if (nullptr == pGameObject)
			return;
	}

	pGameObject->Get_SizeX() = fSizeX;
	pGameObject->Get_SizeY() = fSizeY;

	Safe_Release(pManagement);
	UpdateData(FALSE);
}

void CUI::OnLbnSelchangeListBox()
{
	int iSelect = m_ListBox_Texture.GetCurSel();

	if (iSelect == -1)
		return;
	if (iSelect == 0)
	{
		m_iTextureIdx = iSelect;
	}
	else if (iSelect == 1)
	{
		m_iTextureIdx = iSelect;
	}
	else if (iSelect == 2)
	{
		m_iTextureIdx = iSelect;
	}
	else if (iSelect == 3)
	{
		m_iTextureIdx = iSelect;
	}
	else if (iSelect == 4)
	{
		m_iTextureIdx = iSelect;
	}
}

BOOL CUI::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	TCHAR* strName = L"";

	for (int i = 0; i < 5; ++i)
	{
		if (i == 0)
		{
			strName = L"Texture_Default";
		}
		else if (i == 1)
		{
			strName = L"Texture_HP";
		}
		else if (i == 2)
		{
			strName = L"Texture_MP";
		}
		else if (i == 3)
		{
			strName = L"Texture_HP_Back";
		}
		else if (i == 4)
		{
			strName = L"Texture_MP_Back";

		}
		m_vecTextureName.push_back(strName);
		m_ListBox_Texture.AddString(strName);
	}
	return TRUE;  
}

void CUI::OnMouseMove(UINT nFlags, CPoint point)
{
	UpdateData(TRUE);
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();

	map<const _tchar*, CLayer*>	mapTemp = *pManagement->Get_MapLayer(SCENE_LOGO);
	for (auto& iter : mapTemp)
	{
		auto& iter1 = iter.second->GetGameObjectList()->begin();
		for (; iter1 != iter.second->GetGameObjectList()->end(); ++iter1)
		{
			if ((*iter1)->GetIsUI())
				(*iter1)->GetIsPick() = false;
		}
	}

	m_fCrossCnt = 0.f;
	m_fSideCnt = 0.f;
	CGameObject* pGameObject = nullptr;
	if (m_IsCreateUI)
	{
		if (m_Radio_UI[0].GetCheck())
		{
			pGameObject = pManagement->Get_GameObject(SCENE_LOGO, L"Layer_UI", 0);
			if(pGameObject != nullptr)
				pGameObject->GetIsPick() = true;
		}
		else if (m_Radio_UI[1].GetCheck())
		{
			pGameObject = pManagement->Get_GameObject(SCENE_LOGO, L"Layer_UI", 1);
			if (pGameObject != nullptr)
				pGameObject->GetIsPick() = true;
		}
		else if (m_Radio_UI[2].GetCheck())
		{
			pGameObject = pManagement->Get_GameObject(SCENE_LOGO, L"Layer_UI", 2);
			if (pGameObject != nullptr)
				pGameObject->GetIsPick() = true;
		}
		else if (m_Radio_UI[3].GetCheck())
		{
			pGameObject = pManagement->Get_GameObject(SCENE_LOGO, L"Layer_UI", 3);
			if (pGameObject != nullptr)
				pGameObject->GetIsPick() = true;
		}
	}

	Safe_Release(pManagement);
	CDialogEx::OnMouseMove(nFlags, point);
	UpdateData(FALSE);
}

void CUI::OnBnClickedUp()
{
	m_IsMove = true;
	m_IsMove_Cross = true;
	m_fCrossCnt-=1.f;
}
void CUI::OnBnClickedLeft()
{
	m_IsMove = true;
	m_IsMove_Side = true;
	m_fSideCnt -= 1.f;
}
void CUI::OnBnClickedDown()
{
	m_IsMove = true; 
	m_IsMove_Cross = true;
	m_fCrossCnt += 1.f;;
}
void CUI::OnBnClickedRight()
{
	m_IsMove = true;
	m_IsMove_Side = true;
	m_fSideCnt += 1.f;
}
