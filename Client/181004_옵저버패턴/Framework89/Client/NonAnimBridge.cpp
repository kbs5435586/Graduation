#include "stdafx.h"
#include "NonAnimBridge.h"
#include "Effect.h"

CNonAnimBridge::CNonAnimBridge()
	: m_fLifeTime(0.f), m_fCurTime(0.f)
{
}


CNonAnimBridge::~CNonAnimBridge()
{
}

HRESULT CNonAnimBridge::Initialize()
{
	return S_OK;
}

void CNonAnimBridge::LateInit()
{
}

int CNonAnimBridge::Update()
{
	CEffectBridge::LateInit();

	if (m_fCurTime > m_fLifeTime)
		return DEAD_OBJ;

	m_fCurTime += GET_INSTANCE(CTimeMgr)->GetDeltaTime();

	return NO_EVENT;
}

void CNonAnimBridge::LateUpdate()
{
}

void CNonAnimBridge::Render()
{
	wstring wstrObjKey = m_pEffect->GetObjKey();
	INFO tInfo = m_pEffect->GetInfo();

	const TEXINFO* pTexInfo = GET_INSTANCE(CTextureMgr)->GetTexInfo(wstrObjKey.c_str(),
		m_wstrStateKey.c_str(), 7);
	NULL_CHECK_VOID(pTexInfo);

	float fX = pTexInfo->tImgInfo.Width * 0.5f;
	float fY = pTexInfo->tImgInfo.Height * 0.5f;

	GET_INSTANCE(CDevice)->GetSprite()->SetTransform(&tInfo.matWorld);
	GET_INSTANCE(CDevice)->GetSprite()->Draw(pTexInfo->pTexture, nullptr,
		&D3DXVECTOR3(fX, fY, 0.f), nullptr, D3DCOLOR_ARGB(255, 255, 255, 255));
}

void CNonAnimBridge::Release()
{
}
