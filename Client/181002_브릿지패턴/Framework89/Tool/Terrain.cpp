#include "stdafx.h"
#include "Terrain.h"
#include "ToolView.h"


CTerrain::CTerrain()
{
}


CTerrain::~CTerrain()
{
	Release();
}

HRESULT CTerrain::Initialize()
{
	m_vecTile.reserve(TILEX * TILEY);

	float fX = 0.f, fY = 0.f;
	TILE* pTile = nullptr;

	for (int i = 0; i < TILEY; ++i)
	{
		for (int j = 0; j < TILEX; ++j)
		{
			pTile = new TILE;

			fX = (TILECX * j) + (i % 2) * (TILECX / 2.f);
			fY = (TILECY * i) / 2.f;

			pTile->vPos = { fX, fY, 0.f };
			pTile->vSize = { (float)TILECX, (float)TILECY, 0.f };
			pTile->byDrawID = 2;
			pTile->byOption = 0;
			pTile->iIndex = j + (TILEX * i);

			m_vecTile.push_back(pTile);
		}
	}

	return S_OK;
}

void CTerrain::Update()
{
}

void CTerrain::Render()
{
	D3DXMATRIX matScale, matTrans, matWorld;

	// ���� ����
	RECT rc = {};
	m_pViewWnd->GetClientRect(&rc); // �������� RECT ������ ����.

	float fCurWinCX = float(rc.right - rc.left);
	float fCurWinCY = float(rc.bottom - rc.top);

	float fRatioX = WINCX / fCurWinCX;
	float fRatioY = WINCY / fCurWinCY;

	TCHAR szBuf[MIN_STR] = L"";	

	for (size_t iIndex = 0; iIndex < m_vecTile.size(); ++iIndex)
	{
		D3DXMatrixScaling(&matScale, 1.f, 1.f, 0.f);
		D3DXMatrixTranslation(&matTrans, 
			m_vecTile[iIndex]->vPos.x - m_pViewWnd->GetScrollPos(0),
			m_vecTile[iIndex]->vPos.y - m_pViewWnd->GetScrollPos(1),
			0.f);

		matWorld = matScale * matTrans;		

		SetRatio(&matWorld, fRatioX, fRatioY);
		CDevice::GetInstance()->GetSprite()->SetTransform(&matWorld);

		const TEXINFO* pTexInfo = CTextureMgr::GetInstance()->GetTexInfo(
			L"Terrain", L"Tile", m_vecTile[iIndex]->byDrawID);
		NULL_CHECK_VOID(pTexInfo);

		float fCenterX = pTexInfo->tImgInfo.Width * 0.5f;
		float fCenterY = pTexInfo->tImgInfo.Height * 0.5f;

		CDevice::GetInstance()->GetSprite()->Draw(pTexInfo->pTexture,
			nullptr, &D3DXVECTOR3(fCenterX, fCenterY, 0.f), nullptr, D3DCOLOR_ARGB(255, 255, 255, 255));

		swprintf_s(szBuf, L"%d", iIndex);

		// Direct ��Ʈ�� ��½� ��������Ʈ �̹����� �����Ͽ� ����Ѵ�. ���� ����.
		CDevice::GetInstance()->GetFont()->DrawTextW(CDevice::GetInstance()->GetSprite(),
			szBuf, lstrlen(szBuf), nullptr, 0, D3DCOLOR_ARGB(255, 255, 255, 255));
	}
}

