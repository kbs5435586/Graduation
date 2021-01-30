#include "framework.h"
#include "Orc01.h"
#include "Management.h"

COrc01::COrc01()
{
}

COrc01::COrc01(const COrc01& rhs)
{
}

HRESULT COrc01::Ready_Prototype()
{
	return S_OK;
}

HRESULT COrc01::Ready_GameObject(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;
	if (FAILED(CreateInputLayout()))
		return E_FAIL;

	//m_pMeshCom->GetLoader()->Load_Texture();

	//m_pTransformCom->SetUp_RotationX(XMConvertToRadians(90.f));
	m_pTransformCom->Scaling(_vec3(0.1f, 0.1f, 0.1f));

	return S_OK;
}

_int COrc01::Update_GameObject(const _float& fTimeDelta)
{
	return _int();
}

_int COrc01::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (nullptr == m_pRendererCom || nullptr == m_pFrustumCom)
		return -1;

	//if (m_pFrustumCom->Culling_Frustum(m_pTransformCom))
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONEALPHA, this)))
			return -1;
	}


	return _int();
}

void COrc01::Render_GameObject()
{
	CDevice::GetInstance()->GetCmdLst()->SetGraphicsRootSignature(CDevice::GetInstance()->GetRootSignature(ROOT_SIG_TYPE::RENDER).Get());

	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();

	MAINPASS tPass = {};


	//CDevice::GetInstance()->SetTextureToShader(m_pTextureCom_0, 0, TEXTURE_REGISTER::t0);
	//CDevice::GetInstance()->SetTextureToShader(m_pTextureCom_1, 0, TEXTURE_REGISTER::t1);

	m_pMeshCom->Render_Hierachy_Mesh(m_pMeshCom->GetLoader()->GetScene()->GetRootNode(),
		m_pShaderCom, m_pTransformCom->Get_Matrix(), tPass, m_pTextureCom_0, m_pTextureCom_1, m_pTextureCom_2);

	Safe_Release(pManagement);
}

HRESULT COrc01::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc = {};
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

	if (FAILED(m_pShaderCom->Create_Shader(vecDesc, RS_TYPE::COUNTERCLOCK)))
		return E_FAIL;

	return S_OK;
}

COrc01* COrc01::Create()
{
	COrc01* pInstance = new COrc01();

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MessageBox(0, L"COrc01 Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* COrc01::Clone_GameObject(void* pArg)
{
	COrc01* pInstance = new COrc01(*this);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"COrc01 Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

void COrc01::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pTextureCom_0);
	Safe_Release(m_pTextureCom_1);
	Safe_Release(m_pTextureCom_2);
	Safe_Release(m_pFrustumCom);

	CGameObject::Free();
}

HRESULT COrc01::Ready_Component()
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

	m_pMeshCom = (CStatic_Mesh*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Orc01");
	NULL_CHECK_VAL(m_pMeshCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Mesh", m_pMeshCom)))
		return E_FAIL;

	m_pShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Terrain");
	NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
		return E_FAIL;

	m_pTextureCom_0 = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Orc_01_Body_Albedo");
	NULL_CHECK_VAL(m_pTextureCom_0, E_FAIL);
	if (FAILED(Add_Component(L"Com_Texture_Body", m_pTextureCom_0)))
		return E_FAIL;

	m_pTextureCom_1 = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Orc_01_Armors_Albedo");
	NULL_CHECK_VAL(m_pTextureCom_1, E_FAIL);
	if (FAILED(Add_Component(L"Com_Texture_Armor", m_pTextureCom_1)))
		return E_FAIL;

	m_pTextureCom_2 = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_T_Hair_Albedo");
	NULL_CHECK_VAL(m_pTextureCom_2, E_FAIL);
	if (FAILED(Add_Component(L"Com_Texture_Hair", m_pTextureCom_2)))
		return E_FAIL;

	m_pFrustumCom = (CFrustum*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Frustum");
	NULL_CHECK_VAL(m_pFrustumCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Frustum", m_pFrustumCom)))
		return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}