#include "framework.h"
#include "Management.h"
#include "Light_Camera.h"

CLight_Camera::CLight_Camera()
	: CCamera()
{
}

CLight_Camera::CLight_Camera(const CLight_Camera& rhs)
	: CCamera(rhs)

{
}

HRESULT CLight_Camera::Ready_Prototype()
{
	if (FAILED(CCamera::Ready_Prototype()))
		return E_FAIL;

	return NOERROR;
}

HRESULT CLight_Camera::Ready_GameObject(void* pArg)
{
	if (FAILED(CCamera::Ready_GameObject()))
		return E_FAIL;

	m_pTransform->SetUp_Speed(30.f, XMConvertToRadians(90.f));


	return NOERROR;
}

_int CLight_Camera::Update_GameObject(const _float& fTimeDelta)
{

	return _int();
}

_int CLight_Camera::LastUpdate_GameObject(const _float& fTimeDelta)
{
	Invalidate_ViewProjMatrix(true);


	return _int();
}

void CLight_Camera::Render_GameObject()
{
}

CLight_Camera* CLight_Camera::Create()
{
	CLight_Camera* pInstance = new CLight_Camera();

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MessageBox(0, L"CCamera_Debug Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CLight_Camera::Clone_GameObject(void* pArg, _uint iIdx)
{
	CLight_Camera* pInstance = new CLight_Camera(*this);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CLight_Camera Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	m_iLayerIdx = iIdx;
	return pInstance;
}

void CLight_Camera::Free()
{
	CCamera::Free();
}
