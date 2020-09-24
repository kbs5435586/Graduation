#pragma once
#include "Base.h"
class CGameObject_Manager;
class CScene :
    public CBase
{
protected:
    CScene(ID3D12Device* pGraphic_Device);
    virtual ~CScene() = default;
public:
	virtual HRESULT			Ready_Scene();
	virtual _int			Update_Scene(const _float& fTimeDelta);
	virtual _int			LastUpdate_Scene(const _float& fTimeDelta);
	virtual void			Render_Scene();
protected:
	virtual void			Free();
protected:
	ID3D12Device*			m_pGraphic_Device = nullptr;
private:
	CGameObject_Manager*	m_pObject_Manager = nullptr;  
};

