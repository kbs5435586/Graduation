#pragma once
#include "Base.h"
class CRenderer;
class CFrustum;
class CManagement;

class CMainApp :
    public CBase
{
private:
    CMainApp();
    virtual ~CMainApp() = default;
public:
    HRESULT                     Ready_MainApp();
    _int                        Update_MainApp(const _float& fTimeDelta);
    void                        Render_MainApp();
private:
    HRESULT                     Ready_Device();
    HRESULT                     Ready_Start_Scene(SCENEID eID);
    HRESULT                     Ready_Sound();
private:
    HRESULT                     SetUp_OnShader(const _float& fTimeDelta);
private:
    void                        Compute_Frame();
    void                        SetTime(const _float& fTimeDelta);
private:
    HRESULT                     Create_FbxManager();
    HRESULT                     Ready_Prototype_Component();
    HRESULT                     Ready_Prototype_GameObject();
private:
    _float                        Lerp(_float a, _float b, _float val);
private: 
	_tchar				        m_szFPS[MAX_PATH] = L"";
	_ulong				        m_dwRenderCnt = 0;
	_float				        m_fTimeAcc = 0.f;
private:
    CManagement*                m_pManagement = nullptr;
    CRenderer*                  m_pRenderer = nullptr;
    CFrustum*                   m_pFrustum = nullptr;
    GLOBAL                      m_tGlobal;
private:
    _float                      m_fTimeDelta=0.f;
    _float                      m_fGoldTime = 0.f;
public:
    static				        CMainApp* Create();
private:
    virtual void                Free(); 
private:
    _float                      m_fDateTime = 0.f;
    _float                      m_fDatePer = 0.f;
};

