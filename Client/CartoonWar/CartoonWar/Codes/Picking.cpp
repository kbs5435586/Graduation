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

// ���콺 ��ǥ�� ���ϰ�, ��ȯ. 
void CPicking::Update_Ray()
{
	//D3DVIEWPORT9		ViewPort;
	//m_pGraphic_Device->GetViewport(&ViewPort);

	POINT		ptMouse;
	GetCursorPos(&ptMouse);

	// ViewPort�������� ���콺 ��ǥ
	ScreenToClient(m_hWnd, &ptMouse);

	// ���������̽� �������� ���콺 ��ǥ�� ������.
	_vec4	vMousePos;
	vMousePos.x = ptMouse.x / (WINCX >> 1) - 1.f;
	vMousePos.y = ptMouse.y / ((WINCY >> 1) * -1.f) + 1.f;
	vMousePos.z = 0.0f;
	vMousePos.w = 1.f;

	// �佺���̽� �������� ���콺 ��ǥ.
	_matrix	matProj;
	//m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &matProj);
	matProj = CCamera_Manager::GetInstance()->GetMatProj();
	matProj = Matrix_::Inverse(matProj);
	vMousePos = _vec4::Transform(vMousePos, matProj);
	//D3DXMatrixInverse(&matProj, nullptr, &matProj);
	//D3DXVec4Transform(&vMousePos, &vMousePos, &matProj);


	// ���콺 ������ ������.
	_vec3		vCameraPos(0.f, 0.f, 0.f);

	// ���콺 ����.
	_vec3 vTemp = _vec3(vMousePos.x, vMousePos.y, vMousePos.z);
	_vec3		vMouseRay = Vector3_::Subtract(vTemp, vCameraPos);

	// ���彺���̽� �������� ���̿� ������.
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
