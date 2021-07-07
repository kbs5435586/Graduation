#include "framework.h"
#include "UI_Skill.h"
#include "Management.h"
#include "Layer.h"
#include "UAV.h"

CUI_Skill::CUI_Skill()
{
}

CUI_Skill::CUI_Skill(const CUI_Skill& rhs)
{
}

HRESULT CUI_Skill::Ready_Prototype()
{
	return S_OK;
}

HRESULT CUI_Skill::Ready_GameObject(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;
	if (FAILED(CreateInputLayout()))
		return E_FAIL;


	m_fX = 150.f;
	m_fY = 150.f;

	m_fSizeX = 300.f;
	m_fSizeY = 300.f;
	
	m_CoolTime = 0;

	CManagement::GetInstance()->Add_Data(DATA_TYPE::DATA_SKILL, &m_Active);
	return S_OK;
}

_int CUI_Skill::Update_GameObject(const _float& fTimeDelta)
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return -1;
	pManagement->AddRef();


	if (pManagement->Key_Down(KEY_LBUTTON))
	{
		GetCursorPos(&MousePos);
		ScreenToClient(g_hWnd, &MousePos);

		if (MousePos.x > m_fX - (m_fSizeX / 2) && MousePos.x < m_fX + (m_fSizeX / 2))
		{
			if (MousePos.y > m_fY - (m_fSizeY / 2) && MousePos.y < m_fY + (m_fSizeY / 2))
			{
				m_fSizeX = 250.f;
				m_fSizeY = 250.f;
				IsDown = true;
			}
		}
	}


	if (IsDown)
	{
		if (pManagement->Key_Up(KEY_LBUTTON))
		{
			m_fSizeX = 300.f;
			m_fSizeY = 300.f;


			CManagement::GetInstance()->Notify(DATA_TYPE::DATA_WHICH, &m_Active);

			IsDown = false;
			
		}
	}

	m_CoolTime += fTimeDelta;
	if (m_CoolTime > 10.f)
		m_CoolTime = 0;

	Safe_Release(pManagement);
	return _int();
}

_int CUI_Skill::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (m_pRendererCom != nullptr)
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
			return E_FAIL;
	}
	return _int();
}

void CUI_Skill::Render_GameObject()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();


	MAINPASS	tMainPass = {};


	REP tRep = {};
	tRep.m_arrFloat[0] = m_CoolTime;
	


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

	_int iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->SetData((void*)&tRep);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
		(_uint)CONST_REGISTER::b8)->GetCBV().Get(), iOffeset, CONST_REGISTER::b8);

	CDevice::GetInstance()->SetTextureToShader(m_pTextureCom->GetSRV(), TEXTURE_REGISTER::t0);
	CDevice::GetInstance()->UpdateTable();
	m_pBufferCom->Render_VIBuffer();

	Safe_Release(pManagement);
}

HRESULT CUI_Skill::CreateInputLayout()
{
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });


	if (FAILED(m_pShaderCom->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::LESS)))
		return E_FAIL;

	return S_OK;
}

CUI_Skill* CUI_Skill::Create()
{
	CUI_Skill* pInstance = new CUI_Skill();
	if (FAILED(pInstance->Ready_Prototype()))
	{
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Skill::Clone_GameObject(void* pArg, _uint iIdx)
{
	CUI_Skill* pInstance = new CUI_Skill();
	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		Safe_Release(pInstance);
	}
	m_iLayerIdx = iIdx;
	return pInstance;
}

void CUI_Skill::Free()
{//CManagement::GetInstance()->UnSubscribe(m_pObserverCom);

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pBlendShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pObserverCom);


	CGameObject::Free();
}

HRESULT CUI_Skill::Ready_Component()
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

	m_pShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_UI_Skill");
	NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
		return E_FAIL;

	m_pBlendShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_UI");
	NULL_CHECK_VAL(m_pBlendShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_BlendShader", m_pBlendShaderCom)))
		return E_FAIL;

	m_pTextureCom = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_ClassUI");
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
