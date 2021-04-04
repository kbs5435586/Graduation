#include "framework.h"
#include "Debug_Camera.h"
#include "Management.h"

CDebug_Camera::CDebug_Camera( )
	: CCamera()
{
}

CDebug_Camera::CDebug_Camera(const CDebug_Camera& rhs)
	: CCamera(rhs)
{
}

HRESULT CDebug_Camera::Ready_Prototype()
{
	if (FAILED(CCamera::Ready_Prototype()))
		return E_FAIL;

	return NOERROR;
}

HRESULT CDebug_Camera::Ready_GameObject(void* pArg)
{
	if (FAILED(CDebug_Camera::Ready_Component()))
		return E_FAIL;
	if (FAILED(CCamera::Ready_GameObject()))
		return E_FAIL;
	
	m_pTransform->SetUp_Speed(30.f, XMConvertToRadians(90.f));

	m_ptMouse.x = static_cast<LONG>(WINCX) / 2;
	m_ptMouse.y = static_cast<LONG>(WINCY) / 2;
	ClientToScreen(g_hWnd, &m_ptMouse);

	//CManagement::GetInstance()->Subscribe(m_pObserverCom);
	
	return NOERROR;
}

_int CDebug_Camera::Update_GameObject(const _float& fTimeDelta)
{
	//SetCursorPos(m_ptMouse.x, m_ptMouse.y);
	if (nullptr == m_pInput_Device)
		return -1;

	if (m_pInput_Device->Get_DIKeyState(DIK_W) & 0x80)
	{
		m_pTransform->Go_Straight(fTimeDelta);
	}
	if (m_pInput_Device->Get_DIKeyState(DIK_S) & 0x80)
	{
		m_pTransform->BackWard(fTimeDelta);
	}
	if (m_pInput_Device->Get_DIKeyState(DIK_A) & 0x80)
	{
		m_pTransform->Go_Left(fTimeDelta);
	}
	if (m_pInput_Device->Get_DIKeyState(DIK_D) & 0x80)
	{
		m_pTransform->Go_Right(fTimeDelta);
	}
	_long	MouseMove = 0;
	if (MouseMove = m_pInput_Device->Get_DIMouseMove(CInput::DIM_X))
	{
		m_pTransform->Rotation_Y(MouseMove * fTimeDelta * 0.5f);
	}


	if (MouseMove = CInput::GetInstance()->Get_DIMouseMove(CInput::DIM_Y))
	{
		m_pTransform->Rotation_Axis(XMConvertToRadians((_float)MouseMove) * -fTimeDelta * 30.f, m_pTransform->Get_StateInfo(CTransform::STATE_RIGHT));
	}


	return _int();
}

_int CDebug_Camera::LastUpdate_GameObject(const _float& fTimeDelta)
{
	Invalidate_ViewProjMatrix(0);

	return _int();
}

void CDebug_Camera::Render_GameObject()
{
}

CDebug_Camera* CDebug_Camera::Create()
{
	CDebug_Camera* pInstance = new CDebug_Camera();

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MessageBox(0, L"CCamera_Debug Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CDebug_Camera::Clone_GameObject(void* pArg)
{
	CDebug_Camera* pInstance = new CDebug_Camera(*this);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CCamera_Debug Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

HRESULT CDebug_Camera::Ready_Component()
{
	CManagement* pManagement = CManagement::GetInstance();
	NULL_CHECK_VAL(pManagement, E_FAIL);
	pManagement->AddRef();


	//m_pObserverCom = (CObserver*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Observer");
	//NULL_CHECK_VAL(m_pObserverCom, E_FAIL);
	//if (FAILED(Add_Component(L"Com_Observer", m_pObserverCom)))
	//	return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}

void CDebug_Camera::Free()
{
	//Safe_Release(m_pObserverCom);

	CCamera::Free();
}
