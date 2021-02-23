#pragma once

class CObj;
class CEffectBridge	// ±¸ÇöÃþ
{
public:
	CEffectBridge();
	virtual ~CEffectBridge();

public:
	void SetEffect(CObj* pEffect) { m_pEffect = pEffect; }
	void SetStateKey(wstring wstrStateKey) { m_wstrStateKey = wstrStateKey; }

public:
	virtual HRESULT Initialize()	PURE;
	virtual void LateInit();
	virtual int Update()			PURE;
	virtual void LateUpdate()		PURE;
	virtual void Render()			PURE;
	virtual void Release()			PURE;
	

protected:
	CObj*	m_pEffect;
	wstring	m_wstrStateKey;

	bool	m_bIsInit;

};

