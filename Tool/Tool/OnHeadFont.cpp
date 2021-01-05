#include "stdafx.h"
#include "OnHeadFont.h"
#include "Management.h"


COnHeadFont::COnHeadFont(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

COnHeadFont::COnHeadFont(const COnHeadFont & rhs)
	: CGameObject(rhs)
{
}

HRESULT COnHeadFont::Ready_Prototype()
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	return S_OK;
}

HRESULT COnHeadFont::Ready_GameObject(void * pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;
	_vec3 vScale = { 1.f, 1.f ,1.f};
	m_pTransformCom->Scaling(vScale.x, vScale.y, vScale.z);
	return S_OK;
}

_int COnHeadFont::Update_GameObject(const _float & fTimeDelta)
{
	return _int();
}

_int COnHeadFont::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	_matrix		matView;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &matView);

	D3DXMatrixInverse(&matView, nullptr, &matView);

	_vec3		vRight, vUp, vLook;

	vRight = *(_vec3*)&matView.m[0][0] * m_pTransformCom->Get_Scale().x;
	vUp = *(_vec3*)&matView.m[1][0] * m_pTransformCom->Get_Scale().y;
	vLook = *(_vec3*)&matView.m[2][0] * m_pTransformCom->Get_Scale().z;

	m_pTransformCom->Set_StateInfo(STATE_RIGHT, &vRight);
	m_pTransformCom->Set_StateInfo(STATE_LOOK, &vLook);


	m_pRendererCom->Add_RenderGroup(RENDER_UI, this);
	return _int();
}

_int COnHeadFont::LastUpdate_GameObject(const _float & fTimeDelta, _bool bCheck)
{
	if (nullptr == m_pRendererCom)
		return -1;

	_matrix		matView;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &matView);

	D3DXMatrixInverse(&matView, nullptr, &matView);

	_vec3		vRight, vUp, vLook;

	vRight = *(_vec3*)&matView.m[0][0] * m_pTransformCom->Get_Scale().x;
	vUp = *(_vec3*)&matView.m[1][0] * m_pTransformCom->Get_Scale().y;
	vLook = *(_vec3*)&matView.m[2][0] * m_pTransformCom->Get_Scale().z;

	m_pTransformCom->Set_StateInfo(STATE_RIGHT, &vRight);
	m_pTransformCom->Set_StateInfo(STATE_LOOK, &vLook);


	if(bCheck)
		m_pRendererCom->Add_RenderGroup(RENDER_UI, this);

	return _int();
}

void COnHeadFont::Render_GameObject()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

 	_vec3 ab = *m_pTransformCom->Get_StateInfo(STATE_POSITION);
 	//ab.x -= 3.f;
	ab.y += 3.f;
 	m_pTransformCom->Set_StateInfo(STATE_POSITION, &ab);

	m_pTransformCom->SetUp_OnGraphicDev();

	if (FAILED(m_pFontCom->SetUp_3DFont("Picking", m_pTransformCom, 255, 255, 255)))
		return;

	Safe_Release(pManagement);
}

COnHeadFont * COnHeadFont::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	COnHeadFont*	pInstance = new COnHeadFont(pGraphic_Device);
	if (FAILED(pInstance->Ready_Prototype()))
		return nullptr;
	return pInstance;
}

CGameObject * COnHeadFont::Clone_GameObject(void * pArg)
{
	COnHeadFont*	pInstance = new COnHeadFont(*this);
	if (FAILED(pInstance->Ready_GameObject(pArg)))
		return nullptr;
	return pInstance;
}

void COnHeadFont::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pFontCom);
	CGameObject::Free();
}

HRESULT COnHeadFont::Ready_Component()
{
	CManagement*		pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return E_FAIL;
	pManagement->AddRef();


	// For.Com_Transform
	m_pTransformCom = (CTransform*)pManagement->Clone_Component(SCENE_STATIC, L"Component_Transform");
	if (FAILED(Add_Component(L"Com_Transform", m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	m_pRendererCom = (CRenderer*)pManagement->Clone_Component(SCENE_STATIC, L"Component_Renderer");
	if (FAILED(Add_Component(L"Com_Renderer", m_pRendererCom)))
		return E_FAIL;

	m_pFontCom = (CMyFont*)pManagement->Clone_Component(SCENE_LOGO, L"Component_Font");
	if (FAILED(Add_Component(L"Com_Font", m_pFontCom)))
		return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}

void COnHeadFont::SetPos(_vec3 vPos)
{
	m_pTransformCom->Set_StateInfo(STATE_POSITION, &vPos);
	m_pTransformCom->Scaling(1.f, 1.f, 1.f);
}
