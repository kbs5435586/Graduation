#include "framework.h"
#include "Management.h"
#include "Debug_Camera.h"
#include "Server_Manager.h"

CDebug_Camera::CDebug_Camera()
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
	if (FAILED(CCamera::Ready_GameObject()))
		return E_FAIL;

	m_pTransform->SetUp_Speed(50.f, XMConvertToRadians(90.f));

	m_ptMouse.x = static_cast<LONG>(WINCX) / 2;
	m_ptMouse.y = static_cast<LONG>(WINCY) / 2;
	ClientToScreen(g_hWnd, &m_ptMouse);




	return NOERROR;
}

_int CDebug_Camera::Update_GameObject(const _float& fTimeDelta)
{
	//SetCursorPos(m_ptMouse.x, m_ptMouse.y);
	if (nullptr == m_pInput_Device)
		return -1;
	server->AddRef();

	int id = server->get_my_id();
	CTransform* pTransform;
	if (ENUM_PLAYER1 == id)
	{
		pTransform = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
			L"Layer_Orc02", L"Com_Transform", 0);
	}
	else if (ENUM_PLAYER2 == id)
	{
		pTransform = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
			L"Layer_Orc04", L"Com_Transform", 0);
	}
	...
	else if (ENUM_PLAYER30 == id)
	{
		pTransform = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
			L"Layer_Orc30", L"Com_Transform", 0);
	}
	읽은 pTransform 값 카메라에 이용*/



	//SetCursorPos(m_ptMouse.x, m_ptMouse.y);
	if (nullptr == m_pInput_Device)
		return -1;
	CTransform* pTransform = (CTransform*)CManagement::GetInstance()->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
		L"Layer_Orc02", L"Com_Transform", 0);

	
	_vec3 vPos, vUp, vLook;
	_vec3 vTempUp, vTempLook, vTempRight;


	vPos = *pTransform->Get_StateInfo(CTransform::STATE_POSITION);
	vUp = *pTransform->Get_StateInfo(CTransform::STATE_UP);
	vLook = *pTransform->Get_StateInfo(CTransform::STATE_LOOK);
	
	vPos.y += 1.f;
	vUp = vUp * 300.f;
	vLook = vLook * 300.f;

	_vec3 vTemp = vUp - vLook;
	vPos = vPos + vTemp;
	_long	MouseMove = 0;

	{

		m_pTransform->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
		if (MouseMove = m_pInput_Device->Get_DIMouseMove(CInput::DIM_X))
		{
			pTransform->Rotation_Y(MouseMove * fTimeDelta * 0.5f);
		}
		if (MouseMove = m_pInput_Device->Get_DIMouseMove(CInput::DIM_X))
		{
			m_pTransform->Rotation_Y(MouseMove * fTimeDelta * 0.5f);
		}


		if (MouseMove = CInput::GetInstance()->Get_DIMouseMove(CInput::DIM_Y))
		{
			m_pTransform->Rotation_Axis(XMConvertToRadians((_float)MouseMove) * -fTimeDelta * 30.f, m_pTransform->Get_StateInfo(CTransform::STATE_RIGHT));
		}

	}


	//Safe_Release(server);
	return _int();
}

_int CDebug_Camera::LastUpdate_GameObject(const _float& fTimeDelta)
{
	Invalidate_ViewProjMatrix();

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

CGameObject* CDebug_Camera::Clone_GameObject(void* pArg , _uint iIdx)
{
	CDebug_Camera* pInstance = new CDebug_Camera(*this);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CCamera_Debug Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	m_iLayerIdx = iIdx;
	return pInstance;
}

void CDebug_Camera::Free()
{
	Safe_Release(m_pNaviCom);
	CCamera::Free();
}
