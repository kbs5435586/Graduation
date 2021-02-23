#pragma once
class CObjMgr
{
	DECLARE_SINGLETON(CObjMgr)

public:
	enum OBJID { TERRAIN, PLAYER, MONSTER, EFFECT, UI, END };

private:
	CObjMgr();
	~CObjMgr();

public:
	CObj* GetTerrain() { return m_ObjLst[TERRAIN].front(); }

public:
	void AddObject(CObj* pObj, OBJID eID);
	void Update();
	void LateUpdate();
	void Render();
	void Release();

private:
	OBJLIST m_ObjLst[END];
};

