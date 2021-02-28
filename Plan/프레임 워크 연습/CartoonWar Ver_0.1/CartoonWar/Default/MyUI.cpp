#include "framework.h"
#include "MyUI.h"
#include "Management.h"

CMyUI::CMyUI()
{
}

CMyUI::CMyUI(const CMyUI& rhs)
	:CUI(rhs)
{
}

HRESULT CMyUI::Ready_Prototype()
{
	return S_OK;
}

HRESULT CMyUI::Ready_GameObject(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;
	if (FAILED(CreateInputLayout()))
		return E_FAIL;

	Invisible = FALSE;
	m_fX = 275.f;
	m_fY = 525.f;

	m_fSizeX = 300.f;
	m_fSizeY = 150.f;
	//_vec3 vPos = _vec3(10.f, 5.f, 10.f);
	//m_pTransformCom->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
	//m_pTransformCom->SetUp_RotationX(XMConvertToRadians(90.f));
	//m_pTransformCom->Scaling(_vec3(5.f, 5.f, 5.f));
	return S_OK;
}

_int CMyUI::Update_GameObject(const _float& fTimeDelta)
{
	CManagement* pManagement = CManagement::GetInstance();

	if (nullptr == pManagement)
		return -1;
	pManagement->AddRef();


	if (pManagement->Key_Down(KEY_RIGHT))
	{
		Invisible = !Invisible;
	}
	if (Invisible)
	{
		if (pManagement->Key_Pressing(KEY_LBUTTON))
		{
			GetCursorPos(&MousePos);
			ScreenToClient(g_hWnd, &MousePos);

			m_fX = (_float)MousePos.x;
			m_fY = (_float)MousePos.y;
		}
	}
	


	Safe_Release(pManagement);
	return _int();
}

_int CMyUI::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return -1;

	return _int();
}

void CMyUI::Render_GameObject()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();

	
	//	MAINPASS tMainPass = {};
	//
	//	_matrix matWorld = Matrix_::Identity();
	//	_matrix matView = Matrix_::Identity();
	//	_matrix matProj = CCamera_Manager::GetInstance()->GetMatOrtho();
	//
	//	matWorld._11 = m_fSizeX;
	//	matWorld._22 = m_fSizeY;
	//
	//	matWorld._41 = m_fX - (WINCX >> 1);
	//	matWorld._42 = -m_fY + (WINCY >> 1);
	//
	//
	//	m_pShaderCom->SetUp_OnShader(matWorld, matView, matProj, tMainPass);
	//	_uint iOffset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
	//
	//
	//	ComPtr<ID3D12DescriptorHeap>	pTextureDesc = pManagement->Get_RTT((_uint)MRT::MRT_DEFFERD)->Get_RTT(0)->pRtt->GetSRV().Get();
	//	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(0)->GetCBV().Get(), iOffset, CONST_REGISTER::b0);
	//	CDevice::GetInstance()->SetTextureToShader(pTextureDesc.Get(), TEXTURE_REGISTER::t0);
	//	CDevice::GetInstance()->UpdateTable();
	//
	//if (Invisible)
	//{
	//	m_pBufferCom->Render_VIBuffer();
	//}
	
	Safe_Release(pManagement);
}

HRESULT CMyUI::CreateInputLayout()
{
	//vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	//vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	//vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	//
	//if (FAILED(m_pShaderCom->Create_Shader(vecDesc)))
	//	return E_FAIL;
	//

	return S_OK;
}

CMyUI* CMyUI::Create()
{
	CMyUI* pInstance = new CMyUI();

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MessageBox(0, L"CMyUI Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMyUI::Clone_GameObject(void* pArg)
{
	CMyUI* pInstance = new CMyUI(*this);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CMyUI Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMyUI::Free()
{
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);

	CUI::Free();
}

HRESULT CMyUI::Ready_Component()
{
	CManagement* pManagement = CManagement::GetInstance();
	NULL_CHECK_VAL(pManagement, E_FAIL);
	pManagement->AddRef();


	
	
	//m_pTransformCom = (CTransform*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Transform");
	//NULL_CHECK_VAL(m_pTransformCom, E_FAIL);
	//if (FAILED(Add_Component(L"Com_Transform", m_pTransformCom)))
	//	return E_FAIL;
	//
	//m_pRendererCom = (CRenderer*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Renderer");
	//NULL_CHECK_VAL(m_pRendererCom, E_FAIL);
	//if (FAILED(Add_Component(L"Com_Renderer", m_pRendererCom)))
	//	return E_FAIL;
	//
	//m_pBufferCom = (CBuffer_UITex*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Buffer_UITex");
	//NULL_CHECK_VAL(m_pBufferCom, E_FAIL);
	//if (FAILED(Add_Component(L"Com_Buffer", m_pBufferCom)))
	//	return E_FAIL;
	//
	////Component_Shader_Deffered
	////Component_Shader_Reflect
	//m_pShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Deffered");
	//NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
	//if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
	//	return E_FAIL;
	//m_pTextureCom = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Bricks");
	//NULL_CHECK_VAL(m_pTextureCom, E_FAIL);
	//if (FAILED(Add_Component(L"Com_Texture", m_pTextureCom)))
	//	return E_FAIL;

	
	Safe_Release(pManagement);

	return S_OK;
}
