#include "framework.h"
#include "Management.h"
#include "Inventory_Camera.h"
#include "Server_Manager.h"
#include "UI_ClassTap.h"
#include "Player.h"
#include "NPC.h"

CInventory_Camera::CInventory_Camera()
{
}

CInventory_Camera::CInventory_Camera(const CInventory_Camera& rhs)
{
}

HRESULT CInventory_Camera::Ready_Prototype()
{
	if (FAILED(CCamera::Ready_Prototype()))
		return E_FAIL;

	return NOERROR;
}

HRESULT CInventory_Camera::Ready_GameObject(void* pArg)
{
	if (FAILED(CInventory_Camera::Ready_Component()))
		return E_FAIL;
	if (FAILED(CCamera::Ready_GameObject()))
		return E_FAIL;

	m_pTransform->SetUp_Speed(100.f, XMConvertToRadians(90.f));

	m_ptMouse.x = static_cast<LONG>(WINCX) / 2;
	m_ptMouse.y = static_cast<LONG>(WINCY) / 2;
	ClientToScreen(g_hWnd, &m_ptMouse);


	//CManagement::GetInstance()->Subscribe(m_pObserverCom);

	return NOERROR;
}

_int CInventory_Camera::Update_GameObject(const _float& fTimeDelta)
{
	//SetCursorPos(m_ptMouse.x, m_ptMouse.y);

	/*CServer_Manager* server = CServer_Manager::GetInstance();
	if (nullptr == server)
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
	//if (m_pInput_Device->Get_DIKeyState(DIK_I) & 0x80)
	//{
	//	canSee = !canSee;
	//}

	
	//m_tCameraDesc.vAt = m_pObserverCom->GetVec3Info();


	CGameObject* UI = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_UI", TAPIDX);
	_int which = dynamic_cast<CUI_ClassTap*>(UI)->GetWhich();
	if (which == 0)
	{
		CGameObject* pTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_Player", which);
		m_tCameraDesc.vAt = *dynamic_cast<CTransform*>(pTemp->Get_ComponentPointer(L"Com_Transform"))->Get_StateInfo(CTransform::STATE_POSITION);
		//m_tCameraDesc.vAt = dynamic_cast<CPlayer*>(pTemp).get;
	}
	else
	{
		CGameObject* pTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_NPC", which - 1);
		m_tCameraDesc.vAt = *dynamic_cast<CTransform*>(pTemp->Get_ComponentPointer(L"Com_Transform"))->Get_StateInfo(CTransform::STATE_POSITION);
	}


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

	{
		//{
	//	CTransform* pTransform = (CTransform*)CManagement::GetInstance()->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
	//		L"Layer_Orc02", L"Com_Transform", 0);


	//	_vec3 vPos, vUp, vLook;
	//	_vec3 vTempUp, vTempLook, vTempRight;


	//	vPos = *pTransform->Get_StateInfo(CTransform::STATE_POSITION);
	//	vUp = *pTransform->Get_StateInfo(CTransform::STATE_UP);
	//	vLook = *pTransform->Get_StateInfo(CTransform::STATE_LOOK);

	//	vPos.y += 1.f;
	//	vUp = vUp * 300.f;
	//	vLook = vLook * 300.f;

	//	_vec3 vTemp = vUp - vLook;
	//	vPos = vPos + vTemp;
	//	_long	MouseMove = 0;

	//	{

	//		m_pTransform->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
	//		if (MouseMove = m_pInput_Device->Get_DIMouseMove(CInput::DIM_X))
	//		{
	//			pTransform->Rotation_Y(MouseMove * fTimeDelta * 0.5f);
	//		}
	//		if (MouseMove = m_pInput_Device->Get_DIMouseMove(CInput::DIM_X))
	//		{
	//			m_pTransform->Rotation_Y(MouseMove * fTimeDelta * 0.5f);
	//		}


	//		if (MouseMove = CInput::GetInstance()->Get_DIMouseMove(CInput::DIM_Y))
	//		{
	//			m_pTransform->Rotation_Axis(XMConvertToRadians((_float)MouseMove) * -fTimeDelta * 30.f, m_pTransform->Get_StateInfo(CTransform::STATE_RIGHT));
	//		}

	//	}
	//}



	//Safe_Release(server);
	}

	return _int();
}

_int CInventory_Camera::LastUpdate_GameObject(const _float& fTimeDelta)
{
	Invalidate_ViewProjMatrix(1);

	if (m_IsFix)
	{
		/*CTransform* pTransform = (CTransform*)CManagement::GetInstance()->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
			L"Layer_Orc02", L"Com_Transform", 0);

		_vec3 vUp, vPos, vLook;

		vUp = *m_pTransform->Get_StateInfo(CTransform::STATE::STATE_UP);
		vLook = *m_pTransform->Get_StateInfo(CTransform::STATE::STATE_LOOK);
		vPos = *pTransform->Get_StateInfo(CTransform::STATE::STATE_POSITION);

		vPos.y += 0.3f;

		if (vPos.y <= 0.f)
			vPos.y = 0.f;
		vUp = Vector3_::ScalarProduct(vUp, 5.f);
		vLook = Vector3_::ScalarProduct(vLook, 15.f);
		_vec3 vTemp = Vector3_::Subtract(vUp, vLook);
		vPos = Vector3_::Add(vPos, vTemp);
		m_pTransform->Set_StateInfo(CTransform::STATE::STATE_POSITION, &vPos);*/
	}

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
		MessageBox(0, L"CCamera_Inventory Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CInventory_Camera::Clone_GameObject(void* pArg, _uint iIdx)
{
	CInventory_Camera* pInstance = new CInventory_Camera(*this);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CCamera_Inventory Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	m_iLayerIdx = iIdx;
	return pInstance;
}

HRESULT CInventory_Camera::Ready_Component()
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

void CInventory_Camera::Free()
{
	Safe_Release(m_pNaviCom);
	CCamera::Free();
}