void CTerrain::MiniRender(float fRatio)
{
	D3DXMATRIX matScale, matTrans, matWorld;		

	for (size_t iIndex = 0; iIndex < m_vecTile.size(); ++iIndex)
	{
		D3DXMatrixScaling(&matScale, 1.f, 1.f, 0.f);
		D3DXMatrixTranslation(&matTrans,
			m_vecTile[iIndex]->vPos.x - m_pViewWnd->GetScrollPos(0),
			m_vecTile[iIndex]->vPos.y - m_pViewWnd->GetScrollPos(1),
			0.f);

		matWorld = matScale * matTrans;

		SetRatio(&matWorld, fRatio, fRatio);
		CDevice::GetInstance()->GetSprite()->SetTransform(&matWorld);

		const TEXINFO* pTexInfo = CTextureMgr::GetInstance()->GetTexInfo(
			L"Terrain", L"Tile", m_vecTile[iIndex]->byDrawID);
		NULL_CHECK_VOID(pTexInfo);

		float fCenterX = pTexInfo->tImgInfo.Width * 0.5f;
		float fCenterY = pTexInfo->tImgInfo.Height * 0.5f;

		CDevice::GetInstance()->GetSprite()->Draw(pTexInfo->pTexture,
			nullptr, &D3DXVECTOR3(fCenterX, fCenterY, 0.f), nullptr, D3DCOLOR_ARGB(255, 255, 255, 255));		
	}
}

void CTerrain::Release()
{
	for_each(m_vecTile.begin(), m_vecTile.end(), SafeDelete<TILE*>);
	m_vecTile.clear();
	m_vecTile.shrink_to_fit();
}

void CTerrain::TileChange(const D3DXVECTOR3& vPos, BYTE dwDrawID, BYTE dwOption)
{
	int iIndex = GetTileIndex(vPos);

	if (0 > iIndex)
		return;

	m_vecTile[iIndex]->byDrawID = dwDrawID;
	m_vecTile[iIndex]->byOption = dwOption;
}

void CTerrain::SaveData(const TCHAR * pFilePath)
{
	HANDLE hFile = CreateFile(pFilePath, GENERIC_WRITE, 0, 0,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		AfxMessageBox(L"Tile Save Failed!!");
		return;
	}

	DWORD dwByte = 0;

	for (auto& pTile : m_vecTile)
		WriteFile(hFile, pTile, sizeof(TILE), &dwByte, nullptr);

	CloseHandle(hFile);
}

void CTerrain::LoadData(const TCHAR * pFilePath)
{
	HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, 0,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		AfxMessageBox(L"Tile Load Failed!!");
		return;
	}

	Release();

	DWORD dwByte = 0;
	TILE* pTile = nullptr;
	TILE t = {};

	while(true)
	{
		ReadFile(hFile, &t, sizeof(TILE), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		pTile = new TILE(t);
		m_vecTile.push_back(pTile);
	}

	m_vecTile.shrink_to_fit();
	CloseHandle(hFile);
}

int CTerrain::GetTileIndex(const D3DXVECTOR3 & vPos)
{
	for (size_t iIndex = 0; iIndex < m_vecTile.size(); ++iIndex)
	{
		if (IsPicking(vPos, iIndex))
			return iIndex;
	}

	// ��ã��.
	return -1;
}

