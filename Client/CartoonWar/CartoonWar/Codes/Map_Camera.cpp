#include "framework.h"
#include "Management.h"
#include "Map_Camera.h"
#include "Server_Manager.h"
#include "Player.h"
#include "UI_ClassTap.h"

CMap_Camera::CMap_Camera()
	: CCamera()
{
}

CMap_Camera::CMap_Camera(const CMap_Camera& rhs)
	: CCamera(rhs)

{
}

HRESULT CMap_Camera::Ready_Prototype()
{
	if (FAILED(CCamera::Ready_Prototype()))
		return E_FAIL;

	return NOERROR;
}

HRESULT CMap_Camera::Ready_GameObject(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;
	if (FAILED(CCamera::Ready_GameObject()))
		return E_FAIL;


	_vec3 vPos = { 130.f,100.f,300.f };
	m_pTransform->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
	m_pTransform->SetUp_Speed(10.f, XMConvertToRadians(90.f));
	//100
	m_ptMouse.x = static_cast<LONG>(WINCX) / 2;
	m_ptMouse.y = static_cast<LONG>(WINCY) / 2;
	ClientToScreen(g_hWnd, &m_ptMouse);



	return NOERROR;
}

_int CMap_Camera::Update_GameObject(const _float& fTimeDelta)
{

	if (nullptr == m_pInput_Device)
		return -1;
	
	
	{
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
		//	m_pTransform->Go_Left(fTimeDelta);
		//}
		//if (m_pInput_Device->Get_DIKeyState(DIK_D) & 0x80)
		//{
		//	m_pTransform->Go_Right(fTimeDelta);
		//}
		//_long	MouseMove = 0;
		//if (MouseMove = m_pInput_Device->Get_DIMouseMove(CInput::DIM_X))
		//{
		//	m_pTransform->Rotation_Y(MouseMove * fTimeDelta * 0.5f);
		//}
		//if (MouseMove = CInput::GetInstance()->Get_DIMouseMove(CInput::DIM_Y))
		//{
		//	m_pTransform->Rotation_Axis(XMConvertToRadians((_float)MouseMove) * -fTimeDelta * 30.f, m_pTransform->Get_StateInfo(CTransform::STATE_RIGHT));
		//}
	}
	
	{
	
		CTransform* pTransform = (CTransform*)CManagement::GetInstance()->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
			L"Layer_Player", L"Com_Transform", 0);
	
		CGameObject* pGameObject = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_Player", 0);
	
	
		_vec3 vPos, vRight, vUp, vLook;
		vPos = _vec3(pTransform->Get_StateInfo(CTransform::STATE_POSITION)->x, 170.f, pTransform->Get_StateInfo(CTransform::STATE_POSITION)->z);
		vRight = _vec3(0.1f,0.f,0.f);
		//vUp = *pTransform->Get_StateInfo(CTransform::STATE_UP);
		vUp = _vec3(0.f, 0.f, 0.1f);
		vLook = _vec3(0.f, -0.1f, 0.f);
	
		
		//_vec3 vTemp = vUp - vLook;
		//vPos = vPos + vTemp;
		//vRight = Vector3_::CrossProduct(vUp, vLook);
		//vUp = Vector3_::Normalize(vUp);
		//vLook = Vector3_::Normalize(vLook);
	
		m_pTransform->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
		m_pTransform->Set_StateInfo(CTransform::STATE_RIGHT, &vRight);
		m_pTransform->Set_StateInfo(CTransform::STATE_UP, &vUp);
		m_pTransform->Set_StateInfo(CTransform::STATE_LOOK, &vLook);
	}


	return _int();
}

_int CMap_Camera::LastUpdate_GameObject(const _float& fTimeDelta)
{
	Invalidate_ViewProjMatrix((_short)1);

	return _int();
}

void CMap_Camera::Render_GameObject()
{
}

HRESULT CMap_Camera::Ready_Component()
{
	CManagement* pManagement = CManagement::GetInstance();
	NULL_CHECK_VAL(pManagement, E_FAIL);
	pManagement->AddRef();


	Safe_Release(pManagement);
	return S_OK;
}

CMap_Camera* CMap_Camera::Create()
{
	CMap_Camera* pInstance = new CMap_Camera();

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MessageBox(0, L"CCamera_Map Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMap_Camera::Clone_GameObject(void* pArg, _uint iIdx)
{
	CMap_Camera* pInstance = new CMap_Camera(*this);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CCamera_Map Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	m_iLayerIdx = iIdx;
	return pInstance;
}

void CMap_Camera::Free()
{
	CCamera::Free();
}
