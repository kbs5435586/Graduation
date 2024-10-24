#include "GameObject.h"
class CTransform;
class CRenderer;
class CMesh;
class CShader;
class CAnimator;
//class CNavigation;
//class CCollider;
class CPlayer_Inven :
	public CGameObject
{
private:
	CPlayer_Inven();
	CPlayer_Inven(const CPlayer_Inven& rhs);
	virtual ~CPlayer_Inven() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
private:
	virtual HRESULT							CreateInputLayout();
public:
	static CPlayer_Inven*					Create();
	virtual CGameObject*					Clone_GameObject(void* pArg, _uint iIdx = 0) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	void									Set_Animation(const _float& fTimeDelta);
	void									Change_Class();
	void									AnimVectorClear();
	//void									Compute_Matrix_X();
	//void									Compute_Matrix_Z();
	//void									Obb_Collision();
	//void									Hit_Object(_float& fCnt, _vec3 vStart, _vec3 vEnd, _vec3 vMid);
private:
	//void									Input_Inven_Key(const _float& fTimeDelta);
private:
	//void									Death(const _float& fTimeDelta);
	//void									DeathMontion_Init();
	//void									Attack(const _float& fTimeDelta);
	//void									Combat(const _float& fTimeDelta);
private:
	CTransform*								m_pTransformCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CMesh*									m_pMeshCom[(_uint)CLASS::CLASS_END] = { nullptr };
	CShader*								m_pShaderCom = nullptr;
	CShader*								m_pComputeShaderCom = nullptr;
	CAnimator*								m_pAnimCom[(_uint)CLASS::CLASS_END] = { nullptr };
	//CNavigation* m_pNaviCom = nullptr;
	//CCollider* m_pColiider[2] = { nullptr };
	CTexture*								m_pTextureCom[2] = { nullptr };
private:
	CMesh*									m_pCurMeshCom = nullptr;
	CAnimator*								m_pCurAnimCom = nullptr;
private:
	vector<AnimCtrl>						m_vecAnimCtrl;
private:
	_bool									m_IsOnce = false;
	_bool									m_IsCombat = false;
	_uint									m_iCurAnimIdx = 0;
	_uint									m_iPreAnimIdx = 0;
	_uint									m_iAttackMotion[2] = {};
	_uint									m_iDeathMotion[2] = {};
	_float									m_fDeathTime = 0.f;
	_float									m_fCombatTime = 0.f;
	_uint									m_iCombatMotion[3] = {};
	//_vec3									m_vOBB_Range[2] = {};
private:
	_matrix									m_matLeft = {};
	_matrix									m_matRight = {};
private:
	//_uint									m_iCurMeshNum = 0;
	_vec3									m = {};
	_bool									m_IsActive = {};

};