
// ToolView.cpp : CToolView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "Tool.h"
#endif

#include "ToolDoc.h"
#include "ToolView.h"
#include "MainFrm.h"
#include "MiniView.h"
#include "MyForm.h"
#include "Terrain.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CToolView

IMPLEMENT_DYNCREATE(CToolView, CScrollView)

BEGIN_MESSAGE_MAP(CToolView, CScrollView)
	// ǥ�� �μ� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CScrollView::OnFilePrintPreview)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// ��������
HWND g_hWnd;

// CToolView ����/�Ҹ�

CToolView::CToolView()	
	: m_pTerrain(nullptr)
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CToolView::~CToolView()
{
	SafeDelete(m_pTerrain);
	CTextureMgr::GetInstance()->DestroyInstance();
	CDevice::GetInstance()->DestroyInstance();
}

BOOL CToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CScrollView::PreCreateWindow(cs);
}

// CToolView �׸���

void CToolView::OnDraw(CDC* pDC)
{
	CToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
	CDevice::GetInstance()->Render_Begin();

	//// ������ ��� ������Ʈ�� �׷��� ����� �� ���̿��� ����.
	m_pTerrain->Render();

	CDevice::GetInstance()->Render_End();
}


// CToolView �μ�

BOOL CToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �⺻���� �غ�
	return DoPreparePrinting(pInfo);
}

void CToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ��ϱ� ���� �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
}

void CToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ� �� ���� �۾��� �߰��մϴ�.
}


// CToolView ����

#ifdef _DEBUG
void CToolView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CToolView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CToolDoc* CToolView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CToolDoc)));
	return (CToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CToolView �޽��� ó����


void CToolView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	g_hWnd = m_hWnd;

	// ��ũ�� ����
	CScrollView::SetScrollSizes(MM_TEXT, CSize(TILECX * TILEX, TILECY * TILEY / 2));

	// âũ�⸦ WINCX, WINCY�� ���߱�.
	// AfxGetApp: �츮 MFC�� CWinApp�� ����.
	// CWinApp::GetMainWnd�Լ�: MainFrame �����츦 ����.
	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	
	// GetWindowRect: �ڽ��� ������ ũ�⸦ RECT����ü�� ������ �Լ�.
	RECT rcMainFrm = {};
	pMainFrm->GetWindowRect(&rcMainFrm);

	// SetRect: RECT����ü�� �����ϴ� �Լ�
	SetRect(&rcMainFrm, 0, 0, 
		rcMainFrm.right - rcMainFrm.left, rcMainFrm.bottom - rcMainFrm.top);

	RECT rcView = {};
	GetClientRect(&rcView); // ���� View�������� RECT�� ����.

	// Frame������� View������ ���� ����, ���� ���� ���Ѵ�.
	int iRowFrm = rcMainFrm.right - rcView.right;
	int iColFrm = rcMainFrm.bottom - rcView.bottom;

	pMainFrm->SetWindowPos(nullptr, 0, 0, 
		WINCX + iRowFrm, WINCY + iColFrm, SWP_NOZORDER);

	if (FAILED(CDevice::GetInstance()->InitDevice()))
	{
		// AfxMessageBox: MFC���� �����ϴ� �޽����ڽ� �Լ�.
		AfxMessageBox(L"InitDevice Failed");
		return;
	}

	if(FAILED(CTextureMgr::GetInstance()->InsertTexture(CTextureMgr::TEX_SINGLE,
		L"../Texture/Cube.png", L"Cube")))
	{
		AfxMessageBox(L"Cube Image Insert Failed");
		return;
	}

	if (FAILED(CTextureMgr::GetInstance()->InsertTexture(CTextureMgr::TEX_MULTI,
		L"../Texture/Stage/TERRAIN/Tile/Tile%d.png", L"Terrain", L"Tile", 38)))
	{
		AfxMessageBox(L"TERRAIN Image Insert Failed");
		return;
	}

	if (nullptr == m_pTerrain)
	{
		m_pTerrain = new CTerrain;

		if (FAILED(m_pTerrain->Initialize()))
		{
			SafeDelete(m_pTerrain);
			AfxMessageBox(L"Terrain Object Create Failed!!");
			return;
		}

		m_pTerrain->SetViewWnd(this);
	}
}

// OnLButtonDown�Լ�: WM_LBUTTONDOWN �޽����� �߻��ϸ� ȣ���.
void CToolView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	CScrollView::OnLButtonDown(nFlags, point);

	point.x += CScrollView::GetScrollPos(0);
	point.y += CScrollView::GetScrollPos(1);

	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMyForm* pMyForm = dynamic_cast<CMyForm*>(pMainFrm->m_SecondSplt.GetPane(1, 0));

	int iDrawID = pMyForm->m_MapTool.m_iDrawID;

	m_pTerrain->TileChange(D3DXVECTOR3(float(point.x), float(point.y), 0.f), iDrawID, 1);
	Invalidate(FALSE); // ȭ�鰻��. WM_PAINT �޽��� �߻�!

	
	CMiniView* pMiniView = dynamic_cast<CMiniView*>(pMainFrm->m_SecondSplt.GetPane(0, 0));
	pMiniView->Invalidate(FALSE);
	
}

// OnMouseMove: ���콺�� ������ ������ �߻��ϴ� �޽����� ȣ��.
void CToolView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	CScrollView::OnMouseMove(nFlags, point);

	//if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	//{
	//	point.x += CScrollView::GetScrollPos(0);
	//	point.y += CScrollView::GetScrollPos(1);

	//	m_pTerrain->TileChange(D3DXVECTOR3(float(point.x), float(point.y), 0.f), 3);
	//	Invalidate(FALSE); // ȭ�鰻��. WM_PAINT �޽��� �߻�!

	//	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	//	CMiniView* pMiniView = dynamic_cast<CMiniView*>(pMainFrm->m_SecondSplt.GetPane(0, 0));
	//	pMiniView->Invalidate(FALSE);
	//}
	
}
