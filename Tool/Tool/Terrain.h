#pragma once
#include "GameObject.h"
class CRenderer;
class CBuffer_Terrain;
class CTexture;
class CTransform;
class CShader;
class CPicking;
class CNavi;
class CTerrain :
	public CGameObject
{
private:
	CTerrain(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTerrain(const CTerrain& rhs);
	virtual ~CTerrain()=default;
public:
	virtual HRESULT			Ready_Prototype();
	virtual HRESULT			Ready_GameObject(void* pArg);
	virtual _int			Update_GameObject(const _float& fTimeDelta);
	virtual _int			LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void			Render_GameObject();
public:
	static CTerrain*		Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
private:
	HRESULT					Ready_Component();
	HRESULT					SetUp_ConstantTable(LPD3DXEFFECT pEffect);
private:
	void					Change_Component_Terrain(CSecondWnd* pSecond);
public:
	_vec3					GetPickingPos() { return m_vecPickingPos; }
private:
	_vec3					m_vecPickingPos = {1.f,1.f,1.f};
private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CBuffer_Terrain*		m_pBufferCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CPicking*				m_pPickingCom = nullptr;
	CNavi*					m_pNaviCom = nullptr;
};

