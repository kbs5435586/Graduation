#pragma once
#include "EffectBridge.h"
class CNonAnimBridge :
	public CEffectBridge
{
public:
	CNonAnimBridge();
	virtual ~CNonAnimBridge();

public:
	void SetTime(float fLifeTime) { m_fLifeTime = fLifeTime; }

public:
	// CEffectBridge��(��) ���� ��ӵ�
	virtual HRESULT Initialize() override;
	virtual void LateInit()	override;
	virtual int Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;
	virtual void Release() override;

private:
	float	m_fLifeTime;
	float	m_fCurTime;
};

