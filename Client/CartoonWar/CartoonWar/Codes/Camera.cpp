#include "framework.h"
#include "Camera.h"
#include "Management.h"

CCamera::CCamera( )
	: m_pInput_Device(CInput::GetInstance())
{
	m_pInput_Device->AddRef();
}

CCamera::CCamera(const CCamera& rhs)
	: CGameObject(rhs)
	, m_tCameraDesc(rhs.m_tCameraDesc)
	, m_tProjDesc(rhs.m_tProjDesc)
	, m_matView(rhs.m_matView)
	, m_matProj(rhs.m_matProj)
	, m_pInput_Device(rhs.m_pInput_Device)
	, m_pTransform(rhs.m_pTransform)
{
	m_pInput_Device->AddRef();
}

HRESULT CCamera::Ready_Prototype()
{
	m_matView = Matrix_::Identity();
	m_matProj = Matrix_:: Identity();
	return S_OK;
}

HRESULT CCamera::Ready_GameObject(void* pArg)
{
	m_pTransform = CTransform::Create();
	if (nullptr == m_pTransform)
		return E_FAIL;


	if (FAILED(Add_Component(L"Com_Transform", m_pTransform)))
		return E_FAIL;
	m_pTransform->AddRef();
	return S_OK;
}

_int CCamera::Update_GameObject(const _float& fTimeDelta)
{
	return _int();
}

_int CCamera::LastUpdate_GameObject(const _float& fTimeDelta)
{
	return _int();
}

void CCamera::Render_GameObject()
{
}

HRESULT CCamera::SetUp_CameraProjDesc(const CAMERADESC& CameraDesc, const PROJDESC& ProjDesc)
{
	m_tCameraDesc = CameraDesc;
	m_tProjDesc = ProjDesc;
	SetUp_ViewProjMatrices();

	return S_OK;
}

void CCamera::Invalidate_ViewProjMatrix()
{

	m_matView = m_pTransform->Get_Matrix_Inverse();
	_matrix matOrtho = XMMatrixOrthographicLH((_float)WINCX, (_float)WINCY, 0.f, 1.f);
	CCamera_Manager::GetInstance()->SetMatView(m_matView);
	CCamera_Manager::GetInstance()->SetMatProj(m_matProj);
	CCamera_Manager::GetInstance()->SetMatOrtho(matOrtho);
}
HRESULT CCamera::SetUp_CameraProjDesc(const CAMERADESC& CameraDesc, const PROJDESC& ProjDesc, _int iTemp)
{
	m_tReflectionCameraDesc = CameraDesc;
	m_tReflectionProjDesc = ProjDesc;
	SetUp_ViewProjMatrices(iTemp);
	return S_OK;
}
void CCamera::Invalidate_ViewProjMatrix(_int iTemp)
{
	m_pTransform->SetUp_RotationX(XMConvertToRadians(90.f));
	m_matReflectionView = m_pTransform->Get_Matrix_Inverse();
	CCamera_Manager::GetInstance()->SetReflectionView(m_matReflectionView);
	CCamera_Manager::GetInstance()->SetReflectionProj(m_matReflectionProj);
}

HRESULT CCamera::SetUp_ViewProjMatrices(_int iTemp)
{
	_vec3		vLook;
	vLook = Vector3_::Subtract(m_tReflectionCameraDesc.vAt, m_tReflectionCameraDesc.vEye);
	vLook = Vector3_::Normalize(vLook);

	_vec3		vRight;
	vRight = Vector3_::CrossProduct(m_tReflectionCameraDesc.vAxisY, vLook, false);
	vRight = Vector3_::Normalize(vRight);

	_vec3		vUp;
	vUp = Vector3_::CrossProduct(vLook, vRight);
	vUp = Vector3_::Normalize(vUp);

	m_pTransform->Set_StateInfo(CTransform::STATE_RIGHT, &vRight);
	m_pTransform->Set_StateInfo(CTransform::STATE_UP, &vUp);
	m_pTransform->Set_StateInfo(CTransform::STATE_LOOK, &vLook);
	m_pTransform->Set_StateInfo(CTransform::STATE_POSITION, (const _vec3*)&m_tReflectionCameraDesc.vEye);


	m_matReflectionProj._11 = (float)(1.f / tan((double)(m_tReflectionProjDesc.fFovY * 0.5f))) / m_tReflectionProjDesc.fAspect;
	m_matReflectionProj._22 = (float)(1.f / tan((double)(m_tReflectionProjDesc.fFovY * 0.5f)));
	m_matReflectionProj._33 = m_tReflectionProjDesc.fFar / (m_tReflectionProjDesc.fFar - m_tReflectionProjDesc.fNear);
	m_matReflectionProj._43 = (m_tReflectionProjDesc.fFar * m_tReflectionProjDesc.fNear) / (m_tReflectionProjDesc.fFar - m_tReflectionProjDesc.fNear) * -1.f;
	m_matReflectionProj._34 = 1.f;
	m_matReflectionProj._44 = 0.0f;

	Invalidate_ViewProjMatrix(iTemp);
	return S_OK;
}


