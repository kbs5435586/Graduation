#include "framework.h"
#include "UI_ClassTap.h"
#include "UI_Button.h"
#include "UI_CharInterface.h"
#include "Management.h"
#include "Layer.h"
#include "UAV.h"

CUI_ClassTap::CUI_ClassTap()
{
}

CUI_ClassTap::CUI_ClassTap(const CUI_ClassTap& rhs)
{
}

HRESULT CUI_ClassTap::Ready_Prototype()
{
	return S_OK;
}

HRESULT CUI_ClassTap::Ready_GameObject(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;
	if (FAILED(CreateInputLayout()))
		return E_FAIL;

	m_fX = WINCX / 2;
	m_fY = WINCY / 2;

	m_fSizeX = 450.f;
	m_fSizeY = 650.f;

	CManagement::GetInstance()->Subscribe(m_pObserverCom);
	CManagement::GetInstance()->ReNotify(DATA_TYPE::DATA_NPC);
	////////////////////
	//버튼
	m_button = new CUI_Button;
	m_button->Ready_GameObject();
	m_button->setObs(m_pObserverCom);
	//다른 화면
	m_charInter = new CUI_CharInterface;
	m_charInter->Ready_GameObject();
	
	//다른 UI들에게 켜짐을 알리는
	CManagement::GetInstance()->Add_Data(DATA_TYPE::DATA_BOOL, &m_cansee);

	which = 0;
	CManagement::GetInstance()->Add_Data(DATA_TYPE::DATA_WHICH, &which);

	return S_OK;
}

_int CUI_ClassTap::Update_GameObject(const _float& fTimeDelta)
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return -1;
	pManagement->AddRef();

	
	if (GetAsyncKeyState('I'))
	{
		m_cansee = !m_cansee;

		CManagement::GetInstance()->Notify(DATA_TYPE::DATA_BOOL, &m_cansee);
	}

	//버튼
	m_button->Update_GameObject(fTimeDelta, m_IsTap, 0);
	
	Safe_Release(pManagement);
	return _int();
}

_int CUI_ClassTap::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (m_pRendererCom != nullptr)
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
			return E_FAIL;
	}

	return _int();
}

void CUI_ClassTap::Render_GameObject()
{
	if (m_cansee)
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

		m_pInvenShaderCom->SetUp_OnShader(matWorld, matView, matProj, tMainPass);
		m_charInter->Render_GameObject(m_pInvenShaderCom, m_pBufferCom, m_pTextureCom);
		m_pShaderCom->SetUp_OnShader(matWorld, matView, matProj, tMainPass);
		
		_uint iOffset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffset, CONST_REGISTER::b0);
		CDevice::GetInstance()->SetTextureToShader(m_pTextureCom->GetSRV(), TEXTURE_REGISTER::t0);
		CDevice::GetInstance()->UpdateTable();
		m_pBufferCom->Render_VIBuffer();
		///////////////////////////////////////////////////////////////
		
		m_button->Render_GameObject(m_pShaderCom, m_pBufferCom, m_pTextureCom);
		
		Safe_Release(pManagement);
	}
}

HRESULT CUI_ClassTap::CreateInputLayout()
{
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });


	if (FAILED(m_pShaderCom->Create_Shader(vecDesc)))
		return E_FAIL;
	if (FAILED(m_pInvenShaderCom->Create_Shader(vecDesc)))
		return E_FAIL;

	return S_OK;
}

CUI_ClassTap* CUI_ClassTap::Create()
{
	CUI_ClassTap* pInstance = new CUI_ClassTap();
	if (FAILED(pInstance->Ready_Prototype()))
	{
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_ClassTap::Clone_GameObject(void* pArg, _uint iIdx)
{
	CUI_ClassTap* pInstance = new CUI_ClassTap();
	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		Safe_Release(pInstance);
	}
	m_iLayerIdx = iIdx;
	return pInstance;
}

void CUI_ClassTap::Free()
{
	//CManagement::GetInstance()->UnSubscribe(m_pObserverCom);

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pInvenShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pObserverCom);
	//Safe_Release(m_pCompute_ShaderCom);

	delete[] a;
	delete m_button;
	delete m_charInter;
	CGameObject::Free();
}

