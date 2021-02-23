#include "stdafx.h"
#include "Terrain.h"
#include "Mouse.h"

CTerrain::CTerrain()
{

}

CTerrain::~CTerrain()
{
	Release();
}

HRESULT CTerrain::Initialize()
{
	if (FAILED(LoadData()))
		return E_FAIL;

	return S_OK;
}

void CTerrain::LateInit()
{
	ReadyTileGraph();
}

int CTerrain::Update()
{
	CObj::LateInit();

	float fDeltaTime = GET_INSTANCE(CTimeMgr)->GetDeltaTime();

	if (0.f > CMouse::GetMousePos().x)
		CScrollMgr::SetScroll({ -500.f * fDeltaTime, 0.f, 0.f });
	if (0.f > CMouse::GetMousePos().y)
		CScrollMgr::SetScroll({ 0.f, -500.f * fDeltaTime, 0.f });
	if (WINCX < CMouse::GetMousePos().x)
		CScrollMgr::SetScroll({ 500.f * fDeltaTime, 0.f, 0.f });
	if (WINCY < CMouse::GetMousePos().y)
		CScrollMgr::SetScroll({ 0.f, 500.f * fDeltaTime, 0.f });

	return NO_EVENT;
}

void CTerrain::LateUpdate()
{

}

void CTerrain::Render()
{
	D3DXMATRIX matScale, matTrans;
	D3DXVECTOR3 vScroll = CScrollMgr::GetScroll();

	int iCullX = (int)vScroll.x / TILECX;
	int iCullY = (int)vScroll.y / (TILECY / 2);
	
	int iCullEndX = iCullX + WINCX / TILECX;
	int iCullEndY = iCullY + WINCY / (TILECY / 2);

	for (int i = iCullY; i < iCullEndY + 2; ++i)
	{
		for (int j = iCullX; j < iCullEndX + 2; ++j)
		{
			int iIndex = j + (TILEX * i);

			if (0 > iIndex || m_vecTile.size() <= (size_t)iIndex)
				continue;

			D3DXMatrixScaling(&matScale, 1.f, 1.f, 1.f);
			D3DXMatrixTranslation(&matTrans, m_vecTile[iIndex]->vPos.x - vScroll.x,
				m_vecTile[iIndex]->vPos.y - vScroll.y,
				0.f);

			m_tInfo.matWorld = matScale * matTrans;

			const TEXINFO* pTexInfo = GET_INSTANCE(CTextureMgr)->GetTexInfo(L"TERRAIN",
				L"Tile", m_vecTile[iIndex]->byDrawID);
			NULL_CHECK_VOID(pTexInfo);

			float fCenterX = pTexInfo->tImgInfo.Width * 0.5f;
			float fCenterY = pTexInfo->tImgInfo.Height * 0.5f;

			GET_INSTANCE(CDevice)->GetSprite()->SetTransform(&m_tInfo.matWorld);
			GET_INSTANCE(CDevice)->GetSprite()->Draw(pTexInfo->pTexture, nullptr,
				&D3DXVECTOR3(fCenterX, fCenterY, 0.f), nullptr, D3DCOLOR_ARGB(255, 255, 255, 255));
		}
	}
}

void CTerrain::Release()
{
	for_each(m_vecTile.begin(), m_vecTile.end(), SafeDelete<TILE*>);
	m_vecTile.clear();
	m_vecTile.shrink_to_fit();
}

HRESULT CTerrain::LoadData()
{
	HANDLE hFile = CreateFile(L"../Data/Tile.dat", GENERIC_READ, 0, 0,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		ERR_MSG(L"Tile Load Failed!!");
		return E_FAIL;
	}

	Release();

	DWORD dwByte = 0;
	TILE* pTile = nullptr;
	TILE t = {};

	while (true)
	{
		ReadFile(hFile, &t, sizeof(TILE), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		pTile = new TILE(t);
		m_vecTile.push_back(pTile);
	}

	m_vecTile.shrink_to_fit();
	CloseHandle(hFile);

	return S_OK;
}

void CTerrain::ReadyTileGraph()
{
	// 각 타일의 4방향 인접 정보를 저장.
	m_vecGraph.resize(m_vecTile.size());

	for (int i = 0; i < TILEY; ++i)
	{
		for (int j = 0; j < TILEX; ++j)
		{
			int iIndex = j + (TILEX * i);

			// 좌상
			if ((i != 0) && (0 != iIndex % (TILEX * 2)))
			{
				if ((0 != i % 2) && (0 == m_vecTile[iIndex - TILEX]->byOption))
					m_vecGraph[iIndex].push_back(m_vecTile[iIndex - TILEX]);
				else if((0 == i % 2) && (0 == m_vecTile[iIndex - (TILEX + 1)]->byOption))
					m_vecGraph[iIndex].push_back(m_vecTile[iIndex - (TILEX + 1)]);
			}

			// 우상
			if ((i != 0) && ((TILEX * 2 - 1) != iIndex % (TILEX * 2)))
			{
				if ((0 != i % 2) && (0 == m_vecTile[iIndex - (TILEX - 1)]->byOption))
					m_vecGraph[iIndex].push_back(m_vecTile[iIndex - (TILEX - 1)]);
				else if ((0 == i % 2) && (0 == m_vecTile[iIndex - TILEX]->byOption))
					m_vecGraph[iIndex].push_back(m_vecTile[iIndex - TILEX]);
			}

			// 좌하
			if ((0 != iIndex % (TILEX * 2)) && (i != TILEY - 1))
			{
				if ((0 != i % 2) && (0 == m_vecTile[iIndex + TILEX]->byOption))
					m_vecGraph[iIndex].push_back(m_vecTile[iIndex + TILEX]);
				else if ((0 == i % 2) && (0 == m_vecTile[iIndex + (TILEX - 1)]->byOption))
					m_vecGraph[iIndex].push_back(m_vecTile[iIndex + (TILEX - 1)]);
			}

			// 우하
			if (((TILEX * 2 - 1) != iIndex % (TILEX * 2)) && (i != TILEY - 1))
			{
				if ((0 != i % 2) && (0 == m_vecTile[iIndex + (TILEX + 1)]->byOption))
					m_vecGraph[iIndex].push_back(m_vecTile[iIndex + (TILEX + 1)]);
				else if ((0 == i % 2) && (0 == m_vecTile[iIndex + TILEX]->byOption))
					m_vecGraph[iIndex].push_back(m_vecTile[iIndex + TILEX]);
			}
		}
	}
}
