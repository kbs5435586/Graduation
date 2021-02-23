#include "stdafx.h"
#include "BuffEffect.h"
#include "EffectBridge.h"

CBuffEffect::CBuffEffect()
	: m_fRevAngle(0.f)
{
}


CBuffEffect::~CBuffEffect()
{
	Release();
}

HRESULT CBuffEffect::Initialize()
{
	// 플레이어 위치로부터 300만큼 x축 떨어진다.
	m_tInfo.vPos = { 300.f, 0.f, 0.f };

	return CEffect::Initialize();
}

void CBuffEffect::LateInit()
{
	CEffect::LateInit();

	m_pTarget = GET_INSTANCE(CObjMgr)->GetPlayer();
}

int CBuffEffect::Update()
{
	CObj::LateInit();

	D3DXMATRIX matScale, matRot, matTrans, matRev;

	D3DXMatrixScaling(&matScale, 1.f, 1.f, 0.f);
	D3DXMatrixTranslation(&matTrans, m_tInfo.vPos.x, m_tInfo.vPos.y, 0.f);
	D3DXMatrixRotationZ(&matRot, D3DXToRadian(m_fRevAngle));
	D3DXMatrixRotationZ(&matRev, -D3DXToRadian(m_fRevAngle));

	// 스 * 자 * 이 * 공 * 부
	m_tInfo.matWorld = matScale * matRot * matTrans * matRev * m_pTarget->GetInfo().matWorld;

	m_fRevAngle -= 90.f * GET_INSTANCE(CTimeMgr)->GetDeltaTime();

	return m_pBridge->Update();
}

void CBuffEffect::LateUpdate()
{
	m_pBridge->LateUpdate();
}

void CBuffEffect::Render()
{
	m_pBridge->Render();
}

void CBuffEffect::Release()
{
}
