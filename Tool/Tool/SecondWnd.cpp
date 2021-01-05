// SecondWnd.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "SecondWnd.h"
#include "MainFrm.h"
#include "UnitTool.h"
#include "EffectTool.h"
#include "UI.h"
#include "NaviMesh.h"
#include "AnimationTool.h"
#include "TerrainTool.h"
#include "Management.h"
// CSecondWnd

IMPLEMENT_DYNCREATE(CSecondWnd, CFormView)

CSecondWnd::CSecondWnd()
	: CFormView(IDD_SECONDWND)
{

}

CSecondWnd::~CSecondWnd()
{
	delete m_pUnitTool;
	delete m_pEffectTool;
	delete m_pNaviMesh;
	delete m_pUI;
	delete m_pAnimationTool;
	delete m_pTerrainTool;
}

void CSecondWnd::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_Tab);
}

BEGIN_MESSAGE_MAP(CSecondWnd, CFormView)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CSecondWnd::OnTcnSelchangeTab1)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CSecondWnd 진단입니다.

#ifdef _DEBUG
void CSecondWnd::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CSecondWnd::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CSecondWnd 메시지 처리기입니다.


void CSecondWnd::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	m_Tab.InsertItem(0, L"UnitTool");
	m_Tab.InsertItem(1, L"EffectTool");
	m_Tab.InsertItem(2, L"UI");
	m_Tab.InsertItem(3, L"Navi");
	m_Tab.InsertItem(4, L"Animation");
	m_Tab.InsertItem(5, L"Terrain");

	m_Tab.SetCurSel(0);
	 
	CRect rect;
	m_Tab.GetWindowRect(&rect);

	m_pUnitTool = new CUnitTool;
	m_pUnitTool->Create(IDD_UNITTOOL, &m_Tab);
	m_pUnitTool->MoveWindow(0, 25, rect.Width(), rect.Height());
	m_pUnitTool->ShowWindow(SW_SHOW);

	m_pEffectTool = new CEffectTool;
	m_pEffectTool->Create(IDD_EFFECTTOOL, &m_Tab);
	m_pEffectTool->MoveWindow(0, 25, rect.Width(), rect.Height());
	m_pEffectTool->ShowWindow(SW_HIDE);

	m_pUI = new CUI;
	m_pUI->Create(IDD_UI, &m_Tab);
	m_pUI->MoveWindow(0, 25, rect.Width(), rect.Height());
	m_pUI->ShowWindow(SW_HIDE);

	m_pNaviMesh = new CNaviMesh;
	m_pNaviMesh->Create(IDD_NAVIMESH, &m_Tab);
	m_pNaviMesh->MoveWindow(0, 25, rect.Width(), rect.Height());
	m_pNaviMesh->ShowWindow(SW_HIDE);

	m_pAnimationTool = new CAnimationTool;
	m_pAnimationTool->Create(IDD_ANIMATIONTOOL, &m_Tab);
	m_pAnimationTool->MoveWindow(0, 25, rect.Width(), rect.Height());
	m_pAnimationTool->ShowWindow(SW_HIDE);

	m_pTerrainTool = new CTerrainTool;
	m_pTerrainTool->Create(IDD_TERRAINTOOL, &m_Tab);
	m_pTerrainTool->MoveWindow(0, 25, rect.Width(), rect.Height());
	m_pTerrainTool->ShowWindow(SW_HIDE);
	eMode = MODE_UNIT;
}


void CSecondWnd::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	switch (m_Tab.GetCurSel())
	{
	case 0:
		m_pUnitTool->ShowWindow(SW_SHOW);
		m_pEffectTool->ShowWindow(SW_HIDE);
		m_pUI->ShowWindow(SW_HIDE);
		m_pNaviMesh->ShowWindow(SW_HIDE);
		m_pAnimationTool->ShowWindow(SW_HIDE);
		m_pTerrainTool->ShowWindow(SW_HIDE);
		eMode = MODE_UNIT;
		break;
	case 1:
		m_pEffectTool->ShowWindow(SW_SHOW);
		m_pUnitTool->ShowWindow(SW_HIDE);
		m_pUI->ShowWindow(SW_HIDE);
		m_pNaviMesh->ShowWindow(SW_HIDE);
		m_pAnimationTool->ShowWindow(SW_HIDE);
		m_pTerrainTool->ShowWindow(SW_HIDE);
		eMode = MODE_EFFECT;
		break;
	case 2:
		m_pUI->ShowWindow(SW_SHOW);
		m_pEffectTool->ShowWindow(SW_HIDE);
		m_pUnitTool->ShowWindow(SW_HIDE);
		m_pNaviMesh->ShowWindow(SW_HIDE);
		m_pAnimationTool->ShowWindow(SW_HIDE);
		m_pTerrainTool->ShowWindow(SW_HIDE);
		eMode = MODE_UI;
		break;
	case 3:
		m_pNaviMesh->ShowWindow(SW_SHOW);
		m_pEffectTool->ShowWindow(SW_HIDE);
		m_pUnitTool->ShowWindow(SW_HIDE);
		m_pUI->ShowWindow(SW_HIDE);
		m_pAnimationTool->ShowWindow(SW_HIDE);
		m_pTerrainTool->ShowWindow(SW_HIDE);
		eMode = MODE_NAVIMESH;
		break;
	case 4:
		m_pAnimationTool->ShowWindow(SW_SHOW);
		m_pNaviMesh->ShowWindow(SW_HIDE);
		m_pEffectTool->ShowWindow(SW_HIDE);
		m_pUnitTool->ShowWindow(SW_HIDE);
		m_pUI->ShowWindow(SW_HIDE);
		m_pTerrainTool->ShowWindow(SW_HIDE);
		eMode = MODE_ANIMATE;
		break;
	case 5:
		m_pTerrainTool->ShowWindow(SW_SHOW);
		m_pAnimationTool->ShowWindow(SW_HIDE);
		m_pNaviMesh->ShowWindow(SW_HIDE);
		m_pEffectTool->ShowWindow(SW_HIDE);
		m_pUnitTool->ShowWindow(SW_HIDE);
		m_pUI->ShowWindow(SW_HIDE);
		eMode = MODE_TERRAIN;
		break;
	}
	*pResult = 0;
}


void CSecondWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	CFormView::OnLButtonDown(nFlags, point);


}
