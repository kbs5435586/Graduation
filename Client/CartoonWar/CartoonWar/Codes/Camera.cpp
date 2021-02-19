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

	m_pTransform_Reflect = CTransform::Create();
	if (nullptr == m_pTransform_Reflect)
		return E_FAIL;


	if (FAILED(Add_Component(L"Com_Transform", m_pTransform)))
		return E_FAIL;
	if (FAILED(Add_Component(L"Com_Transform_Reflect", m_pTransform_Reflect)))
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

_matrix CCamera::Calculate_RelfectMatrix(const _float& fHeight)
{
	_vec3 up, position, lookAt, right;

	// 위쪽을 가리키는 벡터를 설정합니다.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// XMVECTOR 구조체에 로드한다.
	// 3D월드에서 카메라의 위치를 ​​설정합니다.
	position.x = m_pTransform->Get_StateInfo(CTransform::STATE_POSITION)->x;
	position.y = -m_pTransform->Get_StateInfo(CTransform::STATE_POSITION)->y + (fHeight * 2.0f);
	position.z = m_pTransform->Get_StateInfo(CTransform::STATE_POSITION)->z;

	// XMVECTOR 구조체에 로드한다.

	// Calculate the rotation in radians.

	// 기본적으로 카메라가 찾고있는 위치를 설정합니다.
	lookAt.x = position.x;
	lookAt.y = position.y;
	lookAt.z = position.z;


	right = Vector3_::CrossProduct(up, lookAt);
	right = Vector3_::Normalize(right);

	m_pTransform_Reflect->Set_StateInfo(CTransform::STATE_RIGHT, &right);
	m_pTransform_Reflect->Set_StateInfo(CTransform::STATE_UP, &up);
	m_pTransform_Reflect->Set_StateInfo(CTransform::STATE_LOOK, &lookAt);
	m_pTransform_Reflect->Set_StateInfo(CTransform::STATE_POSITION, &position);

	m_matReflect = m_pTransform_Reflect->Get_Matrix_Inverse();

	// 마지막으로 세 개의 업데이트 된 벡터에서 뷰 행렬을 만듭니다.
	//m_matReflect = Matrix_::LookAtLH(position, lookAt, up);

	return m_matReflect;
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
	Safe_Release(m_pTransform_Reflect);
	CGameObject::Free();
}
