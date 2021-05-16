#include "framework.h"
#include "MyRect.h"
#include "Management.h"
#include "UAV.h"


CMyRect::CMyRect()
	: CGameObject()
{
}

CMyRect::CMyRect(const CMyRect& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMyRect::Ready_Prototype()
{


	return S_OK;
}

HRESULT CMyRect::Ready_GameObject(void* pArg)
{

	//FLAG tFlag = *(FLAG*)pArg;
	
	if (FAILED(Ready_Component()))
		return E_FAIL;
	if (FAILED(CreateInputLayout()))
		return E_FAIL;

	m_pTransformCom->SetUp_RotationX(XMConvertToRadians(90.f));
	/*_vec3 vPos = tFlag.vPos;
	m_pTransformCom->Set_StateInfo(CTransform::STATE_POSITION, &vPos);*/
	m_pTransformCom->Scaling(10.f, 10.f, 1.f);

	//m_iNum = tFlag.iNum;


	return S_OK;
}

_int CMyRect::Update_GameObject(const _float& fTimeDelta)
{
	if (m_fTempTime >= 1000.f)
	{
		if (!m_IsFix)
		{
			if (m_tRep.m_arrInt[0])
			{
				g_iRedNum++;
			}
			else if (m_tRep.m_arrInt[1])
			{
				g_iBlueNum++;
			}
		}
		m_IsFix = true;
	}

	return _int();
}

_int CMyRect::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONEALPHA, this)))
		return -1;

	m_fDeltaTime = fTimeDelta;
	//m_fTempTime += fTimeDelta * 100.f;
	//if (m_fTempTime <= 1000.f)
	//{
	//	m_tRep.m_arrInt[2] = 0;
	//	m_tRep.m_arrFloat[0] += fTimeDelta;
	//}
	//else
	//{
	//	m_tRep.m_arrInt[2] = 1;
	//	m_tRep.m_arrFloat[0] -= fTimeDelta;
	//}
	//

	return _int();
}

