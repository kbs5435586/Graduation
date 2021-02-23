#pragma once

class CToolView;
class CTerrain
{
public:
	CTerrain();
	~CTerrain();

public:
	void SetViewWnd(CToolView* pView) { m_pViewWnd = pView; }

public:
	HRESULT Initialize();
	void Update();
	void Render();
	void MiniRender(float fRatio);
	void Release();

public:
	void TileChange(const D3DXVECTOR3& vPos, BYTE dwDrawID, BYTE dwOption = 0);
	void SaveData(const TCHAR* pFilePath);
	void LoadData(const TCHAR* pFilePath);
	
private:
	int GetTileIndex(const D3DXVECTOR3& vPos);
	bool IsPicking(const D3DXVECTOR3& vPos, int iIndex);
	void SetRatio(D3DXMATRIX* pOut, float fRatioX, float fRatioY);


private:
	vector<TILE*>	m_vecTile;
	CToolView*		m_pViewWnd;
};

