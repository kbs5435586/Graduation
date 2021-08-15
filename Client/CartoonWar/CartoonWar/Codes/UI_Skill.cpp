#include "framework.h"
#include "UI_Skill.h"
#include "Management.h"
#include "Layer.h"
#include "UAV.h"
#include "Player.h"

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
	if (pArg)
	{
		pArgTemp = *(XMFLOAT3*)pArg;
	}
	if (FAILED(Ready_Component()))
		return E_FAIL;
	if (FAILED(CreateInputLayout()))
		return E_FAIL;

	m_fX = WINCX - pArgTemp.x;
	m_fY = WINCY - pArgTemp.y;
	//m_fX = 150.f;
	//m_fY = 150.f;

	m_fSizeX = 150.f;
	m_fSizeY = 150.f;
	
	
	m_MaxCoolTime = 5.f;
	m_CoolTime = m_MaxCoolTime;
	m_Active = false;


	return S_OK;
}

_int CUI_Skill::Update_GameObject(const _float& fTimeDelta)
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return -1;
	pManagement->AddRef();

	//m_strCoolTime = to_string(m_CoolTime);
	//if (FAILED(pManagement->Create_Font_Buffer(L"IP", m_strCoolTime.c_str(), m_fSizeX, m_fSizeY)))
	//	return E_FAIL;
	
	//10이 꽉찬 상태
	CGameObject* tOne = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_Player", g_iPlayerIdx);
	pClass = (tOne)->GetClass();
	if (pClass == CLASS::CLASS_WORKER || pClass == CLASS::CLASS_CAVALRY || pClass == CLASS::CLASS_INFANTRY || 
		pClass == CLASS::CLASS_SPEARMAN || pClass == CLASS(2) || pClass == CLASS(4))
	{

	}
	else
	{
		if (m_Active)
		{
			if (StartTime)
			{
				m_SkillActive = false;
				
				m_CoolTime += fTimeDelta;

				if (m_CoolTime > m_MaxCoolTime)
				{
					m_Active = false;
					IsOnetouch = false;
					StartTime = false;
					m_CoolTime = m_MaxCoolTime;
				}
			}
		}
		else
		{
			if (pArgTemp.z == 0)
			{
				if (pManagement->Key_Down(KEY_Z))
				{

					m_fSizeX = m_fSizeX - 10.f;
					m_fSizeY = m_fSizeY - 10.f;
					IsDown = true;

				}

				if (IsDown)
				{
					if (pManagement->Key_Up(KEY_Z))
					{
						m_fSizeX = m_fSizeX + 10.f;
						m_fSizeY = m_fSizeY + 10.f;

						m_CoolTime = 0.f;
						m_Active = true;
						m_SkillActive = true;
						IsDown = false;
					}
				}
			}
			else if (pArgTemp.z == 1)
			{
				if (pManagement->Key_Down(KEY_X))
				{

					m_fSizeX = m_fSizeX - 10.f;
					m_fSizeY = m_fSizeY - 10.f;
					IsDown = true;

				}

				if (IsDown)
				{
					if (pManagement->Key_Up(KEY_X))
					{
						m_fSizeX = m_fSizeX + 10.f;
						m_fSizeY = m_fSizeY + 10.f;

						m_CoolTime = 0.f;
						m_Active = true;
						m_SkillActive = true;
						IsDown = false;
					}
				}
			}
		}
	}
	

	Safe_Release(pManagement);
	return _int();
}

_int CUI_Skill::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (m_pRendererCom != nullptr)
	{
		if (pClass == CLASS::CLASS_WORKER || pClass == CLASS::CLASS_CAVALRY || pClass == CLASS::CLASS_INFANTRY ||
			pClass == CLASS::CLASS_SPEARMAN || pClass == CLASS(2) || pClass == CLASS(4))
		{

		}
		else
		{
			if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
				return E_FAIL;
		}
		
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
	//10 = 현재 쿨타임 / (최대쿨타임/2);
	tRep.m_arrFloat[0] = m_CoolTime;
	tRep.m_arrFloat[1] = static_cast<float> (m_MaxCoolTime)/ 10;
	
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

	if (pClass == CLASS::CLASS_MAGE || pClass == CLASS::CLASS_MMAGE)
	{
		if(pArgTemp.z == 0)
			CDevice::GetInstance()->SetTextureToShader(m_pTextureCom->GetSRV(2), TEXTURE_REGISTER::t0);
		if(pArgTemp.z == 1)
			CDevice::GetInstance()->SetTextureToShader(m_pTextureCom->GetSRV(0), TEXTURE_REGISTER::t0);
	}
	else if (pClass == CLASS::CLASS_ARCHER)
	{
		if (pArgTemp.z == 0)
			CDevice::GetInstance()->SetTextureToShader(m_pTextureCom->GetSRV(3), TEXTURE_REGISTER::t0);
		if (pArgTemp.z == 1)
			CDevice::GetInstance()->SetTextureToShader(m_pTextureCom->GetSRV(1), TEXTURE_REGISTER::t0);
	}

	
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

	m_pTextureCom = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Skill");
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
