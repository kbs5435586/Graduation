
// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "Tool.h"

#include "MainFrm.h"
#include "ToolView.h"
#include "MyForm.h"
#include "MiniView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	//if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
	//	!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	//{
	//	TRACE0("도구 모음을 만들지 못했습니다.\n");
	//	return -1;      // 만들지 못했습니다.
	//}

	//if (!m_wndStatusBar.Create(this))
	//{
	//	TRACE0("상태 표시줄을 만들지 못했습니다.\n");
	//	return -1;      // 만들지 못했습니다.
	//}
	//m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	//// TODO: 도구 모음을 도킹할 수 없게 하려면 이 세 줄을 삭제하십시오.
	//m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	//EnableDocking(CBRS_ALIGN_ANY);
	//DockControlBar(&m_wndToolBar);


	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return TRUE;
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기


// OnCreateClient: Frame윈도우에 배치될 윈도우들을 생성하는 함수.
// OnCreateClient함수를 오버라이딩하여 사용자가 윈도우 생성을 직접 컨트롤할 수 있다.
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	// 창 3개 분할
	m_MainSplt.CreateStatic(this, 1, 2);
	m_MainSplt.CreateView(0, 1, RUNTIME_CLASS(CToolView), CSize(WINCX, WINCY), pContext);

	// SetColumnInfo(열 번호, 열의 크기, 허용가능한 최소 크기)
	m_MainSplt.SetColumnInfo(0, 300, 10);

	m_SecondSplt.CreateStatic(&m_MainSplt, 2, 1, WS_CHILD | WS_VISIBLE,
		m_MainSplt.IdFromRowCol(0, 0));	// 부모 Splt의 0, 0 영역에 배치.

	m_SecondSplt.CreateView(0, 0, RUNTIME_CLASS(CMiniView), CSize(300, 300), pContext);
	m_SecondSplt.CreateView(1, 0, RUNTIME_CLASS(CMyForm), CSize(300, 300), pContext);







	//// 창 4개 분할
	//m_MainSplt.CreateStatic(this, 2, 2);

	//m_MainSplt.CreateView(0, 0, RUNTIME_CLASS(CMiniView), CSize(300, 300), pContext);
	//m_MainSplt.CreateView(0, 1, RUNTIME_CLASS(CMyForm), CSize(WINCX, 300), pContext);
	//m_MainSplt.CreateView(1, 0, RUNTIME_CLASS(CMyForm), CSize(300, WINCY), pContext);
	//m_MainSplt.CreateView(1, 1, RUNTIME_CLASS(CToolView), CSize(WINCX, WINCY), pContext);





	//// 창 2개 분할
	//// CreateStatic: 분할 윈도우를 생성.
	//// CreateStatic(부모윈도우, 행, 열)
	//m_MainSplt.CreateStatic(this, 1, 2);

	//// CreateView: 분할된 영역에 View를 생성하여 배치.
	//m_MainSplt.CreateView(0, 0, RUNTIME_CLASS(CMyForm), CSize(300, WINCY), pContext);
	//m_MainSplt.CreateView(0, 1, RUNTIME_CLASS(CToolView), CSize(WINCX, WINCY), pContext);


	return TRUE; /*CFrameWnd::OnCreateClient(lpcs, pContext);*/
}
