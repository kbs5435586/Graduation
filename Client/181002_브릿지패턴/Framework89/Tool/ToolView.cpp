
// ToolView.cpp : CToolView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
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
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CScrollView::OnFilePrintPreview)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// 전역변수
HWND g_hWnd;

// CToolView 생성/소멸

CToolView::CToolView()	
	: m_pTerrain(nullptr)
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CToolView::~CToolView()
{
	SafeDelete(m_pTerrain);
	CTextureMgr::GetInstance()->DestroyInstance();
	CDevice::GetInstance()->DestroyInstance();
}

BOOL CToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CScrollView::PreCreateWindow(cs);
}

// CToolView 그리기

void CToolView::OnDraw(CDC* pDC)
{
	CToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
	CDevice::GetInstance()->Render_Begin();

	//// 앞으로 모든 오브젝트의 그래픽 출력은 이 사이에서 진행.
	m_pTerrain->Render();

	CDevice::GetInstance()->Render_End();
}


// CToolView 인쇄

BOOL CToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CToolView 진단

#ifdef _DEBUG
void CToolView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CToolView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CToolDoc* CToolView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CToolDoc)));
	return (CToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CToolView 메시지 처리기


void CToolView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	g_hWnd = m_hWnd;

	// 스크롤 셋팅
	CScrollView::SetScrollSizes(MM_TEXT, CSize(TILECX * TILEX, TILECY * TILEY / 2));

	// 창크기를 WINCX, WINCY에 맞추기.
	// AfxGetApp: 우리 MFC의 CWinApp을 얻어옴.
	// CWinApp::GetMainWnd함수: MainFrame 윈도우를 얻어옴.
	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	
	// GetWindowRect: 자신의 윈도우 크기를 RECT구조체로 얻어오는 함수.
	RECT rcMainFrm = {};
	pMainFrm->GetWindowRect(&rcMainFrm);

	// SetRect: RECT구조체를 설정하는 함수
	SetRect(&rcMainFrm, 0, 0, 
		rcMainFrm.right - rcMainFrm.left, rcMainFrm.bottom - rcMainFrm.top);

	RECT rcView = {};
	GetClientRect(&rcView); // 현재 View윈도우의 RECT를 얻어옴.

	// Frame윈도우와 View윈도우 간의 가로, 세로 갭을 구한다.
	int iRowFrm = rcMainFrm.right - rcView.right;
	int iColFrm = rcMainFrm.bottom - rcView.bottom;

	pMainFrm->SetWindowPos(nullptr, 0, 0, 
		WINCX + iRowFrm, WINCY + iColFrm, SWP_NOZORDER);

	if (FAILED(CDevice::GetInstance()->InitDevice()))
	{
		// AfxMessageBox: MFC에서 제공하는 메시지박스 함수.
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

// OnLButtonDown함수: WM_LBUTTONDOWN 메시지가 발생하면 호출됨.
void CToolView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CScrollView::OnLButtonDown(nFlags, point);

	point.x += CScrollView::GetScrollPos(0);
	point.y += CScrollView::GetScrollPos(1);

	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMyForm* pMyForm = dynamic_cast<CMyForm*>(pMainFrm->m_SecondSplt.GetPane(1, 0));

	int iDrawID = pMyForm->m_MapTool.m_iDrawID;

	m_pTerrain->TileChange(D3DXVECTOR3(float(point.x), float(point.y), 0.f), iDrawID, 1);
	Invalidate(FALSE); // 화면갱신. WM_PAINT 메시지 발생!

	
	CMiniView* pMiniView = dynamic_cast<CMiniView*>(pMainFrm->m_SecondSplt.GetPane(0, 0));
	pMiniView->Invalidate(FALSE);
	
}

// OnMouseMove: 마우스가 움직일 때마다 발생하는 메시지로 호출.
void CToolView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CScrollView::OnMouseMove(nFlags, point);

	//if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	//{
	//	point.x += CScrollView::GetScrollPos(0);
	//	point.y += CScrollView::GetScrollPos(1);

	//	m_pTerrain->TileChange(D3DXVECTOR3(float(point.x), float(point.y), 0.f), 3);
	//	Invalidate(FALSE); // 화면갱신. WM_PAINT 메시지 발생!

	//	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	//	CMiniView* pMiniView = dynamic_cast<CMiniView*>(pMainFrm->m_SecondSplt.GetPane(0, 0));
	//	pMiniView->Invalidate(FALSE);
	//}
	
}
