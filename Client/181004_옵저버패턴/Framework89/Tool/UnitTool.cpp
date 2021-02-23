// UnitTool.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Tool.h"
#include "UnitTool.h"
#include "afxdialogex.h"


// CUnitTool ��ȭ �����Դϴ�.

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

// DoDataExchange�Լ�: UpdateData�Լ� ȣ�� �� ȣ���.
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

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	// SetCheck: �ش� ��ư�� on / off �����ϴ� �Լ�.
	m_Radio[0].SetCheck(TRUE);

	// ��ư�� ��Ʈ�� ������.
	RECT rc = {};
	m_BTNPUSH.GetClientRect(&rc);

	m_hBitmap = (HBITMAP)LoadImage(nullptr, L"../Texture/JusinLogo1.bmp",
		IMAGE_BITMAP, rc.right - rc.left, rc.bottom - rc.top,
		LR_LOADFROMFILE | LR_CREATEDIBSECTION);

	m_BTNPUSH.SetBitmap(m_hBitmap);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
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


// CUnitTool �޽��� ó�����Դϴ�.


void CUnitTool::OnBnClickedPush()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	// TRUE: ���̾�α׿� �Էµ� ���� ��Ī�� ������ ����.
	UpdateData(TRUE);

	m_strOutput = m_strInput;

	// FALSE: ������ �Էµ� ���� ��Ī�� ���̾�α� ��Ʈ�ѿ��� ����.
	UpdateData(FALSE);
}


void CUnitTool::OnBnClickedAdd()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
			// GetCheck: �ش� ��ư�� üũ�Ǿ� �ִ��� �˻��ϴ� �Լ�.
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

		// AddString: ����Ʈ �ڽ� ��Ͽ� ���ڿ��� �߰�.
		m_ListBox.AddString(m_strName);
	}

	UpdateData(FALSE);
}


void CUnitTool::OnLbnSelchangeListBox()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);

	// GetCurSel: ���� ��� �� ���õ� �༮�� �ε����� ��ȯ.
	int iSelect = m_ListBox.GetCurSel();

	if (-1 == iSelect)
		return;

	CString strSelect = L"";

	// GetText: ����Ʈ ��� �� �ε����� �ش��ϴ� ���ڿ��� ����.
	m_ListBox.GetText(iSelect, strSelect);

	auto& iter_find = m_MapData.find(strSelect);

	if (m_MapData.end() == iter_find)
		return;

	m_strName = iter_find->second->strName;
	m_iAtt = iter_find->second->iAtt;
	m_iDef = iter_find->second->iDef;

	// ��� ���� ��ư�� üũ�� ����.
	for (int i = 0; i < 3; ++i)
		m_Radio[i].SetCheck(FALSE);

	m_Radio[iter_find->second->byJobIndex].SetCheck(TRUE);

	// ��� üũ�ڽ��� ����
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	// CFileDialog: ���� ���� Ȥ�� ���� �۾��� �ʿ��� ��ȭ���ڸ� �����ϴ� Ŭ����.
	CFileDialog Dlg(FALSE, L"dat", L"�������.dat", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Data Files(*.dat)|*.dat||", this);

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
		// GetPathName: ��ȭ���ڿ��� ���õ� ��θ� ��ȯ.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);

	// CFileDialog: ���� ���� Ȥ�� ���� �۾��� �ʿ��� ��ȭ���ڸ� �����ϴ� Ŭ����.
	CFileDialog Dlg(TRUE, L"dat", L"�������.dat", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Data Files(*.dat)|*.dat||", this);

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
		// GetPathName: ��ȭ���ڿ��� ���õ� ��θ� ��ȯ.
		CString FilePath = Dlg.GetPathName();

		HANDLE hFile = CreateFile(FilePath.GetString(), GENERIC_READ, 0, 0,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
			return;

		// ResetContent: ���� ����Ʈ ����� ��� ���.
		m_ListBox.ResetContent();

		// map ����.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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

	// �ش� �ε����� ���ڿ��� ListBox ��Ͽ��� ����.
	m_ListBox.DeleteString(iSelect);

	UpdateData(FALSE);
}


void CUnitTool::OnEnChangeSearchName()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CDialog::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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

	// FindString(startIndex, string): startIndex���� string�� �˻�.
	int iIndex = m_ListBox.FindString(0, m_strSearch);

	if (-1 == iIndex)
		return;

	// SetCurSel: �ε����� �ش��ϴ� ����� ����Ʈ�ڽ����� ���õǾ����� ����.
	m_ListBox.SetCurSel(iIndex);

	UpdateData(FALSE);
}
