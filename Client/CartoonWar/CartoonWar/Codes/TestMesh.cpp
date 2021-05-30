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

	_vec3 vPos = { 0.f,0.f,0.f };
	m_pTransformCom->Set_StateInfo(CTransform::STATE_POSITION, &vPos);

	//m_pTransformCom->SetUp_RotationX(XMConvertToRadians(90.f));
	m_pTransformCom->Scaling(_vec3(0.01f, 0.01f, 0.01f));
	m_pTransformCom->SetUp_Speed(10.f, XMConvertToRadians(90.f));

	return S_OK;
}

_int CTestMesh::Update_GameObject(const _float& fTimeDelta)
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return -1;

	//CBuffer_Terrain_Height* pTerrainBuffer = (CBuffer_Terrain_Height*)pManagement->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE, L"Layer_Terrain", L"Com_Buffer");
	//if (nullptr == pTerrainBuffer)
	//	return -1;

	//_float		fY = pTerrainBuffer->Compute_HeightOnTerrain(m_pTransformCom);

	//m_pTransformCom->Set_PositionY(fY + 0.5f);

	if (GetAsyncKeyState(VK_LEFT))
	{
		m_pTransformCom->Rotation_Y(fTimeDelta);
	}
	else if (GetAsyncKeyState(VK_RIGHT))
	{
		m_pTransformCom->Rotation_Y(-fTimeDelta);
	}

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

	_uint iSubsetNum = m_pMeshCom->GetSubsetNum();
	for (_uint i = 0; i < iSubsetNum; ++i)
	{
		MAINPASS tMainPass = {};
		_matrix matWorld = m_pTransformCom->Get_Matrix();
		_matrix matView = CCamera_Manager::GetInstance()->GetMatView();
		_matrix matProj = CCamera_Manager::GetInstance()->GetMatProj();

		m_pShaderCom->SetUp_OnShader(matWorld, matView, matProj, tMainPass);

		_uint iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);
		CDevice::GetInstance()->SetTextureToShader(m_pTextureCom, TEXTURE_REGISTER::t0);
		CDevice::GetInstance()->UpdateTable();
		m_pMeshCom->Render_Mesh(i);
	}

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

	m_pMeshCom = (CMesh*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Test");
	NULL_CHECK_VAL(m_pMeshCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Mesh", m_pMeshCom))) 
		return E_FAIL;


	m_pShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Toon");
	NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
		return E_FAIL;

	m_pTextureCom = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Toon");
	NULL_CHECK_VAL(m_pTextureCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Texture", m_pTextureCom)))
		return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}

HRESULT CTestMesh::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc = {};
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 48, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 60, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 72, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 88, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

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

CGameObject* CTestMesh::Clone_GameObject(void* pArg, _uint iIdx)
{
	CTestMesh* pInstance = new CTestMesh(*this);
	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CTestMesh Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	m_iLayerIdx = iIdx;
	return pInstance;
}

void CTestMesh::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pTextureCom);

	CGameObject::Free();
}
