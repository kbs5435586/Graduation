// UnitTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "UnitTool.h"
#include "afxdialogex.h"


// CUnitTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CUnitTool, CDialog)

CUnitTool::CUnitTool(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_UNITTOOL, pParent)
	, m_strInput(_T(""))
	, m_strOutput(_T(""))
	, m_strName(_T(""))
	, m_iAtt(0)
	, m_iDef(0)
	, m_strSearch(_T(""))
{

}

CUnitTool::~CUnitTool()
{
	for_each(m_MapData.begin(), m_MapData.end(),
		[](auto& MyPair)
	{
		SafeDelete(MyPair.second);
	});

	m_MapData.clear();

	DeleteObject(m_hBitmap);
}

// DoDataExchange함수: UpdateData함수 호출 시 호출됨.
void CUnitTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strInput);
	DDX_Text(pDX, IDC_EDIT2, m_strOutput);
	DDX_Text(pDX, IDC_EDIT3, m_strName);
	DDX_Text(pDX, IDC_EDIT4, m_iAtt);
	DDX_Text(pDX, IDC_EDIT5, m_iDef);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_RADIO1, m_Radio[0]);
	DDX_Control(pDX, IDC_RADIO2, m_Radio[1]);
	DDX_Control(pDX, IDC_RADIO3, m_Radio[2]);
	DDX_Control(pDX, IDC_CHECK1, m_CheckBox[0]);
	DDX_Control(pDX, IDC_CHECK2, m_CheckBox[1]);
	DDX_Control(pDX, IDC_CHECK3, m_CheckBox[2]);
	DDX_Text(pDX, IDC_EDIT6, m_strSearch);
	DDX_Control(pDX, IDC_BUTTON1, m_BTNPUSH);
}

BOOL CUnitTool::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	// SetCheck: 해당 버튼을 on / off 설정하는 함수.
	m_Radio[0].SetCheck(TRUE);

	// 버튼에 비트맵 입히기.
	RECT rc = {};
	m_BTNPUSH.GetClientRect(&rc);

	m_hBitmap = (HBITMAP)LoadImage(nullptr, L"../Texture/JusinLogo1.bmp",
		IMAGE_BITMAP, rc.right - rc.left, rc.bottom - rc.top,
		LR_LOADFROMFILE | LR_CREATEDIBSECTION);

	m_BTNPUSH.SetBitmap(m_hBitmap);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BEGIN_MESSAGE_MAP(CUnitTool, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CUnitTool::OnBnClickedPush)
	ON_BN_CLICKED(IDC_BUTTON2, &CUnitTool::OnBnClickedAdd)
	ON_LBN_SELCHANGE(IDC_LIST1, &CUnitTool::OnLbnSelchangeListBox)
	ON_BN_CLICKED(IDC_BUTTON3, &CUnitTool::OnBnClickedSave)
	ON_BN_CLICKED(IDC_BUTTON4, &CUnitTool::OnBnClickedLoad)
	ON_BN_CLICKED(IDC_BUTTON5, &CUnitTool::OnBnClickedDelete)
	ON_EN_CHANGE(IDC_EDIT6, &CUnitTool::OnEnChangeSearchName)
END_MESSAGE_MAP()


// CUnitTool 메시지 처리기입니다.


void CUnitTool::OnBnClickedPush()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// TRUE: 다이얼로그에 입력된 값을 매칭된 변수에 갱신.
	UpdateData(TRUE);

	m_strOutput = m_strInput;

	// FALSE: 변수에 입력된 값을 매칭된 다이얼로그 컨트롤에게 갱신.
	UpdateData(FALSE);
}


