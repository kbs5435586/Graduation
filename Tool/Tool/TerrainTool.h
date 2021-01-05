#pragma once
#include "afxwin.h"


// CTerrainTool 대화 상자입니다.

class CGameObject;
class CTerrainTool : public CDialogEx
{
	DECLARE_DYNAMIC(CTerrainTool)

public:
	CTerrainTool(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTerrainTool();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TERRAINTOOL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void			OnBnClickedApply();
	afx_msg void			OnLbnSelchangeTerrain_ComponentList();
	afx_msg void			OnBnClickedAdd();
	afx_msg void			OnBnClickedLoad();
	afx_msg void			OnBnClickedSave();
	virtual BOOL			OnInitDialog();
public:
	CString					m_strInterval;
	CString					m_strSide;
	CString					m_strCross;
	CString					m_strComponentTag;
	CListBox				m_ListBox_Terrain_Component;
public:
	LPDIRECT3DDEVICE9		m_pGraphic_Device = nullptr;
	CGameObject*			m_pGameObject = nullptr;
public:
	vector<MAPINFO*>		m_vecMapInfo;
	_tchar*					m_pOutput_Component_tag=nullptr;
public:
	_float					m_fInterval = 1.f;
	_float					m_fSizeX = 0.f;
	_float					m_fSizeZ = 0.f;
public:
	_int					m_iComponent_Tag_Idx = 0;
	_bool					m_IsApply = false;


};