void CMyRect::Render_GameObject()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();


	MAINPASS tMainPass = {};
	_matrix matWorld = m_pTransformCom->Get_Matrix();
	_matrix matView = CCamera_Manager::GetInstance()->GetMatView();
	_matrix matProj = CCamera_Manager::GetInstance()->GetMatProj();

	m_pShaderCom[0]->SetUp_OnShader(matWorld, matView, matProj, tMainPass);
	_uint iOffset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffset, CONST_REGISTER::b0);



	if (m_iNum == 0)
		CDevice::GetInstance()->SetTextureToShader(pManagement->Get_UAV(L"UAV_Default")->GetSRV().Get(), TEXTURE_REGISTER::t0);
	else if (m_iNum == 1)
		CDevice::GetInstance()->SetTextureToShader(pManagement->Get_UAV(L"UAV_Default_1")->GetSRV().Get(), TEXTURE_REGISTER::t0);
	else if (m_iNum == 2)
		CDevice::GetInstance()->SetTextureToShader(pManagement->Get_UAV(L"UAV_Default_2")->GetSRV().Get(), TEXTURE_REGISTER::t0);
	else if (m_iNum == 3)
		CDevice::GetInstance()->SetTextureToShader(pManagement->Get_UAV(L"UAV_Default_3")->GetSRV().Get(), TEXTURE_REGISTER::t0);
	else if (m_iNum == 4)
		CDevice::GetInstance()->SetTextureToShader(pManagement->Get_UAV(L"UAV_Default_4")->GetSRV().Get(), TEXTURE_REGISTER::t0);
	CDevice::GetInstance()->UpdateTable();




	CDevice::GetInstance()->ClearDummyDesc_CS();
	{
		CTransform* pTransform_Red = (CTransform*)CManagement::GetInstance()->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
			L"Layer_Orc02", L"Com_Transform", 0);

		CTransform* pTransform_Blue = (CTransform*)CManagement::GetInstance()->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
			L"Layer_Orc04", L"Com_Transform", 0);

		_vec3 vThisPos = *m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION);
		_vec3 vRedPos = *pTransform_Red->Get_StateInfo(CTransform::STATE_POSITION);
		_vec3 vBluePos = *pTransform_Blue->Get_StateInfo(CTransform::STATE_POSITION);

		_vec3 vLen_Red = vThisPos - vRedPos;
		_vec3 vLen_Blue = vThisPos - vBluePos;

		_uint iLen_Red = Vector3_::Length(vLen_Red);
		_uint iLen_Blue = Vector3_::Length(vLen_Blue);

		CServer_Manager* server = CServer_Manager::GetInstance();
		if (nullptr == server)
			return;
		server->AddRef();

		if (server->Get_Red(m_iLayerIdx) && !(server->Get_Blue(m_iLayerIdx)))
		{
			m_tRep.m_arrInt[0] = 1;
			m_eCurTeam = TEAM::TEAM_RED;
		}
		if (!(server->Get_Red(m_iLayerIdx)) && server->Get_Blue(m_iLayerIdx))
		{
			m_tRep.m_arrInt[1] = 1;
			m_eCurTeam = TEAM::TEAM_BLUE;
		}
		if (server->Get_Red(m_iLayerIdx) && server->Get_Blue(m_iLayerIdx))
		{
			m_tRep.m_arrInt[1] = 0;
			m_eCurTeam = TEAM::TEAM_END;
		}
		if (!(server->Get_Red(m_iLayerIdx)) && !(server->Get_Blue(m_iLayerIdx)))
		{
			m_tRep.m_arrInt[0] = 0;
			m_eCurTeam = TEAM::TEAM_END;
		}


 		iOffset = CManagement::GetInstance()->GetConstantBuffer((_uint)CONST_REGISTER::b8)->SetData((void*)&m_tRep);
		CDevice::GetInstance()->SetUpContantBufferToShader_CS(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->GetCBV().Get(), iOffset, CONST_REGISTER::b8);
		if(m_iNum ==0 )
			CDevice::GetInstance()->SetUpUAVToRegister(pManagement->Get_UAV(L"UAV_Default"), UAV_REGISTER::u0);
		else if (m_iNum == 1)
			CDevice::GetInstance()->SetUpUAVToRegister(pManagement->Get_UAV(L"UAV_Default_1"), UAV_REGISTER::u0);
		else if (m_iNum == 2)
			CDevice::GetInstance()->SetUpUAVToRegister(pManagement->Get_UAV(L"UAV_Default_2"), UAV_REGISTER::u0);
		else if (m_iNum == 3)
			CDevice::GetInstance()->SetUpUAVToRegister(pManagement->Get_UAV(L"UAV_Default_3"), UAV_REGISTER::u0);
		else if (m_iNum == 4)
			CDevice::GetInstance()->SetUpUAVToRegister(pManagement->Get_UAV(L"UAV_Default_4"), UAV_REGISTER::u0);

		if ((m_tRep.m_arrInt[0] ^ m_tRep.m_arrInt[1]))
		{
			if (!m_IsFix)
			{
				m_tRep.m_arrFloat[0] += m_fDeltaTime;
				m_fTempTime += m_fDeltaTime * 100.f;
				m_IsTemp = false;
				m_eCurTeam;
				if (m_eCurTeam != m_ePreTeam)
				{
					m_fDeltaTime = 0.f;
					m_fTempTime = 0.f;
					m_IsTemp = true;
					m_tRep.m_arrFloat[0] = 0.f;
					switch (m_eCurTeam)
					{
					case TEAM::TEAM_RED:
						m_eCurTeam = TEAM::TEAM_RED;
						break;
					case TEAM::TEAM_BLUE:
						m_eCurTeam = TEAM::TEAM_BLUE;
						break;
					case TEAM::TEAM_END:
						m_eCurTeam = TEAM::TEAM_END;
						break;
					default:
						break;
					}
					m_ePreTeam = m_eCurTeam;

				}

				m_tRep.m_arrInt[3] = (_uint)m_IsTemp;
				m_pShaderCom[1]->UpdateData_CS();

				if (m_iNum == 0)
					CManagement::GetInstance()->Get_UAV(L"UAV_Default")->Dispatch(1, 1000, 1);
				else if (m_iNum == 1)
					CManagement::GetInstance()->Get_UAV(L"UAV_Default_1")->Dispatch(1, 1000, 1);
				else if (m_iNum == 2)
					CManagement::GetInstance()->Get_UAV(L"UAV_Default_2")->Dispatch(1, 1000, 1);
				else if (m_iNum == 3)
					CManagement::GetInstance()->Get_UAV(L"UAV_Default_3")->Dispatch(1, 1000, 1);
				else if (m_iNum == 4)
					CManagement::GetInstance()->Get_UAV(L"UAV_Default_4")->Dispatch(1, 1000, 1);
			}
	
		}
		else
		{
			if (m_eCurTeam != m_ePreTeam)
			{
				m_fDeltaTime = 0.f;
				m_fTempTime = 0.f;
				m_IsTemp = true;
				m_tRep.m_arrFloat[0] = 0.f;
				switch (m_eCurTeam)
				{
				case TEAM::TEAM_RED:
					m_eCurTeam = TEAM::TEAM_RED;
					break;
				case TEAM::TEAM_BLUE:
					m_eCurTeam = TEAM::TEAM_BLUE;
					break;
				case TEAM::TEAM_END:
					m_eCurTeam = TEAM::TEAM_END;
					break;
				default:
					break;
				}
				m_ePreTeam = m_eCurTeam;
			}

			if (!m_IsFix)
			{
				m_tRep.m_arrFloat[0] = 0.f;
				m_fDeltaTime = 0.f;
				m_IsTemp = true;
				m_tRep.m_arrInt[3] = (_uint)m_IsTemp;
				m_pShaderCom[1]->UpdateData_CS();
				if (m_iNum == 0)
					CManagement::GetInstance()->Get_UAV(L"UAV_Default")->Dispatch(1, 1000, 1);
				else if (m_iNum == 1)
					CManagement::GetInstance()->Get_UAV(L"UAV_Default_1")->Dispatch(1, 1000, 1);
				else if (m_iNum == 2)
					CManagement::GetInstance()->Get_UAV(L"UAV_Default_2")->Dispatch(1, 1000, 1);
				else if (m_iNum == 3)
					CManagement::GetInstance()->Get_UAV(L"UAV_Default_3")->Dispatch(1, 1000, 1);
				else if (m_iNum == 4)
					CManagement::GetInstance()->Get_UAV(L"UAV_Default_4")->Dispatch(1, 1000, 1);
			}

		}
		Safe_Release(server);
	}
	

	m_pBufferCom->Render_VIBuffer();
	Safe_Release(pManagement);
}

