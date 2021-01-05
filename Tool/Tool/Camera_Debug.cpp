#include "stdafx.h"
#include "Camera_Debug.h"
#include "Management.h"

CCamera_Debug::CCamera_Debug(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CCamera(pGraphic_Device)
{
}

CCamera_Debug::CCamera_Debug(const CCamera_Debug & rhs)
	: CCamera(rhs)
{

}

HRESULT CCamera_Debug::Ready_Prototype()
{
	if (FAILED(CCamera::Ready_Prototype()))
		return E_FAIL;

	return NOERROR;
}

HRESULT CCamera_Debug::Ready_GameObject(void* pArg)
{
	if (FAILED(CCamera::Ready_GameObject(pArg)))
		return E_FAIL;

	m_pTransform->SetUp_Speed(m_fCameraSpeed, D3DXToRadian(360.f));

	D3DVIEWPORT9			ViewPort;
	m_pGraphic_Device->GetViewport(&ViewPort);
	m_ptMouse.x = ViewPort.Width >> 1;
	m_ptMouse.y = ViewPort.Height >> 1;

	ClientToScreen(g_hWnd, &m_ptMouse);
	
	return NOERROR;
}


_int CCamera_Debug::Update_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pInput_Device)
		return -1;

	_long	MouseMove = 0;
	if (IsMouseCheck)
	{
		CManagement* pManagement = CManagement::GetInstance();
		if (nullptr == pManagement)
			return E_FAIL;
		pManagement->AddRef();

		if (pManagement->KeyPressing(KEY_LEFT))
			m_pTransform->Go_Left(fTimeDelta);
		if (pManagement->KeyPressing(KEY_RIGHT))
			m_pTransform->Go_Right(fTimeDelta);
		if (pManagement->KeyPressing(KEY_UP))
			m_pTransform->Go_Straight(fTimeDelta);
		if (pManagement->KeyPressing(KEY_DOWN))
			m_pTransform->BackWard(fTimeDelta);

		Safe_Release(pManagement);

		SetCursorPos(m_ptMouse.x, m_ptMouse.y);
		if (MouseMove = m_pInput_Device->Get_DIMouseMove(DIM_X))
			m_pTransform->Rotation_Y(D3DXToRadian(MouseMove) * fTimeDelta);
		if (MouseMove = m_pInput_Device->Get_DIMouseMove(DIM_Y))
			m_pTransform->Rotation_Axis(D3DXToRadian(MouseMove) * fTimeDelta, m_pTransform->Get_StateInfo(STATE_RIGHT));
	}
	Invalidate_ViewProjMatrix();
	return _int();
}

_int CCamera_Debug::LastUpdate_GameObject(const _float & fTimeDelta)
{
	return _int();
}

void CCamera_Debug::Render_GameObject()
{
}

CCamera_Debug * CCamera_Debug::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCamera_Debug*	pInstance = new CCamera_Debug(pGraphic_Device);
	if (FAILED(pInstance->Ready_Prototype()))
		Safe_Release(pInstance);
	return pInstance;
}


CGameObject * CCamera_Debug::Clone_GameObject(void* pArg)
{
	CCamera_Debug*	pInstance = new CCamera_Debug(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);
	return pInstance;
}

void CCamera_Debug::Free()
{
	CCamera::Free();
}
