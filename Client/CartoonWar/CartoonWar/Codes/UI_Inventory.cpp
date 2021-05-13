#include "framework.h"
#include "UI_Inventory.h"
#include "Management.h"


CUI_Inventory::CUI_Inventory()
{
}

CUI_Inventory::CUI_Inventory(const CUI_Inventory& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Inventory::Ready_Prototype()
{
	canSee = false;
	return S_OK;
}

HRESULT CUI_Inventory::Ready_GameObject(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;
	if (FAILED(CreateInputLayout()))
		return E_FAIL;

	
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return E_FAIL;
	pManagement->AddRef();

	m_fX = 400.f;
	m_fY = 300.f;

	m_fSizeX = 400.f;
	m_fSizeY = 400.f;

	MousePos = {0 , 0};

	Pos = { (LONG)m_fX ,(LONG)m_fY };
	//CManagement::GetInstance()->Add_Data(DATA_TYPE::DATA_UI_INFO, &Pos);

	int X[4]{ -141,-48,47 ,141}, Y[4]{ -2,53,109 ,166};

	Pos = POINT{ X[0],Y[0] };
	IPos[0] = { -141 ,-2 };
	IPos[1] = { -48 ,53 };
	IPos[2] = { 47 ,109 };
	IPos[3] = { 141 ,166 };

	//if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_InventorySpace", (_uint)SCENEID::SCENE_STAGE, L"Layer_UI", nullptr, &Pos)))
	//	return E_FAIL;
	//Pos = POINT{ X[0],Y[1] };
	//if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_UI_InventorySpace", (_uint)SCENEID::SCENE_STAGE, L"Layer_UI", nullptr, &Pos)))
	//	return E_FAIL;



	
	Safe_Release(pManagement);
	return S_OK;
}

_int CUI_Inventory::Update_GameObject(const _float& fTimeDelta)
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return -1;
	pManagement->AddRef();

	
	if (pManagement->Key_Pressing(KEY_LBUTTON))
	{
		GetCursorPos(&MousePos);
		ScreenToClient(g_hWnd, &MousePos);

		if (MousePos.x > m_fX - (200.f) && MousePos.x < m_fX + (200.f))
		{
			if (MousePos.y > m_fY - (200.f) && MousePos.y < m_fY + (200.f))
			{
				m_fX = (_float)MousePos.x;
				m_fY = (_float)MousePos.y;
				Pos = { (LONG)m_fX ,(LONG)m_fY };
				
				
			//	pManagement->Notify(DATA_TYPE::DATA_UI_INFO, &Pos);
			}
		}
	}
	if (pManagement->Key_Down(KEY_SPACE))
	{
		canSee = !canSee;
	}

	Safe_Release(pManagement);
	return _int();
}

_int CUI_Inventory::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (m_pRendererCom != nullptr)
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
			return E_FAIL;
	}

	//CManagement::GetInstance()->Notify(DATA_TYPE::DATA_UI_INFO, &Pos);

	return _int();
}

