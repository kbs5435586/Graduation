#include "stdafx.h"
#include "HpBar.h"
#include "UIObserver.h"

CHpBar::CHpBar()
	: m_iHp(0)
{
}


CHpBar::~CHpBar()
{
}

HRESULT CHpBar::Initialize()
{
	m_tInfo.vPos = { 10.f, 10.f, 0.f };	// 화면 상단

	m_pObserver = new CUIObserver;
	GET_INSTANCE(CDataSubject)->Subscribe(m_pObserver);

	return S_OK;
}

void CHpBar::LateInit()
{
}

int CHpBar::Update()
{
	CObj::LateInit();
		
	D3DXMatrixTranslation(&m_tInfo.matWorld, m_tInfo.vPos.x, m_tInfo.vPos.y, 0.f);

	return NO_EVENT;
}

void CHpBar::LateUpdate()
{
	m_iHp = dynamic_cast<CUIObserver*>(m_pObserver)->GetStat().iHp;
}

void CHpBar::Render()
{
	TCHAR szBuf[MIN_STR] = L"";
	swprintf_s(szBuf, L"HP: %d", m_iHp);

	GET_INSTANCE(CDevice)->GetSprite()->SetTransform(&m_tInfo.matWorld);
	GET_INSTANCE(CDevice)->GetFont()->DrawTextW(GET_INSTANCE(CDevice)->GetSprite(),
		szBuf, lstrlen(szBuf), nullptr, 0, D3DCOLOR_ARGB(255, 255, 0, 0));
}

void CHpBar::Release()
{
	GET_INSTANCE(CDataSubject)->UnSubscribe(m_pObserver);
}
