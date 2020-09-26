#pragma once
#include "Component.h"
class CShader;
class CMesh :
    public CComponent
{
protected:
	CMesh(ID3D12Device* pGraphic_Device);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;
public:
	HRESULT						Ready_VIBuffer();
	virtual void				Render_Mesh(_matrix matWorld) = 0;;
public:
	virtual CComponent* Clone_Component(void* pArg = nullptr) = 0;
protected:
	virtual void				Free() = 0;
protected:
	FbxScene* m_pFbxScene = nullptr;

};

