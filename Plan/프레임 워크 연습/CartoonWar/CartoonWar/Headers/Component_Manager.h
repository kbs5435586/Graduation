#pragma once
#include "Base.h"

#include "Renderer.h"
#include "Transform.h"
#include "Shader.h"
#include "Texture.h"
#include "Frustum.h"
#include "Navigation.h"
#include "Collider.h"
#include "Static_Mesh.h"
#include "Observer.h"

#include "Buffer_RectCol.h"
#include "Buffer_RectTex.h"
#include "Buffer_CubeCol.h"
#include "Buffer_CubeTex.h"
#include "Buffer_Terrain.h"
#include "Buffer_Terrain_Height.h"
#include "Buffer_Sphere.h"

class CComponent;
class CComponent_Manager :
	public CBase
{
	_DECLARE_SINGLETON(CComponent_Manager)
private:
	CComponent_Manager();
	virtual ~CComponent_Manager() = default;
public:
	HRESULT									Reserve_Component_Manager(const _uint& iNumScene);
	HRESULT									Add_Prototype_Component(const _uint& iSceneID, const _tchar* pComponentTag, CComponent* pComponent);
	CComponent*								Clone_Component(const _uint& iSceneID, const _tchar* pComponentTag, void* pArg);
private:
	CComponent*								Find_Component(const _uint& iSceneID, const _tchar* pComponentTag);
private:
	_uint									m_iNumScene = 0;
	map<const _tchar*, CComponent*>*		m_pMapPrototype = nullptr;
	typedef map<const _tchar*, CComponent*>	MAPPROTOTYPE;
private:
	virtual void Free();
};

