#include "stdafx.h"
#include "AnimBridge.h"
#include "Obj.h"


CAnimBridge::CAnimBridge()
{
}


CAnimBridge::~CAnimBridge()
{
}

HRESULT CAnimBridge::Initialize()
{
	return S_OK;
}

void CAnimBridge::LateInit()
{
}

int CAnimBridge::Update()
{
	CEffectBridge::LateInit();

	m_tFrame.fFrame += m_tFrame.fMax * GET_INSTANCE(CTimeMgr)->GetDeltaTime();

	if (m_tFrame.fFrame > m_tFrame.fMax)
		return DEAD_OBJ;

	return NO_EVENT;
}

void CAnimBridge::LateUpdate()
{
}

void CAnimBridge::Render()
{
	wstring wstrObjKey = m_pEffect->GetObjKey();
	INFO tInfo = m_pEffect->GetInfo();

	const TEXINFO* pTexInfo = GET_INSTANCE(CTextureMgr)->GetTexInfo(wstrObjKey.c_str(),
		m_wstrStateKey.c_str(), (int)m_tFrame.fFrame);
	NULL_CHECK_VOID(pTexInfo);

	float fX = pTexInfo->tImgInfo.Width * 0.5f;
	float fY = pTexInfo->tImgInfo.Height * 0.5f;

	GET_INSTANCE(CDevice)->GetSprite()->SetTransform(&tInfo.matWorld);
	GET_INSTANCE(CDevice)->GetSprite()->Draw(pTexInfo->pTexture, nullptr,
		&D3DXVECTOR3(fX, fY, 0.f), nullptr, D3DCOLOR_ARGB(255, 255, 255, 255));
}

void CAnimBridge::Release()
{
}
