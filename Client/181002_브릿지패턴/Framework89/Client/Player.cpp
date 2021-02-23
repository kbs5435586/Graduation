#include "stdafx.h"
#include "Player.h"
#include "Mouse.h"

#include "Effect.h"
#include "AnimBridge.h"
#include "NonAnimBridge.h"


CPlayer::CPlayer()
{
}


CPlayer::~CPlayer()
{
	Release();
}

HRESULT CPlayer::Initialize()
{
	m_tInfo.vPos = { 450.f, 300.f, 0.f };
	m_tInfo.vSize = { 1.f, 1.f, 0.f };
	m_tFrame = { 0.f , 11.f };

	return S_OK;
}

void CPlayer::LateInit()
{
}

int CPlayer::Update()
{
	CObj::LateInit();
	MoveRoute();
	KeyInput();

	D3DXVECTOR3 vScroll = CScrollMgr::GetScroll();

	D3DXMATRIX matScale, matTrans;
	D3DXMatrixScaling(&matScale, m_tInfo.vSize.x, m_tInfo.vSize.y, 0.f);
	D3DXMatrixTranslation(&matTrans, m_tInfo.vPos.x - vScroll.x, 
		m_tInfo.vPos.y - vScroll.y, 0.f);

	m_tInfo.matWorld = matScale * matTrans;

	return NO_EVENT;
}

void CPlayer::LateUpdate()
{
	CObj::MoveFrame();
}

void CPlayer::Render()
{
	const TEXINFO* pTexInfo = GET_INSTANCE(CTextureMgr)->GetTexInfo(L"PLAYER", L"Dash",
		(int)m_tFrame.fFrame);
	NULL_CHECK_VOID(pTexInfo);

	float fCenterX = pTexInfo->tImgInfo.Width * 0.5f;
	float fCenterY = pTexInfo->tImgInfo.Height * 0.5f;

	GET_INSTANCE(CDevice)->GetSprite()->SetTransform(&m_tInfo.matWorld);
	GET_INSTANCE(CDevice)->GetSprite()->Draw(pTexInfo->pTexture, nullptr,
		&D3DXVECTOR3(fCenterX, fCenterY, 0.f), nullptr, D3DCOLOR_ARGB(255, 255, 255, 255));
}

void CPlayer::Release()
{
}

void CPlayer::KeyInput()
{
	if (GET_INSTANCE(CKeyMgr)->KeyUp(KEY_LBUTTON))
	{
		D3DXVECTOR3 vMouse = CMouse::GetMousePos();
		vMouse += CScrollMgr::GetScroll();

		GET_INSTANCE(CAstarMgr)->StartAstar(m_tInfo.vPos, vMouse);
		GET_INSTANCE(CTimeMgr)->UpdateTime();
	}

	if (GET_INSTANCE(CKeyMgr)->KeyUp(KEY_UP))
	{
		D3DXVECTOR3 vMouse = CMouse::GetMousePos();
		vMouse += CScrollMgr::GetScroll();

		CObj* pEffect = CEffectFactory<CEffect, CAnimBridge>::CreateEffect(vMouse,
			L"BossMultiAttack", { 0.f, 36.f });
		NULL_CHECK_VOID(pEffect);

		GET_INSTANCE(CObjMgr)->AddObject(pEffect, CObjMgr::EFFECT);
	}

	if (GET_INSTANCE(CKeyMgr)->KeyUp(KEY_DOWN))
	{
		D3DXVECTOR3 vMouse = CMouse::GetMousePos();
		vMouse += CScrollMgr::GetScroll();

		CObj* pEffect = CEffectFactory<CEffect, CNonAnimBridge>::CreateEffect(vMouse,
			L"Cray", 3.f);
		NULL_CHECK_VOID(pEffect);

		GET_INSTANCE(CObjMgr)->AddObject(pEffect, CObjMgr::EFFECT);
	}
}

void CPlayer::MoveRoute()
{
	list<TILE*>& BestLst = GET_INSTANCE(CAstarMgr)->GetBestLst();

	if (!BestLst.empty())
	{
		m_tInfo.vDir = BestLst.front()->vPos - m_tInfo.vPos;
		
		float fDist = D3DXVec3Length(&m_tInfo.vDir);
		D3DXVec3Normalize(&m_tInfo.vDir, &m_tInfo.vDir);

		m_tInfo.vPos += m_tInfo.vDir * 300.f * GET_INSTANCE(CTimeMgr)->GetDeltaTime();

		if (3.f > fDist)
			BestLst.pop_front();
	}
}
