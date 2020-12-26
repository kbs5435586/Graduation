#pragma once
#include "Component.h"
class CTransform;
class CFrustum
	: public CComponent
{
private:
	CFrustum(ID3D12Device* pGraphic_Device);
	virtual ~CFrustum() = default;
public:
	HRESULT					Ready_Frustum();
	HRESULT					Transform_ToWorld();
	_bool					Culling_Frustum(CTransform* pTransform, const _float& fRadius = 0.f);
private:
	_bool					Isin_Frustum(const _vec3* pPosition, const _float& fRadius);
private:
	_vec3					m_vOriginal_Point[8] = {};
	_vec3					m_vPoint[8] = {};
	Plane					m_Plane[6] = {};
	
public:
	static	CFrustum*		Create(ID3D12Device* pGraphic_Device);
	virtual CComponent*		Clone_Component(void* pArg = nullptr);
private:
	virtual void			Free();

};