void CUnitTool::OnBnClickedAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	auto& iter_find = m_MapData.find(m_strName);

	if (m_MapData.end() == iter_find)
	{
		UNITDATA* pUnit = new UNITDATA;

		pUnit->strName = m_strName;
		pUnit->iAtt = m_iAtt;
		pUnit->iDef = m_iDef;
		pUnit->byItem = 0;

		for (int i = 0; i < 3; ++i)
		{
			// GetCheck: 해당 버튼이 체크되어 있는지 검사하는 함수.
			if (m_Radio[i].GetCheck())
				pUnit->byJobIndex = i;
		}

		if (m_CheckBox[0].GetCheck())
			pUnit->byItem |= RUBY;
		if (m_CheckBox[1].GetCheck())
			pUnit->byItem |= SAPPHIRE;
		if (m_CheckBox[2].GetCheck())
			pUnit->byItem |= DIAMOND;

		m_MapData.insert({ m_strName, pUnit });

		// AddString: 리스트 박스 목록에 문자열을 추가.
		m_ListBox.AddString(m_strName);
	}

	UpdateData(FALSE);
}


void CUnitTool::OnLbnSelchangeListBox()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	// GetCurSel: 현재 목록 중 선택된 녀석의 인덱스를 반환.
	int iSelect = m_ListBox.GetCurSel();

	if (-1 == iSelect)
		return;

	CString strSelect = L"";

	// GetText: 리스트 목록 중 인덱스에 해당하는 문자열을 얻어옴.
	m_ListBox.GetText(iSelect, strSelect);

	auto& iter_find = m_MapData.find(strSelect);

	if (m_MapData.end() == iter_find)
		return;

	m_strName = iter_find->second->strName;
	m_iAtt = iter_find->second->iAtt;
	m_iDef = iter_find->second->iDef;

	// 모든 라디오 버튼의 체크를 해제.
	for (int i = 0; i < 3; ++i)
		m_Radio[i].SetCheck(FALSE);

	m_Radio[iter_find->second->byJobIndex].SetCheck(TRUE);

	// 모든 체크박스를 해제
	for (int i = 0; i < 3; ++i)
		m_CheckBox[i].SetCheck(FALSE);

	if(iter_find->second->byItem & RUBY)	// 0111 & 0001
		m_CheckBox[0].SetCheck(TRUE);
	if (iter_find->second->byItem & SAPPHIRE)	// 0111 & 0010
		m_CheckBox[1].SetCheck(TRUE);
	if (iter_find->second->byItem & DIAMOND)	
		m_CheckBox[2].SetCheck(TRUE);

	UpdateData(FALSE);
}

void CUnitTool::OnBnClickedSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// CFileDialog: 파일 열기 혹은 저장 작업에 필요한 대화상자를 생성하는 클래스.
	CFileDialog Dlg(FALSE, L"dat", L"제목없음.dat", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Data Files(*.dat)|*.dat||", this);

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
		// GetPathName: 대화상자에서 선택된 경로를 반환.
		CString FilePath = Dlg.GetPathName();
		
		HANDLE hFile = CreateFile(FilePath.GetString(), GENERIC_WRITE, 0, 0,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
			return;

		const TCHAR* pName = nullptr;
		int iLength = 0, iAtt = 0, iDef = 0;
		BYTE byJobIndex = 0, byItem = 0;

		DWORD dwByte = 0;

		for (auto& MyPair : m_MapData)
		{
			pName = MyPair.second->strName.GetString();
			iLength = MyPair.second->strName.GetLength() + 1;
			iAtt = MyPair.second->iAtt;
			iDef = MyPair.second->iDef;
			byJobIndex = MyPair.second->byJobIndex;
			byItem = MyPair.second->byItem;

			WriteFile(hFile, &iLength, sizeof(int), &dwByte, nullptr);
			WriteFile(hFile, pName, sizeof(TCHAR) * iLength, &dwByte, nullptr);
			WriteFile(hFile, &iAtt, sizeof(int), &dwByte, nullptr);
			WriteFile(hFile, &iDef, sizeof(int), &dwByte, nullptr);
			WriteFile(hFile, &byJobIndex, sizeof(BYTE), &dwByte, nullptr);
			WriteFile(hFile, &byItem, sizeof(BYTE), &dwByte, nullptr);
		}

		CloseHandle(hFile);
	}
}