void CUI_Inventory::Render_GameObject()
{
	if (canSee)
	{
		CManagement* pManagement = CManagement::GetInstance();
		if (nullptr == pManagement)
			return;
		pManagement->AddRef();


		MAINPASS tMainPass = {};

		_matrix matWorld = Matrix_::Identity();
		_matrix matView = Matrix_::Identity();
		_matrix matProj = CCamera_Manager::GetInstance()->GetMatOrtho();


		for (int i = 0; i < 4; ++i)
		{
			matWorld._11 = 84;
			matWorld._22 = 50;

			//matWorld._41 = IPos[i].x - (WINCX >> 1);
			//matWorld._42 = -(IPos[i].y) + (WINCY >> 1);
			matWorld._41 = m_fX + IPos[i].x - (WINCX >> 1);
			for (int j = 0; j < 4; ++j)
			{
				matWorld._42 = -(m_fY + IPos[j].y) + (WINCY >> 1);

				m_pShaderCom->SetUp_OnShader(matWorld, matView, matProj, tMainPass);
				_uint iOffset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);

				CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(0)->GetCBV().Get(), iOffset, CONST_REGISTER::b0);

				//if문으로 아이템 정보를 관리하는 벡터의 크기의 수만큼 텍스쳐를 출력하게 한다.
				//CDevice::GetInstance()->SetTextureToShader(m_pITextureCom->GetSRV(), TEXTURE_REGISTER::t0);
				CDevice::GetInstance()->UpdateTable();

				m_pBufferCom->Render_VIBuffer();
			}
		}

		matWorld._11 = 150;
		matWorld._22 = 125;

		matWorld._41 = m_fX - (WINCX >> 1);
		matWorld._42 = -(m_fY - 100) + (WINCY >> 1);

		m_pShaderComT->SetUp_OnShader(matWorld, matView, matProj, tMainPass);
		_uint iOffset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);

		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(0)->GetCBV().Get(), iOffset, CONST_REGISTER::b0);

		ComPtr<ID3D12DescriptorHeap>	pTextureDesc = pManagement->Get_RTT((_uint)MRT::MRT_DEFFERD)->Get_RTT(5)->pRtt->GetSRV().Get();

		CDevice::GetInstance()->SetTextureToShader(pTextureDesc.Get(), TEXTURE_REGISTER::t0);

		CDevice::GetInstance()->UpdateTable();

		m_pBufferCom->Render_VIBuffer();

		/////////////////////////////////////////////

		matWorld._11 = m_fSizeX;
		matWorld._22 = m_fSizeY;

		matWorld._41 = m_fX - (WINCX >> 1);
		matWorld._42 = -m_fY + (WINCY >> 1);

		m_pShaderCom->SetUp_OnShader(matWorld, matView, matProj, tMainPass);
		iOffset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);

		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(0)->GetCBV().Get(), iOffset, CONST_REGISTER::b0);
		CDevice::GetInstance()->SetTextureToShader(m_pTextureCom->GetSRV(), TEXTURE_REGISTER::t0);
		CDevice::GetInstance()->UpdateTable();

		m_pBufferCom->Render_VIBuffer();

		Safe_Release(pManagement);
	}
	
}

HRESULT CUI_Inventory::CreateInputLayout()
{
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

	if (FAILED(m_pShaderCom->Create_Shader(vecDesc, RS_TYPE::DEFAULT)))
		return E_FAIL;
	if (FAILED(m_pShaderComT->Create_Shader(vecDesc, RS_TYPE::DEFAULT)))
		return E_FAIL;

	return S_OK;
}

CUI_Inventory* CUI_Inventory::Create()
{
	CUI_Inventory* pInstance = new CUI_Inventory();

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MessageBox(0, L"CUI_Inventory Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Inventory::Clone_GameObject(void* pArg, _uint iIdx)
{
	CUI_Inventory* pInstance = new CUI_Inventory();
	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		Safe_Release(pInstance);
	}
	m_iLayerIdx = iIdx;
	return pInstance;
}

void CUI_Inventory::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pITextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pShaderComT);

	CGameObject::Free();
}

HRESULT CUI_Inventory::Ready_Component()
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

	m_pShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Texture");
	NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
		return E_FAIL;
	m_pShaderComT = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_UUU");
	NULL_CHECK_VAL(m_pShaderComT, E_FAIL);
	if (FAILED(Add_Component(L"Com_ShaderT", m_pShaderComT)))
		return E_FAIL;


	m_pTextureCom = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Inventory");
	NULL_CHECK_VAL(m_pTextureCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Texture", m_pTextureCom)))
		return E_FAIL;
	m_pITextureCom = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Grass");
	NULL_CHECK_VAL(m_pITextureCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_ITexture", m_pITextureCom)))
		return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}
