#pragma once
#include "GameObject.h"
class COrc04 :
	public CGameObject
{
private:
	COrc04();
	COrc04(const COrc04& rhs);
	virtual ~COrc04() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
private:
	virtual HRESULT							CreateInputLayout();
	void									SetUp_Anim();
public:
	static COrc04* Create();
	virtual CGameObject* Clone_GameObject(void* pArg, const _uint& iIdx = 0) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	void									Set_Animation();
private:
	CTransform* m_pTransformCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CMesh* m_pMeshCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CShader* m_pComputeShaderCom = nullptr;
	CAnimator* m_pAnimCom = nullptr;
	CNavigation* m_pNaviCom = nullptr;
private:
	vector<CTexture*>						m_vecTexture;
	vector<AnimCtrl>						m_vecAnimCtrl;
private:
	_uint									m_iCurAnimIdx = 0;
	_uint									m_iPreAnimIdx = 0;
	_bool									m_IsOnce = false;
private:
	float									m_fRatio = 0.f;
};

