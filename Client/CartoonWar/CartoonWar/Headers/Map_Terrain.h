#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CBuffer_Terrain_Height;
class CTexture;
class CShader;
class CNavigation;
class CFrustum;


class CMap_Terrain :
    public CGameObject
{
private:
	CMap_Terrain();
	CMap_Terrain(const CMap_Terrain& rhs);
	virtual ~CMap_Terrain() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
private:
	virtual HRESULT							CreateInputLayout();
public:
	static CMap_Terrain* Create();
	virtual CGameObject* Clone_GameObject(void* pArg = nullptr, _uint iIdx = 0) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	CTransform* m_pTransformCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CBuffer_Terrain_Height* m_pBufferCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pBrushTextureCom = nullptr;
	CNavigation* m_pNaviCom = nullptr;
	CFrustum* m_pFrustumCom = nullptr;
private:
	CTexture* m_pTextureCom_Grass_Mix = nullptr;
	CTexture* m_pTextureCom_Ground = nullptr;
	CTexture* m_pTextureCom_Fillter = nullptr;

};