HRESULT CUI_ClassTap::Ready_Component()
{
	CManagement* pManagement = CManagement::GetInstance();
	NULL_CHECK_VAL(pManagement, E_FAIL);
	pManagement->AddRef();

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

	m_pInvenShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_UUU");
	NULL_CHECK_VAL(m_pInvenShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_InvenShader", m_pInvenShaderCom)))
		return E_FAIL;

	m_pTextureCom = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Grass");
	NULL_CHECK_VAL(m_pTextureCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Texture", m_pTextureCom)))
		return E_FAIL;

	m_pObserverCom = (CObserver*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Observer");
	NULL_CHECK_VAL(m_pObserverCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Observer", m_pObserverCom)))
		return E_FAIL;


	Safe_Release(pManagement);
	return S_OK;
}

//CUI_CharTap::CUI_CharTap()
//{
//}
//
//CUI_CharTap::CUI_CharTap(const CUI_ClassTap& rhs)
//{
//}
//
//
//HRESULT CUI_CharTap::Ready_GameObject(void* pArg)
//{
//	if (FAILED(CreateInputLayout()))
//		return E_FAIL;
//
//	m_fSizeX = 50.f;
//	m_fSizeY = 50.f;
//	meshnum = 0;
//
//	a = m_pObserverCom->GetIntArrInfo(0);
//
//	return S_OK;
//}
//
//_int CUI_CharTap::Update_GameObject(const _float& fTimeDelta, _bool b[], int idx)
//{
//	CManagement* pManagement = CManagement::GetInstance();
//	if (nullptr == pManagement)
//		return -1;
//	pManagement->AddRef();
//
//	
//	
//	for(int i=0;i<tapnum;++i)
//		++a;
//	//CManagement::GetInstance()->Notify(DATA_TYPE::DATA_INT_ARRAY, m_pObserverCom->GetIntArrInfo(0));
//		
//	if (b[idx] == true)
//		m_fSizeX = 60.f;
//	else
//		m_fSizeX = 50.f;
//	
//		
//	if (pManagement->Key_Pressing(KEY_LBUTTON))
//	{
//		GetCursorPos(&MousePos);
//		ScreenToClient(g_hWnd, &MousePos);
//
//		if (MousePos.x > m_fX - (m_fSizeX/2) && MousePos.x < m_fX + (m_fSizeX / 2))
//		{
//			if (MousePos.y > m_fY - (m_fSizeY / 2) && MousePos.y < m_fY + (m_fSizeY / 2))
//			{
//				for (int i = 0; i < 5; ++i)
//				{
//					if (i == idx)	
//						b[i] = true;
//					else
//						b[i] = false;
//				}
//				*temp = tapnum;
//				CManagement::GetInstance()->Notify(DATA_TYPE::DATA_INT_WHICH, temp);
//				CManagement::GetInstance()->Notify(DATA_TYPE::DATA_INT, &(*a));
//
//			}
//		}
//	}
//
//	Safe_Release(pManagement);
//	return _int();
//}
//
//_int CUI_CharTap::LastUpdate_GameObject(const _float& fTimeDelta)
//{
//	return _int();
//}
//
//void CUI_CharTap::Render_GameObject(CShader* shader, CBuffer_RectTex* buffer, CTexture* texture)
//{
//	CManagement* pManagement = CManagement::GetInstance();
//	if (nullptr == pManagement)
//		return;
//	pManagement->AddRef();
//
//
//	MAINPASS	tMainPass = {};
//
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
//	shader->SetUp_OnShader(matWorld, matView, matProj, tMainPass);
//	_uint iOffset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
//	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffset, CONST_REGISTER::b0);
//	CDevice::GetInstance()->SetTextureToShader(texture->GetSRV(), TEXTURE_REGISTER::t0);
//	CDevice::GetInstance()->UpdateTable();
//	buffer->Render_VIBuffer();
//
//	Safe_Release(pManagement);
//}
//
//void CUI_CharTap::setSize(_float x, _float y)
//{
//	m_fSizeX = x;
//	m_fSizeY = y;
//}
//
//void CUI_CharTap::setPos(_float x, _float y)
//{
//	m_fX = x;
//	m_fY = y;
//}
//
//void CUI_CharTap::setMeshnum(_int num)
//{
//	meshnum = num;
//}
//
//void CUI_CharTap::setTapnum(_int num)
//{
//	tapnum = num;
//}
//
//void CUI_CharTap::setTemp(_int* num)
//{
//	temp = num;
//}
//
//void CUI_CharTap::setObserver(CObserver* obs)
//{
//	m_pObserverCom = obs;
//}
//
//_float CUI_CharTap::getSizeX()
//{
//	return m_fSizeX;
//}
//
//_float CUI_CharTap::getSizeY()
//{
//	return m_fSizeY;
//}
//
//_float CUI_CharTap::getX()
//{
//	return m_fX;
//}
//
//_float CUI_CharTap::getY()
//{
//	return m_fY;
//}
//
//bool CUI_CharTap::getActive()
//{
//	return isActive;
//}


