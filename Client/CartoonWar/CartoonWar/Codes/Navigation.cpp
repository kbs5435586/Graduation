#include "framework.h"
#include "Navigation.h"
#include "Cell.h"

CNavigation::CNavigation()
{
}

CNavigation::CNavigation(const CNavigation& rhs)
	: CComponent(rhs)
	, m_vecCell(rhs.m_vecCell)
{
	for (auto& pCell : m_vecCell)
		pCell->AddRef();
}

HRESULT CNavigation::Ready_Navigation(const _tchar* pFilePath)
{
	// Read Data File
	HANDLE			hFile = 0;
 	_ulong			dwByte = 0;
 
 	hFile = CreateFile(pFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
 	if (0 == hFile)
 		return E_FAIL;
 
 	while (true)
 	{
 		_vec3		vPoints[3];
 
 		ReadFile(hFile, vPoints, sizeof(_vec3) * 3, &dwByte, nullptr);
 		if (0 == dwByte)
 			break;
 
 		CCell*		pCell = CCell::Create(&vPoints[0], &vPoints[1], &vPoints[2], m_vecCell.size());
 		if (nullptr == pCell)
 			return E_FAIL;
 
 		m_vecCell.push_back(pCell);
 	}
 
  	CloseHandle(hFile);
 
  	if (FAILED(Ready_Neighbor()))
  		return E_FAIL;
	return S_OK;
}

HRESULT CNavigation::Ready_Neighbor()
{
	_uint iNumCells = m_vecCell.size();

	for (_uint i = 0; i < iNumCells; ++i)
	{
		for (_uint j = 0; j < iNumCells; ++j)
		{
			if (i == j)
				continue;

			if (m_vecCell[j]->Compare_Point(m_vecCell[i]->Get_Point(POINT_::POINT_A), m_vecCell[i]->Get_Point(POINT_::POINT_B)))
				m_vecCell[i]->Set_Neighbor(NEIGHBOR::NEIGHBOR_AB, m_vecCell[j]);

			if (m_vecCell[j]->Compare_Point(m_vecCell[i]->Get_Point(POINT_::POINT_B), m_vecCell[i]->Get_Point(POINT_::POINT_C)))
				m_vecCell[i]->Set_Neighbor(NEIGHBOR::NEIGHBOR_BC, m_vecCell[j]);

			if (m_vecCell[j]->Compare_Point(m_vecCell[i]->Get_Point(POINT_::POINT_C), m_vecCell[i]->Get_Point(POINT_::POINT_A)))
				m_vecCell[i]->Set_Neighbor(NEIGHBOR::NEIGHBOR_CA, m_vecCell[j]);
		}
	}
	return S_OK;
}

HRESULT CNavigation::Ready_Clone_Navigation(void* pArg)
{
	m_IsClone = true;
	if (nullptr == pArg)
		return S_OK;

	m_iCurrentIdx = *(_uint*)pArg;

	if (m_vecCell.size() <= m_iCurrentIdx)
		return E_FAIL;


	return S_OK;
}

void CNavigation::Render_Navigation()
{
	for (auto& iter : m_vecCell)
	{

		iter->Render_Cell();
	}
}

_bool CNavigation::Move_OnNavigation(const _vec3* vPos, const _vec3* vDirectionPerSec)
{
	if (m_vecCell.size() <= m_iCurrentIdx)
		return false;

	LINE		eOutLine = LINE(-1);
	const CCell* pNeighbor = nullptr;


	_bool IsIn = m_vecCell[m_iCurrentIdx]->Is_inCell(*vPos + *vDirectionPerSec, &eOutLine);
	if (!IsIn)
	{
		if (pNeighbor = m_vecCell[m_iCurrentIdx]->Get_Neighbor(NEIGHBOR(eOutLine)))
			return true;
		else
			return false;
	}
	return _bool(true);
}


CNavigation* CNavigation::Create(const _tchar* pFilePath)
{
	CNavigation* pInstance = new CNavigation();
	if (FAILED(pInstance->Ready_Navigation(pFilePath)))
		return nullptr;

	return pInstance;
}

CComponent* CNavigation::Clone_Component(void* pArg)
{
	CNavigation* pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Ready_Clone_Navigation(pArg)))
	{
		MessageBox(0, L"CNavigation Cloned Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	} 
	return pInstance;
}

void CNavigation::Free()
{
	if (!m_IsClone)
	{
		for (auto& pCell : m_vecCell)
		{
			pCell->Clear_Neighbor();
		}
	} 

	for (auto& pCell : m_vecCell)
		Safe_Release(pCell);

	m_vecCell.clear();

	CComponent::Free();
}
