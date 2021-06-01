#include "framework.h"
#include "UI_WoL.h"
#include "Management.h"
#include "UAV.h"

CUI_WoL::CUI_WoL()
	: CUI()
{
}

CUI_WoL::CUI_WoL(const CUI_WoL& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_WoL::Ready_Prototype()
{
	return S_OK;
}

HRESULT CUI_WoL::Ready_GameObject(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;
	if (FAILED(CreateInputLayout()))
		return E_FAIL;

	//m_fX = 300.f;
	m_fX = WINCX/2 -100.f;
	m_fY = 50.f;

	m_fSizeX = 50.f;
	m_fSizeY = 25.f;


	return S_OK;
}

_int CUI_WoL::Update_GameObject(const _float& fTimeDelta)
{

	return _int();
}

_int CUI_WoL::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (m_pRendererCom != nullptr)
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
			return E_FAIL;
	}

	return _int();
}

void CUI_WoL::Render_GameObject()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();


	MAINPASS	tMainPass = {};


	_matrix matWorld = Matrix_::Identity();
	_matrix matView = Matrix_::Identity();
	_matrix matProj = CCamera_Manager::GetInstance()->GetMatOrtho();

	matWorld._11 = m_fSizeX;
	matWorld._22 = m_fSizeY;

	matWorld._41 = m_fX - (WINCX >> 1);
	matWorld._42 = -m_fY + (WINCY >> 1);


	m_pShaderCom->SetUp_OnShader(matWorld, matView, matProj, tMainPass);
	_uint iOffset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffset, CONST_REGISTER::b0);

	CDevice::GetInstance()->SetTextureToShader(pManagement->Get_UAV(L"UAV_WoL")->GetSRV().Get(), TEXTURE_REGISTER::t0);
	CDevice::GetInstance()->UpdateTable();


	//if(g_iBlueNum ==0 && g_iN)


	m_tRep.m_arrInt[0] = g_iRedNum;
	//m_tRep.m_arrInt[1] = g_iBlueNum;


	CDevice::GetInstance()->ClearDummyDesc_CS();
	iOffset = CManagement::GetInstance()->GetConstantBuffer((_uint)CONST_REGISTER::b8)->SetData((void*)&m_tRep);
	CDevice::GetInstance()->SetUpContantBufferToShader_CS(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->GetCBV().Get(), iOffset, CONST_REGISTER::b8);
	CDevice::GetInstance()->SetUpUAVToRegister(pManagement->Get_UAV(L"UAV_WoL"), UAV_REGISTER::u0);

	m_pCompute_ShaderCom->UpdateData_CS();
	CManagement::GetInstance()->Get_UAV(L"UAV_WoL")->Dispatch(1, 5, 1);


	m_pBufferCom->Render_VIBuffer();
	Safe_Release(pManagement);
}

HRESULT CUI_WoL::CreateInputLayout()
{
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

	if (FAILED(m_pShaderCom->Create_Shader(vecDesc)))
		return E_FAIL;

	return S_OK;
}

CUI_WoL* CUI_WoL::Create()
{
	CUI_WoL* pInstance = new CUI_WoL();
	if (FAILED(pInstance->Ready_Prototype()))
	{
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_WoL::Clone_GameObject(void* pArg, _uint iIdx)
{
	CUI_WoL* pInstance = new CUI_WoL();
	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		Safe_Release(pInstance);
	}
	m_iLayerIdx = iIdx;
	return pInstance;
}

void CUI_WoL::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pCompute_ShaderCom);


	CGameObject::Free();
}

HRESULT CUI_WoL::Ready_Component()
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
	m_pBufferCom = (CBuffer_RectTex*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Buffer_RectTex");
	NULL_CHECK_VAL(m_pBufferCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Buffer", m_pBufferCom)))
		return E_FAIL;
	m_pShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_UI");
	NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
		return E_FAIL;
	m_pCompute_ShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Compute_WoL_Red");
	NULL_CHECK_VAL(m_pCompute_ShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Compute_Shader", m_pCompute_ShaderCom)))
		return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}
