#pragma once
#include "Component.h"
class CPicking :
    public CComponent
{
private:
	CPicking();
	CPicking(const CPicking& rhs);
	virtual ~CPicking() = default;
public:
	_vec3 Get_MouseRayPivot() const {
		return m_vRayPivot;
	}
	_vec3 Get_MouseRay() const {
		return m_vRay;
	}
public:
	HRESULT Ready_Picking(HWND hWnd);
	void Update_Ray();
private:
	HWND			m_hWnd = 0;
	_vec3			m_vRayPivot;
	_vec3			m_vRay;
public:
	static CPicking* Create( HWND hWnd);
	virtual CComponent* Clone_Component(void* pArg = nullptr);
protected:
	virtual void Free();
};

