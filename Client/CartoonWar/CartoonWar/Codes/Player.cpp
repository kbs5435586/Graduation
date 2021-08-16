#include "framework.h"
#include "Management.h"
#include "Player.h"
#include "UI_OnHead.h"
#include "UI_Select.h"
#include "Deffend.h"

#include "UI_ClassTap.h"
#include "UI_Skill.h"
#include "Inventory_Camera.h"
#include "Terrain_Height.h"
#include "Fire.h"
#include "Teleport.h"

#include "Throw_Arrow.h"
CPlayer::CPlayer()
	: CGameObject()
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer::Ready_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Ready_GameObject(void* pArg)
{
	if (pArg)
	{
		m_tUnit = *(UNIT*)pArg;
	}
	m_IsClone = true;
	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(CreateInputLayout()))
		return E_FAIL;

	_vec3 vColliderSize = { 40.f ,160.f,40.f };
	m_pCollider_OBB->Clone_ColliderBox(m_pTransformCom, vColliderSize);
	m_pCollider_AABB->Clone_ColliderBox(m_pTransformCom, vColliderSize);
	m_pCollider_Attack->Clone_ColliderBox(m_pTransformCom, vColliderSize);

	_vec3 vPos = { 75.f, 0.f,75.f };
	m_pTransformCom->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
	m_eCurTeam = TEAM::TEAM_RED;

	m_pTransformCom->SetUp_Speed(50.f, XMConvertToRadians(90.f));
	m_pTransformCom->Scaling(0.1f, 0.1f, 0.1f);
	m_pTransformCom->SetUp_RotationY(XMConvertToRadians(180.f));
	m_tInfo = INFO(2.f, 1, 1, 0);
	for (_uint i = 0; i < (_uint)CLASS::CLASS_END; ++i)
	{
		if (m_pAnimCom[i] == nullptr)
			continue;
		m_pAnimCom[i]->SetBones(m_pMeshCom[i]->GetBones());
		m_pAnimCom[i]->SetAnimClip(m_pMeshCom[i]->GetAnimClip());
		m_pAnimCom[i]->LateInit();
	}

	m_vColShpereSize = { 100.f,100.f,100.f };

	m_eCurClass = CLASS::CLASS_WORKER;
	m_iCurAnimIdx = 0;
	m_iPreAnimIdx = 100;
	m_cMoveCondition = CON_IDLE;
	m_cLastMoveCondition = CON_IDLE;
	m_cRotateCondition = CON_IDLE;
	m_cLastRotateCondition = CON_IDLE;

	m_pCurAnimCom = m_pAnimCom[(_uint)m_eCurClass];
	m_pCurMeshCom = m_pMeshCom[(_uint)m_eCurClass];

	SetSpeed();
	m_matOldWorld = m_pTransformCom->Get_Matrix();;
	m_matOldView = CCamera_Manager::GetInstance()->GetMatView();
	if (m_iLayerIdx % 2 == 0)
		m_eCurTeam = TEAM::TEAM_RED;
	else
		m_eCurTeam = TEAM::TEAM_BLUE;
	m_eCurState = STATE::STATE_IDLE;
	return S_OK;
}

_int CPlayer::Update_GameObject(const _float& fTimeDelta)
{
	CServer_Manager* server = CServer_Manager::GetInstance();
	if (nullptr == server)
		return -1;
	server->AddRef();

	m_IsHit = server->Get_isHitPL(m_iLayerIdx);
	m_IsOnce = server->Get_isHitPL(m_iLayerIdx);
	m_tInfo.fHP = server->Get_PlayerHP(m_iLayerIdx);
	m_IsShow = server->Get_ShowOtherPlayer(m_iLayerIdx);
	m_iCurMeshNum = server->Get_PlayerClass(m_iLayerIdx);
	m_eCurClass = (CLASS)m_iCurMeshNum;
	Change_Class();
	m_pTransformCom->SetSpeed(m_fArrSpeed[(_uint)m_eCurClass]);

	Attack(fTimeDelta);
	Play_Sound(fTimeDelta);
	m_pCollider_OBB->Update_Collider(m_pTransformCom, m_vOBB_Range[0], m_eCurClass);
	m_pCollider_AABB->Update_Collider(m_pTransformCom, m_vOBB_Range[0], m_eCurClass);
	m_pCollider_Attack->Update_Collider(m_pTransformCom, m_vOBB_Range[1], m_eCurClass);


	CBuffer_Terrain_Height* pTerrainBuffer = (CBuffer_Terrain_Height*)CManagement::GetInstance()->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE, L"Layer_Terrain", L"Com_Buffer");
	if (nullptr == pTerrainBuffer)
		return NO_EVENT;

	if (m_IsShow)
	{
		_float		fY = pTerrainBuffer->Compute_HeightOnTerrain(m_pTransformCom);
		m_pTransformCom->Set_PositionY(fY);
		if (m_eCurClass == CLASS::CLASS_MAGE || m_eCurClass == CLASS::CLASS_MMAGE)
		{
			//Z
			Skill_CastFire(fTimeDelta, fY);
			//X
			Skill_CastTeleport(fTimeDelta, fY);

		}
		else if (m_eCurClass == CLASS::CLASS_ARCHER)
		{
			//Z
			//Skill_Fly(fTimeDelta, fY);
			//X
			Skill_Invisible(fTimeDelta);
		}
	}
	
	m_cMoveCondition = server->Get_PlayerMCon(m_iLayerIdx);
	m_cRotateCondition = server->Get_PlayerRCon(m_iLayerIdx);

	CGameObject* UI = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_UI", TAPIDX);
	m_IsActive = dynamic_cast<CUI_ClassTap*>(UI)->GetBool();

	if (m_iLayerIdx == server->Get_PlayerID() && !m_IsActive && !m_IsDead)
		Input_Key(fTimeDelta);

	switch (m_cMoveCondition)
	{
	case CON_STRAIGHT:
		m_pTransformCom->BackWard(fTimeDelta);
		break;
	case CON_RUN:
		m_pTransformCom->BackWard(fTimeDelta * 2.f);
		break;
	case CON_BACK:
		m_pTransformCom->Go_Straight(fTimeDelta);
		break;
	}

	switch (m_cRotateCondition)
	{
	case CON_LEFT:
		m_pTransformCom->Rotation_Y(-fTimeDelta);
		break;
	case CON_RIGHT:
		m_pTransformCom->Rotation_Y(fTimeDelta);
		break;
	}


	m_iCurAnimIdx = server->Get_Anim(m_iLayerIdx);
	if (m_pCurAnimCom->Update(m_vecAnimCtrl[m_iCurAnimIdx], fTimeDelta) && m_IsOnce)
	{
		if (m_IsCombat)
		{
			m_iCurAnimIdx = m_iCombatMotion[0];
		}
		else
		{
			m_iCurAnimIdx = 0;
			server->send_animation_packet(A_IDLE);
		}
		m_IsOnce = false;
		m_IsHit = false; // 수정
		//server->Set_AnimPL(m_iLayerIdx, 0);
		server->Set_isHitPL(m_iLayerIdx, m_IsHit);
		m_IsActioning = false;
	}
	
	Obb_Collision();
	Combat(fTimeDelta);
	Death(fTimeDelta);

	if (m_tInfo.fHP <= 0.f)
	{
		if (!m_IsDeadMotion)
		{		
			m_eCurState = STATE::STATE_DEAD;
			_uint iRand = rand() % 2;
			if (iRand == 0)
				m_iCurAnimIdx = m_iDeathMotion[0];
			else
				m_iCurAnimIdx = m_iDeathMotion[1];
			m_IsDeadMotion = true;
		}
	}
	if (m_IsDead)
	{
		Resurrection();
	}

	if (m_IsParticleRun)
	{
		m_fParticleRunTime += fTimeDelta;
	}
	if (m_fParticleRunTime >= 1.f)
	{
		CBuffer_Terrain_Height* pTerrainBuffer = (CBuffer_Terrain_Height*)CManagement::GetInstance()->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE, L"Layer_Terrain", L"Com_Buffer");
		if (nullptr == pTerrainBuffer)
			return NO_EVENT;

		_float		fY = pTerrainBuffer->Compute_HeightOnTerrain(m_pTransformCom);
		_vec3 vPosition = {};
		vPosition = *m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION);
		_vec3 vTemp = vPosition;
		vTemp.y = fY;

		PARTICLESET tParticleSet;
		tParticleSet.vPos = vTemp;
		tParticleSet.iMaxParticle = 20;
		tParticleSet.fMaxLifeTime = 1.f;
		tParticleSet.iMinLifeTime = 0.5f;

		tParticleSet.fStartScale = 0.5f;
		tParticleSet.fEndScale = 0.1f;

		tParticleSet.fMaxSpeed = 10.f;
		tParticleSet.fMinSpeed = 20.f;
		if (FAILED(CManagement::GetInstance()->Add_GameObjectToLayer(L"GameObject_Particle_Run", (_uint)SCENEID::SCENE_STAGE, L"Layer_Particle", nullptr, (void*)&tParticleSet)))
			return NO_EVENT;
		m_fParticleRunTime = 0.f;
		m_IsParticleRun = false;
	}

	Set_Animation(fTimeDelta);
	Safe_Release(server);
	return NO_EVENT;
}

