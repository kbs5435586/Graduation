#include "framework.h"
#include "Management.h"
#include "TestAnimMesh.h"

CTestAnimMesh::CTestAnimMesh()
{
}

CTestAnimMesh::CTestAnimMesh(const CTestAnimMesh& rhs)
{
}

HRESULT CTestAnimMesh::Ready_Prototype()
{
	return S_OK;
}

HRESULT CTestAnimMesh::Ready_GameObject(void* pArg)
{
	m_IsClone = true;
	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(CreateInputLayout()))
		return E_FAIL;

	m_pTransformCom->Scaling(0.1f, 0.1f, 0.1f);
	m_pTransformCom->SetUp_Speed(10.f, XMConvertToRadians(90.f));


	//m_pAnimCom->GetCurClip() = 1;
	m_pAnimCom->SetBones(m_pMeshCom->GetBones());
	m_pAnimCom->SetAnimClip(m_pMeshCom->GetAnimClip());

	for (auto& iter : m_pMeshCom->m_vecDiffTexturePath)
	{
		CTexture* pTexture = CTexture::Create(iter);
		m_vecTexture.push_back(pTexture);
	}
	return S_OK;
}

_int CTestAnimMesh::Update_GameObject(const _float& fTimeDelta)
{
	return _int();
}

_int CTestAnimMesh::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONEALPHA, this)))
		return -1;

	if (CManagement::GetInstance()->Key_Pressing(KEY_UP))
	{
		_vec3 vLook = {};
		vLook = *m_pTransformCom->Get_StateInfo(CTransform::STATE_LOOK);
		vLook = Vector3_::Normalize(vLook);


		_vec3 vDirectionPerSec = (vLook * 5.f * fTimeDelta);
		_vec3 vSlide = {};
		if (m_pNaviCom->Move_OnNavigation(m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION), &vDirectionPerSec, &vSlide))
		{
			m_pTransformCom->BackWard(fTimeDelta);
		}
		else
		{
			m_pTransformCom->Go_There(vSlide);
		}


	}
	else if (CManagement::GetInstance()->Key_Pressing(KEY_DOWN))
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
	else if (CManagement::GetInstance()->Key_Pressing(KEY_LEFT))
	{
		m_pTransformCom->Rotation_Y(fTimeDelta);
	}
	else if (CManagement::GetInstance()->Key_Pressing(KEY_RIGHT))
	{
		m_pTransformCom->Rotation_Y(-fTimeDelta);
	}

	//else if (CManagement::GetInstance()->Key_Pressing(KEY_LBUTTON))
	//{
	//	Delete_Component(L"Com_Mesh", m_pMeshCom);
	//	m_pMeshCom = (CMesh*)CManagement::GetInstance()->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Orc02_Attack01");
	//	NULL_CHECK_VAL(m_pMeshCom, E_FAIL);
	//	if (FAILED(Add_Component(L"Com_Mesh", m_pMeshCom)))
	//		return E_FAIL;
	//	m_pAnimCom->SetBones(m_pMeshCom->GetBones());
	//	m_pAnimCom->SetAnimClip(m_pMeshCom->GetAnimClip());
	//}
	//else if (CManagement::GetInstance()->Key_Pressing(KEY_RBUTTON))
	//{
	//	Delete_Component(L"Com_Mesh", m_pMeshCom);
	//	m_pMeshCom = (CMesh*)CManagement::GetInstance()->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Orc02_Attack02");
	//	NULL_CHECK_VAL(m_pMeshCom, E_FAIL);
	//	if (FAILED(Add_Component(L"Com_Mesh", m_pMeshCom)))
	//		return E_FAIL;
	//	m_pAnimCom->SetBones(m_pMeshCom->GetBones());
	//	m_pAnimCom->SetAnimClip(m_pMeshCom->GetAnimClip());
	//}

	m_pAnimCom->Update(fTimeDelta);
	return _int();
}

void CTestAnimMesh::Render_GameObject()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();


	_uint iSubsetNum = m_pMeshCom->GetSubsetNum();
	//_uint iSubsetNum = 1;
	for (_uint i = 0; i < iSubsetNum; ++i)
	{

		MAINPASS tMainPass = {};
		_matrix matWorld = m_pTransformCom->Get_Matrix();
		_matrix matView = CCamera_Manager::GetInstance()->GetMatView();
		_matrix matProj = CCamera_Manager::GetInstance()->GetMatProj();
		if (GetAsyncKeyState('O')&0x8000)
			m_IsAnim ^= true;

		REP tRep = {};
		if (m_IsAnim)
		{
			tRep.m_arrInt[0] = 1;
		}
		else
			tRep.m_arrInt[0] = 0;
	


		tRep.m_arrInt[1]=m_pAnimCom->GetBones()->size();

		m_pShaderCom->SetUp_OnShader(matWorld, matView, matProj, tMainPass);

		_uint iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
			(_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);

		iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->SetData((void*)&tRep);
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
			(_uint)CONST_REGISTER::b8)->GetCBV().Get(), iOffeset, CONST_REGISTER::b8);

		CDevice::GetInstance()->SetTextureToShader(m_vecTexture[i]->GetSRV_().Get(), TEXTURE_REGISTER::t0);


		m_pAnimCom->UpdateData(m_pMeshCom, m_pComputeShaderCom);
		CDevice::GetInstance()->UpdateTable();
		m_pMeshCom->Render_Mesh(i);
	}

	Safe_Release(pManagement);
}

HRESULT CTestAnimMesh::CreateInputLayout()
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

CTestAnimMesh* CTestAnimMesh::Create()
{
	CTestAnimMesh* pInstance = new CTestAnimMesh();

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MessageBox(0, L"CTestAnimMesh Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTestAnimMesh::Clone_GameObject(void* pArg, const _uint& iIdx)
{
	CTestAnimMesh* pInstance = new CTestAnimMesh(*this);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CTestAnimMesh Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	m_iLayerIdx = iIdx;
	return pInstance;
}

void CTestAnimMesh::Free()
{
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pComputeShaderCom);
	Safe_Release(m_pAnimCom);
	Safe_Release(m_pNaviCom);
	if (m_IsClone)
	{
		for (auto& iter : m_vecTexture)
		{
			Safe_Release(iter);
		}
	}
	CGameObject::Free();
}

HRESULT CTestAnimMesh::Ready_Component()
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

	m_pComputeShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Compute_Animation");
	NULL_CHECK_VAL(m_pComputeShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_ComputeShader", m_pComputeShaderCom)))
		return E_FAIL;

	m_pAnimCom = (CAnimator*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Animation");
	NULL_CHECK_VAL(m_pAnimCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Anim", m_pAnimCom)))
		return E_FAIL;

	m_pNaviCom = (CNavigation*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_NaviMesh_Test");
	NULL_CHECK_VAL(m_pNaviCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Navi", m_pNaviCom)))
		return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}
