#pragma once
#include "UserInterface.h"
class CMpBar :
	public CUserInterface
{
public:
	CMpBar();
	virtual ~CMpBar();
public:
	virtual HRESULT Initialize() override;
	virtual void LateInit() override;
	virtual int Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;
	virtual void Release();
private:
	int m_iMp;
	D3DXVECTOR3 m_vPos;
	D3DXMATRIX m_matTrans;
	D3DXMATRIX matWorld;
};

