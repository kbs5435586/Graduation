#include "framework.h"
#include "UI_MiniMap.h"
#include "Management.h"
#include "Layer.h"
#include "UAV.h"

CUI_MiniMap::CUI_MiniMap()
	: CUI()
{
}

CUI_MiniMap::CUI_MiniMap(const CUI_MiniMap& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_MiniMap::Ready_Prototype()
{
	return S_OK;
}

HRESULT CUI_MiniMap::Ready_GameObject(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;
	if (FAILED(CreateInputLayout()))
		return E_FAIL;


	m_fX = 200.f;
	m_fY = 200.f;

	m_fSizeX = 300.f;
	m_fSizeY = 300.f;
	//m_tInfo = CManagement::GetInstance()->Get_Layer((_uint)SCENEID::SCENE_STAGE, L"Layer_Player")->Get_BackObject()->GetInfo();
	return S_OK;
}

_int CUI_MiniMap::Update_GameObject(const _float& fTimeDelta)
{
	//m_tInfo = CManagement::GetInstance()->Get_Layer((_uint)SCENEID::SCENE_STAGE, L"Layer_Player")->Get_BackObject()->GetInfo();

	m_fCurCnt += fTimeDelta*10.f;
	if (m_fCurCnt >= m_iMaxTexCnt)
		m_fCurCnt = 0;
	return _int();
}

_int CUI_MiniMap::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (m_pRendererCom != nullptr)
	{
		//if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		//	return E_FAIL;
	}

	return _int();
}

void CUI_MiniMap::Render_GameObject()
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

	iOffset = CManagement::GetInstance()->GetConstantBuffer((_uint)CONST_REGISTER::b8)->SetData((void*)&m_tRep);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->GetCBV().Get(), iOffset, CONST_REGISTER::b8);

	CDevice::GetInstance()->SetTextureToShader(m_pTextureCom->GetSRV((_uint)m_fCurCnt), TEXTURE_REGISTER::t0);
	CDevice::GetInstance()->UpdateTable();




	m_pBufferCom->Render_VIBuffer();
	Safe_Release(pManagement);
}

HRESULT CUI_MiniMap::CreateInputLayout()
{
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });


	if (FAILED(m_pShaderCom->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::LESS, SHADER_TYPE::SHADER_FORWARD, BLEND_TYPE::ALPHABLEND)))
		return E_FAIL;


	return S_OK;
}

CUI_MiniMap* CUI_MiniMap::Create()
{
	CUI_MiniMap* pInstance = new CUI_MiniMap();
	if (FAILED(pInstance->Ready_Prototype()))
	{
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_MiniMap::Clone_GameObject(void* pArg, _uint iIdx)
{
	CUI_MiniMap* pInstance = new CUI_MiniMap();
	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		Safe_Release(pInstance);
	}
	m_iLayerIdx = iIdx;
	return pInstance;
}

void CUI_MiniMap::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);


	CUI::Free();
}

HRESULT CUI_MiniMap::Ready_Component()
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

	//Component_Texture_HPBar
	m_pTextureCom = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_MinimapT");
	NULL_CHECK_VAL(m_pTextureCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Texture", m_pTextureCom)))
		return E_FAIL;
	Safe_Release(pManagement);
	return S_OK;
}