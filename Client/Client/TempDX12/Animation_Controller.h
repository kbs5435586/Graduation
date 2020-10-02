#pragma once
#include "Base.h"
class CAnimation_Controller
	:public CBase
{
private:
	CAnimation_Controller();
	~CAnimation_Controller() = default;
public:
	HRESULT						Ready_Animation_Controller(FbxScene* pFbxScene);
	HRESULT						Set_Animation(FbxScene* pFbxScene, const _uint& iAnimation_Num);
	void						Play_Animation(const _float& fTimeDelta);
public:
	static						CAnimation_Controller* Create(FbxScene* pFbxScene);
private:
	virtual void				Free();
public:
	FbxTime						Get_CurrentTime() { return m_pFbxCurrentTime[m_iAnimationStack]; }
	void						Set_Position(_uint iAnimationStack, _float fPosition);
private:
	_uint						m_iAnimationStack = 0;
	_float						m_fTime = 0.f;
private:
	FbxAnimStack**				m_ppFbxAnimationStacks = nullptr;
	FbxTime*					m_pFbxCurrentTime = nullptr;
	FbxTime*					m_pFbxStartTime = nullptr;
	FbxTime*					m_pFbxStopTime = nullptr;

};

