#include "framework.h"
#include "Throw_Arrow.h"
#include "Management.h"

CThrow_Arrow::CThrow_Arrow()
	: CGameObject()
{
}

CThrow_Arrow::CThrow_Arrow(const CThrow_Arrow& rhs)
	: CGameObject(rhs)
{
}

HRESULT CThrow_Arrow::Ready_Prototype()
{

	return S_OK;
}

HRESULT CThrow_Arrow::Ready_GameObject(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;
	if (FAILED(CreateInputLayout()))
		return E_FAIL;
	m_pTransformCom->SetUp_Speed(50.f, XMConvertToRadians(90.f));

	if (nullptr == pArg)
		return E_FAIL;

	CTransform* pTemp = (CTransform*)pArg;

	_vec3 vRight, vUp, vLook;
	_vec3 vPosition = *pTemp->Get_StateInfo(CTransform::STATE_POSITION);

	vRight = *pTemp->Get_StateInfo(CTransform::STATE_RIGHT);
	vUp = *pTemp->Get_StateInfo(CTransform::STATE_UP);
	vLook = *pTemp->Get_StateInfo(CTransform::STATE_LOOK);

	_matrix		matRot = Matrix_::Identity();
	DirectX::XMStoreFloat4x4(&matRot, DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationY(XMConvertToRadians(90.f))));

	Vector3_::ScalarProduct(vRight, 0.1f, false);
	Vector3_::ScalarProduct(vUp, 0.1f, false);
	Vector3_::ScalarProduct(vLook, 0.1f, false);

	XMMATRIX mat = ::XMLoadFloat4x4(&matRot);

	vRight = Vector3_::TransformNormal(vRight, mat);
	vUp = Vector3_::TransformNormal(vUp, mat);
	vLook = Vector3_::TransformNormal(vLook, mat);

	m_pTransformCom->Set_StateInfo(CTransform::STATE_RIGHT, &vRight);
	m_pTransformCom->Set_StateInfo(CTransform::STATE_UP, &vUp);
	m_pTransformCom->Set_StateInfo(CTransform::STATE_LOOK, &vLook);
	m_pTransformCom->Set_StateInfo(CTransform::STATE_POSITION,& vPosition);


	_vec3 vColliderSize = {};
	m_pColliderCom->Clone_ColliderBox(m_pTransformCom, vColliderSize);
	
	return S_OK;
}

_int CThrow_Arrow::Update_GameObject(const _float& fTimeDelta)
{
	//m_pColliderCom->Update_Collider(m_pTransformCom);
	m_pTransformCom->Go_Straight(fTimeDelta);

	m_fLifeTime += fTimeDelta;
	if (m_fLifeTime >= 15.f)
	{
		m_fLifeTime = 0.f;
		return DEAD_OBJ;
	}


	return NO_EVENT;
}

_int CThrow_Arrow::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONEALPHA, this)))
		return -1;
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this)))
		return -1;


	return _int();
}

void CThrow_Arrow::Render_GameObject()
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
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
			(_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);


		CDevice::GetInstance()->SetTextureToShader(m_pTextureCom, TEXTURE_REGISTER::t0);

		CDevice::GetInstance()->UpdateTable();
		m_pMeshCom->Render_Mesh(i);
	}


	m_pColliderCom->Render_Collider();
	Safe_Release(pManagement);
}

void CThrow_Arrow::Render_GameObject_Shadow()
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
		_matrix matView = CCamera_Manager::GetInstance()->GetShadowView();
		_matrix matProj = CCamera_Manager::GetInstance()->GetShadowMatProj();

		m_pShaderCom_Shadow->SetUp_OnShader(matWorld, matView, matProj, tMainPass);

		_uint iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
			(_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);

		CDevice::GetInstance()->UpdateTable();
		m_pMeshCom->Render_Mesh(i);
	}
	Safe_Release(pManagement);
}

HRESULT CThrow_Arrow::CreateInputLayout()
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
	if (FAILED(m_pShaderCom_Shadow->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::LESS, SHADER_TYPE::SHADER_SHADOW)))
		return E_FAIL;

	return S_OK;
}

CThrow_Arrow* CThrow_Arrow::Create()
{	
	CThrow_Arrow* pInstnace = new CThrow_Arrow;
	if (FAILED(pInstnace->Ready_Prototype()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CGameObject* CThrow_Arrow::Clone_GameObject(void* pArg, _uint iIdx)
{
	CThrow_Arrow* pInstnace = new CThrow_Arrow;
	if (FAILED(pInstnace->Ready_GameObject(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

void CThrow_Arrow::Free()
{
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pShaderCom_Shadow);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTextureCom);

	CGameObject::Free();
}

HRESULT CThrow_Arrow::Ready_Component()
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

	m_pMeshCom= (CMesh*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Arrow");
	NULL_CHECK_VAL(m_pMeshCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Mesh_Worker", m_pMeshCom)))
		return E_FAIL;

	m_pShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Toon");
	NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
		return E_FAIL;

	m_pShaderCom_Shadow = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Shadow");
	NULL_CHECK_VAL(m_pShaderCom_Shadow, E_FAIL);
	if (FAILED(Add_Component(L"Com_ShadowShader", m_pShaderCom_Shadow)))
		return E_FAIL;

	m_pColliderCom = (CCollider*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Collider_OBB");
	NULL_CHECK_VAL(m_pColliderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Collider", m_pColliderCom)))
		return E_FAIL;

	m_pTextureCom = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Undead");
	NULL_CHECK_VAL(m_pTextureCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Texture", m_pTextureCom)))
		return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}
