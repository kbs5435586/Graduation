#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CMesh;
class CShader;
class CAnimator;
class CNavigation;
class CCollider;
class CFrustum;
class CAnimals :
	public CGameObject
{
private:
	CAnimals();
	CAnimals(const CAnimals& rhs);
	virtual ~CAnimals() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
	virtual void							Render_GameObject_Shadow();
private:
	virtual HRESULT							CreateInputLayout();
	void									SetUp_Anim();
public:
	static CAnimals*						Create();
	virtual CGameObject*					Clone_GameObject(void* pArg, _uint iIdx = 0) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	void									Set_Animation();
	void									MeaningLess_Moving(const _float& fTimeDelta);
	void									Set_State();
private:
	void									Obb_Collision();
	void									Hit_Object(_float& fCnt, _vec3 vStart, _vec3 vEnd, _vec3 vMid);
	void									Create_Particle(const _vec3& vPoistion);
public:
	_bool&									GetIsDest(){return m_IsDest;}
private:
	CTransform*								m_pTransformCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CMesh*									m_pMeshCom = nullptr;
	CShader*								m_pShaderCom = nullptr;
	CShader*								m_pComputeShaderCom = nullptr;
	CShader*								m_pShaderCom_Shadow = nullptr;
	CAnimator*								m_pAnimCom = nullptr;
	CNavigation*							m_pNaviCom = nullptr;
	CCollider*								m_pColliderCom_AABB = nullptr;
	CCollider*								m_pColliderCom_OBB = nullptr;
	CTexture*								m_pTextureCom = nullptr;
	CTexture*								m_pTextureCom_Normal = nullptr;
	CFrustum*								m_pFrustumCom = nullptr;
private:
	vector<AnimCtrl>						m_vecAnimCtrl;
private:
	_uint									m_iCurAnimIdx = 0;
	_uint									m_iPreAnimIdx = 0;
	_float									m_fEatTime = 0.f;
	_bool									m_IsOnce = false;
	_bool									m_IsDest = false;
	_bool									m_IsEat = false;
	_vec3									m_vDest = {};
	_vec3									m_vOBB_Range = {};
private:
	ANIMALS									m_eAnimals = ANIMALS::ANIMALS_END;
	STATE									m_eCurState = STATE::STATE_END;
	STATE									m_ePreState = STATE::STATE_END;
};

