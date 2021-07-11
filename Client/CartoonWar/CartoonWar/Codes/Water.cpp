#include"framework.h"
#include "Water.h"
#include "Management.h"

CWater::CWater()
	: CGameObject()
{
}

CWater::CWater(const CWater& rhs)
	: CGameObject(rhs)
{
}

HRESULT CWater::Ready_Prototype()
{
	return S_OK;
}

HRESULT CWater::Ready_GameObject(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(CreateInputLayout()))
		return E_FAIL;
	_vec3 vPos = { 15.f, 0.f, 15.f };
	m_pTransformCom->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
	m_pTransformCom->Scaling(2.f, 2.f, 2.f);
	return S_OK;

}

_int CWater::Update_GameObject(const _float& fTimeDelta)
{
	return _int();
}

_int CWater::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONEALPHA, this)))
		return -1;
	return _int();
}

void CWater::Render_GameObject()
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

	m_tTexInfo.fFrameTime += 0.01f;

	if (m_tTexInfo.fFrameTime > 1000.0f)
	{
		m_tTexInfo.fFrameTime = 0.0f;
	}

	_uint iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);
	iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b4)->SetData((void*)&m_tTexInfo);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b4)->GetCBV().Get(), iOffeset, CONST_REGISTER::b4);


	CDevice::GetInstance()->UpdateTable();


	m_pBufferCom->Render_VIBuffer();

	Safe_Release(pManagement);
}

HRESULT CWater::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc = {};
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

	if (FAILED(m_pShaderCom->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::LESS, SHADER_TYPE::SHADER_DEFFERED, BLEND_TYPE::ALPHABLEND)))
		return E_FAIL;

	return S_OK;
}

CWater* CWater::Create()
{
	CWater* pInstance = new CWater();
	if (FAILED(pInstance->Ready_Prototype()))
		Safe_Release(pInstance);
	return pInstance;
}

CGameObject* CWater::Clone_GameObject(void* pArg, _uint iIdx)
{
	CWater* pInstance = new CWater();
	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);
	m_iLayerIdx = iIdx;

	return pInstance;
}

void CWater::Free()
{
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	CGameObject::Free();
}

HRESULT CWater::Ready_Component()
{
	CManagement* pInstance = CManagement::GetInstance();
	if (pInstance == nullptr)
		return E_FAIL;
	pInstance->AddRef();


	Safe_Release(pInstance);
	return S_OK;
}
