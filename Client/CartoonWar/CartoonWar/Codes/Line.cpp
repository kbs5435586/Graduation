#include "framework.h"
#include "Line.h"

CLine::CLine()
{
}

HRESULT CLine::Ready_Line(const _vec3& vStart, const _vec3 vEnd)
{
	m_vStart = vStart;
	m_vEnd = m_vEnd;

	_vec3 vDir = vEnd - vStart;
	m_vNormal = _vec3(vDir.z * -1.f, 0.f, vDir.x);

	m_vNormal = Vector3_::Normalize(m_vNormal);

	return S_OK;
}

_bool CLine::IsRight(const _vec3& vPos)
{
	_vec3 vTargetPos = vPos;
	vTargetPos.y = 0.f;

	_vec3 vSourDir = vTargetPos - m_vStart;
	vSourDir = Vector3_::Normalize(vSourDir);

	_float fDot = Vector3_::DotProduct(vSourDir, m_vNormal);
	if (0 > fDot)
		return true;

	return _bool(false);
}

CLine* CLine::Create(const _vec3& vStart, const _vec3 vEnd)
{
	CLine* pInstance = new CLine();
	if (FAILED(pInstance->Ready_Line(vStart, vEnd)))
		return nullptr;

	return pInstance;
}

void CLine::Free()
{
}
