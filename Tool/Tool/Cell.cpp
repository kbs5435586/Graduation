#include "stdafx.h"
#include "Cell.h"
#include "Line.h"
#include "Management.h"

CCell::CCell(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device)
	, m_pLineSDK(nullptr)

{
	ZeroMemory(m_pNeighbor, sizeof(CCell*) * NEIGHBOR_END);
	ZeroMemory(m_pLine, sizeof(CLine*) * LINE_END);

	m_pGraphic_Device->AddRef();
}

HRESULT CCell::Ready_Cell(const _vec3 * pPointA, const _vec3 * pPointB, const _vec3 * pPointC, const _uint& iIndex)
{
	m_vPoint[POINT_A] = *pPointA;
	m_vPoint[POINT_B] = *pPointB;
	m_vPoint[POINT_C] = *pPointC;

	m_iIndex = iIndex;

	m_pLine[LINE_AB] = CLine::Create(m_pGraphic_Device, m_vPoint[POINT_A], m_vPoint[POINT_B]);
	m_pLine[LINE_BC] = CLine::Create(m_pGraphic_Device, m_vPoint[POINT_B], m_vPoint[POINT_C]);
	m_pLine[LINE_CA] = CLine::Create(m_pGraphic_Device, m_vPoint[POINT_C], m_vPoint[POINT_A]);

	if (FAILED(D3DXCreateLine(m_pGraphic_Device, &m_pLineSDK)))
		return E_FAIL;

	return NOERROR;
}



_bool CCell::is_InCell(_vec3 vMovedPos, LINE* pOutLine)
{
	for (size_t i = 0; i < LINE_END; i++)
	{
		if (false == m_pLine[i]->is_Right(vMovedPos))
		{
			*pOutLine = LINE(i);
			return false;
		}
	}

	return _bool(true);
}

_bool CCell::Compare_Point(const _vec3 * pSourPoint, const _vec3 * pDestPoint)
{
	if (m_vPoint[POINT_A] == *pSourPoint)
	{
		if (m_vPoint[POINT_B] == *pDestPoint)
			return true;

		if (m_vPoint[POINT_C] == *pDestPoint)
			return true;
	}

	if (m_vPoint[POINT_B] == *pSourPoint)
	{
		if (m_vPoint[POINT_A] == *pDestPoint)
			return true;

		if (m_vPoint[POINT_C] == *pDestPoint)
			return true;
	}

	if (m_vPoint[POINT_C] == *pSourPoint)
	{
		if (m_vPoint[POINT_A] == *pDestPoint)
			return true;

		if (m_vPoint[POINT_B] == *pDestPoint)
			return true;
	}

	return _bool(false);
}

void CCell::Clear_Neighbor()
{
	for (auto& pNeighbor : m_pNeighbor)
		Safe_Release(pNeighbor);
}

void CCell::Render_Cell()
{
	if (nullptr == m_pLineSDK)
		return;

	_vec3			vPoint[4];
	memcpy(&vPoint, m_vPoint, sizeof(_vec3) * 3);
	vPoint[3] = m_vPoint[0];

	_matrix			matView, matProj;
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &matProj);

	for (size_t i = 0; i < 4; ++i)
	{
		D3DXVec3TransformCoord(&vPoint[i], &vPoint[i], &matView);
		if (0.0f > vPoint[i].z)
			vPoint[i].z = 0.f;

		D3DXVec3TransformCoord(&vPoint[i], &vPoint[i], &matProj);
	}


	_matrix			matTmp;

	m_pLineSDK->SetWidth(2.f);

	m_pLineSDK->Begin();

	m_pLineSDK->DrawTransform(vPoint, 4, D3DXMatrixIdentity(&matTmp), D3DXCOLOR(0.f, 1.f, 0.f, 1.f));

	m_pLineSDK->End();

}

CCell * CCell::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _vec3 * pPointA, const _vec3 * pPointB, const _vec3 * pPointC, const _uint& iIndex)
{
	CCell*	pInstance = new CCell(pGraphic_Device);

	if (FAILED(pInstance->Ready_Cell(pPointA, pPointB, pPointC, iIndex)))
	{
		MessageBox(0, L"CCell Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCell::Free()
{
	for (auto& pLine : m_pLine)
		Safe_Release(pLine);

	Safe_Release(m_pLineSDK);
	Safe_Release(m_pGraphic_Device);
}
