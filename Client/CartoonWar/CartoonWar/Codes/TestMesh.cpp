#include "framework.h"
#include "Management.h"
#include "TestMesh.h"

CTestMesh::CTestMesh()
	: CGameObject()
{
}

CTestMesh::CTestMesh(const CTestMesh& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTestMesh::Ready_Prototype()
{
	return S_OK;
}

HRESULT CTestMesh::Ready_GameObject(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;
	if (FAILED(CreateInputLayout()))
		return E_FAIL;

	//m_pTransformCom->SetUp_RotationY(XMConvertToRadians(90.f));
	m_pTransformCom->Scaling(_vec3(0.1f, 0.1f, 0.1f));
	m_pTransformCom->SetUp_Speed(10.f, XMConvertToRadians(90.f));

	return S_OK;
}

_int CTestMesh::Update_GameObject(const _float& fTimeDelta)
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return -1;

	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		m_pTransformCom->BackWard(fTimeDelta);
	}
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		m_pTransformCom->Rotation_Y(-fTimeDelta);
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		m_tInfo.fHP -= 1.f;
		m_pTransformCom->Rotation_Y(fTimeDelta);
	}

	CBuffer_Terrain_Height* pTerrainBuffer = (CBuffer_Terrain_Height*)pManagement->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE, L"Layer_Terrain", L"Com_Buffer");
	if (nullptr == pTerrainBuffer)
		return -1;

	_float		fY = pTerrainBuffer->Compute_HeightOnTerrain(m_pTransformCom);

	m_pTransformCom->Set_PositionY(fY + 0.5f);

	return _int();
}

_int CTestMesh::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONEALPHA, this)))
		return -1;

	return _int();
}

void CTestMesh::Render_GameObject()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();


	MAINPASS tMainPass = {};
	_matrix matWorld = m_pTransformCom->Get_Matrix();
	_matrix matView = CCamera_Manager::GetInstance()->GetMatView();
	_matrix matProj = CCamera_Manager::GetInstance()->GetMatProj();

	m_pShaderCom->SetUp_OnShader(matWorld, matView, matProj, tMainPass);



	_uint iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);


	CDevice::GetInstance()->SetTextureToShader(m_pTexture_Hatch[0]->GetSRV(), TEXTURE_REGISTER::t0);
	CDevice::GetInstance()->SetTextureToShader(m_pTexture_Hatch[1]->GetSRV(), TEXTURE_REGISTER::t1);

	m_pMeshCom->Render_Hierachy_Mesh(m_pMeshCom->GetLoader()->GetScene()->GetRootNode(),
		m_pShaderCom, m_pTransformCom->Get_Matrix(), tMainPass, *m_pTexture_Hatch, L"Texture_Orc_01");

	CDevice::GetInstance()->UpdateTable();


	Safe_Release(pManagement);
}

HRESULT CTestMesh::Ready_Component()
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

	m_pMeshCom = (CStatic_Mesh*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Static_Stone");
	NULL_CHECK_VAL(m_pMeshCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Mesh", m_pMeshCom)))
		return E_FAIL;

	m_pShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Hatching");
	NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
		return E_FAIL;

	m_pTexture_Hatch[0] = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Hatch_123");
	NULL_CHECK_VAL(m_pTexture_Hatch[0], E_FAIL);
	if (FAILED(Add_Component(L"Com_Texture_Hatch_123", m_pTexture_Hatch[0])))
		return E_FAIL;
	m_pTexture_Hatch[1] = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Hatch_456");
	NULL_CHECK_VAL(m_pTexture_Hatch[1], E_FAIL);
	if (FAILED(Add_Component(L"Com_Texture_Hatch_456", m_pTexture_Hatch[1])))
		return E_FAIL;



	Safe_Release(pManagement);
	return S_OK;
}

HRESULT CTestMesh::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc = {};
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

	if (FAILED(m_pShaderCom->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::LESS, SHADER_TYPE::SHADER_DEFFERED)))
		return E_FAIL;

	return S_OK;
}

CTestMesh* CTestMesh::Create()
{
	CTestMesh* pInstance = new CTestMesh();
	if (FAILED(pInstance->Ready_Prototype()))
	{
		MessageBox(0, L"CTestMesh Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTestMesh::Clone_GameObject(void* pArg)
{
	CTestMesh* pInstance = new CTestMesh(*this);
	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CTestMesh Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTestMesh::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTexture_Hatch[0]);
	Safe_Release(m_pTexture_Hatch[1]);


	CGameObject::Free();
}
