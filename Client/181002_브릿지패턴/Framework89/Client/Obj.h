#pragma once
class CObj
{
public:
	CObj();
	virtual ~CObj();

public:
	const INFO& GetInfo() const { return m_tInfo; }
	const wstring& GetObjKey() const { return m_wstrObjKey; }

public:
	void SetPos(const D3DXVECTOR3& vPos) { m_tInfo.vPos = vPos; }

public:
	virtual HRESULT Initialize() PURE;
	virtual void LateInit();
	virtual int Update() PURE;
	virtual void LateUpdate() PURE;
	virtual void Render() PURE;
	virtual void Release() PURE;

protected:
	void MoveFrame();

protected:
	bool	m_bIsInit;

	INFO	m_tInfo;
	FRAME	m_tFrame;

	wstring	m_wstrObjKey;
};

