#include "framework.h"
#include "Management.h"
#include "Weapon01.h"
#include "StructedBuffer.h"

CWeapon01::CWeapon01()
	: CWeapon()
{
}

CWeapon01::CWeapon01(const CWeapon01& rhs)
	: CWeapon(rhs)
{
}

HRESULT CWeapon01::Ready_Prototype()
{
	return S_OK;
}

HRESULT CWeapon01::Ready_GameObject(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(CreateInputLayout()))
		return E_FAIL;

	_int	iMoveX = rand() % 500 + 1;
	_int	iMoveZ = rand() % 500 + 1;
	_vec3 vPos = _vec3(iMoveX, 0.f, iMoveZ);
	m_pTransformCom->Scaling(0.1f, 0.1f, 0.1f);
	m_pTransformCom->Set_StateInfo(CTransform::STATE_POSITION, &vPos);


	return S_OK;
}

_int CWeapon01::Update_GameObject(const _float& fTimeDelta)
{


	return _int();
}

_int CWeapon01::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONEALPHA, this)))
		return -1;
	return _int();
}

void CWeapon01::Render_GameObject()
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

		REP tRep = {};
		tRep.m_arrInt[0] = m_iBoneIdx;
		tRep.m_arrInt[1] = m_IsPicked;

		_uint iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(), 
			iOffeset, CONST_REGISTER::b0);

		iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->SetData((void*)&tRep);
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->GetCBV().Get(),
			iOffeset, CONST_REGISTER::b8);

		m_pMeshCom->SetUp_Texture(i);
		if (nullptr != m_pStructedBuffer)
		{
			m_pStructedBuffer->Update_Data(TEXTURE_REGISTER::t8);
		}

		CDevice::GetInstance()->UpdateTable();
		m_pMeshCom->Render_Mesh(i);
	}

	Safe_Release(pManagement);
}

HRESULT CWeapon01::CreateInputLayout()
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

CWeapon01* CWeapon01::Create()
{
	CWeapon01* pInstance = new CWeapon01();

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MessageBox(0, L"CWeapon01 Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CWeapon01::Clone_GameObject(void* pArg, const _uint& iIdx)
{
	CWeapon01* pInstance = new CWeapon01(*this);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CWeapon01 Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	m_iLayerIdx = iIdx;
	return pInstance;
}

void CWeapon01::Free()
{
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);

	if (m_IsClone)
	{
		for (auto& iter : m_vecTexture)
		{
			Safe_Release(iter);
		}
	}

	CGameObject::Free();
}

HRESULT CWeapon01::Ready_Component()
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

	m_pMeshCom = (CMesh*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Weapon1");
	NULL_CHECK_VAL(m_pMeshCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Mesh", m_pMeshCom)))
		return E_FAIL;

	m_pShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Weapon");
	NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
		return E_FAIL;


	Safe_Release(pManagement);
	return S_OK;
}
