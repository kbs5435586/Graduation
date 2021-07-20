#include "GameObject.h"
class CTransform;
class CRenderer;
class CMesh;
class CShader;
class CAnimator;
class CNavigation;
class CCollider;
class CUI_OnHead;
class CUI_OnHeadBack;
class CPlayer :
	public CGameObject
{
private:
	CPlayer();
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
	virtual void							Render_GameObject_Shadow();
	virtual void							Render_PostEffect();
	virtual void							Render_Blur();
	virtual void							Render_Ref();
private:
	virtual HRESULT							CreateInputLayout();
public:
	static CPlayer*							Create();
	virtual CGameObject*					Clone_GameObject(void* pArg, _uint iIdx = 0) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	void									Set_Animation(const _float& fTimeDelta);
	void									Change_Class();
	void									AnimVectorClear();
	void									Compute_Matrix_X();
	void									Compute_Matrix_Z();
	void									Obb_Collision();
	void									Hit_Object(_float& fCnt, _vec3 vStart, _vec3 vEnd, _vec3 vMid);
private:
	void									Input_Key(const _float& fTimeDelta);
private:
	void									Death(const _float& fTimeDelta);
	void									DeathMontion_Init();
	void									Attack(const _float& fTimeDelta);
	void									Combat(const _float& fTimeDelta);
	void									SetSpeed();
public:
	CLASS&									GetClass(){return m_eCurClass;}
private:
	CTransform*								m_pTransformCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CMesh*									m_pMeshCom[(_uint)CLASS::CLASS_END] = {nullptr};
	CShader*								m_pShaderCom = nullptr;
	CShader*								m_pComputeShaderCom = nullptr;
	CShader*								m_pShaderCom_Shadow = nullptr;
	CShader*								m_pShaderCom_PostEffect = nullptr;
	CShader*								m_pShaderCom_Blur = nullptr;
	CShader*								m_pShaderCom_Reflection = nullptr;
	CAnimator*								m_pAnimCom[(_uint)CLASS::CLASS_END] = {nullptr};
	CNavigation*							m_pNaviCom = nullptr;
	CCollider*								m_pColiider[2] = { nullptr };
	CTexture*								m_pTextureCom[2] = {nullptr};
	CFrustum*								m_pFrustumCom = nullptr;
private:
	CUI_OnHead*								m_pUI_OnHead = nullptr;
	CUI_OnHeadBack*							m_pUI_OnHeadBack = nullptr;
private:
	CMesh*									m_pCurMeshCom = nullptr;
	CAnimator*								m_pCurAnimCom = nullptr;
private:
	vector<AnimCtrl>						m_vecAnimCtrl;
private:
	_bool									m_IsOnce = false;
	_bool									m_IsCombat = false;
	_bool									m_IsSlide = false;
	_uint									m_iCurAnimIdx = 0;
	_uint									m_iPreAnimIdx =0;
	_uint									m_iAttackMotion[2] = {};
	_uint									m_iDeathMotion[2] = {};
	_float									m_fDeathTime = 0.f;
	_float									m_fCombatTime = 0.f;
	_uint									m_iCombatMotion[3] = {};
	_vec3									m_vOBB_Range[2] = {};
	_float									m_fSpeed = 0.f;
	_float									m_fArrSpeed[(_uint)CLASS::CLASS_END] = {};
	_float									m_fArrSpeedUP[(_uint)CLASS::CLASS_END] = {};
	_ubyte									m_cMoveCondition;
	_ubyte									m_cLastMoveCondition;
	_ubyte									m_cRotateCondition;
	_ubyte									m_cLastRotateCondition;
private:
	_matrix									m_matLeft = {};
	_matrix									m_matRight = {};
private:
	CLASS									m_eCurClass = CLASS::CLASS_END;
	CLASS									m_ePreClass = CLASS::CLASS_END;
	PLAYER									m_tPlayer = {};
private:
	_uint									m_iCurMeshNum = 0;

};

