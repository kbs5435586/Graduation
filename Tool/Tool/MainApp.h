#pragma once
#include "Base.h"
#include "Device.h"

class CRenderer;
class CManagement;
class CMainApp :
	public CBase
{
public:
	CMainApp();
	virtual ~CMainApp()=default;
public:
	HRESULT Ready_MainApp();
	_int	Update_MainApp(const _float& fTimeDelta);
	void	Render_MainApp();
private:
	HRESULT Ready_Default_Setting(WINMODE eMode, const _uint& iSizeX, const _uint& iSizeY);
	HRESULT Ready_Prototype_Component();
	HRESULT Ready_Prototype_GameObject();
	HRESULT Ready_Start_Scene(SCENEID eSceneID);
	HRESULT Ready_Render_State();
	HRESULT Ready_Sampler_State();
private:
	HRESULT	Ready_Test_NaviMesh();
public:
	static CMainApp* Create();
protected:
	virtual void Free();
private:
	LPDIRECT3DDEVICE9	m_pGraphic_Device = nullptr;
	CManagement*		m_pManagement = nullptr;
	CRenderer*			m_pRenderer = nullptr;
private:
	_tchar		m_szFPS[MAX_PATH] = L"";
	_ulong		m_dwRenderCnt = 0;
	_float		m_fTimeAcc = 0.f;


};

