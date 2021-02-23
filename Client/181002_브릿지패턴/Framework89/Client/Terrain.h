#pragma once

#include "Obj.h"

class CTerrain : public CObj
{
public:
	CTerrain();
	virtual ~CTerrain();

public:
	vector<TILE*>& GetTile() { return m_vecTile; }
	vector<list<TILE*>>& GetGraph() { return m_vecGraph; }

public:
	virtual HRESULT Initialize();
	virtual void LateInit();
	virtual int Update();
	virtual void LateUpdate();
	virtual void Render();
	virtual void Release();

private:
	HRESULT LoadData();
	void ReadyTileGraph();

private:
	vector<TILE*>			m_vecTile;
	vector<list<TILE*>>		m_vecGraph;	
};
