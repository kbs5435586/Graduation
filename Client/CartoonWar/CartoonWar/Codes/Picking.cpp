#include "framework.h"
#include "Picking.h"
#include "Component.h"


CPicking::CPicking()
	: CComponent()
{

}

CPicking::CPicking(const CPicking& rhs)
	: CComponent(rhs)
	, m_hWnd(rhs.m_hWnd)
{

}


HRESULT CPicking::Ready_Picking(HWND hWnd)
{
	m_hWnd = hWnd;

	return NOERROR;
}

// 마우스 좌표를 구하고, 변환. 
void CPicking::Update_Ray()
{
	//D3DVIEWPORT9		ViewPort;
	//m_pGraphic_Device->GetViewport(&ViewPort);

	POINT		ptMouse;
	GetCursorPos(&ptMouse);

	// ViewPort영역상의 마우스 좌표
	ScreenToClient(m_hWnd, &ptMouse);

	// 투영스페이스 영역상의 마우스 좌표를 만들자.
	_vec4	vMousePos;
	vMousePos.x = ptMouse.x / (WINCX >> 1) - 1.f;
	vMousePos.y = ptMouse.y / ((WINCY >> 1) * -1.f) + 1.f;
	vMousePos.z = 0.0f;
	vMousePos.w = 1.f;

	// 뷰스페이스 영역상의 마우스 좌표.
	_matrix	matProj;
	//m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &matProj);
	matProj = CCamera_Manager::GetInstance()->GetMatProj();
	matProj = Matrix_::Inverse(matProj);
	vMousePos = _vec4::Transform(vMousePos, matProj);
	//D3DXMatrixInverse(&matProj, nullptr, &matProj);
	//D3DXVec4Transform(&vMousePos, &vMousePos, &matProj);


	// 마우스 레이의 시작점.
	_vec3		vCameraPos(0.f, 0.f, 0.f);

	// 마우스 레이.
	_vec3 vTemp = _vec3(vMousePos.x, vMousePos.y, vMousePos.z);
	_vec3		vMouseRay = Vector3_::Subtract(vTemp, vCameraPos);

	// 월드스페이스 영역상의 레이와 시작점.
	_matrix	matView;
	matView = CCamera_Manager::GetInstance()->GetMatView();
	matView = Matrix_::Inverse(matView);
	m_vRayPivot = _vec3::Transform(vCameraPos, matView);
	m_vRay = _vec3::TransformNormal(vMouseRay, matView);
}

CPicking* CPicking::Create(HWND hWnd)
{
	CPicking* pInstance = new CPicking();

	if (FAILED(pInstance->Ready_Picking(hWnd)))
	{
		MessageBox(0, L"CPicking Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;

}

CComponent* CPicking::Clone_Component(void* pArg)
{
	return new CPicking(*this);
}


void CPicking::Free()
{
	CComponent::Free();
}
