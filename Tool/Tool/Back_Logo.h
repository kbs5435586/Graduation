#pragma once
#include "GameObject.h"

class CTransform;
class CRenderer;
class CTexture;
class CBuffer_RcTex;
class CShader;
class CBack_Logo :
	public CGameObject
{
private:
	explicit CBack_Logo(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CBack_Logo(const CBack_Logo& rhs);
	virtual ~CBack_Logo() = default;
public:
	virtual HRESULT Ready_Prototype();
	virtual HRESULT Ready_GameObject();
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject();
private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CBuffer_RcTex*		m_pBufferCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
private:
	_vec4				m_vColor = {};
private:
	HRESULT Ready_Component(); 
public:
	static CBack_Logo* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject() override;
protected:
	virtual void Free();

};

