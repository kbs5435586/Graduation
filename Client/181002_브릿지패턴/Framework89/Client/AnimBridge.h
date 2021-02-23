#pragma once
#include "EffectBridge.h"
class CAnimBridge :
	public CEffectBridge
{
public:
	CAnimBridge();
	virtual ~CAnimBridge();

public:
	void SetFrame(const FRAME& frame) { m_tFrame = frame; }

public:
	// CEffectBridge��(��) ���� ��ӵ�
	virtual HRESULT Initialize() override;
	virtual void LateInit()	override;
	virtual int Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;
	virtual void Release() override;

private:
	FRAME	m_tFrame;
};

