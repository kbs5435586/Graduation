#include "stdafx.h"
#include "Line.h"



CLine::CLine(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device)
{
	m_pGraphic_Device->AddRef();
}

HRESULT CLine::Ready_Line(const _vec3 & vStart, const _vec3 & vEnd)
{
	m_vStart = vStart;
	m_vEnd = vEnd;

	_vec3	vDir = vEnd - vStart;

	m_vNormal = _vec3(vDir.z * -1.f, 0.f, vDir.x);

	D3DXVec3Normalize(&m_vNormal, &m_vNormal);

	return NOERROR;
}

_bool CLine::is_Right(const _vec3 & vPosition)
{
	_vec3		vTargetPos = vPosition;
	vTargetPos.y = 0.f;

	_vec3		vSourDir = vTargetPos - m_vStart;
	D3DXVec3Normalize(&vSourDir, &vSourDir);

	if (0 > D3DXVec3Dot(&vSourDir, &m_vNormal))
		return true;

	return _bool(false);
}

CLine * CLine::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _vec3 & vStart, const _vec3 & vEnd)
{
	CLine*	pInstance = new CLine(pGraphic_Device);

	if (FAILED(pInstance->Ready_Line(vStart, vEnd)))
	{
		MessageBox(0, L"CLine Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLine::Free()
{
	Safe_Release(m_pGraphic_Device);
}
