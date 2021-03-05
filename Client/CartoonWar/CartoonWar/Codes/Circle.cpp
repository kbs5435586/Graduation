#include "framework.h"
#include "Circle.h"
#include "Management.h"

CCircle::CCircle()
	: CGameObject()
{
}

CCircle::CCircle(const CCircle& rhs)
	: CGameObject(rhs)
{
}

HRESULT CCircle::Ready_Prototype()
{
	return S_OK;
}

HRESULT CCircle::Ready_GameObject(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;
	if (FAILED(CreateInputLayout()))
		return E_FAIL;

	return S_OK;
}

_int CCircle::Update_GameObject(const _float& fTimeDelta)
{
	return _int();
}

_int CCircle::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONEALPHA, this)))
		return -1;

	return _int();
}

void CCircle::Render_GameObject()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();


	MAINPASS tMainPass = {};
	_matrix matWorld = m_pTransformCom->Get_Matrix();
	_matrix matView = CCamera_Manager::GetInstance()->GetMatView();
	_matrix matProj = CCamera_Manager::GetInstance()->GetMatProj();
	_matrix matReflect = CCamera_Manager::GetInstance()->Get_ReflectMatrix((_uint)SCENEID::SCENE_STAGE, L"Layer_Camera", 0, -1.5f);

	m_pShaderCom->SetUp_OnShader(matWorld, matView, matProj, tMainPass);
	_uint iOffset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);

	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffset, CONST_REGISTER::b0);
	CDevice::GetInstance()->UpdateTable();


	m_pBufferCom->Render_VIBuffer();
	Safe_Release(pManagement);
}

HRESULT CCircle::CreateInputLayout()
{
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

	if (FAILED(m_pShaderCom->Create_Shader(vecDesc, RS_TYPE::WIREFRAME, DEPTH_STENCIL_TYPE::LESS, SHADER_TYPE::SHADER_DEFFERED)))
		return E_FAIL;

	return S_OK;
}

CCircle* CCircle::Create()
{
	CCircle* pInstance = new CCircle();

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MessageBox(0, L"CCircle Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCircle::Clone_GameObject(void* pArg)
{
	CCircle* pInstance = new CCircle(*this);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CCircle Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCircle::Free()
{
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);

	CGameObject::Free();
}

HRESULT CCircle::Ready_Component()
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

	m_pBufferCom = (CBuffer_Sphere*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Buffer_Sphere");
	NULL_CHECK_VAL(m_pBufferCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Buffer", m_pBufferCom)))
		return E_FAIL;

	m_pShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Default");
	NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
		return E_FAIL;


	Safe_Release(pManagement);

	return S_OK;
}