_int CPlayer::LastUpdate_GameObject(const _float& fTimeDelta)
{
	CServer_Manager* server = CServer_Manager::GetInstance();
	if (nullptr == server)
		return -1;
	server->AddRef();

	if (nullptr == m_pRendererCom)
		return -1;

	if (m_IsShow)
	{
		if (m_pFrustumCom->Culling_Frustum(m_pTransformCom, 20.f))
		{
			m_IsFrustum = true;

			if (CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_Player", g_iPlayerIdx)->GetIsRun())
			{
				if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLUR, this)))
					return -1;
			}
			if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this)))
				return -1;
			if (m_IsInvisibleON) //  server->Get_PlayerInvisible(m_iLayerIdx)
			{
				if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_POST, this)))
					return -1;
			}
			else
			{
				if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONEALPHA, this)))
					return -1;
			}
			//if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_REF, this)))
			//	return -1;
			//if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_POST, this)))
			//	return -1;
		}
		else
		{
			m_matOldWorld = m_pTransformCom->Get_Matrix();;
			m_matOldView = CCamera_Manager::GetInstance()->GetMatView();
			m_IsFrustum = false;
		}
	}

	Safe_Release(server);
	return _int();
}

void CPlayer::Render_GameObject()
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
		tRep.m_arrInt[2] = g_DefferedRender;

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
				CDevice::GetInstance()->SetTextureToShader(m_pTextureCom[1], TEXTURE_REGISTER::t0, (_uint)m_tUnit.eColor);
		}
		else
		{
			CDevice::GetInstance()->SetTextureToShader(m_pTextureCom[1], TEXTURE_REGISTER::t0, (_uint)m_tUnit.eColor);
		}

		m_pCurAnimCom->UpdateData(m_pCurMeshCom, m_pComputeShaderCom);
		CDevice::GetInstance()->UpdateTable();
		m_pCurMeshCom->Render_Mesh(i);
	}


	if (g_IsCollisionBox)
	{
		m_pCollider_OBB->Render_Collider();
		m_pCollider_Attack->Render_Collider(1);
		m_pCollider_AABB->Render_Collider();
	}


	m_matOldView = CCamera_Manager::GetInstance()->GetMatView();
	m_iBlurCnt++;
	if (m_iBlurCnt >= MAX_BLURCNT)
	{
		m_matOldWorld = m_pTransformCom->Get_Matrix();
		m_iBlurCnt = 0;
	}
	Safe_Release(pManagement);
}

void CPlayer::Render_GameObject_Shadow()
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

void CPlayer::Render_GameObject_Map()
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
		_matrix matView = CCamera_Manager::GetInstance()->GetMapMatView();
		_matrix matProj = CCamera_Manager::GetInstance()->GetMapMatProj();

		REP tRep = {};
		tRep.m_arrInt[0] = 1;
		tRep.m_arrInt[1] = m_pCurAnimCom->GetBones()->size();
		tRep.m_arrInt[2] = g_DefferedRender;

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
				CDevice::GetInstance()->SetTextureToShader(m_pTextureCom[1], TEXTURE_REGISTER::t0, (_uint)m_tUnit.eColor);
		}
		else
		{
			CDevice::GetInstance()->SetTextureToShader(m_pTextureCom[1], TEXTURE_REGISTER::t0, (_uint)m_tUnit.eColor);
		}

		m_pCurAnimCom->UpdateData(m_pCurMeshCom, m_pComputeShaderCom);
		CDevice::GetInstance()->UpdateTable();
		m_pCurMeshCom->Render_Mesh(i);
	}


	m_pCollider_OBB->Render_Collider();
	m_pCollider_Attack->Render_Collider(1);
	//m_pCollider_Hit->Render_Collider();
	//m_pColiider[1]->Render_Collider();



	Safe_Release(pManagement);
}

void CPlayer::Render_PostEffect()
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

		m_pShaderCom_PostEffect->SetUp_OnShader(matWorld, matView, matProj, tMainPass);

		_uint iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
			(_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);

		iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->SetData((void*)&tRep);
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
			(_uint)CONST_REGISTER::b8)->GetCBV().Get(), iOffeset, CONST_REGISTER::b8);

		ComPtr<ID3D12DescriptorHeap>	pPostEffectTex = CManagement::GetInstance()->GetPostEffectTex()->GetSRV().Get();
		CDevice::GetInstance()->SetTextureToShader(pPostEffectTex.Get(), TEXTURE_REGISTER::t0);


		m_pCurAnimCom->UpdateData(m_pCurMeshCom, m_pComputeShaderCom);
		CDevice::GetInstance()->UpdateTable();
		m_pCurMeshCom->Render_Mesh(i);
	}
	Safe_Release(pManagement);
}

void CPlayer::Render_Blur()
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

		tRep.m_arrMat[0] = m_matOldWorld;//OldWorld
		tRep.m_arrMat[1] = m_matOldView;//OldView

		m_pShaderCom_Blur->SetUp_OnShader(matWorld, matView, matProj, tMainPass);

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

	//m_matOldWorld = m_pTransformCom->Get_Matrix();
	//m_matOldView = CCamera_Manager::GetInstance()->GetMatView();

	Safe_Release(pManagement);
}

void CPlayer::Render_Ref()
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
		_matrix matReflectionView = CCamera_Manager::GetInstance()->GetReflectionView();
		_matrix matReflectionProj = CCamera_Manager::GetInstance()->GetReflectionMatProj();

		REP tRep = {};
		tRep.m_arrInt[0] = 1;
		tRep.m_arrInt[1] = m_pCurAnimCom->GetBones()->size();
		tRep.m_arrInt[2] = g_DefferedRender;

		m_pShaderCom_Reflection->SetUp_OnShader(matWorld, matReflectionView, matReflectionProj, tMainPass);

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
				CDevice::GetInstance()->SetTextureToShader(m_pTextureCom[1], TEXTURE_REGISTER::t0, (_uint)m_tUnit.eColor);
		}
		else
		{
			CDevice::GetInstance()->SetTextureToShader(m_pTextureCom[1], TEXTURE_REGISTER::t0, (_uint)m_tUnit.eColor);
		}

		m_pCurAnimCom->UpdateData(m_pCurMeshCom, m_pComputeShaderCom);
		CDevice::GetInstance()->UpdateTable();
		m_pCurMeshCom->Render_Mesh(i);
	}


	Safe_Release(pManagement);
}

