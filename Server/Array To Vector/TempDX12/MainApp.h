#pragma once
#include "Base.h"
class CDevice;
class CManagement;
class CRenderer;
class CMainApp :
    public CBase
{
private:
    CMainApp();
	virtual ~CMainApp() =default;
public:
	HRESULT				Ready_MainApp();
	_int				Update_MainApp(const _float& fTimeDelta);
	void				Render_MainApp();
private:
	HRESULT				Ready_Device();
	HRESULT				Ready_Start_Scene(SCENEID eSceneID);
	void				ComputeFrame();
private:
	HRESULT				Ready_Prototype_Component();
	HRESULT				Ready_Prototype_GameObject();
public:
	static				CMainApp* Create();
	virtual void		Free();
private:
	_tchar				m_szFPS[MAX_PATH] = L"";
	_ulong				m_dwRenderCnt = 0;
	_float				m_fTimeAcc = 0.f;
private:
	CDevice*			m_pDevice = nullptr;
	CManagement*		m_pManagement = nullptr;
	ID3D12Device*		m_pGraphic_Device = nullptr;
	CRenderer*			m_pRenderer = nullptr;



};

