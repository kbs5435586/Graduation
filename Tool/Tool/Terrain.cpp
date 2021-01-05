#include "stdafx.h"
#include "Terrain.h"
#include "Management.h"
#include "Light_Mgr.h"

#include "MainFrm.h"
#include "SecondWnd.h"
#include "UnitTool.h"
#include "TerrainTool.h"
#include "GameObject.h"
#include "Layer.h"
CTerrain::CTerrain(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CTerrain::CTerrain(const CTerrain & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTerrain::Ready_Prototype()
{
	return S_OK;
}

HRESULT CTerrain::Ready_GameObject(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;


	return S_OK;
}

_int CTerrain::Update_GameObject(const _float & fTimeDelta)
{
	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	if (nullptr == pMainFrm)
		return _int();
	CSecondWnd* pSecond = dynamic_cast<CSecondWnd*>(pMainFrm->m_MainSplt.GetPane(0, 1));

	if (m_pPickingCom != nullptr)
		m_pPickingCom->Update_Ray();

	Change_Component_Terrain(pSecond);

	if (nullptr == m_pBufferCom)
		return _int();

	return _int();
}

_int CTerrain::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return E_FAIL;
	pManagement->AddRef();
	
	if (pManagement->KeyDown(KEY_F3))
	{
		if (eMode == MODE_UNIT)
		{
			map<const _tchar*, CLayer*>	mapTemp=*pManagement->Get_MapLayer(SCENE_LOGO);

			for (auto& iter : mapTemp)
			{
				auto& iter1 = iter.second->GetGameObjectList()->begin();
				for (; iter1 != iter.second->GetGameObjectList()->end();)
				{
					if ((*iter1)->GetIsPick())
					{
						Safe_Release(*iter1);
						iter1 = iter.second->GetGameObjectList()->erase(iter1);
					}
					else
						iter1++;
				}
			}
		}
		if(eMode==MODE_NAVIMESH)
			m_pNaviCom->Delete_Cell();
	}

	Safe_Release(pManagement);
	
	if (FAILED(m_pRendererCom->Add_RenderGroup(RENDER_NONEALPHA, this)))
		return -1;
	return _int();
}

void CTerrain::Render_GameObject()
{
	if (nullptr == m_pBufferCom ||
		nullptr == m_pShaderCom)
		return;

	LPD3DXEFFECT pEffect = m_pShaderCom->GetEffectHandle();
	if (nullptr == pEffect)
		return;

	pEffect->AddRef();

	if (FAILED(SetUp_ConstantTable(pEffect)))
		return;

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(0);

	m_pBufferCom->Render_VIBuffer();

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	m_pNaviCom->Render_Navigation();
}

CTerrain * CTerrain::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTerrain* pInstance = new CTerrain(pGraphic_Device);
	if (FAILED(pInstance->Ready_Prototype()))
		return nullptr;
	return pInstance;
}

CGameObject * CTerrain::Clone_GameObject(void* pArg)
{
	CTerrain* pInstance = new CTerrain(*this);
	if (FAILED(pInstance->Ready_GameObject(pArg)))
		return nullptr;
	return pInstance;
}

void CTerrain::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pPickingCom);
	Safe_Release(m_pNaviCom);
	CGameObject::Free();
}

HRESULT CTerrain::Ready_Component()
{
	CManagement*	pManagement = CManagement::GetInstance();
	if (pManagement == nullptr)
		return E_FAIL;
	pManagement->AddRef();

	m_pTransformCom = (CTransform*)pManagement->Clone_Component(SCENE_STATIC, L"Component_Transform");
	if (FAILED(Add_Component(L"Com_Transform", m_pTransformCom)))
		return E_FAIL;

	m_pRendererCom = (CRenderer*)pManagement->Clone_Component(SCENE_STATIC, L"Component_Renderer");
	if (FAILED(Add_Component(L"Com_Renderer", m_pRendererCom)))
		return E_FAIL;

	m_pTextureCom = (CTexture*)pManagement->Clone_Component(SCENE_LOGO, L"Component_Texture_Terrain");
	if (FAILED(Add_Component(L"Com_Texture", m_pTextureCom)))
		return E_FAIL;

	m_pBufferCom = (CBuffer_Terrain*)pManagement->Clone_Component(SCENE_LOGO, L"Component_Buffer_Terrain");
	if (FAILED(Add_Component(L"Com_Buffer", m_pBufferCom)))
		return E_FAIL;

	m_pShaderCom = (CShader*)pManagement->Clone_Component(SCENE_LOGO, L"Component_Shader_Terrain");
	if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
		return E_FAIL;

	m_pPickingCom = (CPicking*)pManagement->Clone_Component(SCENE_LOGO, L"Component_Picking");
	if (FAILED(Add_Component(L"Com_Picking", m_pPickingCom)))
		return E_FAIL;

	m_pNaviCom = (CNavi*)pManagement->Clone_Component(SCENE_LOGO, L"Component_NaviMesh");
	if (FAILED(Add_Component(L"Com_Navi", m_pNaviCom)))
		return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}

HRESULT CTerrain::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (FAILED(m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld")))
		return E_FAIL;

	_matrix		matView, matProj;


	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	if (FAILED(m_pTextureCom->SetUp_OnShader(pEffect, "g_DiffuseTexture", 1)))
		return E_FAIL;

	D3DMATERIAL9	MtrlInfo;
	m_pGraphic_Device->SetMaterial(&MtrlInfo);

	const D3DLIGHT9*	pLightInfo = CLight_Mgr::GetInstance()->Get_LightInfo();
	if (nullptr == pLightInfo)
		return E_FAIL;

	pEffect->SetVector("g_vLightDir", &_vec4(pLightInfo->Direction, 0.f));
	pEffect->SetVector("g_vLightDiffuse", (_vec4*)&pLightInfo->Diffuse);
	pEffect->SetVector("g_vLightSpecular", (_vec4*)&pLightInfo->Specular);
	pEffect->SetVector("g_vLightAmbient", (_vec4*)&pLightInfo->Ambient);

	pEffect->SetVector("g_vMtrlSpecular", &_vec4(1.f, 1.f, 1.f, 1.f));
	pEffect->SetVector("g_vMtrlAmbient", &_vec4(0.3f, 0.3f, 0.3f, 1.f));
	pEffect->SetFloat("g_fPower", 30.f);

	D3DXMatrixInverse(&matView, nullptr, &matView);
	pEffect->SetVector("g_vCamPosition", (_vec4*)&matView.m[3][0]);
	return S_OK;
}

void CTerrain::Change_Component_Terrain(CSecondWnd * pSecond)
{
	if (pSecond->m_pTerrainTool->m_IsApply)
	{
		CManagement*	pManagement = CManagement::GetInstance();
		if (pManagement == nullptr)
			return;
		pManagement->AddRef();

		if (FAILED(Delete_Component(L"Com_Buffer", m_pBufferCom)))
			return;

		m_pBufferCom = (CBuffer_Terrain*)pManagement->Clone_Component(SCENE_LOGO, pSecond->m_pTerrainTool->m_pOutput_Component_tag);
		if (FAILED(Add_Component(L"Com_Buffer", m_pBufferCom)))
			return;

		Safe_Release(pManagement);
		pSecond->m_pTerrainTool->m_IsApply = false;
	}

}

