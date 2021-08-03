#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CMesh;
class CShader;
class CAnimator;
class CNavigation;
class CCollider;
class CUI_OnHead;
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
	virtual void							Render_PostEffect();
	virtual void							Render_Blur();
private:
	virtual HRESULT							CreateInputLayout();
public:
	static CAnimals*						Create();
	virtual CGameObject*					Clone_GameObject(void* pArg, _uint iIdx = 0) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	void									Set_Animation(const _float& fTimeDelta);
	void									Set_Animals(ANIMALS eAnimals);
	void									AnimVectorClear();
	void									Obb_Collision();
	void									Hit_Object(_float& fCnt, _vec3 vStart, _vec3 vEnd, _vec3 vMid);
	void									Resurrection();
private:
	CTransform*								m_pTransformCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CMesh*									m_pMeshCom[(_uint)ANIMALS::ANIMALS_END] = { nullptr };
	CShader*								m_pShaderCom = nullptr;
	CShader*								m_pComputeShaderCom = nullptr;
	CShader*								m_pShaderCom_Shadow = nullptr;
	CShader*								m_pShaderCom_PostEffect = nullptr;
	CShader*								m_pShaderCom_Blur = nullptr;
	CAnimator*								m_pAnimCom[(_uint)ANIMALS::ANIMALS_END] = { nullptr };
	CNavigation*							m_pNaviCom = nullptr;
	CCollider*								m_pCollider_OBB = nullptr;
	CCollider*								m_pCollider_AABB = nullptr;
	CCollider*								m_pCollider_Attack = nullptr;
	CTexture*								m_pTextureCom[2] = { nullptr };
	CFrustum*								m_pFrustumCom = nullptr;
private:
	CUI_OnHead*								m_pUI_OnHead = nullptr;
private:
	vector<AnimCtrl>						m_vecAnimCtrl;
private:
	_bool									m_IsOnce = false;
	_bool									m_IsCombat = false;
	_bool									m_IsSlide = false;
	_uint									m_iCurAnimIdx = 0;
	_uint									m_iPreAnimIdx = 0;
	_vec3									m_vOBB_Range[2] = {};
	_float									m_fArrSpeed[(_uint)ANIMALS::ANIMALS_END] = {};
	_float									m_fArrSpeedUP[(_uint)ANIMALS::ANIMALS_END] = {};
private:
	ANIMALS									m_eAnimals = ANIMALS::ANIMALS_END;

};