HRESULT CPlayer::CreateInputLayout()
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
	if (FAILED(m_pShaderCom_PostEffect->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::NO_DEPTHTEST_NO_WRITE, SHADER_TYPE::SHADER_POST_EFFECT)))
		return E_FAIL;
	if (FAILED(m_pShaderCom_Blur->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::LESS_NO_WRITE, SHADER_TYPE::SHADER_BLUR)))
		return E_FAIL;
	return S_OK;
}

CPlayer* CPlayer::Create()
{
	CPlayer* pInstance = new CPlayer();

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MessageBox(0, L"CPlayer Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;

}

CGameObject* CPlayer::Clone_GameObject(void* pArg, _uint iIdx)
{
	CPlayer* pInstance = new CPlayer(*this);

	pInstance->m_iLayerIdx = iIdx;
	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MessageBox(0, L"CPlayer Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer::Free()
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
	Safe_Release(m_pShaderCom_PostEffect);
	Safe_Release(m_pShaderCom_Blur);
	Safe_Release(m_pFrustumCom);
	Safe_Release(m_pCollider_OBB);
	Safe_Release(m_pCollider_AABB);
	Safe_Release(m_pCollider_Attack);
	Safe_Release(m_pTextureCom[0]);
	Safe_Release(m_pTextureCom[1]);
	Safe_Release(m_pNaviCom);

	CGameObject::Free();
}

HRESULT CPlayer::Ready_Component()
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

	if(m_tUnit.eSpecies == SPECIES::SPECIES_UNDEAD)
	{
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
	}
	else if (m_tUnit.eSpecies == SPECIES::SPECIES_HUMAN)
	{
		m_pMeshCom[(_uint)CLASS::CLASS_WORKER] = (CMesh*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Human_Worker");
		NULL_CHECK_VAL(m_pMeshCom[(_uint)CLASS::CLASS_WORKER], E_FAIL);
		if (FAILED(Add_Component(L"Com_Mesh_Worker", m_pMeshCom[(_uint)CLASS::CLASS_WORKER])))
			return E_FAIL;

		m_pMeshCom[(_uint)CLASS::CLASS_CAVALRY] = (CMesh*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Human_Light_Cavalry");
		NULL_CHECK_VAL(m_pMeshCom[(_uint)CLASS::CLASS_CAVALRY], E_FAIL);
		if (FAILED(Add_Component(L"Com_Mesh_LightCavalry", m_pMeshCom[(_uint)CLASS::CLASS_CAVALRY])))
			return E_FAIL;

		m_pMeshCom[(_uint)CLASS::CLASS_CAVALRY + 1] = (CMesh*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Human_Heavy_Carvalry");
		NULL_CHECK_VAL(m_pMeshCom[(_uint)CLASS::CLASS_CAVALRY + 1], E_FAIL);
		if (FAILED(Add_Component(L"Com_Mesh_HeavyCavalry", m_pMeshCom[(_uint)CLASS::CLASS_CAVALRY + 1])))
			return E_FAIL;

		m_pMeshCom[(_uint)CLASS::CLASS_INFANTRY] = (CMesh*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Human_Light_Infantry");
		NULL_CHECK_VAL(m_pMeshCom[(_uint)CLASS::CLASS_INFANTRY], E_FAIL);
		if (FAILED(Add_Component(L"Com_Mesh_LightInfantry", m_pMeshCom[(_uint)CLASS::CLASS_INFANTRY])))
			return E_FAIL;

		m_pMeshCom[(_uint)CLASS::CLASS_INFANTRY + 1] = (CMesh*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Human_Heavy_Infantry");
		NULL_CHECK_VAL(m_pMeshCom[(_uint)CLASS::CLASS_INFANTRY + 1], E_FAIL);
		if (FAILED(Add_Component(L"Com_Mesh_HeavyInfantry", m_pMeshCom[(_uint)CLASS::CLASS_INFANTRY + 1])))
			return E_FAIL;

		m_pMeshCom[(_uint)CLASS::CLASS_SPEARMAN] = (CMesh*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Human_SpearMan");
		NULL_CHECK_VAL(m_pMeshCom[(_uint)CLASS::CLASS_SPEARMAN], E_FAIL);
		if (FAILED(Add_Component(L"Com_Mesh_Spearman", m_pMeshCom[(_uint)CLASS::CLASS_SPEARMAN])))
			return E_FAIL;

		m_pMeshCom[(_uint)CLASS::CLASS_MAGE] = (CMesh*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Human_Mage");
		NULL_CHECK_VAL(m_pMeshCom[(_uint)CLASS::CLASS_MAGE], E_FAIL);
		if (FAILED(Add_Component(L"Com_Mesh_Mage", m_pMeshCom[(_uint)CLASS::CLASS_MAGE])))
			return E_FAIL;

		m_pMeshCom[(_uint)CLASS::CLASS_MMAGE] = (CMesh*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Human_Mounted_Mage");
		NULL_CHECK_VAL(m_pMeshCom[(_uint)CLASS::CLASS_MMAGE], E_FAIL);
		if (FAILED(Add_Component(L"Com_Mesh_MountedMage", m_pMeshCom[(_uint)CLASS::CLASS_MMAGE])))
			return E_FAIL;

		m_pMeshCom[(_uint)CLASS::CLASS_ARCHER] = (CMesh*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Human_Archer");
		NULL_CHECK_VAL(m_pMeshCom[(_uint)CLASS::CLASS_ARCHER], E_FAIL);
		if (FAILED(Add_Component(L"Com_Mesh_Archer", m_pMeshCom[(_uint)CLASS::CLASS_ARCHER])))
			return E_FAIL;
	}


	m_pFrustumCom = (CFrustum*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Frustum");
	NULL_CHECK_VAL(m_pFrustumCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Frustum", m_pFrustumCom)))
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
	//Component_Shader_PostEffect
	m_pShaderCom_PostEffect = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_PostEffect");
	NULL_CHECK_VAL(m_pShaderCom_PostEffect, E_FAIL);
	if (FAILED(Add_Component(L"Com_PostEffectShader", m_pShaderCom_PostEffect)))
		return E_FAIL;
	//m_pShaderCom_Blur
	m_pShaderCom_Blur = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Blur");
	NULL_CHECK_VAL(m_pShaderCom_Blur, E_FAIL);
	if (FAILED(Add_Component(L"Com_BlurShader", m_pShaderCom_Blur)))
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


	m_pCollider_OBB = (CCollider*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Collider_OBB");
	NULL_CHECK_VAL(m_pCollider_OBB, E_FAIL);
	if (FAILED(Add_Component(L"Com_Collider_OBB", m_pCollider_OBB)))
		return E_FAIL;

	m_pCollider_AABB = (CCollider*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Collider_AABB");
	NULL_CHECK_VAL(m_pCollider_AABB, E_FAIL);
	if (FAILED(Add_Component(L"Com_Collider_AABB", m_pCollider_AABB)))
		return E_FAIL;

	m_pCollider_Attack = (CCollider*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Collider_ATTACK");
	NULL_CHECK_VAL(m_pCollider_Attack, E_FAIL);
	if (FAILED(Add_Component(L"Com_Collider_Attack", m_pCollider_Attack)))
		return E_FAIL;

	m_pTextureCom[0] = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Horse");
	NULL_CHECK_VAL(m_pTextureCom[0], E_FAIL);
	if (FAILED(Add_Component(L"Com_Texture0", m_pTextureCom[0])))
		return E_FAIL;

	if (m_tUnit.eSpecies == SPECIES::SPECIES_HUMAN)
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


	m_pNaviCom = (CNavigation*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_NaviMesh");
	NULL_CHECK_VAL(m_pNaviCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Navi", m_pNaviCom)))
		return E_FAIL;


	Safe_Release(pManagement);
	return S_OK;
}

void CPlayer::Set_Animation(const _float& fTimeDelta)
{
	if (m_iCurAnimIdx != m_iPreAnimIdx)
	{

		m_vecAnimCtrl[m_iCurAnimIdx].fCurTime = 0.f;
		m_iPreAnimIdx = m_iCurAnimIdx;
	}
}

void CPlayer::Change_Class()
{
	if (m_eCurClass != m_ePreClass)
	{
		m_fRunSoundTime = 0.f;
		m_pCurAnimCom = m_pAnimCom[(_uint)m_eCurClass];
		m_pCurMeshCom = m_pMeshCom[(_uint)m_eCurClass];
		m_iCurAnimIdx = 0;

		//여기
		SkillClear();
		DeathMontion_Init();
		AnimVectorClear();
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
			m_vOBB_Range[0] = { 10.f ,80.f,10.f };
			m_vOBB_Range[1] = { 30.f ,80.f,30.f };
			m_iCombatMotion[0] = 0;
			m_iCombatMotion[1] = 1;
			m_iCombatMotion[2] = 2;
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
			m_vOBB_Range[0] = { 10.f ,80.f,10.f };
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
			m_vOBB_Range[0] = { 40.f ,80.f,40.f };
			m_vOBB_Range[1] = { 80.f ,80.f,80.f };
			m_iCombatMotion[0] = 4;
			m_iCombatMotion[1] = 5;
			m_iCombatMotion[2] = 3;
		}
		break;
		case CLASS(2):
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
			m_vOBB_Range[0] = { 40.f ,80.f,40.f };
			m_vOBB_Range[1] = { 80.f ,80.f,80.f };
			m_iCombatMotion[0] = 4;
			m_iCombatMotion[1] = 5;
			m_iCombatMotion[2] = 3;
		}
		break;
		case CLASS(4):
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
			m_vOBB_Range[0] = { 10.f ,80.f,10.f };
			m_vOBB_Range[1] = { 30.f ,80.f,30.f };
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
			m_vOBB_Range[1] = { 100.f ,80.f,100.f };
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
			m_vOBB_Range[0] = { 20.f ,80.f,60.f };
			m_vOBB_Range[1] = { 80.f ,80.f,80.f };
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
			m_iCombatMotion[0] = 3;
			m_iCombatMotion[1] = 4;
			m_iCombatMotion[2] = 2;
		}
		break;
		}
		m_ePreClass = m_eCurClass;
	}
}

void CPlayer::AnimVectorClear()
{
	m_vecAnimCtrl.clear();
	m_vecAnimCtrl.shrink_to_fit();
}

void CPlayer::Obb_Collision()
{
	if (m_IsBack)
	{
		if (m_IsOBB_Collision && m_fBazierCnt <= 1.f)
		{
			if (!m_IsBazier)
			{
				_vec3 vTargetPos = { m_matAttackedTarget.m[3][0], m_matAttackedTarget.m[3][1], m_matAttackedTarget.m[3][2] };
				_vec3 vPos = *m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION);
				_vec3 vTemp = { vPos - vTargetPos };
				vTemp *= 5.f;
				m_vStartPoint = vPos;
				m_vEndPoint = *m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION) + (vTemp);
				m_vMidPoint = (m_vStartPoint + m_vEndPoint) / 2;
				m_vMidPoint.y += 10.f;
				m_eCurState = STATE::STATE_HITTED;
				Create_Particle(*m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION));
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
	else
	{
		if (m_IsOBB_Collision && m_fBazierCnt <= 1.f)
		{
			if (!m_IsBazier)
			{
				_vec3 vTargetPos = { m_matAttackedTarget.m[3][0], m_matAttackedTarget.m[3][1], m_matAttackedTarget.m[3][2] };
				_vec3 vPos = *m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION);
				_vec3 vTemp = { vPos - vTargetPos };
				vTemp *= 1.f;
				m_vStartPoint = vPos;
				m_vEndPoint = *m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION) + (vTemp);
				m_vMidPoint = (m_vStartPoint + m_vEndPoint) / 2;
				//m_vMidPoint.y += 10.f;
				Create_Particle(*m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION));
				m_eCurState = STATE::STATE_HITTED;
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

}

void CPlayer::Hit_Object(_float& fCnt, _vec3 vStart, _vec3 vEnd, _vec3 vMid)
{
	_float fX = (pow((1.f - fCnt), 2) * vStart.x) + (2 * fCnt * (1.f - fCnt) * vMid.x) + (pow(fCnt, 2) * vEnd.x);
	_float fY = (pow((1.f - fCnt), 2) * vStart.y) + (2 * fCnt * (1.f - fCnt) * vMid.y) + (pow(fCnt, 2) * vEnd.y);
	_float fZ = (pow((1.f - fCnt), 2) * vStart.z) + (2 * fCnt * (1.f - fCnt) * vMid.z) + (pow(fCnt, 2) * vEnd.z);

	_vec3 vPos = { fX, fY, fZ };
	m_pTransformCom->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
	fCnt += 0.02f;
}

void CPlayer::Select_Class()
{

}

void CPlayer::Input_Key(const _float& fTimeDelta)
{
	CServer_Manager* server = CServer_Manager::GetInstance();
	if (nullptr == server)
		return;
	server->AddRef();

	if (0 >= m_tInfo.fHP)
		return;

	if (CManagement::GetInstance()->Key_Up(KEY_DOWN))
	{
		if (!m_IsCombat)
		{
			m_cMoveCondition = CON_IDLE;
			if (m_cLastMoveCondition != m_cMoveCondition)
			{
				server->send_condition_packet(CON_TYPE_MOVE, CON_IDLE);
				server->send_animation_packet(A_IDLE);
				m_cLastMoveCondition = m_cMoveCondition;
			}
		}
		else
			m_iCurAnimIdx = m_iCombatMotion[0];
		m_IsActioning = false;
	}
	if (CManagement::GetInstance()->Key_Up(KEY_LEFT))
	{
		if (!m_IsCombat)
		{
			m_cRotateCondition = CON_IDLE;
			if (m_cLastRotateCondition != m_cRotateCondition)
			{
				server->send_condition_packet(CON_TYPE_ROTATE, CON_IDLE);
				server->send_animation_packet(A_IDLE);
				m_cLastRotateCondition = m_cRotateCondition;
			}
		}
		else
			m_iCurAnimIdx = m_iCombatMotion[0];
		m_IsActioning = false;
	}
	if (CManagement::GetInstance()->Key_Up(KEY_RIGHT))
	{
		if (!m_IsCombat)
		{
			m_cRotateCondition = CON_IDLE;
			if (m_cLastRotateCondition != m_cRotateCondition)
			{
				server->send_condition_packet(CON_TYPE_ROTATE, CON_IDLE);
				server->send_animation_packet(A_IDLE);
				m_cLastRotateCondition = m_cRotateCondition;
			}
		}
		else
			m_iCurAnimIdx = m_iCombatMotion[0];
		m_IsActioning = false;
	}
	if (CManagement::GetInstance()->Key_Up(KEY_UP))
	{
		if (!m_IsCombat)
		{
			m_cMoveCondition = CON_IDLE;
			if (m_cLastMoveCondition != m_cMoveCondition)
			{
				server->send_condition_packet(CON_TYPE_MOVE, CON_IDLE);
				server->send_animation_packet(A_IDLE);
				m_cLastMoveCondition = m_cMoveCondition;
			}
		}
		else
			m_iCurAnimIdx = m_iCombatMotion[0];
		m_IsRun = false;
		m_IsActioning = false;
	}

	if (!m_IsActioning)
	{
		if (CManagement::GetInstance()->Key_Down(KEY_LBUTTON))
		{
			duration<double> cool_time = duration_cast<duration<double>>(high_resolution_clock::now()
				- server->Get_Attack_Cooltime());
			if (cool_time.count() > 2) // ↑ 쿨타임 2초 계산해주는 식
			{
				server->send_attack_packet();
				server->Set_Attack_CoolTime(high_resolution_clock::now());
				server->send_animation_packet(A_ATTACK);
				CManagement::GetInstance()->Play_Sound(CHANNEL_ATTACK, SOUND_OBJECT, ATTACK);
			}

			if (m_eCurClass == CLASS::CLASS_ARCHER)
			{
				CGameObject* pOwnPlayer = nullptr;
				_matrix matTemp = m_pTransformCom->Get_Matrix();
				if (FAILED(CManagement::GetInstance()->Add_GameObjectToLayer(L"GameObject_ThrowArrow", (_uint)SCENEID::SCENE_STAGE, L"Layer_Arrow", &pOwnPlayer, (void*)&matTemp)))
					return;
				server->send_arrow_packet();
				dynamic_cast<CThrow_Arrow*>(pOwnPlayer)->GetOwnPlayer() = this;
				m_eCurState = STATE::STATE_ARROW;
			}
			else if (m_eCurClass == CLASS::CLASS_WORKER)
			{
				//_matrix matTemp = m_pTransformCom->Get_Matrix();
				//if (FAILED(CManagement::GetInstance()->Add_GameObjectToLayer(L"GameObject_Deffend", (_uint)SCENEID::SCENE_STAGE, L"Layer_Deffend", nullptr, (void*)&matTemp)))
				//	return;
				m_eCurState = STATE::STATE_ATTACK;
				if (FAILED(CManagement::GetInstance()->Add_GameObjectToLayer(L"GameObject_EffectBox", (_uint)SCENEID::SCENE_STAGE, L"Layer_EffectBox", nullptr, m_pTransformCom)))
					return;
			}
			else
			{
				//enum Sound_Character { SOUND_OBJECT, SOUND_BG, SOUND_END };
				//enum SoundState { ATTACK, WALK, RUN, HIT, DIE, HITTED, BG_STAGE, SHOOT, BG, LOGO, END };
				//enum SoundChannel { CHANNEL_ATTACK, CHANNEL_EFEECT, CHANNEL_BG, CHANNEL_FLASH, CHANNEL_KILL, CHANNEL_END };
				//Play_Sound(SoundChannel eChannel, Sound_Character eCharacter, SoundState State, const _float& fVolume, FMOD_MODE eMode)
				m_eCurState = STATE::STATE_ATTACK;
				if (FAILED(CManagement::GetInstance()->Add_GameObjectToLayer(L"GameObject_EffectBox", (_uint)SCENEID::SCENE_STAGE, L"Layer_EffectBox", nullptr, m_pTransformCom)))
					return;
			}

			if (m_IsFire)
			{
				m_IsFire = false;

				list<CGameObject*> lst = CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_SkillFire");
				int numver = lst.size();
				CGameObject* fire = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_SkillFire", numver - 1);
				dynamic_cast<CFire*>(fire)->SetSCheck(true);

				// 파이어 좌표 
				_vec3 vPos = *dynamic_cast<CTransform*>(fire->Get_ComponentPointer(L"Com_Transform"))->Get_StateInfo(CTransform::STATE_POSITION);
				XMFLOAT2 fPos = { vPos.x, vPos.z };

				server->send_fire_packet(vPos.x, vPos.z);

				CGameObject* sTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_UI", 22);
				dynamic_cast<CUI_Skill*>(sTemp)->SetStime(true);
			}
			if (m_IsTeleport)
			{
				m_IsTeleport = false;

				list<CGameObject*> lst = CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_SkillTeleport");
				int numver = lst.size();
				CGameObject* tele = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_SkillTeleport", numver - 1);

				// 텔레포트 좌표 
				_vec3 vPos = *dynamic_cast<CTransform*>(tele->Get_ComponentPointer(L"Com_Transform"))->Get_StateInfo(CTransform::STATE_POSITION);
				XMFLOAT2 fPos = { vPos.x, vPos.z };

				server->send_teleport_packet(vPos.x, vPos.z);

				CGameObject* pTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_UI", 23);
				dynamic_cast<CUI_Skill*>(pTemp)->SetStime(true);
			}

			m_IsActioning = true;
			m_IsOnce = true;
			m_IsHit = true;
			//m_IsCombat = true;
		}
		if (CManagement::GetInstance()->Key_Up(KEY_LBUTTON))
		{
			m_eCurState = STATE::STATE_IDLE;
		}
	}

	if (CManagement::GetInstance()->Key_Pressing(KEY_DOWN))
	{
		if (!m_IsCombat)
			server->send_animation_packet(A_WALK);
		else
			m_iCurAnimIdx = m_iCombatMotion[1];

		m_cMoveCondition = CON_BACK;
		if (m_cLastMoveCondition != m_cMoveCondition)
		{
			server->send_condition_packet(CON_TYPE_MOVE, CON_BACK);
			m_cLastMoveCondition = m_cMoveCondition;
		}
		m_IsActioning = true;
		m_eCurState = STATE::STATE_WALK;
	}
	if (CManagement::GetInstance()->Key_Pressing(KEY_LEFT))
	{
		if (!m_IsCombat)
			server->send_animation_packet(A_WALK);
		else
			m_iCurAnimIdx = m_iCombatMotion[1];

		m_cRotateCondition = CON_LEFT;
		if (m_cLastRotateCondition != m_cRotateCondition)
		{
			server->send_condition_packet(CON_TYPE_ROTATE, CON_LEFT);
			m_cLastRotateCondition = m_cRotateCondition;
		}
		m_IsActioning = true;
		m_eCurState = STATE::STATE_IDLE;
	}
	
	if (CManagement::GetInstance()->Key_Pressing(KEY_RIGHT))
	{
		if (!m_IsCombat)
			server->send_animation_packet(A_WALK);
		else
			m_iCurAnimIdx = m_iCombatMotion[1];

		m_cRotateCondition = CON_RIGHT;
		if (m_cLastRotateCondition != m_cRotateCondition)
		{
			server->send_condition_packet(CON_TYPE_ROTATE, CON_RIGHT);
			m_cLastRotateCondition = m_cRotateCondition;
		}
		m_IsActioning = true;
		m_eCurState = STATE::STATE_WALK;
	}

	if (CManagement::GetInstance()->Key_Combine(KEY_UP, KEY_SHIFT))
	{
		m_IsSlide = true;
		if (!m_IsCombat)
			server->send_animation_packet(A_RUN);
		else
			m_iCurAnimIdx = m_iCombatMotion[2];

		_vec3 vLook = {};
		vLook = *m_pTransformCom->Get_StateInfo(CTransform::STATE_LOOK);
		vLook = Vector3_::Normalize(vLook);

		m_pTransformCom->SetSpeed(m_fArrSpeedUP[(_uint)m_eCurClass]);
		_vec3 vDirectionPerSec = (vLook * 5.f * fTimeDelta);
		_vec3 vSlide = {};
		if (!m_pNaviCom->Move_OnNavigation(m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION), &vDirectionPerSec, &vSlide))
		{
			m_cMoveCondition = CON_RUN;
			if (m_cLastMoveCondition != m_cMoveCondition)
			{
				server->send_condition_packet(CON_TYPE_MOVE, CON_RUN);
				m_cLastMoveCondition = m_cMoveCondition;
			}
		}
		else
		{
			m_pTransformCom->Go_There(vSlide);
		}

		m_IsRun = true;
		m_IsActioning = true;
		m_IsParticleRun = true;
		m_eCurState = STATE::STATE_RUN;

	}
	else if (CManagement::GetInstance()->Key_Pressing(KEY_UP))
	{
		m_IsSlide = true;
		if (!m_IsCombat)
			server->send_animation_packet(A_WALK);
		else
			m_iCurAnimIdx = m_iCombatMotion[1];

		_vec3 vLook = {};
		vLook = *m_pTransformCom->Get_StateInfo(CTransform::STATE_LOOK);
		vLook = Vector3_::Normalize(vLook);

		m_pTransformCom->SetSpeed(m_fArrSpeed[(_uint)m_eCurClass]);
		_vec3 vDirectionPerSec = (vLook * 5.f * fTimeDelta);
		_vec3 vSlide = {};
		if (!m_IsSlide)
		{
			if (m_pNaviCom->Move_OnNavigation(m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION), &vDirectionPerSec, &vSlide))
			{
				m_cMoveCondition = CON_STRAIGHT;
				if (m_cLastMoveCondition != m_cMoveCondition)
				{
					server->send_condition_packet(CON_TYPE_MOVE, CON_STRAIGHT);
					m_cLastMoveCondition = m_cMoveCondition;
				}
			}
			else
			{
				m_pTransformCom->Go_There(vSlide);
			}
		}
		else
		{
			m_cMoveCondition = CON_STRAIGHT;
			if (m_cLastMoveCondition != m_cMoveCondition)
			{
				server->send_condition_packet(CON_TYPE_MOVE, CON_STRAIGHT);
				m_cLastMoveCondition = m_cMoveCondition;
			}
			m_IsSlide = false;
		}

		m_IsActioning = true;
		m_IsRun = false;

		m_fRunSoundTime += fTimeDelta;
		if (m_eCurClass == CLASS::CLASS_CAVALRY || m_eCurClass == CLASS(2) || m_eCurClass == CLASS::CLASS_MMAGE)
		{
			if (m_fRunSoundTime >= 24.f)
			{
				CManagement::GetInstance()->Play_Sound(CHANNEL_EFEECT, SOUND_OBJECT, WALK);
				m_fRunSoundTime = 0.f;
			}
		}
		else
		{
			if (m_fRunSoundTime >= 0.5f)
			{
				CManagement::GetInstance()->Play_Sound(CHANNEL_EFEECT, SOUND_OBJECT, WALK);
				m_fRunSoundTime = 0.f;
			}
		}
	}

	if (CManagement::GetInstance()->Key_Down(KEY_F1))
	{
		for (auto& iter : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_UI_Select"))
		{
			dynamic_cast<CUI_Select*>(iter)->Get_IsCheck() = false;
		}
		CGameObject* pGameObject = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_UI_Select", 0);
		dynamic_cast<CUI_Select*>(pGameObject)->Get_IsCheck() = true;
		server->Set_TroopClass(T_INFT);
	}
	if (CManagement::GetInstance()->Key_Down(KEY_F2))
	{
		for (auto& iter : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_UI_Select"))
		{
			dynamic_cast<CUI_Select*>(iter)->Get_IsCheck() = false;
		}
		CGameObject* pGameObject = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_UI_Select", 1);
		dynamic_cast<CUI_Select*>(pGameObject)->Get_IsCheck() = true;
		server->Set_TroopClass(T_HORSE);
	}
	if (CManagement::GetInstance()->Key_Down(KEY_F3))
	{
		for (auto& iter : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_UI_Select"))
		{
			dynamic_cast<CUI_Select*>(iter)->Get_IsCheck() = false;
		}
		CGameObject* pGameObject = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_UI_Select", 2);
		dynamic_cast<CUI_Select*>(pGameObject)->Get_IsCheck() = true;
		server->Set_TroopClass(T_MAGE);
	}
	if (CManagement::GetInstance()->Key_Down(KEY_F4))
	{
		for (auto& iter : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_UI_Select"))
		{
			dynamic_cast<CUI_Select*>(iter)->Get_IsCheck() = false;
		}
		CGameObject* pGameObject = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_UI_Select", 3);
		dynamic_cast<CUI_Select*>(pGameObject)->Get_IsCheck() = true;
		server->Set_TroopClass(T_BOW);
	}
	if (CManagement::GetInstance()->Key_Down(KEY_F5))
	{
		for (auto& iter : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_UI_Select"))
		{
			dynamic_cast<CUI_Select*>(iter)->Get_IsCheck() = false;
		}
		CGameObject* pGameObject = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_UI_Select", 4);
		dynamic_cast<CUI_Select*>(pGameObject)->Get_IsCheck() = true;
		server->Set_TroopClass(T_ALL);
	}

	if (CManagement::GetInstance()->Key_Down(KEY_1))
	{
		server->send_npc_act_packet(DO_FOLLOW);
	}
	if (CManagement::GetInstance()->Key_Down(KEY_2))
	{
		server->send_npc_act_packet(DO_ATTACK);
	}
	if (CManagement::GetInstance()->Key_Down(KEY_3))
	{
		server->send_npc_act_packet(DO_HOLD);
	}

	Safe_Release(server);
}

void CPlayer::Compute_Matrix_Z()
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

void CPlayer::Compute_Matrix_X()
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

void CPlayer::Death(const _float& fTimeDelta)
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

void CPlayer::DeathMontion_Init()
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
	case CLASS(4):
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
	case CLASS(2):
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

void CPlayer::Attack(const _float& fTimeDelta)
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
}

void CPlayer::Combat(const _float& fTimeDelta)
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

void CPlayer::SetSpeed()
{
	m_fArrSpeed[(_uint)CLASS::CLASS_WORKER] = 10.f;
	m_fArrSpeedUP[(_uint)CLASS::CLASS_WORKER] = 20.f;

	m_fArrSpeed[(_uint)CLASS::CLASS_CAVALRY] = 20.f;
	m_fArrSpeedUP[(_uint)CLASS::CLASS_CAVALRY] = 40.f;

	m_fArrSpeed[(_uint)CLASS(2)] = 20.f;
	m_fArrSpeedUP[(_uint)CLASS(2)] = 40.f;

	m_fArrSpeed[(_uint)CLASS::CLASS_INFANTRY] = 10.f;
	m_fArrSpeedUP[(_uint)CLASS::CLASS_INFANTRY] = 20.f;

	m_fArrSpeed[(_uint)CLASS(4)] = 10.f;
	m_fArrSpeedUP[(_uint)CLASS(4)] = 20.f;

	m_fArrSpeed[(_uint)CLASS::CLASS_SPEARMAN] = 10.f;
	m_fArrSpeedUP[(_uint)CLASS::CLASS_SPEARMAN] = 20.f;

	m_fArrSpeed[(_uint)CLASS::CLASS_MAGE] = 10.f;
	m_fArrSpeedUP[(_uint)CLASS::CLASS_MAGE] = 20.f;

	m_fArrSpeed[(_uint)CLASS::CLASS_MMAGE] = 20.f;
	m_fArrSpeedUP[(_uint)CLASS::CLASS_MMAGE] = 40.f;

	m_fArrSpeed[(_uint)CLASS::CLASS_ARCHER] = 15.f;
	m_fArrSpeedUP[(_uint)CLASS::CLASS_ARCHER] = 30.f;

}

void CPlayer::Resurrection()
{
	m_eCurClass = CLASS::CLASS_WORKER;
	m_iCurAnimIdx = 0;
	_vec3 vPos = { 50.f,0.f,50.f };
	m_pTransformCom->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
	m_pTransformCom->SetUp_Speed(50.f, XMConvertToRadians(90.f));
	m_pTransformCom->Scaling(0.1f, 0.1f, 0.1f);
	m_pTransformCom->SetUp_RotationY(XMConvertToRadians(180.f));
	m_tInfo = INFO(1, 1, 1, 0);
	m_IsDead = false;
	m_IsDeadMotion = false;
	m_eCurState = STATE::STATE_IDLE;
}

void CPlayer::Play_Sound(const _float& fTimeDelta)
{
	if (m_ePreState != m_eCurState)
	{
		CManagement::GetInstance()->Pause_Sound();
		switch (m_eCurState)
		{
		case STATE::STATE_IDLE:
			CManagement::GetInstance()->Play_Sound(CHANNEL_EFEECT, SOUND_OBJECT, IDLE, 0.2f, FMOD_LOOP_NORMAL);
			break;
		case STATE::STATE_WALK:
		{
			if (m_eCurClass == CLASS::CLASS_MMAGE || m_eCurClass == CLASS::CLASS_CAVALRY || m_eCurClass == CLASS(2))
			{
				CManagement::GetInstance()->Play_Sound(CHANNEL_EFEECT, SOUND_OBJECT, WALK, 0.2f, FMOD_LOOP_NORMAL);
			}

			else
			{
				CManagement::GetInstance()->Play_Sound(CHANNEL_EFEECT, SOUND_OBJECT, WALK, 0.2f, FMOD_LOOP_NORMAL);
			}
		}
			break;
		case STATE::STATE_RUN:
		{
			if (m_eCurClass == CLASS::CLASS_MMAGE || m_eCurClass == CLASS::CLASS_CAVALRY || m_eCurClass == CLASS(2))
			{
				CManagement::GetInstance()->Play_Sound(CHANNEL_EFEECT, SOUND_OBJECT, HORSE_RUN, 2.f, FMOD_LOOP_NORMAL);
			}
			else
			{
				CManagement::GetInstance()->Play_Sound(CHANNEL_EFEECT, SOUND_OBJECT, RUN, 2.f, FMOD_LOOP_NORMAL);
			}
		}
			break;
		case STATE::STATE_ATTACK:
			CManagement::GetInstance()->Play_Sound(CHANNEL_EFEECT, SOUND_OBJECT, ATTACK, 0.2f, FMOD_LOOP_NORMAL);
			break;
		case STATE::STATE_ARROW:
			CManagement::GetInstance()->Play_Sound(CHANNEL_EFEECT, SOUND_OBJECT, SHOOT, 0.2f);
			break;
		case STATE::STATE_DEAD:
			CManagement::GetInstance()->Play_Sound(CHANNEL_KILL, SOUND_OBJECT, DIE, 0.2f);
			break;
		case STATE::STATE_HITTED:
			CManagement::GetInstance()->Play_Sound(CHANNEL_FLASH, SOUND_OBJECT, HITTED, 0.2f);
			break;
		}

		m_ePreState = m_eCurState;
	}

}

void CPlayer::Create_Particle(const _vec3& vPoistion)
{
	if (m_IsParticle)
	{
		_vec3 vTemp = vPoistion;
		vTemp.y += 5.f;
		PARTICLESET tParticleSet;
		tParticleSet.vPos = vTemp;
		tParticleSet.iMaxParticle = 300;
		tParticleSet.fMaxLifeTime = 0.2f;
		tParticleSet.iMinLifeTime = 0.01f;

		tParticleSet.fStartScale = 0.5f;
		tParticleSet.fEndScale = 0.2f;

		tParticleSet.fMaxSpeed = 30.f;
		tParticleSet.fMinSpeed = 50.f;
		if (FAILED(CManagement::GetInstance()->Add_GameObjectToLayer(L"GameObject_Particle_Default", (_uint)SCENEID::SCENE_STAGE, L"Layer_Particle", nullptr, (void*)&tParticleSet)))
			return;
		m_IsParticle = false;
	}

}

void CPlayer::SkillClear()
{
	//CServer_Manager* server = CServer_Manager::GetInstance();
	//if (nullptr == server)
	//	return;
	//server->AddRef();
	///

	if (m_eCurClass == CLASS::CLASS_MAGE || m_eCurClass == CLASS::CLASS_MMAGE)
	{}
	else
	{
		if (m_IsFire)
		{
			list<CGameObject*> lst = CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_SkillFire");
			int numver = lst.size();
			CGameObject* fire = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_SkillFire", numver - 1);
			fire->GetIsDead() = true;
		}
		if (m_IsTeleport)
		{
			list<CGameObject*> lst = CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_SkillTeleport");
			int numver = lst.size();
			CGameObject* tele = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_SkillTeleport", numver - 1);
			tele->GetIsDead() = true;
		}
	}

	m_DeffendOnce = false;
	m_IsDeffend = false;

	m_InvisibleOnce = false;
	m_IsInvisible = false;
	m_IsInvisibleON = false;
	//server->send_invisible_packet(false);

	m_GetFire = false;
	m_IsFire = false;

	m_GetTeleport = false;
	m_IsTeleport = false;
	m_OneORTwo = false;

	CGameObject* zTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_UI", 22);
	dynamic_cast<CUI_Skill*>(zTemp)->SetSkillActive(false);
	dynamic_cast<CUI_Skill*>(zTemp)->SetActive(false);
	dynamic_cast<CUI_Skill*>(zTemp)->SetStime(false);
	dynamic_cast<CUI_Skill*>(zTemp)->SetCoolTime(dynamic_cast<CUI_Skill*>(zTemp)->GetMaxCoolTime());

	CGameObject* xTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_UI", 23);
	dynamic_cast<CUI_Skill*>(xTemp)->SetSkillActive(false);
	dynamic_cast<CUI_Skill*>(xTemp)->SetActive(false);
	dynamic_cast<CUI_Skill*>(xTemp)->SetStime(false);
	dynamic_cast<CUI_Skill*>(xTemp)->SetCoolTime(dynamic_cast<CUI_Skill*>(xTemp)->GetMaxCoolTime());

	///
	//Safe_Release(server);
}

void CPlayer::Skill_Deffend(const _float& fTimeDelta)
{
	CGameObject* pTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_UI", 23);
	m_IsDeffend = dynamic_cast<CUI_Skill*>(pTemp)->GetActive();

	if (!m_IsDeffend)
	{
		if (!m_DeffendOnce)
		{
			m_IsDeffend = false;
			m_DeffendOnce = true;			
		}
	}

	if(m_IsDeffend && m_DeffendOnce)
	{
		m_DeffendOnce = false;
		dynamic_cast<CUI_Skill*>(pTemp)->SetStime(true);

		_matrix matTemp = m_pTransformCom->Get_Matrix();
		if (FAILED(CManagement::GetInstance()->Add_GameObjectToLayer(L"GameObject_Deffend", (_uint)SCENEID::SCENE_STAGE, L"Layer_Deffend", nullptr, (void*)&matTemp)))
			return;	
	}
}


void CPlayer::Skill_Invisible(const _float& fTimeDelta)
{
	CServer_Manager* server = CServer_Manager::GetInstance();
	if (nullptr == server)
		return;
	server->AddRef();

	CGameObject* pTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_UI", 23);	
	m_IsInvisible = dynamic_cast<CUI_Skill*>(pTemp)->GetSkillActive();
	m_IsInvisibleON = server->Get_isInvisible(m_iLayerIdx);

	if (!m_IsInvisible)
	{
		if (!m_InvisibleOnce)
		{				
			m_IsInvisible = false;
			m_InvisibleOnce = true;			
			server->send_invisible_packet(false);
		}		
	}
	

	if (m_IsInvisible && m_InvisibleOnce)
	{
		m_InvisibleOnce = false;
		server->send_invisible_packet(true);
	}

	Safe_Release(server);
}

void CPlayer::Skill_CastFire(const _float& fTimeDelta, _float fY)
{
	if (!m_IsFire)
	{
		CGameObject* pTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_UI", 22);
		m_GetFire = dynamic_cast<CUI_Skill*>(pTemp)->GetActive();

		_bool bStart = dynamic_cast<CUI_Skill*>(pTemp)->GetSTime();
		if (m_GetFire && !bStart)
		{
			XMFLOAT2 fTemp = { 0,0 };
			if (FAILED(CManagement::GetInstance()->Add_GameObjectToLayer(L"GameObject_Fire", (_uint)SCENEID::SCENE_STAGE, L"Layer_SkillFire", nullptr, (void*)&fTemp)))
				return;

			m_IsFire = true;	//true				
		}
	}
	else
	{
		list<CGameObject*> lst = CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_SkillFire");
		int numver = lst.size();
		if (numver > 0)
		{
			CGameObject* fire = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_SkillFire", numver - 1);
			_vec3* iter0_Pos = dynamic_cast<CTransform*>(fire->Get_ComponentPointer(L"Com_Transform"))->Get_StateInfo(CTransform::STATE_POSITION);

			CGameObject* buffercom = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_Terrain", 0);
			BRUSHINFO bPos = dynamic_cast<CTerrain_Height*>(buffercom)->GetBrushINFO();

			*iter0_Pos = _vec3(bPos.vBrushPos.x, bPos.vBrushPos.y, bPos.vBrushPos.z);
		}
	}
}

void CPlayer::Skill_CastTeleport(const _float& fTimeDelta, _float fY)
{
	if (!m_IsTeleport)
	{
		CGameObject* pTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_UI", 23);
		m_GetTeleport = dynamic_cast<CUI_Skill*>(pTemp)->GetActive();

		_bool bStart = dynamic_cast<CUI_Skill*>(pTemp)->GetSTime();
		if (m_GetTeleport && !bStart)
		{	
			XMFLOAT2 fTemp = { 0,0 };
			if (FAILED(CManagement::GetInstance()->Add_GameObjectToLayer(L"GameObject_Teleport", (_uint)SCENEID::SCENE_STAGE, L"Layer_SkillTeleport", nullptr, (void*)&fTemp)))
				return;
			
			m_IsTeleport = true;	//true
		}	
	}
	else
	{
		list<CGameObject*> lst = CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_SkillTeleport");
		int numver = lst.size();
		if (numver > 0)
		{
			CGameObject* fire = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_SkillTeleport", numver - 1);

			
			_vec3* iter0_Pos = dynamic_cast<CTransform*>(fire->Get_ComponentPointer(L"Com_Transform"))->Get_StateInfo(CTransform::STATE_POSITION);

			CGameObject* buffercom = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_Terrain", 0);
			BRUSHINFO bTemp = dynamic_cast<CTerrain_Height*>(buffercom)->GetBrushINFO();

			*iter0_Pos = _vec3(bTemp.vBrushPos.x, fY, bTemp.vBrushPos.z);
		}
	}
}


//if (m_IsTeleport)
//{
//_bool IsTwo = dynamic_cast<CUI_Skill*>(pTemp)->GetIsTwo();
//if (IsTwo)
//{
//	dynamic_cast<CUI_Skill*>(pTemp)->SetIsTwo(false);
//
//	list<CGameObject*> lst = CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_SkillTeleport");
//	int numver = lst.size();
//
//	CGameObject* tOne = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_SkillTeleport", numver - 2);
//	CGameObject* tTwo = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_SkillTeleport", numver - 1);
//
//	dynamic_cast<CTeleport*>(tTwo)->SetSCheck(true);
//
//	dynamic_cast<CTeleport*>(tOne)->SetMyFriend(tTwo);
//	dynamic_cast<CTeleport*>(tTwo)->SetMyFriend(tOne);
//}	
//else
//{
//	list<CGameObject*> lst = CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_SkillTeleport");
//	int numver = lst.size();
//	CGameObject* tOne = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_SkillTeleport", numver - 1);
//	dynamic_cast<CTeleport*>(tOne)->SetSCheck(true);
//	dynamic_cast<CUI_Skill*>(pTemp)->SetIsTwo(true);
//}
//}


//void CPlayer::Skill_Fly(const _float& fTimeDelta, _float fY)
//{
//	CGameObject* pTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_UI", 22);
//	m_IsSTime = dynamic_cast<CUI_Skill*>(pTemp)->GetSTime();
//	if (!m_IsOn && !m_IsSTime)
//	{
//		CGameObject* pTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_UI", 22);
//		m_IsFly_START = dynamic_cast<CUI_Skill*>(pTemp)->GetSkillActive();
//		m_IsSTime = dynamic_cast<CUI_Skill*>(pTemp)->GetSTime();
//		m_fFlyCoolTImeMax = dynamic_cast<CUI_Skill*>(pTemp)->GetMaxCoolTime();
//	}
//
//	if (m_IsFly_START && !m_IsSTime)
//	{
//		m_IsOn = true;
//		
//		if (m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION)->y < fY + 20.f)
//			m_pTransformCom->UP(fTimeDelta);
//		else
//		{
//			m_IsFly_END = true;
//			m_pTransformCom->m_fVel = 0.f;
//		}
//
//		m_fCoolTime_ONE += fTimeDelta;
//		if (m_fCoolTime_ONE > m_fFlyCoolTImeMax)
//		{		
//			
//			m_IsOn = false;
//			m_fCoolTime_ONE = 0.f;
//			CGameObject* sTemp = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_UI", 22);
//			dynamic_cast<CUI_Skill*>(sTemp)->SetStime(true);
//		}
//	}
//	if (m_IsFly_END)
//	{
//		if (m_pTransformCom->Get_Scale().x < 0.1f)
//			m_pTransformCom->Scaling(m_pTransformCom->Get_Scale().x + 0.001f, m_pTransformCom->Get_Scale().y + 0.001f,
//				m_pTransformCom->Get_Scale().z + 0.001f);
//		if (m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION)->y > fY)
//			m_pTransformCom->Fallen(fTimeDelta);
//		else
//		{
//			m_IsFly_START = false;
//			m_IsFly_END = false;
//			m_IsStart = false;
//
//			m_pTransformCom->m_fVel = 0.f;
//		}	
//	}
//
//}