void CUnitTool::OnBnClickedLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	// CFileDialog: 파일 열기 혹은 저장 작업에 필요한 대화상자를 생성하는 클래스.
	CFileDialog Dlg(TRUE, L"dat", L"제목없음.dat", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Data Files(*.dat)|*.dat||", this);

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
		// GetPathName: 대화상자에서 선택된 경로를 반환.
		CString FilePath = Dlg.GetPathName();

		HANDLE hFile = CreateFile(FilePath.GetString(), GENERIC_READ, 0, 0,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
			return;

		// ResetContent: 현재 리스트 목록을 모두 비움.
		m_ListBox.ResetContent();

		// map 비우기.
		for_each(m_MapData.begin(), m_MapData.end(),
			[](auto& MyPair)
		{
			SafeDelete(MyPair.second);
		});

		m_MapData.clear();

		TCHAR szName[MAX_STR] = L"";
		int iLength = 0, iAtt = 0, iDef = 0;
		BYTE byJobIndex = 0, byItem = 0;

		UNITDATA* pUnit = nullptr;
		DWORD dwByte = 0;

		while(true)
		{
			ReadFile(hFile, &iLength, sizeof(int), &dwByte, nullptr);
			ReadFile(hFile, szName, sizeof(TCHAR) * iLength, &dwByte, nullptr);
			ReadFile(hFile, &iAtt, sizeof(int), &dwByte, nullptr);
			ReadFile(hFile, &iDef, sizeof(int), &dwByte, nullptr);
			ReadFile(hFile, &byJobIndex, sizeof(BYTE), &dwByte, nullptr);
			ReadFile(hFile, &byItem, sizeof(BYTE), &dwByte, nullptr);

			if (0 == dwByte)
				break;

			pUnit = new UNITDATA;
			pUnit->strName = szName;
			pUnit->iAtt = iAtt;
			pUnit->iDef = iDef;
			pUnit->byJobIndex = byJobIndex;
			pUnit->byItem = byItem;

			m_MapData.insert({ pUnit->strName, pUnit });
			m_ListBox.AddString(pUnit->strName);
		}

		CloseHandle(hFile);
	}

	UpdateData(FALSE);
}


void CUnitTool::OnBnClickedDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	CString strFind = L"";

	int iSelect = m_ListBox.GetCurSel();

	if (-1 == iSelect)
		return;

	m_ListBox.GetText(iSelect, strFind);

	auto& iter_find = m_MapData.find(strFind);

	if (m_MapData.end() == iter_find)
		return;

	SafeDelete(iter_find->second);
	m_MapData.erase(strFind);

	// 해당 인덱스의 문자열을 ListBox 목록에서 제거.
	m_ListBox.DeleteString(iSelect);

	UpdateData(FALSE);
}


void CUnitTool::OnEnChangeSearchName()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	auto& iter_find = m_MapData.find(m_strSearch);

	if (m_MapData.end() == iter_find)
		return;

	m_strName = iter_find->second->strName;
	m_iAtt = iter_find->second->iAtt;
	m_iDef = iter_find->second->iDef;

	for (int i = 0; i < 3; ++i)
	{
		m_Radio[i].SetCheck(FALSE);
		m_CheckBox[i].SetCheck(FALSE);
	}

	m_Radio[iter_find->second->byJobIndex].SetCheck(TRUE);

	if (iter_find->second->byItem & RUBY)
		m_CheckBox[0].SetCheck(TRUE);
	if (iter_find->second->byItem & SAPPHIRE)
		m_CheckBox[1].SetCheck(TRUE);
	if (iter_find->second->byItem & DIAMOND)
		m_CheckBox[2].SetCheck(TRUE);

	// FindString(startIndex, string): startIndex부터 string을 검색.
	int iIndex = m_ListBox.FindString(0, m_strSearch);

	if (-1 == iIndex)
		return;

	// SetCurSel: 인덱스에 해당하는 목록을 리스트박스에서 선택되어지게 보임.
	m_ListBox.SetCurSel(iIndex);

	UpdateData(FALSE);
}
