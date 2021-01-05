#pragma once
#include "afxwin.h"


// CUnitTool 대화 상자입니다.
class CGameObject;
class CUnitTool : public CDialogEx
{
	DECLARE_DYNAMIC(CUnitTool)

public:
	CUnitTool(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CUnitTool();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UNITTOOL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnLbnSelchangeStatic_Mesh();
	afx_msg void OnBnClickedApply();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedLoad();
	virtual BOOL OnInitDialog();
public:
	CString m_strX_Pos;
	CString m_strY_Pos;
	CString m_strZ_Pos;
public:
	CString m_strX_Scale;
	CString m_strY_Scale;
	CString m_strZ_Scale;
public:
	CString m_strX_Rot;
	CString m_strY_Rot;
	CString m_strZ_Rot;
public:
	CListBox m_ListBox_Static;
	CListBox m_ListBox_Dynamic;
public:
	_float	m_fX_Pos = 0.f;
	_float	m_fY_Pos = 0.f;
	_float	m_fZ_Pos = 0.f;

	_float	m_fX_Scale = 0.f;
	_float	m_fY_Scale = 0.f;
	_float	m_fZ_Scale = 0.f;

	_float	m_fX_Rot = 0.f;
	_float	m_fY_Rot = 0.f;
	_float	m_fZ_Rot = 0.f;
public:
	_bool	m_IsStatic = false;
private:
	map<_tchar*, _int>		m_mapGameObject;
public:
	map<_tchar*, vector<_int>>			m_mapDynamicObject;
	map<const _tchar*, map<_int, _tchar*>>	m_mapDynamic;
public:
	_tchar*					m_pLayerTag = L"";
	_tchar*					m_pGameObjectTag = L"";	
public:
	_tchar*					m_pDynamicLayerTag = L"";
	_tchar*					m_pDynamicObjectTag = L"";
public:
	void	Update_Mode();

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedUpdate();

	afx_msg void OnLbnSelchangeDynamic_Mesh();
	CString m_strCameraSpeed;
	afx_msg void OnBnClickedCameraApply();
};
