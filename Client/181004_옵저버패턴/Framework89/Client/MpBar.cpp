#include "stdafx.h"
#include "MpBar.h"
#include "UIObserver.h"


CMpBar::CMpBar()
{
}


CMpBar::~CMpBar()
{
	Release();
}

HRESULT CMpBar::Initialize()
{

	m_tInfo.vPos = { 0.f, 100.f, 0.f };

	m_pObserver = new CUIObserver;
	CDataSubject::GetInstance()->Subscribe(m_pObserver);


	return S_OK;
}

void CMpBar::LateInit()
{
}

int CMpBar::Update()
{
	CObj::LateInit();
	D3DXMatrixTranslation(&m_matTrans, m_tInfo.vPos.x, m_tInfo.vPos.y, 0.f);

	matWorld=m_matTrans*CObjMgr::GetInstance()->GetPlayer()->GetInfo().matWorld;

	return 0;
}

void CMpBar::LateUpdate()
{
	m_iMp = dynamic_cast<CUIObserver*>(m_pObserver)->GetStat().iMp;
}

void CMpBar::Render()
{
	TCHAR szBuf[MIN_STR] = L"";
	swprintf_s(szBuf, L"MP: %d", m_iMp);

	GET_INSTANCE(CDevice)->GetSprite()->SetTransform(&matWorld);

	GET_INSTANCE(CDevice)->GetFont()->DrawTextW(GET_INSTANCE(CDevice)->GetSprite(),
		szBuf, lstrlen(szBuf), nullptr, 0, D3DCOLOR_ARGB(255, 255, 255, 255));
}

void CMpBar::Release()
{
	CDataSubject::GetInstance()->UnSubscribe(m_pObserver);
}
