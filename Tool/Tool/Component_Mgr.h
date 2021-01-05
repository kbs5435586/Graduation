#pragma once
#include "Base.h"
#include "Transform.h"
#include "Renderer.h"
#include "Texture.h"
#include "Buffer_RcTex.h"
#include "Buffer_Terrain.h"
#include "Buffer_CubeTex.h"
#include "Picking.h"
#include "Shader.h"
#include "Static_Mesh.h"
#include "MyFont.h"
#include "Navi.h"
#include "Dynamic_Mesh.h"
class CComponent_Mgr :
	public CBase
{
	_DECLARE_SINGLETON(CComponent_Mgr)
public:
	CComponent_Mgr();
	virtual ~CComponent_Mgr()=default;
public:
	HRESULT			Reserve_Component_Manager(const _uint& iNumScene);
	HRESULT			Add_Prototype_Component(const _uint& iSceneID, const _tchar* pComponentTag, CComponent* pComponent);
	HRESULT			Delete_Prototype_Component(const _uint& iSceneID, const _tchar* pComponentTag, CComponent* pComponent);
	CComponent*		Clone_Component(const _uint& iSceneID, const _tchar* pComponentTag, void* pArg=nullptr);
private:
	map<const _tchar*, CComponent*>*		m_pMapPrototype = nullptr;
	typedef map<const _tchar*, CComponent*>	MAPPROTOTYPE;
private:
	_uint		m_iNumScene = 0;
private:
	CComponent* Find_Component(const _uint& iSceneID, const _tchar* pComponentTag);
protected:
	virtual void Free();
};

