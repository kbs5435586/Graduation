#pragma once
#include "Component.h"
class CCell;
class CNavi :
	public CComponent
{
private:
	CNavi(LPDIRECT3DDEVICE9 pGraphic_Device);
	CNavi(const CNavi& rhs);
	virtual ~CNavi()=default;
public:
	HRESULT Ready_Navigation();
	HRESULT Ready_Neighbor();
	HRESULT Ready_Clone_Navigation(void* pArg);
	_bool Move_OnNavigation(const _vec3* vPosition, const _vec3* vDirectionPerSec);
	void Render_Navigation();
public:
	HRESULT		Delete_Cell();
public:
	HRESULT		Set_PositionVector(_vec3 vPos);
	HRESULT		Set_CellVector();
	HRESULT		Set_CellVector(vector<_vec3> vTotalPos);
	void		Clear_Position_Vector();
public:
	vector<_vec3>	&GetTotalPos() { return m_vTotlaPos; }
	vector<CCell*>	&GetCellVector() { return m_vecCell; }
private:
	vector<CCell*>			m_vecCell;
	typedef vector<CCell*>	VECCELL;
private:
	vector<_vec3>				m_vPos;
	vector<_vec3>				m_vTotlaPos;
private:
	_uint	m_iCurrentIdx = 0;
public:
	static CNavi* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone_Component(void* pArg);
	virtual void Free();

};

