#pragma once
#include "Scene.h"
class CManagement;
class CScene_Logo :
    public CScene
{
private:
    CScene_Logo();
    virtual ~CScene_Logo() = default;
public:
	virtual HRESULT			Ready_Scene() override;
	virtual _int			Update_Scene(const _float& fTimeDelta) override;
	virtual _int			LastUpdate_Scene(const _float& fTimeDelta) override;
	virtual void			Render_Scene() override;
private:
	HRESULT					Ready_Prototype_GameObject(CManagement* pManagement);
	HRESULT					Ready_Prototype_Component(CManagement* pManagement);
	HRESULT					Ready_Layer(CManagement* pManagement);
	HRESULT					Ready_Light(CManagement* pManagement);
private:
	HRESULT					Ready_Add_Prototype_Shader(CManagement* pManagement);
	HRESULT					Ready_Add_Prototype_Buffer(CManagement* pManagement);
private:
	HRESULT					Ready_Layer_Rect(const _tchar* pLayerTag, CManagement* pManagement);
	HRESULT					Ready_Layer_Debug_Camera(const _tchar* pLayerTag, CManagement* pManagement);
public:
	CRITICAL_SECTION		m_tCritical_Section;
	HANDLE					m_hThread_Handle;
public:
	static CScene_Logo*		Create();
protected:
	virtual void			Free();
};

