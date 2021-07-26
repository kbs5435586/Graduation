#include "stdafx.h"
#include "Buffer_Terrain_Height.h"

CBuffer_Terrain_Height::CBuffer_Terrain_Height(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CVIBuffer(pGraphic_Device)
{
}

CBuffer_Terrain_Height::CBuffer_Terrain_Height(const CBuffer_Terrain_Height & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CBuffer_Terrain_Height::Ready_VIBuffer(const _tchar * pHeightMap, const _float & fInterval)
{

	return S_OK;
}

void CBuffer_Terrain_Height::Render_VIBuffer()
{
}

CBuffer_Terrain_Height * CBuffer_Terrain_Height::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar * pHeightMap, const _float & fInterval)
{
	CBuffer_Terrain_Height*	pInstance = new CBuffer_Terrain_Height(pGraphic_Device);

	if (FAILED(pInstance->Ready_VIBuffer(pHeightMap, fInterval)))
	{
		MessageBox(0, L"CBuffer_Terrain Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CBuffer_Terrain_Height::Clone_Component(void * pArg)
{
	return new CBuffer_Terrain_Height(*this);
}

void CBuffer_Terrain_Height::Free()
{
}
