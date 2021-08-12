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
	//_vec3 vPos = tFlag.vPos;
	//m_pTransformCom->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
	//m_pTransformCom->Scaling(10.f, 10.f, 10.f);
	m_pTransformCom->Scaling(4.f, 4.f, 4.f);
	//m_iNum = tFlag.iNum;

	return S_OK;
}

_int CMyRect::Update_GameObject(const _float& fTimeDelta)
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return -1;


	CGameObject* pTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_Player", g_iPlayerIdx);
	_vec3 sTemp = *dynamic_cast<CTransform*>(pTemp->Get_ComponentPointer(L"Com_Transform"))->Get_StateInfo(CTransform::STATE_POSITION);
	//dynamic_cast<CPlayer*>(pTemp).get
	m_pTransformCom->Set_StateInfo(CTransform::STATE_POSITION, &sTemp);

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

	CBuffer_Terrain_Height* pTerrainBuffer = (CBuffer_Terrain_Height*)pManagement->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE, L"Layer_Terrain", L"Com_Buffer");
	if (nullptr == pTerrainBuffer)
		return -1;

	_float		fY = pTerrainBuffer->Compute_HeightOnTerrain(m_pTransformCom);

	m_pTransformCom->Set_PositionY(120.f);

	

	return _int();
}

_int CMyRect::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	//if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_MAP, this)))
	//	return -1;

	m_fDeltaTime += fTimeDelta;
	if (m_fDeltaTime > 10.f)
		m_fDeltaTime = 0;

	return _int();
}

void CMyRect::Render_GameObject()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();


	REP tRep = {};
	tRep.m_arrFloat[0] = m_fDeltaTime;

	MAINPASS tMainPass = {};
	_matrix matWorld = m_pTransformCom->Get_Matrix();
	_matrix matView = CCamera_Manager::GetInstance()->GetMatView();
	_matrix matProj = CCamera_Manager::GetInstance()->GetMatProj();

	m_pShaderCom->SetUp_OnShader(matWorld, matView, matProj, tMainPass);
	_uint iOffset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffset, CONST_REGISTER::b0);

	

	CTransform* pTransform_Red = (CTransform*)CManagement::GetInstance()->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
		L"Layer_Player", L"Com_Transform", 0);

	CTransform* pTransform_Blue = (CTransform*)CManagement::GetInstance()->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
		L"Layer_NPC", L"Com_Transform", 0);

	_vec3 vThisPos = *m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION);
	_vec3 vRedPos = *pTransform_Red->Get_StateInfo(CTransform::STATE_POSITION);
	_vec3 vBluePos = *pTransform_Blue->Get_StateInfo(CTransform::STATE_POSITION);

	_vec3 vLen_Red = vThisPos - vRedPos;
	_vec3 vLen_Blue = vThisPos - vBluePos;

	_uint iLen_Red = Vector3_::Length(vLen_Red);
	_uint iLen_Blue = Vector3_::Length(vLen_Blue);

	if (iLen_Red < 30.f)
	{
		m_tRep.m_arrInt[0] = 1;
		m_eCurTeam = TEAM::TEAM_RED;
	}
	if (iLen_Blue < 30.f)
	{
		m_tRep.m_arrInt[1] = 1;
		m_eCurTeam = TEAM::TEAM_BLUE;
	}
	if (iLen_Blue > 30.f)
	{
		m_tRep.m_arrInt[1] = 0;
		m_eCurTeam = TEAM::TEAM_END;
	}
	if (iLen_Red > 30.f)
	{
		m_tRep.m_arrInt[0] = 0;
		m_eCurTeam = TEAM::TEAM_END;
	}


 	iOffset = CManagement::GetInstance()->GetConstantBuffer((_uint)CONST_REGISTER::b8)->SetData((void*)&tRep);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->GetCBV().Get(), iOffset, CONST_REGISTER::b8);
	
	CDevice::GetInstance()->SetTextureToShader(m_pTextureCom->GetSRV(), TEXTURE_REGISTER::t0);
	CDevice::GetInstance()->UpdateTable();

	
	


	
	m_pBufferCom->Render_VIBuffer();
	Safe_Release(pManagement);
}

void CMyRect::Render_GameObject_Map()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();


	REP tRep = {};
	tRep.m_arrFloat[0] = m_fDeltaTime;

	MAINPASS tMainPass = {};
	_matrix matWorld = m_pTransformCom->Get_Matrix();
	_matrix matView = CCamera_Manager::GetInstance()->GetMapMatView();
	_matrix matProj = CCamera_Manager::GetInstance()->GetMapMatProj();

	m_pShaderCom->SetUp_OnShader(matWorld, matView, matProj, tMainPass);
	_uint iOffset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffset, CONST_REGISTER::b0);



	CTransform* pTransform_Red = (CTransform*)CManagement::GetInstance()->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
		L"Layer_Player", L"Com_Transform", 0);

	CTransform* pTransform_Blue = (CTransform*)CManagement::GetInstance()->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
		L"Layer_NPC", L"Com_Transform", 0);

	_vec3 vThisPos = *m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION);
	_vec3 vRedPos = *pTransform_Red->Get_StateInfo(CTransform::STATE_POSITION);
	_vec3 vBluePos = *pTransform_Blue->Get_StateInfo(CTransform::STATE_POSITION);

	_vec3 vLen_Red = vThisPos - vRedPos;
	_vec3 vLen_Blue = vThisPos - vBluePos;

	_uint iLen_Red = Vector3_::Length(vLen_Red);
	_uint iLen_Blue = Vector3_::Length(vLen_Blue);

	if (iLen_Red < 30.f)
	{
		m_tRep.m_arrInt[0] = 1;
		m_eCurTeam = TEAM::TEAM_RED;
	}
	if (iLen_Blue < 30.f)
	{
		m_tRep.m_arrInt[1] = 1;
		m_eCurTeam = TEAM::TEAM_BLUE;
	}
	if (iLen_Blue > 30.f)
	{
		m_tRep.m_arrInt[1] = 0;
		m_eCurTeam = TEAM::TEAM_END;
	}
	if (iLen_Red > 30.f)
	{
		m_tRep.m_arrInt[0] = 0;
		m_eCurTeam = TEAM::TEAM_END;
	}


	iOffset = CManagement::GetInstance()->GetConstantBuffer((_uint)CONST_REGISTER::b8)->SetData((void*)&tRep);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->GetCBV().Get(), iOffset, CONST_REGISTER::b8);

	CDevice::GetInstance()->SetTextureToShader(m_pTextureCom->GetSRV(), TEXTURE_REGISTER::t0);
	CDevice::GetInstance()->UpdateTable();






	m_pBufferCom->Render_VIBuffer();
	Safe_Release(pManagement);
}

HRESULT CMyRect::CreateInputLayout()
{
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });


	if (FAILED(m_pShaderCom->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::LESS, SHADER_TYPE::SHADER_DEFFERED, BLEND_TYPE::ALPHABLEND)))
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
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	
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

	m_pShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_UI_Flag");
	NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Shader0", m_pShaderCom)))
		return E_FAIL;

	m_pTextureCom = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_RoundButtonUI");
	NULL_CHECK_VAL(m_pTextureCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Texture", m_pTextureCom)))
		return E_FAIL;
	//m_pTextureCom = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_ClassUI");
	//NULL_CHECK_VAL(m_pTextureCom, E_FAIL);
	//if (FAILED(Add_Component(L"Com_Texture", m_pTextureCom)))
	//	return E_FAIL;


	Safe_Release(pManagement);

	return S_OK;
}
