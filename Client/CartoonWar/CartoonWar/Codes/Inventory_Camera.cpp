#include "framework.h"
#include "Inventory_Camera.h"
#include "Management.h"

CInventory_Camera::CInventory_Camera()
	: CCamera()
{
}

CInventory_Camera::CInventory_Camera(const CInventory_Camera& rhs)
	: CCamera(rhs)
{
}

HRESULT CInventory_Camera::Ready_Prototype()
{
	if (FAILED(CCamera::Ready_Prototype()))
		return E_FAIL;
	num = 1;

	
	return NOERROR;
}

HRESULT CInventory_Camera::Ready_GameObject(void* pArg)
{
	if (FAILED(CInventory_Camera::Ready_Component()))
		return E_FAIL;
	if (FAILED(CCamera::Ready_GameObject()))
		return E_FAIL;
	
	
	_vec3 startPos(0.f, 500.f, 0.f);


	//m_pTransform->SetUp_RotationX(1000);
	m_pTransform->SetUp_Speed(30.f, XMConvertToRadians(90.f));

	m_ptMouse.x = static_cast<LONG>(WINCX) / 2;
	m_ptMouse.y = static_cast<LONG>(WINCY) / 2;
	ClientToScreen(g_hWnd, &m_ptMouse);

	CManagement::GetInstance()->Subscribe(m_pObserverCom);

	return NOERROR;
}

_int CInventory_Camera::Update_GameObject(const _float& fTimeDelta)
{
	
	//SetCursorPos(m_ptMouse.x, m_ptMouse.y);
	//if (nullptr == m_pInput_Device)
	//	return -1;
	//
	//if (m_pInput_Device->Get_DIKeyState(DIK_W) & 0x80)
	//{
	//	m_pTransform->Go_Straight(fTimeDelta);
	//}
	//if (m_pInput_Device->Get_DIKeyState(DIK_S) & 0x80)
	//{
	//	m_pTransform->BackWard(fTimeDelta);
	//}
	//if (m_pInput_Device->Get_DIKeyState(DIK_A) & 0x80)
	//{
	//	//m_pTransform->Rotation_Y(fTimeDelta * 0.5f);
	//	m_pTransform->Go_Left(5 * fTimeDelta);
	//}
	//if (m_pInput_Device->Get_DIKeyState(DIK_D) & 0x80)
	//{
	//	m_pTransform->Go_Right(fTimeDelta);
	//}
	//_long	MouseMove = 0;
	//if (MouseMove = m_pInput_Device->Get_DIMouseMove(CInput::DIM_X))
	//{
	//	m_pTransform->Go_Left(fTimeDelta * 0.5f);
	//	m_pTransform->Rotation_Y(MouseMove * fTimeDelta * 0.5f);
	//	//if (MouseMove > 0)
	//	//{
	//	//	m_pTransform->Go_Right(MouseMove * fTimeDelta * 0.5f);
	//	//}
	//	//else
	//	//{
	//	//	m_pTransform->Go_Right(-MouseMove * fTimeDelta * 0.5f);
	//	//}
	//	//
	//	//m_pTransform->Rotation_Y(MouseMove * fTimeDelta * 0.5f);
	//}
	//
	//if (MouseMove = CInput::GetInstance()->Get_DIMouseMove(CInput::DIM_Y))
	//{
	//	m_pTransform->Rotation_Axis(XMConvertToRadians((_float)MouseMove) * -fTimeDelta*30.f, m_pTransform->Get_StateInfo(CTransform::STATE_RIGHT));
	//}
	if (m_pInput_Device->Get_DIKeyState(DIK_I) & 0x80)
	{
		canSee = !canSee;
	}


	m_tCameraDesc.vAt = m_pObserverCom->GetVec3Info(); 
	m_tCameraDesc.vAt = m_tCameraDesc.vAt + XMFLOAT3(15.f, 0.f, 15.f);
	_vec3		vLook;
	_vec3		temp = *m_pTransform->Get_StateInfo(CTransform::STATE_POSITION);
	vLook = Vector3_::Subtract(m_tCameraDesc.vAt, *m_pTransform->Get_StateInfo(CTransform::STATE_POSITION));
	vLook = Vector3_::Normalize(vLook);

	_vec3		vRight;
	vRight = Vector3_::CrossProduct(m_tCameraDesc.vAxisY, vLook, false);
	vRight = Vector3_::Normalize(vRight);

	_vec3		vUp;
	vUp = Vector3_::CrossProduct(vLook, vRight);
	vUp = Vector3_::Normalize(vUp);

	m_pTransform->Set_StateInfo(CTransform::STATE_RIGHT, &vRight);
	m_pTransform->Set_StateInfo(CTransform::STATE_UP, &vUp);
	m_pTransform->Set_StateInfo(CTransform::STATE_LOOK, &vLook);

	return _int();
}

_int CInventory_Camera::LastUpdate_GameObject(const _float& fTimeDelta)
{
	Invalidate_ViewProjMatrix(1);
	return _int();
}

void CInventory_Camera::Render_GameObject()
{
}

CInventory_Camera* CInventory_Camera::Create()
{
	CInventory_Camera* pInstance = new CInventory_Camera();

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MessageBox(0, L"CInventory_Camera Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CInventory_Camera::Clone_GameObject(void* pArg, _uint iIdx)
{
	CInventory_Camera* pInstance = new CInventory_Camera(*this);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CInventory_Camera Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

HRESULT CInventory_Camera::Ready_Component()
{
	CManagement* pManagement = CManagement::GetInstance();
	NULL_CHECK_VAL(pManagement, E_FAIL);
	pManagement->AddRef();


	m_pObserverCom = (CObserver*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Observer");
	NULL_CHECK_VAL(m_pObserverCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Observer", m_pObserverCom)))
		return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}

void CInventory_Camera::Free()
{
	Safe_Release(m_pObserverCom);

	CCamera::Free();
}