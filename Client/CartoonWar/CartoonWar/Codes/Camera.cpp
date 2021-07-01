#include "framework.h"
#include "Camera.h"

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
	m_matView = _matrix();
	m_matProj = _matrix();
	return S_OK;
}

HRESULT CCamera::Ready_GameObject(void* pArg)
{
	m_pTransform = CTransform::Create();
	if (nullptr == m_pTransform)
		return E_FAIL;


	if (FAILED(Add_Component(L"Com_Transform", m_pTransform)))
		return E_FAIL;
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

void CCamera::ShadowMatView()
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
	vLook = m_tShadowCameraDesc.vAt - m_tShadowCameraDesc.vEye;
	vLook.Normalize();

	_vec3		vRight;
	vRight = m_tShadowCameraDesc.vAxisY.Cross(vLook);
	vRight.Normalize();

	_vec3		vUp;
	vUp = vLook.Cross(vRight);
	vUp.Normalize();

	m_pTransform->Set_StateInfo(CTransform::STATE_RIGHT, &vRight);
	m_pTransform->Set_StateInfo(CTransform::STATE_UP, &vUp);
	m_pTransform->Set_StateInfo(CTransform::STATE_LOOK, &vLook);
	m_pTransform->Set_StateInfo(CTransform::STATE_POSITION, (const _vec3*)&m_tShadowCameraDesc.vEye);
	//m_pTransform->Scaling(2.f, 2.f, 2.f);

	CCamera_Manager::GetInstance()->SetShadowMatWorld(m_pTransform->Get_Matrix());

	m_matShadowProj._11 = (float)(1.f / tan((double)(m_tShadowProjDesc.fFovY * 0.5f))) / m_tShadowProjDesc.fAspect;
	m_matShadowProj._22 = (float)(1.f / tan((double)(m_tShadowProjDesc.fFovY * 0.5f)));
	m_matShadowProj._33 = m_tShadowProjDesc.fFar / (m_tShadowProjDesc.fFar - m_tShadowProjDesc.fNear);
	m_matShadowProj._43 = (m_tShadowProjDesc.fFar * m_tShadowProjDesc.fNear) / (m_tShadowProjDesc.fFar - m_tShadowProjDesc.fNear) * -1.f;
	m_matShadowProj._34 = 1.f;
	m_matShadowProj._44 = 0.0f;


	Invalidate_ViewProjMatrix(IsShadow);
	return S_OK;
}

void CCamera::Invalidate_ViewProjMatrix(_bool IsShadow)
{
	//m_matShadowView = m_pTransform->Get_Matrix_Inverse();
	//m_matShadowProj = XMMatrixOrthographicLH((_float)600.f, (_float)600.f, 1.f, 100000.f);
	//m_matShadowProj = XMMatrixOrthographicLH((_float)512, (_float)512, 1.f, 100000.f);

	//D3DXVECTOR3 g_vEyePt(30.f, 100.0f, -20.0f);
	//D3DXVECTOR3 g_vLookatPt(0.0f, 0.f, 0.f);

	_vec3 g_vEyePt(30.f, 100.0f, -20.0f);
	_vec3 g_vLookatPt(0.0f, 0.f, 0.f);

	m_matShadowView = XMMatrixLookAtLH(g_vEyePt, g_vLookatPt, _vec3(0.f,1.f,0.f));
	m_matShadowProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), _float(WINCX) / WINCY, 0.2f, 500.0f);
	CCamera_Manager::GetInstance()->SetShadowMatView(m_matShadowView);
	CCamera_Manager::GetInstance()->SetShadowMatProj(m_matShadowProj);
}


HRESULT CCamera::SetUp_ViewProjMatrices()
{
	_vec3		vLook;
	vLook = m_tShadowCameraDesc.vAt - m_tShadowCameraDesc.vEye;
	vLook.Normalize();

	_vec3		vRight;
	vRight = m_tShadowCameraDesc.vAxisY.Cross(vLook);
	vRight.Normalize();

	_vec3		vUp;
	vUp = vLook.Cross(vRight);
	vUp.Normalize();

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
