#pragma once
#include "GameObject.h"
class CUI :
    public CGameObject
{
protected:
    CUI();
    CUI(const CUI& rhs);
    virtual ~CUI() = default;
protected:
    virtual CGameObject* Clone_GameObject(void* pArg) override;
protected:
    virtual void		 Free();
protected:
    _float	m_fX, m_fY, m_fSizeX, m_fSizeY;
    int deep;
};