bool CTerrain::IsPicking(const D3DXVECTOR3& vPos, int iIndex)
{
	// ##2. ������ �̿��� ��ŷ

	// ��ī��Ʈ ����
	// �������� 4���� �������� �ð�������� ���Ѵ�.
	D3DXVECTOR3 vPoint[4] =
	{
		/*12��*/
		{ m_vecTile[iIndex]->vPos.x, m_vecTile[iIndex]->vPos.y + TILECY / 2, 0.f },
		/*3��*/
		{ m_vecTile[iIndex]->vPos.x + TILECX / 2, m_vecTile[iIndex]->vPos.y, 0.f },
		/*6��*/
		{ m_vecTile[iIndex]->vPos.x, m_vecTile[iIndex]->vPos.y - TILECY / 2, 0.f },
		/*9��*/
		{ m_vecTile[iIndex]->vPos.x - TILECX / 2, m_vecTile[iIndex]->vPos.y, 0.f },
	};

	// 4���� �������� ���� ���׺��͸� �ð�������� ���Ѵ�.
	D3DXVECTOR3 vDir[4] =
	{
		vPoint[1] - vPoint[0],
		vPoint[2] - vPoint[1],
		vPoint[3] - vPoint[2],
		vPoint[0] - vPoint[3]
	};

	// �� �������͸� ���Ѵ�
	D3DXVECTOR3 vNormal[4] =
	{
		{ -vDir[0].y, vDir[0].x, 0.f },
		{ -vDir[1].y, vDir[1].x, 0.f },
		{ -vDir[2].y, vDir[2].x, 0.f },
		{ -vDir[3].y, vDir[3].x, 0.f }
	};

	// ���콺 ��ǥ���������� �� ������ ���⺤�͸� ���Ѵ�.
	D3DXVECTOR3 vMouseDir[4] = 
	{
		vPos - vPoint[0],
		vPos - vPoint[1],
		vPos - vPoint[2],
		vPos - vPoint[3]
	};

	// vMouseDir�� vNormal�� �����Ͽ� ��� ����(�а�)�� ������ true!
	for (int i = 0; i < 4; ++i)
	{
		if (0.f <= D3DXVec3Dot(&vMouseDir[i], &vNormal[i]))
			return false;
	}

	return true;










	//// ##1. ������ �������� �̿��� ��ŷ

	//// ��ī��Ʈ ����
	//// �������� 4���� ���⸦ �ð�������� ���Ѵ�.
	//float fGradient[4] = 
	//{
	//	-(TILECY * 0.5f) / (TILECX * 0.5f), 
	//	(TILECY * 0.5f) / (TILECX * 0.5f),
	//	-(TILECY * 0.5f) / (TILECX * 0.5f),
	//	(TILECY * 0.5f) / (TILECX * 0.5f)
	//};

	//// �������� 4���� �������� �ð�������� ���Ѵ�.
	//D3DXVECTOR3 vPoint[4] =
	//{
	//	/*12��*/
	//	{ m_vecTile[iIndex]->vPos.x, m_vecTile[iIndex]->vPos.y + TILECY / 2, 0.f },
	//	/*3��*/
	//	{ m_vecTile[iIndex]->vPos.x + TILECX / 2, m_vecTile[iIndex]->vPos.y, 0.f },
	//	/*6��*/
	//	{ m_vecTile[iIndex]->vPos.x, m_vecTile[iIndex]->vPos.y - TILECY / 2, 0.f },
	//	/*9��*/
	//	{ m_vecTile[iIndex]->vPos.x - TILECX / 2, m_vecTile[iIndex]->vPos.y, 0.f },
	//};

	//// �ð�������� b(y����)�� ���Ѵ�.
	//// y = ax + b ==> b = y - ax
	//float b[4] = 
	//{
	//	vPoint[0].y - fGradient[0] * vPoint[0].x,
	//	vPoint[1].y - fGradient[1] * vPoint[1].x,
	//	vPoint[2].y - fGradient[2] * vPoint[2].x,
	//	vPoint[3].y - fGradient[3] * vPoint[3].x
	//};

	//// 0 = ax + b - y: ���� ��(x, y)�� ���� ������ ���� ����.
	//// ���� = ax + b - y: ���� ��(x, y)�� ���� �������� ���� �ִ�.
	//// ��� = ax + b - y: ���� ��(x, y)�� ���� �������� �Ʒ��� �ִ�.

	//// �ð� ����
	//return (0 <= fGradient[0] * vPos.x + b[0] - vPos.y) && (0 >= fGradient[1] * vPos.x + b[1] - vPos.y)
	//	&& (0 >= fGradient[2] * vPos.x + b[2] - vPos.y) && (0 <= fGradient[3] * vPos.x + b[3] - vPos.y);
}

void CTerrain::SetRatio(D3DXMATRIX* pOut, float fRatioX, float fRatioY)
{
	pOut->_11 *= fRatioX;
	pOut->_21 *= fRatioX;
	pOut->_31 *= fRatioX;
	pOut->_41 *= fRatioX;

	pOut->_12 *= fRatioY;
	pOut->_22 *= fRatioY;
	pOut->_32 *= fRatioY;
	pOut->_42 *= fRatioY;
}
