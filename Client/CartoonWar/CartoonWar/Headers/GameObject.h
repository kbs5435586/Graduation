#pragma once
#include "Base.h"
class CShader;
class CComponent;
class CGameObject :
    public CBase
{
protected:
    CGameObject();
    CGameObject(const CGameObject& rhs);
	virtual ~CGameObject()=default;
public:
	CComponent*								Get_ComponentPointer(const _tchar* pComponentTag);
public:
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
	virtual void							Render_GameObject_Shadow();
	virtual void							Render_PostEffect();
	virtual void							Render_Blur();
	virtual void							Render_Ref();
public:
	INFO&									GetInfo(){return m_tInfo;}
	_bool&									GetOBBCollision(){return m_IsOBB_Collision;}
	_bool&									GetIsHit(){return m_IsHit;}
	_bool&									GetIsDead(){return m_IsDead;}
	_bool&									GetIsParticle(){return m_IsParticle;}
	_bool&									GetIsBack() {return m_IsBack;}
	_bool&									GetIsTree() {return m_IsTree;}
	_bool&									GetIsRun(){return m_IsRun;}
	_bool&									GetIsCheckRange_Flag() {return m_IsCheck_FlagRange;}
	CLASS&									GetClass() { return m_eCurClass; }
	TEAM&									GetCurTeam() { return m_eCurTeam; }
	TEAM&									GetPreTeam() { return m_ePreTeam; }
	_uint									GetLayerIdx() { return m_iLayerIdx; }
public:
	_matrix&								GetAttackedObject_Matrix(){return m_matAttackedTarget;}
public:
	virtual HRESULT							CreateInputLayout();
protected:
	HRESULT									Add_Component(const _tchar* pComponentTag, CComponent* pComponent);
	HRESULT									Delete_Component(const _tchar* pComponentTag, CComponent* pComponent);
	CComponent*								Find_Component(const _tchar* pComponentTag);
public:
	virtual CGameObject*					Clone_GameObject(void* pArg = nullptr, _uint iIdx= 0) PURE;
	virtual void							Free();
protected:
	map<const _tchar*, CComponent*>			m_mapComponent;
	typedef map<const _tchar*, CComponent*>	MAPCOMPONENT;
protected:
	INFO									m_tInfo = {};
public:
	_uint									m_iLayerIdx = 0;
public:
	_bool&									GetIsHit_PostEffect(){return m_IsHit_PostEffect; }
protected:
	_float									m_fBazierCnt = 0.f;
	_bool									m_IsClone = false;
	_bool									m_IsOBB_Collision = false; 
	_bool									m_IsBazier = false;
	_bool									m_IsDead = false;
	_bool									m_IsDeadMotion = false;
	_bool									m_IsParticle = false;
	_bool									m_IsOldMatrix = false;
	_bool									m_IsOldMatrix_Temp = false;
	_bool									m_IsPick = false;
	_bool									m_IsActioning = false;
	_bool									m_IsRun = false;
	_bool									m_IsBack = false;
	_bool									m_IsTree = false;
	TEAM									m_eCurTeam = TEAM::TEAM_END;
	TEAM									m_ePreTeam = TEAM::TEAM_END;
	_bool									m_IsHit_PostEffect = false;
	_vec3									m_vColShpereSize = {};
	CLASS									m_eCurClass = CLASS::CLASS_END;
	CLASS									m_ePreClass = CLASS::CLASS_END;
	UNIT									m_tUnit = {};
	_bool									m_IsCheck_FlagRange = false;
protected:
	_uint									m_iBlurCnt = 0;
protected:
	_matrix									m_matOldWorld;
	_matrix									m_matOldView;
protected:
	_bool									m_IsHit = false;
	_matrix									m_matAttackedTarget = {};
protected:
	_vec3									m_vStartPoint = {};
	_vec3									m_vEndPoint = {};
	_vec3									m_vMidPoint = {};
protected:
	_float									m_fLifeTime = 0.f;
};

