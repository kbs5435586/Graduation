#include "stdafx.h"
#include "..\Headers\Monster.h"
#include "Management.h"

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CMonster::CMonster(const CMonster & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMonster::Ready_Prototype()
{
	return S_OK;
}

HRESULT CMonster::Ready_GameObject()
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->SetUp_Speed(5.0f, D3DXToRadian(90.0f));
	m_pTransformCom->Set_StateInfo(STATE_POSITION, &_vec3(rand() % 10, 0.5f, rand() % 10));
	return S_OK;
}

_int CMonster::Update_GameObject(const _float & fTimeDelta)
{
	if (nullptr != m_pPickingCom)
		m_pPickingCom->Update_Ray();

	CManagement*	pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return -1;

	pManagement->AddRef();

	CBuffer_Terrain*	pTerrainBuffer = (CBuffer_Terrain*)pManagement->Get_ComponentPointer(SCENE_STAGE, L"Layer_Terrain", L"Com_Buffer");
	if (nullptr == pTerrainBuffer)
		return -1;

	_float		fY = pTerrainBuffer->Compute_HeightOnTerrain(m_pTransformCom);

	m_pTransformCom->Set_PositionY(fY+0.5f);

	Safe_Release(pManagement);

	return _int();
}

_int CMonster::LastUpdate_GameObject(const _float & fTimeDelta)
{
	_matrix matView;
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, nullptr, &matView);

	_vec3		vRight, vUp, vLook;

	vRight = *(_vec3*)&matView.m[0][0] * m_pTransformCom->Get_Scale().x;
	vUp = *(_vec3*)&matView.m[1][0] * m_pTransformCom->Get_Scale().y;
	vLook = *(_vec3*)&matView.m[2][0] * m_pTransformCom->Get_Scale().z;

	m_pTransformCom->Set_StateInfo(STATE_RIGHT, &vRight);
	m_pTransformCom->Set_StateInfo(STATE_UP, &vUp);
	m_pTransformCom->Set_StateInfo(STATE_LOOK, &vLook);

	_bool		isPick = false;
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		isPick = m_pBufferCom->Picking_ToBuffer(nullptr, m_pTransformCom, m_pPickingCom);


	if (false == isPick)
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(RENDER_NONEALPHA, this)))
			return -1;
	}

	return _int();
}

void CMonster::Render_GameObject()
{
	if (nullptr == m_pBufferCom)
		return;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 100);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pTransformCom->SetUp_OnGraphicDev();
	if (FAILED(m_pTextureCom->SetUp_OnGraphicDev()))
		return;
	m_pBufferCom->Render_VIBuffer();


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}

CMonster * CMonster::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMonster* pInstance = new CMonster(pGraphic_Device);
	if (FAILED(pInstance->Ready_Prototype()))
		return nullptr;
	return pInstance;
}

CGameObject * CMonster::Clone_GameObject()
{
	CMonster* pInstance = new CMonster(*this);
	if (FAILED(pInstance->Ready_GameObject()))
		return nullptr;
	return pInstance;
}

void CMonster::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pPickingCom);
	CGameObject::Free();
}

HRESULT CMonster::Ready_Component()
{
	CManagement*		pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return E_FAIL;
	pManagement->AddRef();

	m_pTransformCom = (CTransform*)pManagement->Clone_Component(SCENE_STATIC, L"Component_Transform");
	if (FAILED(Add_Component(L"Com_Transform", m_pTransformCom)))
		return E_FAIL;

	m_pRendererCom = (CRenderer*)pManagement->Clone_Component(SCENE_STATIC, L"Component_Renderer");
	if (FAILED(Add_Component(L"Com_Renderer", m_pRendererCom)))
		return E_FAIL;

	m_pTextureCom = (CTexture*)pManagement->Clone_Component(SCENE_STAGE, L"Component_Texture_Monster");
	if (FAILED(Add_Component(L"Com_Texture", m_pTextureCom)))
		return E_FAIL;

	m_pBufferCom = (CBuffer_RcTex*)pManagement->Clone_Component(SCENE_STATIC, L"Component_Buffer_RcTex");
	if (FAILED(Add_Component(L"Com_Buffer", m_pBufferCom)))
		return E_FAIL;

	m_pPickingCom = (CPicking*)pManagement->Clone_Component(SCENE_STAGE, L"Component_Picking");
	if (FAILED(Add_Component(L"Com_Picking", m_pPickingCom)))
		return E_FAIL;

	Safe_Release(pManagement);

	return NOERROR;
}
