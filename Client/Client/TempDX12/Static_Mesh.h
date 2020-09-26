#pragma once
#include "Mesh.h"
class CStatic_Mesh :
    public CMesh
{
private:
	CStatic_Mesh(ID3D12Device* pGraphic_Device);
	CStatic_Mesh(const CStatic_Mesh& rhs);
	virtual ~CStatic_Mesh() = default;
public:
    HRESULT         Ready_Component();
    void            Render_Mesh(_matrix matWorld);
protected:
    ID3D12Device*   m_pGraphic_Device = nullptr;
    _bool           m_IsClone = false;
public:
    virtual CComponent* Clone_Component(void* pArg = nullptr) = 0;
protected:
    virtual void Free();
};

