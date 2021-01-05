
// ToolView.h : CToolView 클래스의 인터페이스
//

#pragma once

class CToolDoc;
class CMainFrame;
class CSecondWnd;
class CToolView : public CView
{
protected: // serialization에서만 만들어집니다.
	CToolView();
	DECLARE_DYNCREATE(CToolView)

// 특성입니다.
public:
	CToolDoc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
	virtual ~CToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	void Mode_Unit(CMainFrame* pMainFrm, CSecondWnd* pSecondWnd);
	void Mode_Effect(CMainFrame* pMainFrm, CSecondWnd* pSecondWnd);
	void Mode_UI(CMainFrame* pMainFrm, CSecondWnd* pSecondWnd);
	void Mode_NaviMesh(CMainFrame* pMainFrm, CSecondWnd* pSecondWnd);
public:
	void Mode_Unit_Drag(CMainFrame* pMainFrm, CSecondWnd* pSecondWnd);
	void Mode_Effect_Drag(CMainFrame* pMainFrm, CSecondWnd* pSecondWnd);
	void Mode_UI_Drag(CMainFrame* pMainFrm, CSecondWnd* pSecondWnd);
	void Mode_NaviMesh_Drag(CMainFrame* pMainFrm, CSecondWnd* pSecondWnd);
public:
	_int			m_iNaviMeshCnt = 0;
};

#ifndef _DEBUG  // ToolView.cpp의 디버그 버전
inline CToolDoc* CToolView::GetDocument() const
   { return reinterpret_cast<CToolDoc*>(m_pDocument); }
#endif

