#pragma once
#include "Base.h"
class CGameObject_Manager;
class CScene :
    public CBase
{
protected:
    CScene();
    virtual ~CScene() = default;
public:
	virtual HRESULT			Ready_Scene();
	virtual _int			Update_Scene(const _float& fTimeDelta);
	virtual _int			LastUpdate_Scene(const _float& fTimeDelta);
	virtual void			Render_Scene();
protected:
	virtual void			Free();
private:
	CGameObject_Manager*	m_pObject_Manager = nullptr;  
};
