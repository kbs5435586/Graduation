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

	HRESULT					Ready_Layer(CManagement* pManagement);
	HRESULT					Ready_Light(CManagement* pManagement);

private:
	HRESULT					Ready_Add_Prototype_Texture(CManagement* pManagement);
	HRESULT					Ready_Add_Prototype_Buffer(CManagement* pManagement);
	HRESULT					Ready_Add_Prototype_Function(CManagement* pManagement);
	HRESULT					Ready_Prototype_Component(CManagement* pManagement);
public:
	HRESULT					Ready_Add_Prototype_Shader(CManagement* pManagement);
	HRESULT					Ready_Add_Prototype_Mesh(CManagement* pManagement);
private:
	HRESULT					Ready_Layer_Logo(const _tchar* pLayerTag, CManagement* pManagement);
	HRESULT					Ready_Layer_UI(const _tchar* pLayerTag, CManagement* pManagement);
private:
	void					Input_ID_IP(const _float& fTimeDelta);
public:
	CRITICAL_SECTION		m_tCritical_Section_Shader = {};
	HANDLE					m_hThread_Handle_Shader = {};
public:
	CRITICAL_SECTION		m_tCritical_Section_Mesh = {};
	HANDLE					m_hThread_Handle_Mesh = {};
public:

public:
	CRITICAL_SECTION		m_tCritical_Section_Component = {};
	HANDLE					m_hThread_Handle_Component = {};
private:
	string					m_strIP = {};
	string					m_strID = {};
private:
	_bool					m_IsIP = true;
private:
	CInput*					m_pInput = nullptr;
public:
	static CScene_Logo* Create();
protected:
	virtual void			Free();
private:
	_float					m_fInputTime = 0.1f;
};

