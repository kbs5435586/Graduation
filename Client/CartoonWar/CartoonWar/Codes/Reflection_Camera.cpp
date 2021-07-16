#include "framework.h"
#include "Management.h"
#include "Reflection_Camera.h"

CReflection_Camera::CReflection_Camera()
	: CCamera()
{
}

CReflection_Camera::CReflection_Camera(const CReflection_Camera& rhs)
	: CCamera(rhs)

{
}

HRESULT CReflection_Camera::Ready_Prototype()
{
	if (FAILED(CCamera::Ready_Prototype()))
		return E_FAIL;

	return NOERROR;
}

HRESULT CReflection_Camera::Ready_GameObject(void* pArg)
{
	if (FAILED(CCamera::Ready_GameObject()))
		return E_FAIL;
	return NOERROR;
}

_int CReflection_Camera::Update_GameObject(const _float& fTimeDelta)
{
	_vec3 vTemp = *dynamic_cast<CCamera*>(CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_Camera").front())->GetTransform()->Get_StateInfo(CTransform::STATE_POSITION);
	vTemp.y = -vTemp.y + (-0.f * 2);;
	//vTemp.y = -vTemp.y +(20.f*2);
	m_pTransform->Set_StateInfo(CTransform::STATE_POSITION, &vTemp);
	
	return _int();
}

_int CReflection_Camera::LastUpdate_GameObject(const _float& fTimeDelta)
{
	Invalidate_ViewProjMatrix(1);


	return _int();
}

void CReflection_Camera::Render_GameObject()
{
}

CReflection_Camera* CReflection_Camera::Create()
{
	CReflection_Camera* pInstance = new CReflection_Camera();

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MessageBox(0, L"CCamera_Debug Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CReflection_Camera::Clone_GameObject(void* pArg, _uint iIdx)
{
	CReflection_Camera* pInstance = new CReflection_Camera(*this);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CReflection_Camera Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	m_iLayerIdx = iIdx;
	return pInstance;
}

void CReflection_Camera::Free()
{
	CCamera::Free();
}

