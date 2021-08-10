#include "framework.h"
#include "Management.h"
#include "Debug_Camera.h"
#include "Server_Manager.h"
#include "Player.h"

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

	m_pTransform->SetUp_Speed(300.f, XMConvertToRadians(90.f));

	m_ptMouse.x = static_cast<LONG>(WINCX) / 2;
	m_ptMouse.y = static_cast<LONG>(WINCY) / 2;
	ClientToScreen(g_hWnd, &m_ptMouse);




	return NOERROR;
}

_int CDebug_Camera::Update_GameObject(const _float& fTimeDelta)
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

	if (nullptr == m_pInput_Device)
		return -1;

	if (CManagement::GetInstance()->Key_Down(KEY_V))
	{
		m_IsFix ^= true;
	}

	{
		/*	if (m_pInput_Device->Get_DIKeyState(DIK_W) & 0x80)
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
			}*/
	}

	{

		CTransform* pTransform = (CTransform*)CManagement::GetInstance()->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
			L"Layer_Player", L"Com_Transform", g_iPlayerIdx);

		CGameObject* pGameObject = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_Player", g_iPlayerIdx);


		_vec3 vPos, vRight, vUp, vLook;
		vPos = *pTransform->Get_StateInfo(CTransform::STATE_POSITION);
		vRight = {};
		vUp = *pTransform->Get_StateInfo(CTransform::STATE_UP);
		vLook = *pTransform->Get_StateInfo(CTransform::STATE_LOOK);

		if (m_IsFix)
		{
			if (dynamic_cast<CPlayer*>(pGameObject)->GetClass() == CLASS::CLASS_WORKER ||
				dynamic_cast<CPlayer*>(pGameObject)->GetClass() == CLASS::CLASS_ARCHER ||
				dynamic_cast<CPlayer*>(pGameObject)->GetClass() == CLASS::CLASS_INFANTRY ||
				dynamic_cast<CPlayer*>(pGameObject)->GetClass() == CLASS(4) ||
				dynamic_cast<CPlayer*>(pGameObject)->GetClass() == CLASS::CLASS_SPEARMAN ||
				dynamic_cast<CPlayer*>(pGameObject)->GetClass() == CLASS::CLASS_MAGE)
			{
				vPos.y += 5.f;
				vUp *= 20.f;
				vLook *= -5.f;
			}
			else
			{
				vPos.y += 5.f;
				vUp *= 49.f;
				vLook *= -2.f;
			}
		}
		else
		{
			vPos.y += 5.f;
			vUp *= 100.f;
			vLook *= -250.f;
		}


		_vec3 vTemp = vUp - vLook;
		vPos = vPos + vTemp;
		vRight = Vector3_::CrossProduct(vUp, vLook);
		vUp = Vector3_::Normalize(vUp);
		vLook = Vector3_::Normalize(vLook);

		m_pTransform->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
		m_pTransform->Set_StateInfo(CTransform::STATE_RIGHT, &vRight);
		m_pTransform->Set_StateInfo(CTransform::STATE_UP, &vUp);
		m_pTransform->Set_StateInfo(CTransform::STATE_LOOK, &vLook);
	}



	if (CManagement::GetInstance()->Key_Pressing(KEY_RBUTTON))
	{
		m_tProjDesc.fFovY = XMConvertToRadians(30.f);
		m_tProjDesc.fAspect = _float(WINCX) / WINCY;
		m_tProjDesc.fNear = g_Near;
		m_tProjDesc.fFar = 600.f;


		m_matProj._11 = (float)(1.f / tan((double)(m_tProjDesc.fFovY * 0.5f))) / m_tProjDesc.fAspect;
		m_matProj._22 = (float)(1.f / tan((double)(m_tProjDesc.fFovY * 0.5f)));
		m_matProj._33 = m_tProjDesc.fFar / (m_tProjDesc.fFar - m_tProjDesc.fNear);
		m_matProj._43 = (m_tProjDesc.fFar * m_tProjDesc.fNear) / (m_tProjDesc.fFar - m_tProjDesc.fNear) * -1.f;
		m_matProj._34 = 1.f;
		m_matProj._44 = 0.0f;

	}
	if (CManagement::GetInstance()->Key_Up(KEY_RBUTTON))
	{
		m_tProjDesc.fFovY = XMConvertToRadians(60.f);
		m_tProjDesc.fAspect = _float(WINCX) / WINCY;
		m_tProjDesc.fNear = g_Near;
		m_tProjDesc.fFar = 600.f;


		m_matProj._11 = (float)(1.f / tan((double)(m_tProjDesc.fFovY * 0.5f))) / m_tProjDesc.fAspect;
		m_matProj._22 = (float)(1.f / tan((double)(m_tProjDesc.fFovY * 0.5f)));
		m_matProj._33 = m_tProjDesc.fFar / (m_tProjDesc.fFar - m_tProjDesc.fNear);
		m_matProj._43 = (m_tProjDesc.fFar * m_tProjDesc.fNear) / (m_tProjDesc.fFar - m_tProjDesc.fNear) * -1.f;
		m_matProj._34 = 1.f;
		m_matProj._44 = 0.0f;
	}



	CBuffer_Terrain_Height* pTerrainBuffer = (CBuffer_Terrain_Height*)CManagement::GetInstance()->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE, L"Layer_Terrain", L"Com_Buffer");
	if (nullptr == pTerrainBuffer)
		return NO_EVENT;

	//_float		fY = pTerrainBuffer->Compute_HeightOnTerrain(m_pTransform);
	//_float		fThisY = m_pTransform->Get_Matrix()._42;
	//if (fThisY <= fY)
	//{
	//	CTransform* pTransform = (CTransform*)CManagement::GetInstance()->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
	//		L"Layer_Player", L"Com_Transform", 0);
	//	CGameObject* pGameObject = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_Player", g_iPlayerIdx);


	//	_vec3 vPos, vRight, vUp, vLook;
	//	vPos = *pTransform->Get_StateInfo(CTransform::STATE_POSITION);
	//	vRight = {};
	//	vUp = *pTransform->Get_StateInfo(CTransform::STATE_UP);
	//	vLook = *pTransform->Get_StateInfo(CTransform::STATE_LOOK);

	//		if (m_IsFix)
	//	{
	//		if (dynamic_cast<CPlayer*>(pGameObject)->GetClass() == CLASS::CLASS_WORKER ||
	//			dynamic_cast<CPlayer*>(pGameObject)->GetClass() == CLASS::CLASS_ARCHER ||
	//			dynamic_cast<CPlayer*>(pGameObject)->GetClass() == CLASS::CLASS_INFANTRY ||
	//			dynamic_cast<CPlayer*>(pGameObject)->GetClass() == CLASS(4) ||
	//			dynamic_cast<CPlayer*>(pGameObject)->GetClass() == CLASS::CLASS_SPEARMAN ||
	//			dynamic_cast<CPlayer*>(pGameObject)->GetClass() == CLASS::CLASS_MAGE)
	//		{
	//			vPos.y += 5.f;
	//			vUp *= 20.f;
	//			vLook *= -5.f;
	//		}
	//		else
	//		{
	//			vPos.y += 5.f;
	//			vUp *= 40.f;
	//			vLook *= -5.f;
	//		}

	//	}
	//	else
	//	{
	//		vPos.y += 5.f;
	//		vUp *= 50.f;
	//		vLook *= -20.f;
	//	}
	//	_vec3 vTemp = vUp - vLook;
	//	vPos = vPos + vTemp;
	//	vRight = Vector3_::CrossProduct(vUp, vLook);
	//	vUp = Vector3_::Normalize(vUp);
	//	vLook = Vector3_::Normalize(vLook);


	//	_long	MouseMove = 0;
	//	m_pTransform->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
	//	m_pTransform->Set_StateInfo(CTransform::STATE_RIGHT, &vRight);
	//	m_pTransform->Set_StateInfo(CTransform::STATE_UP, &vUp);
	//	m_pTransform->Set_StateInfo(CTransform::STATE_LOOK, &vLook);
	//}



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

CGameObject* CDebug_Camera::Clone_GameObject(void* pArg, _uint iIdx)
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
