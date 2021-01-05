#pragma once
#include "Base.h"
class CLine :
	public CBase
{
private:
	explicit CLine(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLine() = default;
public:
	HRESULT Ready_Line(const _vec3& vStart, const _vec3& vEnd);
	_bool is_Right(const _vec3& vPosition);
private:
	LPDIRECT3DDEVICE9		m_pGraphic_Device = nullptr;
private:
	_vec3			m_vStart;
	_vec3			m_vEnd;
	_vec3			m_vNormal;
public:
	static CLine* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _vec3& vStart, const _vec3& vEnd);
	virtual void Free();

};

