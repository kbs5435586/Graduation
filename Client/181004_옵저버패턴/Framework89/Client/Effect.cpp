#include "stdafx.h"
#include "Effect.h"
#include "EffectBridge.h"


CEffect::CEffect()
	: m_pBridge(nullptr)
{
}


CEffect::~CEffect()
{
	Release();
}

HRESULT CEffect::Initialize()
{
	m_wstrObjKey = L"EFFECT";

	return S_OK;
}

void CEffect::LateInit()
{
	m_pBridge->SetEffect(this);
}

int CEffect::Update()
{
	CObj::LateInit();

	D3DXMATRIX matScale, matTrans;
	D3DXVECTOR3 vScroll = CScrollMgr::GetScroll();

	D3DXMatrixScaling(&matScale, 1.f, 1.f, 0.f);
	D3DXMatrixTranslation(&matTrans, m_tInfo.vPos.x - vScroll.x,
		m_tInfo.vPos.y - vScroll.y, 0.f);

	m_tInfo.matWorld = matScale * matTrans;

	return m_pBridge->Update();
}

void CEffect::LateUpdate()
{
	m_pBridge->LateUpdate();
}

void CEffect::Render()
{
	m_pBridge->Render();
}

void CEffect::Release()
{
	SafeDelete(m_pBridge);
}
