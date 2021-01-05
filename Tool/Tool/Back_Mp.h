#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CTexture;
class CBuffer_RcTex;
class CShader;
class CBack_Mp :
	public CGameObject
{
private:
	CBack_Mp(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBack_Mp(const CBack_Mp& rhs);
	virtual ~CBack_Mp() = default;
public:
	virtual HRESULT			Ready_Prototype();
	virtual HRESULT			Ready_GameObject(void* pArg);
	virtual _int			Update_GameObject(const _float& fTimeDelta);
	virtual _int			LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void			Render_GameObject();
public:
	static	CBack_Mp*		Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
private:
	HRESULT					Ready_Component();
	HRESULT					SetUp_ConstantTable(LPD3DXEFFECT pEffect);
private:
	void					Move_UI_Window(CSecondWnd* pSecond);
private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
};

