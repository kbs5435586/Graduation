#include "framework.h"
#include "Management.h"
#include "NPC.h"
#include "UI_OnHead.h"
#include "UI_OnHeadBack.h"
CNPC::CNPC()
	: CGameObject()

{
}

CNPC::CNPC(const CNPC& rhs)
	: CGameObject(rhs)
{
}

HRESULT CNPC::Ready_Prototype()
{
	return S_OK;
}

HRESULT CNPC::Ready_GameObject(void* pArg)
{
	if (pArg)
	{
		m_tPlayer = *(PLAYER*)pArg;
	}
	m_IsClone = true;
	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(CreateInputLayout()))
		return E_FAIL;

	//Compute_Matrix();
	//_vec3 vPos = { _float(rand() % 50),0.f,_float(rand() % 50) };
	_vec3 vPos = {75.f,0.f,75.f };
	m_pTransformCom->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
	m_pTransformCom->SetUp_Speed(10.f, XMConvertToRadians(90.f));
	m_pTransformCom->Scaling(0.1f, 0.1f, 0.1f);

	m_tInfo = INFO(100, 1, 1, 0);
	for (_uint i = 0; i < (_uint)CLASS::CLASS_END; ++i)
	{
		if (m_pAnimCom[i] == nullptr)
			continue;
		m_pAnimCom[i]->SetBones(m_pMeshCom[i]->GetBones());
		m_pAnimCom[i]->SetAnimClip(m_pMeshCom[i]->GetAnimClip());
		m_pAnimCom[i]->LateInit();
	}
	//_vec3 vColliderSize = { 40.f ,160.f,40.f };
	_vec3 vColliderSize = { 40.f ,160.f,40.f };
	m_pColiider[0]->Clone_ColliderBox(m_pTransformCom, vColliderSize);
	m_pColiider[1]->Clone_ColliderBox(m_pTransformCom, vColliderSize);

	m_eCurClass = CLASS::CLASS_WORKER;
	m_iCurAnimIdx = 0;
	m_iPreAnimIdx = 100;

	m_pCurAnimCom = m_pAnimCom[(_uint)m_eCurClass];
	m_pCurMeshCom = m_pMeshCom[(_uint)m_eCurClass];



	m_pUI_OnHead = CUI_OnHead::Create();
	if (nullptr == m_pUI_OnHead)
		return E_FAIL;
	if (FAILED(m_pUI_OnHead->Ready_GameObject((void*)&vPos)))
		return E_FAIL;

	m_pUI_OnHeadBack = CUI_OnHeadBack::Create();
	if (nullptr == m_pUI_OnHeadBack)
		return E_FAIL;
	if (FAILED(m_pUI_OnHeadBack->Ready_GameObject((void*)&vPos)))
		return E_FAIL;

	

	return S_OK;
}

_int CNPC::Update_GameObject(const _float& fTimeDelta)
{
	//m_pColiider[0]->Update_Collider(m_pTransformCom, m_eCurClass);
	//m_pColiider[1]->Update_Collider(m_pTransformCom);

	m_pUI_OnHead->Update_GameObject(fTimeDelta);
	m_pUI_OnHead->SetPosition(*m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION), m_eCurClass);
	m_pUI_OnHead->SetInfo(m_tInfo);

	m_pUI_OnHeadBack->Update_GameObject(fTimeDelta);
	m_pUI_OnHeadBack->SetPosition(*m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION), m_eCurClass);
	m_pUI_OnHeadBack->SetInfo(m_tInfo);
	m_pTransformCom->Set_PositionY(0.f);


	Change_Class();
	//Obb_Collision();
	Combat(fTimeDelta);
	Death(fTimeDelta);
	if (m_tInfo.fHP <= 0.f)
	{
		if (!m_IsDeadMotion)
		{
			_uint iRand = rand() % 2;
			if (iRand == 0)
				m_iCurAnimIdx = m_iDeathMotion[0];
			else
				m_iCurAnimIdx = m_iDeathMotion[1];
			m_IsDeadMotion = true;
		}

	}

	Set_Animation(fTimeDelta);
	if (m_pCurAnimCom->Update(m_vecAnimCtrl[m_iCurAnimIdx], fTimeDelta) && m_IsOnce)
	{
		m_iCurAnimIdx = 0;
		m_IsOnce = false;
	}

	if (m_IsDead)
		return DEAD_OBJ;
	return NO_EVENT;
}

_int CNPC::LastUpdate_GameObject(const _float& fTimeDelta)
{
	CServer_Manager* server = CServer_Manager::GetInstance();
	if (nullptr == server)
		return -1;
	server->AddRef();

	if (nullptr == m_pRendererCom)
		return -1;

	if (server->Get_ShowNPC(m_iLayerIdx))
	{
		m_pUI_OnHead->LastUpdate_GameObject(fTimeDelta);
		m_pUI_OnHeadBack->LastUpdate_GameObject(fTimeDelta);
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONEALPHA, this)))
			return -1;
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this)))
			return -1;

		m_tInfo.fHP = server->Get_NpcHP(m_iLayerIdx);
		m_iCurAnimIdx = server->Get_AnimNPC(m_iLayerIdx);
	}
	
	Set_Animation(fTimeDelta);
	if (m_pCurAnimCom->Update(m_vecAnimCtrl[m_iCurAnimIdx], fTimeDelta) && m_IsOnce)
	{
		m_iCurAnimIdx = 0;
		m_IsOnce = false;
	}

	Safe_Release(server);
	return _int();
}

void CNPC::Render_GameObject()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();


	_uint iSubsetNum = m_pCurMeshCom->GetSubsetNum();
	for (_uint i = 0; i < iSubsetNum; ++i)
	{
		MAINPASS tMainPass = {};
		_matrix matWorld = m_pTransformCom->Get_Matrix();
		_matrix matView = CCamera_Manager::GetInstance()->GetMatView();
		_matrix matProj = CCamera_Manager::GetInstance()->GetMatProj();

		REP tRep = {};
		tRep.m_arrInt[0] = 1;
		tRep.m_arrInt[1] = m_pCurAnimCom->GetBones()->size();

		m_pShaderCom->SetUp_OnShader(matWorld, matView, matProj, tMainPass);

		_uint iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
			(_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);

		iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->SetData((void*)&tRep);
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
			(_uint)CONST_REGISTER::b8)->GetCBV().Get(), iOffeset, CONST_REGISTER::b8);

		if (iSubsetNum >= 2)
		{
			if (i == 0)
				CDevice::GetInstance()->SetTextureToShader(m_pTextureCom[0], TEXTURE_REGISTER::t0, (_uint)HORSE::HORSE_A);
			else
				CDevice::GetInstance()->SetTextureToShader(m_pTextureCom[1], TEXTURE_REGISTER::t0, (_uint)m_tPlayer.eColor);
		}
		else
		{
			CDevice::GetInstance()->SetTextureToShader(m_pTextureCom[1], TEXTURE_REGISTER::t0, (_uint)m_tPlayer.eColor);
		}

		m_pCurAnimCom->UpdateData(m_pCurMeshCom, m_pComputeShaderCom);
		CDevice::GetInstance()->UpdateTable();
		m_pCurMeshCom->Render_Mesh(i);
	}


	//m_pColiider[0]->Render_Collider();
	//m_pColiider[1]->Render_Collider();
	Safe_Release(pManagement);
}

void CNPC::Render_GameObject_Shadow()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();


	_uint iSubsetNum = m_pCurMeshCom->GetSubsetNum();
	for (_uint i = 0; i < iSubsetNum; ++i)
	{
		MAINPASS tMainPass = {};
		_matrix matWorld = m_pTransformCom->Get_Matrix();
		_matrix matView = CCamera_Manager::GetInstance()->GetShadowView();
		_matrix matProj = CCamera_Manager::GetInstance()->GetShadowMatProj();

		//_matrix matView = CCamera_Manager::GetInstance()->GetMatView();
		//_matrix matProj = CCamera_Manager::GetInstance()->GetMatProj();

		REP tRep = {};
		tRep.m_arrInt[0] = 1;
		tRep.m_arrInt[1] = m_pCurAnimCom->GetBones()->size();

		m_pShaderCom_Shadow->SetUp_OnShader(matWorld, matView, matProj, tMainPass);

		_uint iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
			(_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);

		iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->SetData((void*)&tRep);
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
			(_uint)CONST_REGISTER::b8)->GetCBV().Get(), iOffeset, CONST_REGISTER::b8);

		m_pCurAnimCom->UpdateData(m_pCurMeshCom, m_pComputeShaderCom);
		CDevice::GetInstance()->UpdateTable();
		m_pCurMeshCom->Render_Mesh(i);
	}
	Safe_Release(pManagement);
}

