#include "framework.h"
#include "Sphere.h"
#include "Management.h"

CShpere::CShpere()
	: CGameObject()
{
}

CShpere::CShpere(const CShpere& rhs)
	: CGameObject(rhs)
{
}

HRESULT CShpere::Ready_Prototype()
{

	return S_OK;
}

HRESULT CShpere::Ready_GameObject(void* pArg)
{

	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(CreateInputLayout()))
		return E_FAIL;



	_vec3 vPos = _vec3(20.f, 0.f, 0.f);
	m_pTransformCom->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
	m_pTransformCom->SetUp_Speed(10.f, XMConvertToRadians(30.f));
	m_pTransformCom->Scaling(100.f, 100.f, 100.f);

	return S_OK;
}

_int CShpere::Update_GameObject(const _float& fTimeDelta)
{

	return _int();
}

_int CShpere::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONEALPHA, this)))
		return -1;

	return _int();
}

void CShpere::Render_GameObject()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();


	MAINPASS tMainPass = {};
	_matrix matWorld = m_pTransformCom->Get_Matrix();
	_matrix matView = CCamera_Manager::GetInstance()->GetMatView();
	_matrix matProj = CCamera_Manager::GetInstance()->GetMatProj();

	REFLECT	tReflect = {};

	m_pShaderCom->SetUp_OnShader(matWorld, matView, matProj, tMainPass);

	_uint iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);


	CDevice::GetInstance()->SetTextureToShader(m_pTextureCom->GetSRV(0), TEXTURE_REGISTER::t0);
	CDevice::GetInstance()->SetTextureToShader(m_pTextureCom->GetSRV(1), TEXTURE_REGISTER::t1);
	CDevice::GetInstance()->SetTextureToShader(m_pTextureCom->GetSRV(2), TEXTURE_REGISTER::t2);
	CDevice::GetInstance()->SetTextureToShader(m_pTextureCom->GetSRV(3), TEXTURE_REGISTER::t3);
	CDevice::GetInstance()->SetTextureToShader(m_pTextureCom->GetSRV(4), TEXTURE_REGISTER::t4);
	CDevice::GetInstance()->SetTextureToShader(m_pTextureCom->GetSRV(5), TEXTURE_REGISTER::t5);

	CDevice::GetInstance()->UpdateTable();


	m_pBufferCom->Render_VIBuffer();
	//m_pBufferCom;
	Safe_Release(pManagement);
}

HRESULT CShpere::CreateInputLayout()
{
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;

	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });


	if (FAILED(m_pShaderCom->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::LESS, SHADER_TYPE::SHADER_DEFFERED)))
		return E_FAIL;
	return S_OK;
}


CShpere* CShpere::Create()
{
	CShpere* pInstance = new CShpere();

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MessageBox(0, L"CShpere Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	} 
	return pInstance;
}

CGameObject* CShpere::Clone_GameObject(void* pArg, _uint iIdx)
{
	CShpere* pInstance = new CShpere(*this);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CShpere Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	m_iLayerIdx = iIdx;
	return pInstance;
}

void CShpere::Free()
{
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);

	CGameObject::Free();
}

HRESULT CShpere::Ready_Component()
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

	m_pShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Hatching");
	NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
		return E_FAIL;


	m_pTextureCom = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Hatch");
	NULL_CHECK_VAL(m_pTextureCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Texture", m_pTextureCom)))
		return E_FAIL;
	Safe_Release(pManagement);

	return S_OK;
}
