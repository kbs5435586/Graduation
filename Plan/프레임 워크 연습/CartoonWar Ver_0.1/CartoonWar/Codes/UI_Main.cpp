#include "framework.h"
#include "UI_Main.h"
#include "Management.h"

CUI_Main::CUI_Main()
	: CUI()
{
}

CUI_Main::CUI_Main(const CUI_Main& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Main::Ready_Prototype()
{
	return S_OK;
}

HRESULT CUI_Main::Ready_GameObject(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;
	if (FAILED(CreateInputLayout()))
		return E_FAIL;

	m_fX = 400.f;
	m_fY = 300.f;

	m_fSizeX = 800.f;
	m_fSizeY = 600.f;
	return S_OK;
}

_int CUI_Main::Update_GameObject(const _float& fTimeDelta)
{
	return _int();
}

_int CUI_Main::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (m_pRendererCom != nullptr)
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
			return E_FAIL;
	}
	return _int();
}

void CUI_Main::Render_GameObject()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();


	MAINPASS tMainPass = {};

	_matrix matWorld = Matrix_::Identity();
	_matrix matView = Matrix_::Identity();
	_matrix matProj = CCamera_Manager::GetInstance()->GetMatOrtho();

	matWorld._11 = m_fSizeX;
	matWorld._22 = m_fSizeY;

	matWorld._41 = m_fX - (WINCX >> 1);
	matWorld._42 = -m_fY + (WINCY >> 1);


	m_pShaderCom->SetUp_OnShader(matWorld, matView, matProj, tMainPass);
	_uint iOffset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);




	ComPtr<ID3D12DescriptorHeap>	pTextureDesc0 = pManagement->Get_RTT((_uint)MRT::MRT_DEFFERD)->Get_RTT(0)->pRtt->GetSRV().Get();
	ComPtr<ID3D12DescriptorHeap>	pTextureDesc1 = pManagement->Get_RTT((_uint)MRT::MRT_DEFFERD)->Get_RTT(1)->pRtt->GetSRV().Get();
	ComPtr<ID3D12DescriptorHeap>	pTextureDesc2 = pManagement->Get_RTT((_uint)MRT::MRT_DEFFERD)->Get_RTT(2)->pRtt->GetSRV().Get();
	ComPtr<ID3D12DescriptorHeap>	pTextureDesc3 = pManagement->Get_RTT((_uint)MRT::MRT_DEFFERD)->Get_RTT(3)->pRtt->GetSRV().Get();
	ComPtr<ID3D12DescriptorHeap>	pTextureDesc4 = pManagement->Get_RTT((_uint)MRT::MRT_DEFFERD)->Get_RTT(4)->pRtt->GetSRV().Get();
	ComPtr<ID3D12DescriptorHeap>	pTextureDesc5 = pManagement->Get_RTT((_uint)MRT::MRT_DEFFERD)->Get_RTT(5)->pRtt->GetSRV().Get();
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(0)->GetCBV().Get(), iOffset, CONST_REGISTER::b0);
	CDevice::GetInstance()->SetTextureToShader(pTextureDesc0.Get(), TEXTURE_REGISTER::t0);
	CDevice::GetInstance()->SetTextureToShader(pTextureDesc1.Get(), TEXTURE_REGISTER::t1);
	CDevice::GetInstance()->SetTextureToShader(pTextureDesc2.Get(), TEXTURE_REGISTER::t2);
	CDevice::GetInstance()->SetTextureToShader(pTextureDesc3.Get(), TEXTURE_REGISTER::t3);
	CDevice::GetInstance()->SetTextureToShader(pTextureDesc4.Get(), TEXTURE_REGISTER::t4);
	CDevice::GetInstance()->SetTextureToShader(pTextureDesc5.Get(), TEXTURE_REGISTER::t5);
	CDevice::GetInstance()->UpdateTable();


	m_pBufferCom->Render_VIBuffer();
	Safe_Release(pManagement);
}

HRESULT CUI_Main::CreateInputLayout()
{
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

	if (FAILED(m_pShaderCom->Create_Shader(vecDesc)))
		return E_FAIL;

	return S_OK;
}

CUI_Main* CUI_Main::Create()
{
	CUI_Main* pInstance = new CUI_Main();
	if (FAILED(pInstance->Ready_Prototype()))
	{
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Main::Clone_GameObject(void* pArg)
{
	CUI_Main* pInstance = new CUI_Main();
	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Main::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);


	CUI::Free();
}

HRESULT CUI_Main::Ready_Component()
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

	m_pShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Merge_Deffered");
	NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
		return E_FAIL;


	Safe_Release(pManagement);
	return S_OK;
}
