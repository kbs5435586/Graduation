#include "framework.h"
#include "UI_StatusBar.h"
#include "Management.h"

CUI_StatusBar::CUI_StatusBar()
{
}

CUI_StatusBar::CUI_StatusBar(const CUI_StatusBar& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_StatusBar::Ready_Prototype()
{
	return S_OK;
}

HRESULT CUI_StatusBar::Ready_GameObject(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;
	if (FAILED(CreateInputLayout()))
		return E_FAIL;


	m_pTransformCom->SetUp_Speed(10.f, XMConvertToRadians(90.f));
	m_fX = 0.0f;
	m_fY = 0.0f;
	//m_fZ = .0f;

	m_fSizeX = 100.0f;
	m_fSizeY = 100.0f;
	//m_fSizeZ = 100.0f;

	return S_OK;
}

_int CUI_StatusBar::Update_GameObject(const _float& fTimeDelta)
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return -1;
	pManagement->AddRef();

	if (pManagement->Key_Pressing(KEY_LEFT))
	{
		m_fX -= 10;
		//m_pTransformCom->Rotation_Y(fTimeDelta);
	}
	if (pManagement->Key_Pressing(KEY_RIGHT))
	{
		m_fX +=10;
		//m_pTransformCom->Rotation_Z(.1f);
		//m_pTransformCom->Rotation_Y(.01f);
		//m_fY = ((_vec3*)pArg)->y;
		//m_pTransformCom->Rotation_X(.01f);
	}
	if (pManagement->Key_Pressing(KEY_UP))
	{
		//if(m_fSizeY <= 100)
		m_fY -= 10;
		//m_pTransformCom->BackWard(fTimeDelta);

	}
	//else
	//{
	//	//if(m_fSizeY >= 10)
	//		//m_fY -= 10;
	//}
	if (pManagement->Key_Pressing(KEY_DOWN))
	{
		m_fY += 10;
		//m_pTransformCom->Go_Straight(fTimeDelta);
	}
	if (pManagement->Key_Pressing(KEY_LBUTTON))
	{
		GetCursorPos(&MousePos);
		ScreenToClient(g_hWnd, &MousePos);

		m_fX = (_float)MousePos.x - m_fSizeX;
		m_fY = (_float)MousePos.y + m_fSizeY;
		//m_fX = pManagement->Mouse_Down(KEY_LBUTTON).x;
		//m_fY = pManagement->Mouse_Down(KEY_LBUTTON).y;
		//m_fZ = pManagement->Mouse_Down(KEY_LBUTTON).z;
		//m_fSizeY -= 10;	
	}





	Safe_Release(pManagement);
	return _int();
}

_int CUI_StatusBar::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (m_pRendererCom != nullptr)
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
			return E_FAIL;
	}

	return _int();
}

void CUI_StatusBar::Render_GameObject()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();


	MAINPASS tMainPass = {};
	_matrix matWorld = m_pTransformCom->Get_Matrix();
	_matrix matView = m_pTransformCom->Get_Matrix();
	//_matrix matWorld = Matrix_::Identity();
	//_matrix matView = Matrix_::Identity();
	_matrix matProj = CCamera_Manager::GetInstance()->GetMatOrtho();

	matWorld._11 = m_fSizeX;
	matWorld._22 = m_fSizeY;
	//matWorld._33 = m_fSizeZ;
	matWorld._41 = m_fX + m_fSizeX - (WINCX >> 1);
	matWorld._42 = m_fSizeY -m_fY + (WINCY >> 1);
	//matWorld._43 = m_fZ + m_fSizeZ - (WINCX >> 1);


	m_pShaderCom->SetUp_OnShader(matWorld, matView, matProj, tMainPass);
	_uint iOffset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);

	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(0)->GetCBV().Get(), iOffset, CONST_REGISTER::b0);
	//CDevice::GetInstance()->SetTextureToShader(m_pTextureCom->GetSRV(), 0, TEXTURE_REGISTER::t0);
	CDevice::GetInstance()->UpdateTable();


	m_pBufferCom->Render_VIBuffer();
	Safe_Release(pManagement);
}

HRESULT CUI_StatusBar::CreateInputLayout()
{
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

	if (FAILED(m_pShaderCom->Create_Shader(vecDesc, RS_TYPE::SKYBOX)))
		return E_FAIL;

	return S_OK;
}

CUI_StatusBar* CUI_StatusBar::Create()
{
	CUI_StatusBar* pInstance = new CUI_StatusBar();
	if (FAILED(pInstance->Ready_Prototype()))
	{
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_StatusBar::Clone_GameObject(void* pArg)
{
	CUI_StatusBar* pInstance = new CUI_StatusBar();
	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_StatusBar::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pBufferCom);
	//Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);


	CGameObject::Free();
}

HRESULT CUI_StatusBar::Ready_Component()
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

	m_pBufferCom = (CBuffer_RectTex*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Buffer_RectCol");
	NULL_CHECK_VAL(m_pBufferCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Buffer", m_pBufferCom)))
		return E_FAIL;

	m_pShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Default");
	NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
		return E_FAIL;

	//m_pTextureCom = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Bricks");
	//NULL_CHECK_VAL(m_pTextureCom, E_FAIL);
	//if (FAILED(Add_Component(L"Com_Texture", m_pTextureCom)))
	//	return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}
