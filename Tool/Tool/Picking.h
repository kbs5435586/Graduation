#pragma once
#include "Component.h"
class CPicking :
	public CComponent
{
private:
	CPicking(LPDIRECT3DDEVICE9 pGraphic_Device);
	CPicking(const CPicking& rhs);
	virtual ~CPicking()=default;
public:
	HRESULT			Ready_Picking(HWND hWnd);
	void			Update_Ray();

public:
	static	CPicking* Create(LPDIRECT3DDEVICE9 pGraphic_Device, HWND hWnd);
	virtual CComponent* Clone_Component(void* pArg);
	virtual void		Free();
public:
	_vec3 Get_MouseRayPivot() const {return m_vRayPivot;}
	_vec3 Get_MouseRay() const { return m_vRay; }
private:
	HWND			m_hWnd;
	_vec3			m_vRayPivot = {};
	_vec3			m_vRay = {};
};