HRESULT CNPC::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc = {};
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 48, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 60, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 72, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 88, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

	if (FAILED(m_pShaderCom->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::LESS, SHADER_TYPE::SHADER_DEFFERED)))
		return E_FAIL;
	if (FAILED(m_pShaderCom_Shadow->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::LESS, SHADER_TYPE::SHADER_SHADOW)))
		return E_FAIL;
	return S_OK;
}

CNPC* CNPC::Create()
{
	CNPC* pInstance = new CNPC();

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MessageBox(0, L"CNPC Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;

}

CGameObject* CNPC::Clone_GameObject(void* pArg, _uint iIdx)
{
	CNPC* pInstance = new CNPC(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MessageBox(0, L"CNPC Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	m_iLayerIdx = iIdx;
	return pInstance;
}

void CNPC::Free()
{
	for (_uint i = 0; i < (_uint)CLASS::CLASS_END; ++i)
	{

		Safe_Release(m_pMeshCom[i]);
		Safe_Release(m_pAnimCom[i]);
	}
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pShaderCom_Shadow);
	Safe_Release(m_pComputeShaderCom);
	Safe_Release(m_pColiider[0]);
	Safe_Release(m_pColiider[1]);
	Safe_Release(m_pTextureCom[0]);
	Safe_Release(m_pTextureCom[1]);
	//Safe_Release(m_pNaviCom);


	Safe_Release(m_pUI_OnHead);
	Safe_Release(m_pUI_OnHeadBack);
	CGameObject::Free();
}

HRESULT CNPC::Ready_Component()
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

	m_pMeshCom[(_uint)CLASS::CLASS_WORKER] = (CMesh*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Undead_Worker");
	NULL_CHECK_VAL(m_pMeshCom[(_uint)CLASS::CLASS_WORKER], E_FAIL);
	if (FAILED(Add_Component(L"Com_Mesh_Worker", m_pMeshCom[(_uint)CLASS::CLASS_WORKER])))
		return E_FAIL;

	m_pMeshCom[(_uint)CLASS::CLASS_CAVALRY] = (CMesh*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Undead_Light_Cavalry");
	NULL_CHECK_VAL(m_pMeshCom[(_uint)CLASS::CLASS_CAVALRY], E_FAIL);
	if (FAILED(Add_Component(L"Com_Mesh_LightCavalry", m_pMeshCom[(_uint)CLASS::CLASS_CAVALRY])))
		return E_FAIL;

	m_pMeshCom[(_uint)CLASS::CLASS_CAVALRY + 1] = (CMesh*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Undead_Heavy_Carvalry");
	NULL_CHECK_VAL(m_pMeshCom[(_uint)CLASS::CLASS_CAVALRY + 1], E_FAIL);
	if (FAILED(Add_Component(L"Com_Mesh_HeavyCavalry", m_pMeshCom[(_uint)CLASS::CLASS_CAVALRY + 1])))
		return E_FAIL;

	m_pMeshCom[(_uint)CLASS::CLASS_INFANTRY] = (CMesh*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Undead_Light_Infantry");
	NULL_CHECK_VAL(m_pMeshCom[(_uint)CLASS::CLASS_INFANTRY], E_FAIL);
	if (FAILED(Add_Component(L"Com_Mesh_LightInfantry", m_pMeshCom[(_uint)CLASS::CLASS_INFANTRY])))
		return E_FAIL;

	m_pMeshCom[(_uint)CLASS::CLASS_INFANTRY + 1] = (CMesh*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Undead_Heavy_Infantry");
	NULL_CHECK_VAL(m_pMeshCom[(_uint)CLASS::CLASS_INFANTRY + 1], E_FAIL);
	if (FAILED(Add_Component(L"Com_Mesh_HeavyInfantry", m_pMeshCom[(_uint)CLASS::CLASS_INFANTRY + 1])))
		return E_FAIL;

	m_pMeshCom[(_uint)CLASS::CLASS_SPEARMAN] = (CMesh*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Undead_SpearMan");
	NULL_CHECK_VAL(m_pMeshCom[(_uint)CLASS::CLASS_SPEARMAN], E_FAIL);
	if (FAILED(Add_Component(L"Com_Mesh_Spearman", m_pMeshCom[(_uint)CLASS::CLASS_SPEARMAN])))
		return E_FAIL;

	m_pMeshCom[(_uint)CLASS::CLASS_MAGE] = (CMesh*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Undead_Mage");
	NULL_CHECK_VAL(m_pMeshCom[(_uint)CLASS::CLASS_MAGE], E_FAIL);
	if (FAILED(Add_Component(L"Com_Mesh_Mage", m_pMeshCom[(_uint)CLASS::CLASS_MAGE])))
		return E_FAIL;

	m_pMeshCom[(_uint)CLASS::CLASS_MMAGE] = (CMesh*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Undead_Mounted_Mage");
	NULL_CHECK_VAL(m_pMeshCom[(_uint)CLASS::CLASS_MMAGE], E_FAIL);
	if (FAILED(Add_Component(L"Com_Mesh_MountedMage", m_pMeshCom[(_uint)CLASS::CLASS_MMAGE])))
		return E_FAIL;

	m_pMeshCom[(_uint)CLASS::CLASS_ARCHER] = (CMesh*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Undead_Archer");
	NULL_CHECK_VAL(m_pMeshCom[(_uint)CLASS::CLASS_ARCHER], E_FAIL);
	if (FAILED(Add_Component(L"Com_Mesh_Archer", m_pMeshCom[(_uint)CLASS::CLASS_ARCHER])))
		return E_FAIL;



	m_pShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Toon");
	NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
		return E_FAIL;

	m_pComputeShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Compute_Animation");
	NULL_CHECK_VAL(m_pComputeShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_ComputeShader", m_pComputeShaderCom)))
		return E_FAIL;
	//Component_Shader_Shadow
	m_pShaderCom_Shadow = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Shadow");
	NULL_CHECK_VAL(m_pShaderCom_Shadow, E_FAIL);
	if (FAILED(Add_Component(L"Com_ShadowShader", m_pShaderCom_Shadow)))
		return E_FAIL;



	m_pAnimCom[(_uint)CLASS::CLASS_WORKER] = (CAnimator*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Animation");
	NULL_CHECK_VAL(m_pAnimCom[(_uint)CLASS::CLASS_WORKER], E_FAIL);
	if (FAILED(Add_Component(L"Com_Anim0", m_pAnimCom[(_uint)CLASS::CLASS_WORKER])))
		return E_FAIL;
	m_pAnimCom[(_uint)CLASS::CLASS_CAVALRY] = (CAnimator*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Animation");
	NULL_CHECK_VAL(m_pAnimCom[(_uint)CLASS::CLASS_CAVALRY], E_FAIL);
	if (FAILED(Add_Component(L"Com_Anim1", m_pAnimCom[(_uint)CLASS::CLASS_CAVALRY])))
		return E_FAIL;
	m_pAnimCom[(_uint)CLASS::CLASS_CAVALRY + 1] = (CAnimator*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Animation");
	NULL_CHECK_VAL(m_pAnimCom[(_uint)CLASS::CLASS_CAVALRY + 1], E_FAIL);
	if (FAILED(Add_Component(L"Com_Anim2", m_pAnimCom[(_uint)CLASS::CLASS_CAVALRY + 1])))
		return E_FAIL;
	m_pAnimCom[(_uint)CLASS::CLASS_INFANTRY] = (CAnimator*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Animation");
	NULL_CHECK_VAL(m_pAnimCom[(_uint)CLASS::CLASS_INFANTRY], E_FAIL);
	if (FAILED(Add_Component(L"Com_Anim3", m_pAnimCom[(_uint)CLASS::CLASS_INFANTRY])))
		return E_FAIL;
	m_pAnimCom[(_uint)CLASS::CLASS_INFANTRY + 1] = (CAnimator*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Animation");
	NULL_CHECK_VAL(m_pAnimCom[(_uint)CLASS::CLASS_INFANTRY + 1], E_FAIL);
	if (FAILED(Add_Component(L"Com_Anim4", m_pAnimCom[(_uint)CLASS::CLASS_INFANTRY + 1])))
		return E_FAIL;
	m_pAnimCom[(_uint)CLASS::CLASS_SPEARMAN] = (CAnimator*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Animation");
	NULL_CHECK_VAL(m_pAnimCom[(_uint)CLASS::CLASS_SPEARMAN], E_FAIL);
	if (FAILED(Add_Component(L"Com_Anim5", m_pAnimCom[(_uint)CLASS::CLASS_SPEARMAN])))
		return E_FAIL;
	m_pAnimCom[(_uint)CLASS::CLASS_MAGE] = (CAnimator*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Animation");
	NULL_CHECK_VAL(m_pAnimCom[(_uint)CLASS::CLASS_MAGE], E_FAIL);
	if (FAILED(Add_Component(L"Com_Anim6", m_pAnimCom[(_uint)CLASS::CLASS_MAGE])))
		return E_FAIL;
	m_pAnimCom[(_uint)CLASS::CLASS_MMAGE] = (CAnimator*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Animation");
	NULL_CHECK_VAL(m_pAnimCom[(_uint)CLASS::CLASS_MMAGE], E_FAIL);
	if (FAILED(Add_Component(L"Com_Anim7", m_pAnimCom[(_uint)CLASS::CLASS_MMAGE])))
		return E_FAIL;
	m_pAnimCom[(_uint)CLASS::CLASS_ARCHER] = (CAnimator*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Animation");
	NULL_CHECK_VAL(m_pAnimCom[(_uint)CLASS::CLASS_ARCHER], E_FAIL);
	if (FAILED(Add_Component(L"Com_Anim8", m_pAnimCom[(_uint)CLASS::CLASS_ARCHER])))
		return E_FAIL;


	m_pColiider[0] = (CCollider*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Collider_OBB");
	NULL_CHECK_VAL(m_pColiider[0], E_FAIL);
	if (FAILED(Add_Component(L"Com_Collider_OBB", m_pColiider[0])))
		return E_FAIL;

	m_pColiider[1] = (CCollider*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Collider_AABB");
	NULL_CHECK_VAL(m_pColiider[1], E_FAIL);
	if (FAILED(Add_Component(L"Com_Collider_AABB", m_pColiider[1])))
		return E_FAIL;

	m_pTextureCom[0] = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Horse");
	NULL_CHECK_VAL(m_pTextureCom[0], E_FAIL);
	if (FAILED(Add_Component(L"Com_Texture0", m_pTextureCom[0])))
		return E_FAIL;

	if (m_tPlayer.eSpecies == SPECIES::SPECIES_HUMAN)
	{
		m_pTextureCom[1] = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Human");
		NULL_CHECK_VAL(m_pTextureCom[1], E_FAIL);
		if (FAILED(Add_Component(L"Com_Texture1", m_pTextureCom[1])))
			return E_FAIL;
	}
	else
	{
		m_pTextureCom[1] = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Undead");
		NULL_CHECK_VAL(m_pTextureCom[1], E_FAIL);
		if (FAILED(Add_Component(L"Com_Texture1", m_pTextureCom[1])))
			return E_FAIL;
	}



	//m_pNaviCom = (CNavigation*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_NaviMesh_Test");
	//NULL_CHECK_VAL(m_pNaviCom, E_FAIL);
	//if (FAILED(Add_Component(L"Com_Navi", m_pNaviCom)))
	//	return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}

void CNPC::Set_Animation(const _float& fTimeDelta)
{
	if (m_iCurAnimIdx != m_iPreAnimIdx)
	{
		Attack(fTimeDelta);
		m_vecAnimCtrl[m_iCurAnimIdx].fCurTime = 0.f;
		m_iPreAnimIdx = m_iCurAnimIdx;
	}

}

void CNPC::Change_Class()
{
	if (m_eCurClass != m_ePreClass)
	{
		m_pCurAnimCom = m_pAnimCom[(_uint)m_eCurClass];
		m_pCurMeshCom = m_pMeshCom[(_uint)m_eCurClass];
		m_iCurAnimIdx = 0;
		DeathMontion_Init();
		AnimVectorClear();
		if (m_tPlayer.eSpecies == SPECIES::SPECIES_HUMAN)
		{
			switch (m_eCurClass)
			{
			case CLASS::CLASS_WORKER:
			{
				//idle		
				//walk		
				//run		
				//attack	
				//death a	
				//death b	
				//take damage
				m_vecAnimCtrl.push_back(AnimCtrl(0, 100, 0.000f, 3.333f));
				m_vecAnimCtrl.push_back(AnimCtrl(101, 136, 3.366f, 4.533f));
				m_vecAnimCtrl.push_back(AnimCtrl(137, 167, 4.566f, 5.566f));
				m_vecAnimCtrl.push_back(AnimCtrl(168, 193, 5.599f, 6.433f));
				m_vecAnimCtrl.push_back(AnimCtrl(194, 249, 6.466f, 8.300f));
				m_vecAnimCtrl.push_back(AnimCtrl(250, 300, 8.333f, 10.000f));
				m_vecAnimCtrl.push_back(AnimCtrl(301, 321, 10.033f, 10.699f));
				m_vOBB_Range[0] = { 20.f ,80.f,20.f };
				m_vOBB_Range[1] = { 30.f ,80.f,30.f };
				m_iCombatMotion[0] = 0;
				m_iCombatMotion[1] = 1;
				m_iCombatMotion[2] = 3;
			}
			break;
			case CLASS::CLASS_INFANTRY:
			{
				//	idle		
				//	walk		
				//	run			
				//	charge		
				//	combat idle	
				//	combat walk	
				//	attack a	
				//	attack b	
				//	take damage	
				//	death a		
				//	death b		
				m_vecAnimCtrl.push_back(AnimCtrl(0, 100, 0.00f, 3.333f));
				m_vecAnimCtrl.push_back(AnimCtrl(101, 137, 3.366f, 4.566f));
				m_vecAnimCtrl.push_back(AnimCtrl(138, 168, 4.599f, 5.599f));
				m_vecAnimCtrl.push_back(AnimCtrl(169, 194, 5.633f, 6.466f));
				m_vecAnimCtrl.push_back(AnimCtrl(195, 255, 6.500f, 8.500f));
				m_vecAnimCtrl.push_back(AnimCtrl(256, 292, 8.533f, 9.733f));
				m_vecAnimCtrl.push_back(AnimCtrl(293, 323, 9.766f, 10.766f));
				m_vecAnimCtrl.push_back(AnimCtrl(324, 354, 10.800f, 11.800f));
				m_vecAnimCtrl.push_back(AnimCtrl(355, 370, 11.833f, 12.333f));
				m_vecAnimCtrl.push_back(AnimCtrl(371, 420, 12.366f, 14.000f));
				m_vecAnimCtrl.push_back(AnimCtrl(421, 370, 14.033f, 15.666f));
				m_vOBB_Range[0] = { 20.f ,80.f,20.f };
				m_vOBB_Range[1] = { 30.f ,80.f,30.f };
				m_iCombatMotion[0] = 4;
				m_iCombatMotion[1] = 5;
				m_iCombatMotion[2] = 3;
			}
			break;
			case CLASS::CLASS_CAVALRY:
			{
				//idle 		
				//walk 		
				//run 		
				//charge 	
				//combat idle 
				//combat walk
				//combat hit a
				//combat hit b 
				//take damage 
				//death a 	
				//death b 	
				m_vecAnimCtrl.push_back(AnimCtrl(0, 100, 0.00f, 3.333f));
				m_vecAnimCtrl.push_back(AnimCtrl(101, 131, 3.366f, 4.366f));
				m_vecAnimCtrl.push_back(AnimCtrl(132, 156, 4.400f, 5.200f));
				m_vecAnimCtrl.push_back(AnimCtrl(157, 181, 5.233f, 6.033f));
				m_vecAnimCtrl.push_back(AnimCtrl(182, 242, 6.066f, 8.066f));
				m_vecAnimCtrl.push_back(AnimCtrl(243, 273, 8.099f, 9.099f));
				m_vecAnimCtrl.push_back(AnimCtrl(274, 314, 9.133f, 10.466f));
				m_vecAnimCtrl.push_back(AnimCtrl(315, 355, 10.500f, 11.833f));
				m_vecAnimCtrl.push_back(AnimCtrl(356, 371, 11.866f, 12.366f));
				m_vecAnimCtrl.push_back(AnimCtrl(372, 437, 12.400f, 14.566f));
				m_vecAnimCtrl.push_back(AnimCtrl(438, 503, 14.600f, 16.766f));
				m_vOBB_Range[0] = { 20.f ,120.f,60.f };
				m_vOBB_Range[1] = { 30.f ,120.f,70.f };
				m_iCombatMotion[0] = 4;
				m_iCombatMotion[1] = 5;
				m_iCombatMotion[2] = 3;
			}
			break;
			case CLASS::CLASS_SPEARMAN:
			{
				//		idle		
				//		walk		
				//		run			
				//		charge		
				//		combat idle	
				//		combat walk	
				//		attack		
				//		take damage	
				//		death a		
				//		death b		
				m_vecAnimCtrl.push_back(AnimCtrl(0, 100, 0.00f, 3.333f));
				m_vecAnimCtrl.push_back(AnimCtrl(101, 137, 3.366f, 4.566f));
				m_vecAnimCtrl.push_back(AnimCtrl(138, 168, 4.599f, 5.599f));
				m_vecAnimCtrl.push_back(AnimCtrl(169, 194, 5.633f, 6.466f));
				m_vecAnimCtrl.push_back(AnimCtrl(195, 255, 6.500f, 8.500f));
				m_vecAnimCtrl.push_back(AnimCtrl(256, 292, 8.533f, 9.733f));
				m_vecAnimCtrl.push_back(AnimCtrl(293, 323, 9.766f, 10.766f));
				m_vecAnimCtrl.push_back(AnimCtrl(324, 339, 10.800f, 11.300f));
				m_vecAnimCtrl.push_back(AnimCtrl(340, 390, 11.333f, 13.000f));
				m_vecAnimCtrl.push_back(AnimCtrl(391, 441, 13.033f, 14.699f));
				m_vOBB_Range[0] = { 20.f ,80.f,20.f };
				m_vOBB_Range[1] = { 30.f ,80.f,60.f };
				m_iCombatMotion[0] = 4;
				m_iCombatMotion[1] = 5;
				m_iCombatMotion[2] = 3;
			}
			break;
			case CLASS::CLASS_MAGE:
			{
				//	idle		
				//	walk		
				//	run			
				//	charge		
				//	combat idle	
				//	combat walk	
				//	attack a	
				//	attack b	
				//	take damage	
				//	death a		
				//	death b		
				//	cast a		
				//	cast b		
				//	cast c		
				m_vecAnimCtrl.push_back(AnimCtrl(0, 100, 0.00f, 3.333f));
				m_vecAnimCtrl.push_back(AnimCtrl(101, 137, 3.366f, 4.566f));
				m_vecAnimCtrl.push_back(AnimCtrl(138, 168, 4.599f, 5.599f));
				m_vecAnimCtrl.push_back(AnimCtrl(169, 194, 5.633f, 6.466f));
				m_vecAnimCtrl.push_back(AnimCtrl(195, 255, 6.500f, 8.500f));
				m_vecAnimCtrl.push_back(AnimCtrl(256, 291, 8.533f, 9.699f));
				m_vecAnimCtrl.push_back(AnimCtrl(292, 322, 9.733f, 10.733f));
				m_vecAnimCtrl.push_back(AnimCtrl(323, 353, 10.766f, 11.766f));
				m_vecAnimCtrl.push_back(AnimCtrl(354, 374, 11.800f, 12.466f));
				m_vecAnimCtrl.push_back(AnimCtrl(375, 426, 12.500f, 14.199f));
				m_vecAnimCtrl.push_back(AnimCtrl(427, 477, 14.233f, 15.900f));
				m_vecAnimCtrl.push_back(AnimCtrl(478, 518, 15.933f, 17.266f));
				m_vecAnimCtrl.push_back(AnimCtrl(519, 559, 17.300f, 18.633f));
				m_vecAnimCtrl.push_back(AnimCtrl(560, 620, 18.666f, 20.666f));
				m_vOBB_Range[0] = { 20.f ,80.f,20.f };
				m_vOBB_Range[1] = { 30.f ,80.f,30.f };
				m_iCombatMotion[0] = 4;
				m_iCombatMotion[1] = 5;
				m_iCombatMotion[2] = 3;
			}
			break;
			case CLASS::CLASS_MMAGE:
			{
				//	idle	
				//	walk	
				//	run		
				//	attack	
				//	take damage
				//	death a	
				//	death b	
				//	cast a	
				//	cast b	
				//	cast load
				m_vecAnimCtrl.push_back(AnimCtrl(0, 100, 0.00f, 3.333f));
				m_vecAnimCtrl.push_back(AnimCtrl(101, 131, 3.366f, 4.355f));
				m_vecAnimCtrl.push_back(AnimCtrl(132, 156, 4.400f, 5.200f));
				m_vecAnimCtrl.push_back(AnimCtrl(157, 197, 5.233f, 6.566f));
				m_vecAnimCtrl.push_back(AnimCtrl(198, 218, 6.599f, 7.266f));
				m_vecAnimCtrl.push_back(AnimCtrl(219, 284, 7.299f, 9.466f));
				m_vecAnimCtrl.push_back(AnimCtrl(285, 150, 9.500f, 11.666f));
				m_vecAnimCtrl.push_back(AnimCtrl(351, 391, 11.699f, 13.033f));
				m_vecAnimCtrl.push_back(AnimCtrl(392, 432, 13.066f, 14.400f));
				m_vecAnimCtrl.push_back(AnimCtrl(433, 493, 14.433f, 16.433f));
				m_vOBB_Range[0] = { 20.f ,120.f,60.f };
				m_vOBB_Range[1] = { 30.f ,120.f,70.f };
				m_iCombatMotion[0] = 0;
				m_iCombatMotion[1] = 1;
				m_iCombatMotion[2] = 3;
			}
			break;
			case CLASS::CLASS_ARCHER:
			{

				//	idle		
				//	walk		
				//	run			
				//	combat_idle	
				//	combat walk	
				//	attack a	
				//	take damage	
				//	death a		
				//	death b		
				m_vecAnimCtrl.push_back(AnimCtrl(0, 100, 0.00f, 3.333f));
				m_vecAnimCtrl.push_back(AnimCtrl(101, 137, 3.366f, 4.566f));
				m_vecAnimCtrl.push_back(AnimCtrl(138, 168, 4.599f, 5.599f));
				m_vecAnimCtrl.push_back(AnimCtrl(169, 229, 5.633f, 7.633f));
				m_vecAnimCtrl.push_back(AnimCtrl(230, 266, 7.666f, 8.866f));
				m_vecAnimCtrl.push_back(AnimCtrl(267, 307, 8.900f, 10.233f));
				m_vecAnimCtrl.push_back(AnimCtrl(308, 323, 10.266f, 10.766f));
				m_vecAnimCtrl.push_back(AnimCtrl(324, 373, 10.800f, 12.433f));
				m_vecAnimCtrl.push_back(AnimCtrl(374, 423, 12.466f, 14.100f));
				m_vOBB_Range[0] = { 20.f ,80.f,20.f };
				m_vOBB_Range[1] = { 20.f ,80.f,20.f };
				m_iCombatMotion[0] = 4;
				m_iCombatMotion[1] = 5;
				m_iCombatMotion[2] = 3;
			}
			break;
			case CLASS::CLASS_PRIEST:
			{
				m_vecAnimCtrl.push_back(AnimCtrl(0, 100, 0.00f, 3.333f));
				m_vecAnimCtrl.push_back(AnimCtrl(101, 137, 3.366f, 4.566f));
				m_vecAnimCtrl.push_back(AnimCtrl(138, 168, 4.599f, 5.599f));
				m_vecAnimCtrl.push_back(AnimCtrl(169, 194, 5.633f, 6.466f));
				m_vecAnimCtrl.push_back(AnimCtrl(195, 255, 6.500f, 8.500f));
				m_vecAnimCtrl.push_back(AnimCtrl(256, 291, 8.533f, 9.699f));
				m_vecAnimCtrl.push_back(AnimCtrl(292, 322, 9.733f, 10.733f));
				m_vecAnimCtrl.push_back(AnimCtrl(323, 353, 10.766f, 11.766f));
				m_vecAnimCtrl.push_back(AnimCtrl(354, 374, 11.800f, 12.466f));
				m_vecAnimCtrl.push_back(AnimCtrl(367, 426, 12.500f, 14.199f));
				m_vecAnimCtrl.push_back(AnimCtrl(327, 477, 14.233f, 15.900f));
				m_vecAnimCtrl.push_back(AnimCtrl(478, 518, 15.933f, 17.266f));
				m_vecAnimCtrl.push_back(AnimCtrl(519, 559, 17.300f, 18.633f));
				m_vecAnimCtrl.push_back(AnimCtrl(560, 620, 18.666f, 20.666f));
				m_vOBB_Range[0] = { 20.f ,80.f,20.f };
				m_vOBB_Range[1] = { 30.f ,80.f,30.f };
			}
			break;
			}
		}
		else
		{
			switch (m_eCurClass)
			{
			case CLASS::CLASS_WORKER:
			{
				//idle		
				//walk		
				//run		
				//attack	
				//death a	
				//death b	
				//take damage
				m_vecAnimCtrl.push_back(AnimCtrl(0, 100, 0.000f, 3.333f));
				m_vecAnimCtrl.push_back(AnimCtrl(101, 136, 3.366f, 4.533f));
				m_vecAnimCtrl.push_back(AnimCtrl(137, 167, 4.566f, 5.566f));
				m_vecAnimCtrl.push_back(AnimCtrl(168, 193, 5.599f, 6.433f));
				m_vecAnimCtrl.push_back(AnimCtrl(194, 249, 6.466f, 8.300f));
				m_vecAnimCtrl.push_back(AnimCtrl(250, 300, 8.333f, 10.000f));
				m_vecAnimCtrl.push_back(AnimCtrl(301, 321, 10.033f, 10.699f));
				m_vOBB_Range[0] = { 20.f ,80.f,20.f };
				m_vOBB_Range[1] = { 30.f ,80.f,30.f };
				m_iCombatMotion[0] = 0;
				m_iCombatMotion[1] = 1;
				m_iCombatMotion[2] = 3;
			}
			break;
			case CLASS::CLASS_INFANTRY:
			{
				//	idle		
				//	walk		
				//	run			
				//	charge		
				//	combat idle	
				//	combat walk	
				//	attack a	
				//	attack b	
				//	take damage	
				//	death a		
				//	death b		
				m_vecAnimCtrl.push_back(AnimCtrl(0, 100, 0.00f, 3.333f));
				m_vecAnimCtrl.push_back(AnimCtrl(101, 137, 3.366f, 4.566f));
				m_vecAnimCtrl.push_back(AnimCtrl(138, 168, 4.599f, 5.599f));
				m_vecAnimCtrl.push_back(AnimCtrl(169, 194, 5.633f, 6.466f));
				m_vecAnimCtrl.push_back(AnimCtrl(195, 255, 6.500f, 8.500f));
				m_vecAnimCtrl.push_back(AnimCtrl(256, 292, 8.533f, 9.733f));
				m_vecAnimCtrl.push_back(AnimCtrl(293, 323, 9.766f, 10.766f));
				m_vecAnimCtrl.push_back(AnimCtrl(324, 354, 10.800f, 11.800f));
				m_vecAnimCtrl.push_back(AnimCtrl(355, 370, 11.833f, 12.333f));
				m_vecAnimCtrl.push_back(AnimCtrl(371, 420, 12.366f, 14.000f));
				m_vecAnimCtrl.push_back(AnimCtrl(421, 370, 14.033f, 15.666f));
				m_vOBB_Range[0] = { 20.f ,80.f,20.f };
				m_vOBB_Range[1] = { 30.f ,80.f,30.f };
				m_iCombatMotion[0] = 4;
				m_iCombatMotion[1] = 5;
				m_iCombatMotion[2] = 3;
			}
			break;
			case CLASS::CLASS_CAVALRY:
			{
				//idle 		
				//walk 		
				//run 		
				//charge 	
				//combat idle 
				//combat walk
				//combat hit a
				//combat hit b 
				//take damage 
				//death a 	
				//death b 	
				m_vecAnimCtrl.push_back(AnimCtrl(0, 100, 0.00f, 3.333f));
				m_vecAnimCtrl.push_back(AnimCtrl(101, 131, 3.366f, 4.366f));
				m_vecAnimCtrl.push_back(AnimCtrl(132, 156, 4.400f, 5.200f));
				m_vecAnimCtrl.push_back(AnimCtrl(157, 181, 5.233f, 6.033f));
				m_vecAnimCtrl.push_back(AnimCtrl(182, 242, 6.066f, 8.066f));
				m_vecAnimCtrl.push_back(AnimCtrl(243, 273, 8.099f, 9.099f));
				m_vecAnimCtrl.push_back(AnimCtrl(274, 314, 9.133f, 10.466f));
				m_vecAnimCtrl.push_back(AnimCtrl(315, 355, 10.500f, 11.833f));
				m_vecAnimCtrl.push_back(AnimCtrl(356, 371, 11.866f, 12.366f));
				m_vecAnimCtrl.push_back(AnimCtrl(372, 437, 12.400f, 14.566f));
				m_vecAnimCtrl.push_back(AnimCtrl(438, 503, 14.600f, 16.766f));
				m_vOBB_Range[0] = { 20.f ,120.f,60.f };
				m_vOBB_Range[1] = { 30.f ,120.f,70.f };
				m_iCombatMotion[0] = 4;
				m_iCombatMotion[1] = 5;
				m_iCombatMotion[2] = 3;
			}
			break;
			case CLASS::CLASS_SPEARMAN:
			{
				//		idle		
				//		walk		
				//		run			
				//		charge		
				//		combat idle	
				//		combat walk	
				//		attack		
				//		take damage	
				//		death a		
				//		death b		
				m_vecAnimCtrl.push_back(AnimCtrl(0, 100, 0.00f, 3.333f));
				m_vecAnimCtrl.push_back(AnimCtrl(101, 137, 3.366f, 4.566f));
				m_vecAnimCtrl.push_back(AnimCtrl(138, 168, 4.599f, 5.599f));
				m_vecAnimCtrl.push_back(AnimCtrl(169, 194, 5.633f, 6.466f));
				m_vecAnimCtrl.push_back(AnimCtrl(195, 255, 6.500f, 8.500f));
				m_vecAnimCtrl.push_back(AnimCtrl(256, 292, 8.533f, 9.733f));
				m_vecAnimCtrl.push_back(AnimCtrl(293, 323, 9.766f, 10.766f));
				m_vecAnimCtrl.push_back(AnimCtrl(324, 339, 10.800f, 11.300f));
				m_vecAnimCtrl.push_back(AnimCtrl(340, 390, 11.333f, 13.000f));
				m_vecAnimCtrl.push_back(AnimCtrl(391, 441, 13.033f, 14.699f));
				m_vOBB_Range[0] = { 20.f ,80.f,20.f };
				m_vOBB_Range[1] = { 30.f ,80.f,60.f };
				m_iCombatMotion[0] = 4;
				m_iCombatMotion[1] = 5;
				m_iCombatMotion[2] = 3;
			}
			break;
			case CLASS::CLASS_MAGE:
			{
				//	idle		
				//	walk		
				//	run			
				//	charge		
				//	combat idle	
				//	combat walk	
				//	attack a	
				//	attack b	
				//	take damage	
				//	death a		
				//	death b		
				//	cast a		
				//	cast b		
				//	cast c		
				m_vecAnimCtrl.push_back(AnimCtrl(0, 100, 0.00f, 3.333f));
				m_vecAnimCtrl.push_back(AnimCtrl(101, 137, 3.366f, 4.566f));
				m_vecAnimCtrl.push_back(AnimCtrl(138, 168, 4.599f, 5.599f));
				m_vecAnimCtrl.push_back(AnimCtrl(169, 194, 5.633f, 6.466f));
				m_vecAnimCtrl.push_back(AnimCtrl(195, 255, 6.500f, 8.500f));
				m_vecAnimCtrl.push_back(AnimCtrl(256, 291, 8.533f, 9.699f));
				m_vecAnimCtrl.push_back(AnimCtrl(292, 322, 9.733f, 10.733f));
				m_vecAnimCtrl.push_back(AnimCtrl(323, 353, 10.766f, 11.766f));
				m_vecAnimCtrl.push_back(AnimCtrl(354, 374, 11.800f, 12.466f));
				m_vecAnimCtrl.push_back(AnimCtrl(375, 426, 12.500f, 14.199f));
				m_vecAnimCtrl.push_back(AnimCtrl(427, 477, 14.233f, 15.900f));
				m_vecAnimCtrl.push_back(AnimCtrl(478, 518, 15.933f, 17.266f));
				m_vecAnimCtrl.push_back(AnimCtrl(519, 559, 17.300f, 18.633f));
				m_vecAnimCtrl.push_back(AnimCtrl(560, 620, 18.666f, 20.666f));
				m_vOBB_Range[0] = { 20.f ,80.f,20.f };
				m_vOBB_Range[1] = { 30.f ,80.f,30.f };
				m_iCombatMotion[0] = 4;
				m_iCombatMotion[1] = 5;
				m_iCombatMotion[2] = 3;
			}
			break;
			case CLASS::CLASS_MMAGE:
			{

				//	idle	
				//	walk	
				//	run		
				//	attack	
				//	take damage
				//	death a	
				//	death b	
				//	cast a	
				//	cast b	
				//	cast load
				m_vecAnimCtrl.push_back(AnimCtrl(0, 100, 0.00f, 3.333f));
				m_vecAnimCtrl.push_back(AnimCtrl(101, 131, 3.366f, 4.355f));
				m_vecAnimCtrl.push_back(AnimCtrl(132, 156, 4.400f, 5.200f));
				m_vecAnimCtrl.push_back(AnimCtrl(157, 197, 5.233f, 6.566f));
				m_vecAnimCtrl.push_back(AnimCtrl(198, 218, 6.599f, 7.266f));
				m_vecAnimCtrl.push_back(AnimCtrl(219, 284, 7.299f, 9.466f));
				m_vecAnimCtrl.push_back(AnimCtrl(285, 150, 9.500f, 11.666f));
				m_vecAnimCtrl.push_back(AnimCtrl(351, 391, 11.699f, 13.033f));
				m_vecAnimCtrl.push_back(AnimCtrl(392, 432, 13.066f, 14.400f));
				m_vecAnimCtrl.push_back(AnimCtrl(433, 493, 14.433f, 16.433f));
				m_vOBB_Range[0] = { 20.f ,120.f,60.f };
				m_vOBB_Range[1] = { 30.f ,120.f,70.f };
				m_iCombatMotion[0] = 0;
				m_iCombatMotion[1] = 1;
				m_iCombatMotion[2] = 2;
			}
			break;
			case CLASS::CLASS_ARCHER:
			{
				//	idle		
				//	walk		
				//	run			
				//	combat_idle	
				//	combat walk	
				//	attack a	
				//	take damage	
				//	death a		
				//	death b	
				m_vecAnimCtrl.push_back(AnimCtrl(0, 100, 0.00f, 3.333f));
				m_vecAnimCtrl.push_back(AnimCtrl(101, 137, 3.366f, 4.566f));
				m_vecAnimCtrl.push_back(AnimCtrl(138, 168, 4.599f, 5.599f));
				m_vecAnimCtrl.push_back(AnimCtrl(169, 229, 5.633f, 7.633f));
				m_vecAnimCtrl.push_back(AnimCtrl(230, 266, 7.666f, 8.866f));
				m_vecAnimCtrl.push_back(AnimCtrl(267, 307, 8.900f, 10.233f));
				m_vecAnimCtrl.push_back(AnimCtrl(308, 323, 10.266f, 10.766f));
				m_vecAnimCtrl.push_back(AnimCtrl(324, 373, 10.800f, 12.433f));
				m_vecAnimCtrl.push_back(AnimCtrl(374, 423, 12.466f, 14.100f));
				m_vOBB_Range[0] = { 20.f ,80.f,20.f };
				m_vOBB_Range[1] = { 20.f ,80.f,20.f };
				m_iCombatMotion[0] = 4;
				m_iCombatMotion[1] = 5;
				m_iCombatMotion[2] = 3;
			}
			break;
			}
		}

		m_ePreClass = m_eCurClass;
	}
}

void CNPC::AnimVectorClear()
{
	m_vecAnimCtrl.clear();
	m_vecAnimCtrl.shrink_to_fit();
}

void CNPC::Obb_Collision()
{
	if (m_IsOBB_Collision && m_fBazierCnt <= 1.f)
	{
		if (!m_IsBazier)
		{
			_vec3 vTargetPos = { m_matAttackedTarget.m[3][0], m_matAttackedTarget.m[3][1], m_matAttackedTarget.m[3][2] };
			_vec3 vPos = *m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION);
			_vec3 vTemp = Vector3_::Subtract(vPos, vTargetPos);
			vTemp.Normalize();
			m_vStartPoint = vPos;
			m_vEndPoint = Vector3_::Add(*m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION), vTemp);
			//m_vEndPoint = *m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION);
			m_vMidPoint = Vector3_::Add(m_vStartPoint, m_vEndPoint);
			m_vMidPoint /= 2.f;
			//m_vMidPoint.y += 2.f;
			_vec3 vParticlePos = Vector3_::Subtract(*m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION), vTemp);
			Create_Particle(vParticlePos);
			m_IsBazier = true;		
		}
		Hit_Object(m_fBazierCnt, m_vStartPoint, m_vEndPoint, m_vMidPoint);
	}
	if (m_fBazierCnt >= 1.f)
	{
		m_fBazierCnt = 0.f;
		m_IsOBB_Collision = false;
		m_IsBazier = false;
	}
}

void CNPC::Hit_Object(_float& fCnt, _vec3 vStart, _vec3 vEnd, _vec3 vMid)
{
	_float fX = (pow((1.f - fCnt), 2) * vStart.x) + (2 * fCnt * (1.f - fCnt) * vMid.x) + (pow(fCnt, 2) * vEnd.x);
	_float fY = (pow((1.f - fCnt), 2) * vStart.y) + (2 * fCnt * (1.f - fCnt) * vMid.y) + (pow(fCnt, 2) * vEnd.y);
	_float fZ = (pow((1.f - fCnt), 2) * vStart.z) + (2 * fCnt * (1.f - fCnt) * vMid.z) + (pow(fCnt, 2) * vEnd.z);

	_vec3 vPos = { fX, fY, fZ };
	m_pTransformCom->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
	fCnt += 0.01f;


}

void CNPC::Create_Particle(const _vec3& vPos)
{
	if (m_IsParticle)
	{
		PARTICLESET tParticleSet;
		tParticleSet.vPos = vPos;
		tParticleSet.iMaxParticle = 30;
		tParticleSet.fMaxLifeTime = 1.f;
		tParticleSet.iMinLifeTime = 1.f;

		tParticleSet.fStartScale = 1.f;
		tParticleSet.fEndScale = 1.f;

		tParticleSet.fMaxSpeed = 10.f;
		tParticleSet.fMinSpeed = 100.f;
		if (FAILED(CManagement::GetInstance()->Add_GameObjectToLayer(L"GameObject_Particle_Default", (_uint)SCENEID::SCENE_STAGE, L"Layer_Particle", nullptr, (void*)&tParticleSet)))
			return;
		m_IsParticle = false;
	}

}

void CNPC::Compute_Matrix_X()
{
	_vec3		vPos = *m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION);
	_vec3		vSize = m_pTransformCom->Get_Scale();
	_matrix		matLeft = Matrix_::Identity();
	_matrix		matRight = Matrix_::Identity();

	_vec3		vRight(1.f, 0.f, 0.f), vUp(0.f, 1.f, 0.f), vLook(0.f, 0.f, 1.f);
	DirectX::XMStoreFloat4x4(&matLeft, DirectX::XMMatrixRotationX(XMConvertToRadians(100.f)));
	vRight *= 0.1f;
	vUp *= 0.1f;
	vLook *= 0.1f;
	XMMATRIX mat = ::XMLoadFloat4x4(&matLeft);
	vRight = Vector3_::TransformNormal(vRight, mat);
	vUp = Vector3_::TransformNormal(vUp, mat);
	vLook = Vector3_::TransformNormal(vLook, mat);

	memcpy(&matLeft.m[0][0], &vRight, sizeof(_vec3));
	memcpy(&matLeft.m[1][0], &vUp, sizeof(_vec3));
	memcpy(&matLeft.m[2][0], &vLook, sizeof(_vec3));
	matLeft.Translation(vPos);

	vRight = { 1.f, 0.f, 0.f };
	vUp = { 0.f, 1.f, 0.f };
	vLook = { 0.f, 0.f, 1.f };
	DirectX::XMStoreFloat4x4(&matRight, DirectX::XMMatrixRotationX(XMConvertToRadians(-100.f)));
	vRight *= 0.1f;
	vUp *= 0.1f;
	vLook *= 0.1f;
	mat = ::XMLoadFloat4x4(&matRight);
	vRight = Vector3_::TransformNormal(vRight, mat);
	vUp = Vector3_::TransformNormal(vUp, mat);
	vLook = Vector3_::TransformNormal(vLook, mat);

	memcpy(&matRight.m[0][0], &vRight, sizeof(_vec3));
	memcpy(&matRight.m[1][0], &vUp, sizeof(_vec3));
	memcpy(&matRight.m[2][0], &vLook, sizeof(_vec3));
	matRight.Translation(vPos);


	m_matLeft = matLeft;
	m_matRight = matRight;
}

void CNPC::Compute_Matrix_Z()
{
	_vec3		vPos = *m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION);
	_vec3		vSize = m_pTransformCom->Get_Scale();
	_matrix		matLeft = Matrix_::Identity();
	_matrix		matRight = Matrix_::Identity();

	_vec3		vRight(1.f, 0.f, 0.f), vUp(0.f, 1.f, 0.f), vLook(0.f, 0.f, 1.f);
	DirectX::XMStoreFloat4x4(&matLeft, DirectX::XMMatrixRotationZ(XMConvertToRadians(100.f)));
	vRight *= 0.1f;
	vUp *= 0.1f;
	vLook *= 0.1f;
	XMMATRIX mat = ::XMLoadFloat4x4(&matLeft);
	vRight = Vector3_::TransformNormal(vRight, mat);
	vUp = Vector3_::TransformNormal(vUp, mat);
	vLook = Vector3_::TransformNormal(vLook, mat);

	memcpy(&matLeft.m[0][0], &vRight, sizeof(_vec3));
	memcpy(&matLeft.m[1][0], &vUp, sizeof(_vec3));
	memcpy(&matLeft.m[2][0], &vLook, sizeof(_vec3));
	matLeft.Translation(vPos);

	vRight = { 1.f, 0.f, 0.f };
	vUp = { 0.f, 1.f, 0.f };
	vLook = { 0.f, 0.f, 1.f };
	DirectX::XMStoreFloat4x4(&matRight, DirectX::XMMatrixRotationZ(XMConvertToRadians(-100.f)));
	vRight *= 0.1f;
	vUp *= 0.1f;
	vLook *= 0.1f;
	mat = ::XMLoadFloat4x4(&matRight);
	vRight = Vector3_::TransformNormal(vRight, mat);
	vUp = Vector3_::TransformNormal(vUp, mat);
	vLook = Vector3_::TransformNormal(vLook, mat);

	memcpy(&matRight.m[0][0], &vRight, sizeof(_vec3));
	memcpy(&matRight.m[1][0], &vUp, sizeof(_vec3));
	memcpy(&matRight.m[2][0], &vLook, sizeof(_vec3));
	matRight.Translation(vPos);


	m_matLeft = matLeft;
	m_matRight = matRight;
}

void CNPC::Death(const _float& fTimeDelta)
{
	DeathMontion_Init();
	if (m_iCurAnimIdx == m_iDeathMotion[1])
	{
		if (!m_IsDead)
		{

			m_fDeathTime += fTimeDelta * 1.2f;
			_matrix matTemp = Matrix::Lerp(m_pTransformCom->Get_Matrix(), m_matLeft, fTimeDelta * 1.2f);
			m_pTransformCom->Set_Matrix(matTemp);
			if (m_fDeathTime >= 1.7f)
			{
				m_fDeathTime = 0.f;
				m_IsDead = true;
			}
		}

	}
	else if (m_iCurAnimIdx == m_iDeathMotion[0])
	{
		if (!m_IsDead)
		{
			m_fDeathTime += fTimeDelta * 1.2f;
			_matrix matTemp = Matrix::Lerp(m_pTransformCom->Get_Matrix(), m_matRight, fTimeDelta * 1.2f);
			m_pTransformCom->Set_Matrix(matTemp);
			if (m_fDeathTime >= 1.7f)
			{
				m_fDeathTime = 0.f;
				m_IsDead = true;
			}
		}
	}

}

void CNPC::DeathMontion_Init()
{
	m_iDeathMotion[0] = 100;
	m_iDeathMotion[1] = 100;
	switch (m_eCurClass)
	{
	case CLASS::CLASS_WORKER:
		Compute_Matrix_X();
		m_iDeathMotion[0] = 4;
		m_iDeathMotion[1] = 5;
		break;
	case CLASS::CLASS_INFANTRY:
		Compute_Matrix_X();
		m_iDeathMotion[0] = 9;
		m_iDeathMotion[1] = 10;
		break;
	case CLASS::CLASS_MAGE:
		Compute_Matrix_X();
		m_iDeathMotion[0] = 9;
		m_iDeathMotion[1] = 10;
		break;
	case CLASS::CLASS_CAVALRY:
		Compute_Matrix_Z();
		m_iDeathMotion[0] = 9;
		m_iDeathMotion[1] = 10;
		break;
	case CLASS::CLASS_SPEARMAN:
		Compute_Matrix_X();
		m_iDeathMotion[0] = 8;
		m_iDeathMotion[1] = 9;
		break;
	case CLASS::CLASS_MMAGE:
		Compute_Matrix_Z();
		m_iDeathMotion[0] = 5;
		m_iDeathMotion[1] = 6;
		break;
	case CLASS::CLASS_ARCHER:
		Compute_Matrix_X();
		m_iDeathMotion[0] = 7;
		m_iDeathMotion[1] = 8;
		break;
	}

}

void CNPC::Attack(const _float& fTimeDelta)
{
	m_iAttackMotion[0] = 100;
	m_iAttackMotion[1] = 100;
	switch (m_eCurClass)
	{
	case CLASS::CLASS_WORKER:
	case CLASS::CLASS_MMAGE:
		m_iAttackMotion[0] = 3;
		m_iAttackMotion[1] = 3;
		break;
	case CLASS::CLASS_INFANTRY:
	case CLASS::CLASS_CAVALRY:
	case CLASS::CLASS_MAGE:
	case CLASS::CLASS_PRIEST:
		m_iAttackMotion[0] = 6;
		m_iAttackMotion[1] = 7;
		break;
	case CLASS::CLASS_SPEARMAN:
		m_iAttackMotion[0] = 6;
		m_iAttackMotion[1] = 6;
		break;
	case CLASS::CLASS_ARCHER:
		m_iAttackMotion[0] = 5;
		m_iAttackMotion[1] = 5;
		break;
	case CLASS(2):
		m_iAttackMotion[0] = 6;
		m_iAttackMotion[1] = 7;
		break;
	case CLASS(4):
		m_iAttackMotion[0] = 6;
		m_iAttackMotion[1] = 7;
		break;
	}

	if (m_iCurAnimIdx == m_iAttackMotion[1] || m_iCurAnimIdx == m_iAttackMotion[0])
	{
		m_pColiider[0]->Change_ColliderBoxSize(m_pTransformCom, m_vOBB_Range[1]);
		m_pColiider[1]->Change_ColliderBoxSize(m_pTransformCom, m_vOBB_Range[1]);
	}
	else
	{
		m_pColiider[0]->Change_ColliderBoxSize(m_pTransformCom, m_vOBB_Range[0]);
		m_pColiider[1]->Change_ColliderBoxSize(m_pTransformCom, m_vOBB_Range[0]);
	}
}

void CNPC::Combat(const _float& fTimeDelta)
{
	if (m_IsCombat)
	{
		m_fCombatTime += fTimeDelta;
	}
	if (m_fCombatTime >= 10.f)
	{
		m_fCombatTime = 0.f;
		m_IsCombat = false;
	}
}

void CNPC::Chase_Player(const _float& fTimeDelta, _float fLenght)
{

	if (CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_Player").size() == 0)
	{
		return;
	}

	_vec3 vPlayerPos = *dynamic_cast<CTransform*>(CManagement::GetInstance()->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE, L"Layer_Player", L"Com_Transform"))->Get_StateInfo(CTransform::STATE_POSITION);
	_vec3 vt = Vector3_::Subtract(vPlayerPos, *m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION));

	_float fLength = vt.Length();
	_vec3 vP_M = Vector3_::Subtract(m_vDest, *m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION));

	vP_M.Normalize();

	_vec3 vTemp = *m_pTransformCom->Get_StateInfo(CTransform::STATE_LOOK);
	vTemp *= -1.f;
	vTemp.Normalize();
	float fDot = Vector3_::DotProduct(vTemp, vP_M);

	_float fAngle = XMConvertToDegrees(fDot);
	if (fAngle >= 56.f && fAngle <= 57.f)
	{
		m_IsRotateEnd = false;
		if (fLength >= 6.f)
		{
			m_pTransformCom->Go_ToTarget(&vPlayerPos, fTimeDelta);
			m_iCurAnimIdx = 1;
		}
	}
	else
	{
		if (m_eRot_Dir == ROTATE_DIR::ROT_DIR_LEFT)
			m_pTransformCom->Rotation_Y(-fTimeDelta);
		else if (m_eRot_Dir == ROTATE_DIR::ROT_DIR_RIGHT)
			m_pTransformCom->Rotation_Y(fTimeDelta);

	}



	m_IsOnce = false;

}

void CNPC::MeaningLess_Move(const _float& fTimeDelta)
{
	if (!m_IsDest)
	{
		srand(unsigned(time(NULL)));
		_int	iMoveX = rand() % 100 + 1;
		_int	iMoveZ = rand() % 100 + 1;

		m_vDest = { (_float)iMoveX, 0.f, (_float)iMoveZ };

		m_IsDest = true;
	}


	_vec3 vt = Vector3_::Subtract(*m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION), m_vDest);
	_int	iDestLength = vt.Length();


	_vec3 vP_M = Vector3_::Subtract(m_vDest, *m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION));
	vP_M.Normalize();

	_vec3 vTemp = *m_pTransformCom->Get_StateInfo(CTransform::STATE_LOOK);
	vTemp *= -1.f;
	vTemp.Normalize();
	float fDot = Vector3_::DotProduct(vTemp, vP_M);

	if (!m_IsRotateEnd)
	{
		if (fabs(m_pTransformCom->Get_StateInfo(CTransform::STATE_LOOK)->x) > fabs(m_pTransformCom->Get_StateInfo(CTransform::STATE_LOOK)->z))
		{
			if (m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION)->x <= m_vDest.x)
			{
				m_eRot_Dir = ROTATE_DIR::ROT_DIR_RIGHT;
			}
			else
			{
				m_eRot_Dir = ROTATE_DIR::ROT_DIR_LEFT;
			}
			_int i = 0;
		}
		else
		{
			if (m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION)->z <= m_vDest.z)
			{
				if (m_pTransformCom->Get_StateInfo(CTransform::STATE_LOOK)->z <= 0)
				{
					m_eRot_Dir = ROTATE_DIR::ROT_DIR_LEFT;
				}
				else
				{
					m_eRot_Dir = ROTATE_DIR::ROT_DIR_RIGHT;
				}

			}
			else
			{
				if (m_pTransformCom->Get_StateInfo(CTransform::STATE_LOOK)->z <= 0)
				{
					m_eRot_Dir = ROTATE_DIR::ROT_DIR_RIGHT;
				}
				else
				{
					m_eRot_Dir = ROTATE_DIR::ROT_DIR_LEFT;;
				}
			}
		}


		m_IsRotateEnd = true;
	}
	
	_float fAngle = XMConvertToDegrees(fDot);
	if (fAngle >= 56.f && fAngle <= 57.f)
	{
		m_IsRotateEnd = false;
		if (iDestLength >= 3.f)
		{
			m_pTransformCom->Go_ToTarget(&m_vDest, fTimeDelta);
			m_iCurAnimIdx = 1;
		}
		else
		{
			m_IsDest = false;
		}

	}
	else
	{
		if (m_eRot_Dir == ROTATE_DIR::ROT_DIR_LEFT)
			m_pTransformCom->Rotation_Y(-fTimeDelta);
		else if (m_eRot_Dir == ROTATE_DIR::ROT_DIR_RIGHT)
			m_pTransformCom->Rotation_Y(fTimeDelta);
	}
	m_IsOnce = false;

}

void CNPC::ComputeDir()
{
	if (CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_Player").size() == 0)
	{
		return;
	}

	_vec3 vThisTransformOutput = {};
	_vec3 vPlayerPos = *dynamic_cast<CTransform*>(CManagement::GetInstance()->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE, L"Layer_Player", L"Com_Transform"))->Get_StateInfo(CTransform::STATE_POSITION);
	_vec3 vP_M = Vector3_::Subtract(vPlayerPos, *m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION));

	vP_M.Normalize();

	_vec3 vTemp = *m_pTransformCom->Get_StateInfo(CTransform::STATE_LOOK);
	vTemp *= -1.f;
	vTemp.Normalize(vThisTransformOutput);
	_float fDot = Vector3_::DotProduct(vThisTransformOutput, vP_M);


	if (!m_IsRotateEnd)
	{

		if (fabs(m_pTransformCom->Get_StateInfo(CTransform::STATE_LOOK)->x) > fabs(m_pTransformCom->Get_StateInfo(CTransform::STATE_LOOK)->z))
		{
			if (m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION)->x <= vPlayerPos.x)
			{
				m_eRot_Dir = ROTATE_DIR::ROT_DIR_RIGHT;
			}
			else
			{
				m_eRot_Dir = ROTATE_DIR::ROT_DIR_LEFT;
			}
			_int i = 0;
		}
		else
		{
			if (m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION)->z <= vPlayerPos.z)
			{
				if (m_pTransformCom->Get_StateInfo(CTransform::STATE_LOOK)->z <= 0)
				{
					m_eRot_Dir = ROTATE_DIR::ROT_DIR_LEFT;
				}
				else
				{
					m_eRot_Dir = ROTATE_DIR::ROT_DIR_RIGHT;
				}

			}
			else
			{
				if (m_pTransformCom->Get_StateInfo(CTransform::STATE_LOOK)->z <= 0)
				{
					m_eRot_Dir = ROTATE_DIR::ROT_DIR_RIGHT;
				}
				else
				{
					m_eRot_Dir = ROTATE_DIR::ROT_DIR_LEFT;
				}
			}
		}


		m_IsRotateEnd = true;
	}

}

void CNPC::Change_State(const _float fTimeDelta)
{
	ComputeDir();
	if (CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_Player").size() == 0)
	{
		return;
	}

	_vec3 vPlayerPos = *dynamic_cast<CTransform*>(CManagement::GetInstance()->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE, L"Layer_Player", L"Com_Transform"))->Get_StateInfo(CTransform::STATE_POSITION);
	_vec3 vP_M = Vector3_::Subtract(vPlayerPos, *m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION));

	_float fLength = vP_M.Length();
	//_float fLength = 100.f;
	if (fLength <= 15.f)
	{
	
		m_IsAttackMotion = false; 
		Chase_Player(fTimeDelta, fLength);
		m_IsHit = false;
	}
	else
	{
		MeaningLess_Move(fTimeDelta);
		m_IsAttackMotion = false;
		m_IsHit = false; 
	}
}