HRESULT CMyRect::CreateInputLayout()
{
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });


	if (FAILED(m_pShaderCom[0]->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::LESS, SHADER_TYPE::SHADER_DEFFERED)))
		return E_FAIL;


	return S_OK;
}

CMyRect* CMyRect::Create()
{
	CMyRect* pInstance = new CMyRect();

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MessageBox(0, L"CMyRect Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMyRect::Clone_GameObject(void* pArg, _uint iIdx)
{
	CMyRect* pInstance = new CMyRect(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MessageBox(0, L"CMyRect Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	m_iLayerIdx = iIdx;
	return pInstance;
}

void CMyRect::Free()
{

	Safe_Release(m_pBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom[0]);
	Safe_Release(m_pShaderCom[1]);

	CGameObject::Free();
}

HRESULT CMyRect::Ready_Component(_uint iNum)
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

	m_pShaderCom[0] = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_UI");
	NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Shader0", m_pShaderCom[0])))
		return E_FAIL;

	if (iNum == 0)
	{
		m_pShaderCom[1] = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Compute_Default");
		NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
		if (FAILED(Add_Component(L"Com_Shader1", m_pShaderCom[1])))
			return E_FAIL;
	}
	else if (iNum == 1)
	{
		m_pShaderCom[1] = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Compute_Default_1");
		NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
		if (FAILED(Add_Component(L"Com_Shader1", m_pShaderCom[1])))
			return E_FAIL;
	}
	else if (iNum == 2)
	{
		m_pShaderCom[1] = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Compute_Default_2");
		NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
		if (FAILED(Add_Component(L"Com_Shader1", m_pShaderCom[1])))
			return E_FAIL;
	}
	else if (iNum == 3)
	{
		m_pShaderCom[1] = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Compute_Default_3");
		NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
		if (FAILED(Add_Component(L"Com_Shader1", m_pShaderCom[1])))
			return E_FAIL;
	}
	else if (iNum == 4)
	{
		m_pShaderCom[1] = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Compute_Default_4");
		NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
		if (FAILED(Add_Component(L"Com_Shader1", m_pShaderCom[1])))
			return E_FAIL;
	}



	Safe_Release(pManagement);

	return S_OK;
}
