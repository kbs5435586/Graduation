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
	if (pArg)
	{
		m = *(_bool*)pArg;
	}

	if (FAILED(Ready_Component()))
		return E_FAIL;
	if (FAILED(CreateInputLayout()))
		return E_FAIL;

	if (m)
	{
		m_fX = WINCX / 2;
		m_fY = WINCY / 2;

		m_fSizeX = 900;
		m_fSizeY = 900;
	}
	else
	{
		m_fX = WINCX / 2;
		m_fY = WINCY / 2;

		m_fSizeX = WINCX;
		m_fSizeY = WINCY;
	}
	
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
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this)))
			return E_FAIL;
	}
	if (GetAsyncKeyState('7'))
		m_iTemp = 0;
	if (GetAsyncKeyState('8'))
		m_iTemp = 1;
	if (GetAsyncKeyState('9'))
		m_iTemp = 2;
	if (GetAsyncKeyState('0'))
		m_iTemp = 3;
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

	REP		tRep = {};
	tRep.m_arrInt[0] = m_iTemp;


	m_pShaderCom->SetUp_OnShader(matWorld, matView, matProj, tMainPass);
	_uint iOffset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);


	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(0)->GetCBV().Get(), iOffset, CONST_REGISTER::b0);
	iOffset = CManagement::GetInstance()->GetConstantBuffer((_uint)CONST_REGISTER::b8)->SetData((void*)&tRep);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->GetCBV().Get(), iOffset, CONST_REGISTER::b8);

	ComPtr<ID3D12DescriptorHeap>	pDiffuseTex = pManagement->Get_RTT((_uint)MRT::MRT_DEFFERD)->Get_RTT(0)->pRtt->GetSRV().Get();
	ComPtr<ID3D12DescriptorHeap>	pShadeTex = pManagement->Get_RTT((_uint)MRT::MRT_LIGHT)->Get_RTT(0)->pRtt->GetSRV().Get();
	ComPtr<ID3D12DescriptorHeap>	pSpecularTex = pManagement->Get_RTT((_uint)MRT::MRT_LIGHT)->Get_RTT(1)->pRtt->GetSRV().Get();
	ComPtr<ID3D12DescriptorHeap>	pBlurTex = pManagement->Get_RTT((_uint)MRT::MRT_BLUR)->Get_RTT(0)->pRtt->GetSRV().Get();

	CDevice::GetInstance()->SetTextureToShader(pDiffuseTex.Get(), TEXTURE_REGISTER::t0);
	CDevice::GetInstance()->SetTextureToShader(pShadeTex.Get(), TEXTURE_REGISTER::t1);
	CDevice::GetInstance()->SetTextureToShader(pSpecularTex.Get(), TEXTURE_REGISTER::t2);
	CDevice::GetInstance()->SetTextureToShader(pBlurTex.Get(), TEXTURE_REGISTER::t3);



	CDevice::GetInstance()->UpdateTable();


	m_pBufferCom->Render_VIBuffer();
	Safe_Release(pManagement);
}

HRESULT CUI_Main::CreateInputLayout()
{
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

	if (FAILED(m_pShaderCom->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::NO_DEPTHTEST_NO_WRITE, SHADER_TYPE::SHADER_LIGHT)))
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

CGameObject* CUI_Main::Clone_GameObject(void* pArg , _uint iIdx)
{
	CUI_Main* pInstance = new CUI_Main();
	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		Safe_Release(pInstance);
	}
	m_iLayerIdx = iIdx;
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
