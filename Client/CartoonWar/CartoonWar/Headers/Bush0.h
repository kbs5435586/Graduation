#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CMesh;
class CShader;
class CTexture;
class CBush0 :
	public CGameObject
{
private:
	CBush0();
	CBush0(const CBush0& rhs);
	virtual ~CBush0() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
private:
	virtual HRESULT							CreateInputLayout();
public:
	static CBush0* Create();
	virtual CGameObject* Clone_GameObject(void* pArg, const _uint& iIdx = 0) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	CTransform* m_pTransformCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CMesh* m_pMeshCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;

};
