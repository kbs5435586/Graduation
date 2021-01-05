#include "stdafx.h"
#include "Navi.h"
#include "Management.h"
#include "Cell.h"

CNavi::CNavi(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{
}

CNavi::CNavi(const CNavi & rhs)
	: CComponent(rhs)
	, m_vecCell(rhs.m_vecCell)
{
	for (auto& pCell : m_vecCell)
		pCell->AddRef();
}

HRESULT CNavi::Ready_Navigation()
{
// 	HANDLE			hFile = 0;
// 	_ulong			dwByte = 0;
// 
// 	hFile = CreateFile(pFileName, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
// 	if (0 == hFile)
// 		return E_FAIL;
// 
// 	_uint			iNumCells = 0;
// 	ReadFile(hFile, &iNumCells, sizeof(_uint), &dwByte, nullptr);
// 
// 	m_vecCell.reserve(iNumCells);
// 
// 	while (true)
// 	{
// 		_vec3		vPoints[3];
// 
// 		ReadFile(hFile, vPoints, sizeof(_vec3) * 3, &dwByte, nullptr);
// 		if (0 == dwByte)
// 			break;
// 
// 		CCell*		pCell = CCell::Create(m_pGraphic_Device, &vPoints[0], &vPoints[1], &vPoints[2], m_vecCell.size());
// 		if (nullptr == pCell)
// 			return E_FAIL;
// 
// 		m_vecCell.push_back(pCell);
// 	}
// 
// 	CloseHandle(hFile);
// 
//  	if (FAILED(Ready_Neighbor()))
//  		return E_FAIL;
	return S_OK;
}

HRESULT CNavi::Ready_Neighbor()
{
	_uint	iNumCells = m_vecCell.size();

	for (size_t i = 0; i < iNumCells; ++i)
	{
		for (size_t j = 0; j < iNumCells; j++)
		{
			if (i == j)
				continue;

			if (true == m_vecCell[j]->Compare_Point(m_vecCell[i]->Get_Point(POINT_A), m_vecCell[i]->Get_Point(POINT_B)))
				m_vecCell[i]->Set_Neighbor(NEIGHBOR_AB, m_vecCell[j]);

			if (true == m_vecCell[j]->Compare_Point(m_vecCell[i]->Get_Point(POINT_B), m_vecCell[i]->Get_Point(POINT_C)))
				m_vecCell[i]->Set_Neighbor(NEIGHBOR_BC, m_vecCell[j]);

			if (true == m_vecCell[j]->Compare_Point(m_vecCell[i]->Get_Point(POINT_C), m_vecCell[i]->Get_Point(POINT_A)))
				m_vecCell[i]->Set_Neighbor(NEIGHBOR_CA, m_vecCell[j]);
		}
	}

	return S_OK;
}

HRESULT CNavi::Ready_Clone_Navigation(void * pArg)
{
	if (nullptr == pArg)
		return NOERROR;

	m_iCurrentIdx = *(_uint*)pArg;

	if (m_vecCell.size() <= m_iCurrentIdx)
		return E_FAIL;

	return S_OK;
}

_bool CNavi::Move_OnNavigation(const _vec3 * vPosition, const _vec3 * vDirectionPerSec)
{
	if (m_vecCell.size() <= m_iCurrentIdx)
		return false;

	LINE			eOutLine =LINE(-1);
	const CCell*	pNeighbor = nullptr;

	_bool	isIn = m_vecCell[m_iCurrentIdx]->is_InCell(*vPosition + *vDirectionPerSec, &eOutLine);

	if (false == isIn)
	{
		if (pNeighbor = m_vecCell[m_iCurrentIdx]->Get_Neighbor(NEIGHBOR(eOutLine)))
		{
			m_iCurrentIdx = pNeighbor->Get_CellIndex();
			return true;
		}
		else
			return false;
	}
	else
		return true;
}

void CNavi::Render_Navigation()
{
	for (auto& pCell : m_vecCell)
		pCell->Render_Cell();
}

HRESULT CNavi::Delete_Cell()
{
	auto& iter = m_vecCell.begin();
	for (; iter != m_vecCell.end();)
	{
		if (iter[0]->GetIsLast() == true)
		{
			iter = m_vecCell.erase(iter);
		}
		else
			iter++;
	}

	if(m_vecCell.size()>=1)
		m_vecCell.back()->GetIsLast() = true;


	for (_int i = 0; i < 3; ++i)
		m_vTotlaPos.pop_back();

	return S_OK;
}

HRESULT CNavi::Set_PositionVector(_vec3 vPos)
{
	m_vPos.push_back(vPos);
	m_vTotlaPos.push_back(vPos);
	return S_OK;
}

HRESULT CNavi::Set_CellVector()
{
	for (auto& iter : m_vecCell)
		iter->GetIsLast() = false;

	CCell*		pCell = CCell::Create(m_pGraphic_Device, &m_vPos[0], &m_vPos[1], &m_vPos[2], m_vecCell.size());
	if (nullptr == pCell)
		return E_FAIL;
	
	pCell->GetIsLast() = true;
	m_vecCell.push_back(pCell);
	return S_OK;
}

HRESULT CNavi::Set_CellVector(vector<_vec3> vTotalPos)
{
	for (auto& iter : m_vecCell)
		iter->GetIsLast() = false;

	for (size_t i = 0; i < vTotalPos.size();)
	{
		CCell*		pCell = CCell::Create(m_pGraphic_Device, &vTotalPos[i], &vTotalPos[i + 1], &vTotalPos[i + 2], m_vecCell.size());
		if (nullptr == pCell)
			return E_FAIL;

		pCell->GetIsLast() = true;
		m_vecCell.push_back(pCell);
		i += 3;
	}
	return S_OK;
}

void CNavi::Clear_Position_Vector()
{
	m_vPos.clear();
	m_vPos.shrink_to_fit();
}


CNavi * CNavi::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CNavi* pInstance = new CNavi(pGraphic_Device);
	if (FAILED(pInstance->Ready_Navigation()))
		return nullptr;
	return pInstance;
}

CComponent * CNavi::Clone_Component(void* pArg)
{
	CNavi*	pInstance = new CNavi(*this);

	if (FAILED(pInstance->Ready_Clone_Navigation(pArg)))
	{
		MessageBox(0, L"CNavigation Cloned Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNavi::Free()
{
	if (false == m_isClone)
	{
		for (auto& pCell : m_vecCell)
			pCell->Clear_Neighbor();
	}
	for (auto& pCell : m_vecCell)
		Safe_Release(pCell);

	m_vecCell.clear();
	CComponent::Free();
}
