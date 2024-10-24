#include "framework.h"
#include "Map_Terrain.h"
#include "Management.h"
#include "UI_Skill.h"

CMap_Terrain::CMap_Terrain()
	: CGameObject()
{
}

CMap_Terrain::CMap_Terrain(const CMap_Terrain& rhs)
	: CGameObject(rhs)
{

}

HRESULT CMap_Terrain::Ready_Prototype()
{

	return S_OK;
}

HRESULT CMap_Terrain::Ready_GameObject(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(CreateInputLayout()))
		return E_FAIL;


	m_pTransformCom->SetUp_Speed(10.f, XMConvertToRadians(30.f));
	return S_OK;
}

_int CMap_Terrain::Update_GameObject(const _float& fTimeDelta)
{
	

	return _int();
}

_int CMap_Terrain::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;
	//if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONEALPHA, this)))
	//	return -1;

	//����
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_MAP, this)))
		return -1;

	
	return _int();
}

void CMap_Terrain::Render_GameObject()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();


	MAINPASS tMainPass = {};
	_matrix matWorld = m_pTransformCom->Get_Matrix();
	_matrix matView = CCamera_Manager::GetInstance()->GetMapMatView();
	_matrix matProj = CCamera_Manager::GetInstance()->GetMapMatProj();
	m_pShaderCom->SetUp_OnShader(matWorld, matView, matProj, tMainPass);

	REP tRep = {};
	tRep.m_arrInt[2] = g_DefferedRender;

	

	_uint iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);

	iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->SetData((void*)&tRep);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
		(_uint)CONST_REGISTER::b8)->GetCBV().Get(), iOffeset, CONST_REGISTER::b8);

	//iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b10)->SetData((void*)&m_tBrush);
	//CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
	//	(_uint)CONST_REGISTER::b10)->GetCBV().Get(), iOffeset, CONST_REGISTER::b10);

	CDevice::GetInstance()->SetTextureToShader(m_pTextureCom_Ground->GetSRV(), TEXTURE_REGISTER::t0);
	CDevice::GetInstance()->SetTextureToShader(m_pTextureCom_Ground->GetSRV(1), TEXTURE_REGISTER::t1);
	CDevice::GetInstance()->SetTextureToShader(m_pTextureCom_Ground->GetSRV(2), TEXTURE_REGISTER::t2);


	CDevice::GetInstance()->SetTextureToShader(m_pTextureCom_Fillter->GetSRV(), TEXTURE_REGISTER::t8);
	CDevice::GetInstance()->SetTextureToShader(m_pBrushTextureCom->GetSRV(), TEXTURE_REGISTER::t9);

	CDevice::GetInstance()->UpdateTable();
	m_pBufferCom->Render_VIBuffer();
	m_pNaviCom->Render_Navigation();

	m_matOldView = CCamera_Manager::GetInstance()->GetMapMatView();
	m_iBlurCnt++;
	if (m_iBlurCnt >= 100)
	{
		m_matOldWorld = m_pTransformCom->Get_Matrix();
		m_iBlurCnt = 0;
	}

	Safe_Release(pManagement);
}


HRESULT CMap_Terrain::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc = {};
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	//vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	//vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	//vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	//vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	//vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });


	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 48, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 60, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 72, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 88, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });


	//if (FAILED(m_pShaderCom->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::LESS, SHADER_TYPE::SHADER_DEFFERED, BLEND_TYPE::DEFAULT, D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST)))
	//	return E_FAIL;
	if (FAILED(m_pShaderCom->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::LESS, SHADER_TYPE::SHADER_DEFFERED, BLEND_TYPE::DEFAULT)))
		return E_FAIL;
	return S_OK;
}

CMap_Terrain* CMap_Terrain::Create()
{
	CMap_Terrain* pInstance = new CMap_Terrain();

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MessageBox(0, L"CMap_Terrain Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMap_Terrain::Clone_GameObject(void* pArg, _uint iIdx)
{
	CMap_Terrain* pInstance = new CMap_Terrain(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MessageBox(0, L"CMap_Terrain Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	m_iLayerIdx = iIdx;
	return pInstance;
}

void CMap_Terrain::Free()
{
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pBrushTextureCom);
	Safe_Release(m_pNaviCom);
	Safe_Release(m_pFrustumCom);

	Safe_Release(m_pTextureCom_Grass_Mix);
	Safe_Release(m_pTextureCom_Ground);


	CGameObject::Free();
}

HRESULT CMap_Terrain::Ready_Component()
{
	CManagement* pManagement = CManagement::GetInstance();
	NULL_CHECK_VAL(pManagement, E_FAIL);
	pManagement->AddRef();

	m_pTransformCom = (CTransform*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Transform");
	NULL_CHECK_VAL(m_pTransformCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Transform", m_pTransformCom)))
		return E_FAIL;

	m_pRendererCom = (CRenderer*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Renderer");
	NULL_CHECK_VAL(m_pRendererCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Renderer", m_pRendererCom)))
		return E_FAIL;

	m_pBufferCom = (CBuffer_Terrain_Height*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Buffer_Terrain_Height");
	NULL_CHECK_VAL(m_pBufferCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Buffer", m_pBufferCom)))
		return E_FAIL;

	//m_pShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Terrain_Tess");
	//NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
	//if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
	//	return E_FAIL;

	m_pShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Terrain");
	NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
		return E_FAIL;

	{
		m_pTextureCom_Grass_Mix = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Grass_Mix");
		NULL_CHECK_VAL(m_pTextureCom_Grass_Mix, E_FAIL);
		if (FAILED(Add_Component(L"Com_Texture_Grass_Mix", m_pTextureCom_Grass_Mix)))
			return E_FAIL;
		m_pTextureCom_Ground = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_NGround");
		NULL_CHECK_VAL(m_pTextureCom_Ground, E_FAIL);
		if (FAILED(Add_Component(L"Com_Texture_Ground", m_pTextureCom_Ground)))
			return E_FAIL;
		m_pTextureCom_Fillter = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Fillter");
		NULL_CHECK_VAL(m_pTextureCom_Fillter, E_FAIL);
		if (FAILED(Add_Component(L"Com_Texture_Fillter", m_pTextureCom_Fillter)))
			return E_FAIL;
	}


	m_pNaviCom = (CNavigation*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_NaviMesh");
	NULL_CHECK_VAL(m_pNaviCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Navi", m_pNaviCom)))
		return E_FAIL;
	m_pBrushTextureCom = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_BrushTemp");
	NULL_CHECK_VAL(m_pBrushTextureCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Texture_Brush", m_pBrushTextureCom)))
		return E_FAIL;

	m_pFrustumCom = (CFrustum*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Frustum");
	NULL_CHECK_VAL(m_pFrustumCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Frustum", m_pFrustumCom)))
		return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}