HRESULT CCamera::SetUp_CameraProjDesc(const CAMERADESC& CameraDesc, const PROJDESC& ProjDesc, _bool IsShadow)
{
	m_tShadowCameraDesc = CameraDesc;
	m_tShadowProjDesc = ProjDesc;
	SetUp_ViewProjMatrices(IsShadow);

	return S_OK;
}

HRESULT CCamera::SetUp_ViewProjMatrices(_bool IsShadow)
{
	_vec3		vLook;
	vLook = Vector3_::Subtract(m_tShadowCameraDesc.vAt, m_tShadowCameraDesc.vEye);
	vLook = Vector3_::Normalize(vLook);

	_vec3		vRight;
	vRight = Vector3_::CrossProduct(m_tShadowCameraDesc.vAxisY, vLook, false);
	vRight = Vector3_::Normalize(vRight);

	_vec3		vUp;
	vUp = Vector3_::CrossProduct(vLook, vRight, false);
	vUp = Vector3_::Normalize(vUp);

	m_pTransform->Set_StateInfo(CTransform::STATE_RIGHT, &vRight);
	m_pTransform->Set_StateInfo(CTransform::STATE_UP, &vUp);
	m_pTransform->Set_StateInfo(CTransform::STATE_LOOK, &vLook);
	m_pTransform->Set_StateInfo(CTransform::STATE_POSITION, (const _vec3*)&m_tShadowCameraDesc.vEye);

	CCamera_Manager::GetInstance()->SetShadowMatWorld(m_pTransform->Get_Matrix());

	Invalidate_ViewProjMatrix(IsShadow);
	return S_OK;
}

HRESULT CCamera::Once_SetUp_ViewProjMatrices(_bool IsShadow, _vec3 vPos)
{

	m_tShadowCameraDesc.vEye = vPos;
	m_tShadowCameraDesc.vAt = vPos / 1000.f;
	_vec3		vLook;
	vLook = Vector3_::Subtract(m_tShadowCameraDesc.vAt, m_tShadowCameraDesc.vEye);
	vLook = Vector3_::Normalize(vLook);

	_vec3		vRight;
	vRight = Vector3_::CrossProduct(m_tShadowCameraDesc.vAxisY, vLook, false);
	vRight = Vector3_::Normalize(vRight);

	_vec3		vUp;
	vUp = Vector3_::CrossProduct(vLook, vRight, false);
	vUp = Vector3_::Normalize(vUp);

	m_pTransform->Set_StateInfo(CTransform::STATE_RIGHT, &vRight);
	m_pTransform->Set_StateInfo(CTransform::STATE_UP, &vUp);
	m_pTransform->Set_StateInfo(CTransform::STATE_LOOK, &vLook);
	m_pTransform->Set_StateInfo(CTransform::STATE_POSITION, (const _vec3*)&m_tShadowCameraDesc.vEye);

	return S_OK;
}

void CCamera::Invalidate_ViewProjMatrix(_bool IsShadow)
{
	_vec3 vPos = *dynamic_cast<CTransform*>
		(CManagement::GetInstance()->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE, L"Layer_Player", L"Com_Transform"))->Get_StateInfo(CTransform::STATE_POSITION);

	vPos.y += 1000.f;
	vPos.x += -1000.f;
	vPos.z += -1000.f;
	m_pTransform->Set_StateInfo(CTransform::STATE_POSITION, &vPos);

	m_matShadowView = m_pTransform->Get_Matrix_Inverse();
	m_matShadowProj = XMMatrixOrthographicLH((_float)150.f, (_float)150.f, 1.f, 100000.f);

	CCamera_Manager::GetInstance()->SetShadowMatView(m_matShadowView);
	CCamera_Manager::GetInstance()->SetShadowMatProj(m_matShadowProj);
}


HRESULT CCamera::SetUp_ViewProjMatrices()
{
	_vec3		vLook;
	vLook = Vector3_::Subtract(m_tCameraDesc.vAt, m_tCameraDesc.vEye);
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
	m_pTransform->Set_StateInfo(CTransform::STATE_POSITION, (const _vec3*)&m_tCameraDesc.vEye);


	m_matProj._11 = (float)(1.f / tan((double)(m_tProjDesc.fFovY * 0.5f))) / m_tProjDesc.fAspect;
	m_matProj._22 = (float)(1.f / tan((double)(m_tProjDesc.fFovY * 0.5f)));
	m_matProj._33 = m_tProjDesc.fFar / (m_tProjDesc.fFar - m_tProjDesc.fNear);
	m_matProj._43 = (m_tProjDesc.fFar * m_tProjDesc.fNear) / (m_tProjDesc.fFar - m_tProjDesc.fNear) * -1.f;
	m_matProj._34 = 1.f;
	m_matProj._44 = 0.0f;

	Invalidate_ViewProjMatrix();
	return S_OK;
}

void CCamera::Free()
{
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pTransform);

	CGameObject::Free();
}
