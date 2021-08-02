#include "framework.h"
#include "Animals.h"
#include "Management.h"
#include "UI_OnHead.h"

CAnimals::CAnimals()
	: CGameObject()
{
}

CAnimals::CAnimals(const CAnimals& rhs)
	: CGameObject(rhs)
{
}

HRESULT CAnimals::Ready_Prototype()
{
	return S_OK;
}

HRESULT CAnimals::Ready_GameObject(void* pArg)
{
	if (pArg)
		m_eAnimals = *(ANIMALS*)pArg;

	m_IsClone = true;
	if (FAILED(Ready_Component()))
		return E_FAIL;
	if (FAILED(CreateInputLayout()))
		return E_FAIL;
	Set_Animals(m_eAnimals);

	_vec3 vPos = { 25.f,0.f,25.f };
	m_pTransformCom->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
	m_pTransformCom->SetUp_Speed(50.f, XMConvertToRadians(90.f));
	m_pTransformCom->Scaling(0.1f, 0.1f, 0.1f);
	m_pTransformCom->SetUp_RotationX(XMConvertToRadians(90.f));
	m_tInfo = INFO(100.f,1.f,1.f,0.f);

	for (_uint i = 0; i < (_uint)ANIMALS::ANIMALS_END; ++i)
	{
		if (m_pAnimCom[i] == nullptr)
			continue;
		m_pAnimCom[i]->SetBones(m_pMeshCom[i]->GetBones());
		m_pAnimCom[i]->SetAnimClip(m_pMeshCom[i]->GetAnimClip());
		m_pAnimCom[i]->LateInit();
	}
	_vec3 vColliderSize = { 400.f ,160.f,40.f };
	m_vOBB_Range[0] = vColliderSize;
	m_vOBB_Range[1] = vColliderSize;
	m_pCollider_OBB->Clone_ColliderBox(m_pTransformCom, m_vOBB_Range[0]);
	m_pCollider_AABB->Clone_ColliderBox(m_pTransformCom, m_vOBB_Range[0]);
	m_pCollider_Attack->Clone_ColliderBox(m_pTransformCom, m_vOBB_Range[1]);

	m_iCurAnimIdx = 0;
	m_iPreAnimIdx = 100;

	m_pUI_OnHead = CUI_OnHead::Create();
	if (nullptr == m_pUI_OnHead)
		return E_FAIL;
	if (FAILED(m_pUI_OnHead->Ready_GameObject((void*)&vPos)))
		return E_FAIL;

	return S_OK;
}

_int CAnimals::Update_GameObject(const _float& fTimeDelta)
{
	m_pCollider_OBB->Update_Collider(m_pTransformCom, m_vOBB_Range[0]);
	//m_pCollider_AABB->Update_Collider(m_pTransformCom, m_vOBB_Range[0]);
	m_pCollider_Attack->Update_Collider(m_pTransformCom, m_vOBB_Range[1]);

	m_pUI_OnHead->Update_GameObject(fTimeDelta);
	m_pUI_OnHead->SetPosition(*m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION), m_eAnimals);
	m_pUI_OnHead->SetInfo(m_tInfo);

	CBuffer_Terrain_Height* pTerrainBuffer = (CBuffer_Terrain_Height*)CManagement::GetInstance()->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE, L"Layer_Terrain", L"Com_Buffer");
	if (nullptr == pTerrainBuffer)
		return NO_EVENT;

	_float		fY = pTerrainBuffer->Compute_HeightOnTerrain(m_pTransformCom);
	m_pTransformCom->Set_PositionY(fY);

	Obb_Collision();

	if (m_tInfo.fHP <= 0.f)
	{
		m_IsDead = true;
	}
	
	if (m_IsDead)
		Resurrection();
	if (m_pAnimCom[(_uint)m_eAnimals]->Update(m_vecAnimCtrl[m_iCurAnimIdx], fTimeDelta, true) && m_IsOnce)
	{
		m_IsOnce = false;
	}

	return _int();
}

_int CAnimals::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;
	m_pUI_OnHead->LastUpdate_GameObject(fTimeDelta);

	if (m_pFrustumCom->Culling_Frustum(m_pTransformCom), 10.f)
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONEALPHA, this)))
			return -1;
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this)))
			return -1;
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLUR, this)))
			return -1;
		//if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_POST, this)))
		//	return -1;
	}
	else
	{
		m_matOldWorld = m_pTransformCom->Get_Matrix();;
		m_matOldView = CCamera_Manager::GetInstance()->GetMatView();
	}


	Set_Animation(fTimeDelta);
	return _int();
}

void CAnimals::Render_GameObject()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();

	_uint iSubsetNum = m_pMeshCom[(_uint)m_eAnimals]->GetSubsetNum();
	for (_uint i = 0; i < iSubsetNum; ++i)
	{
		MAINPASS tMainPass = {};
		_matrix matWorld = m_pTransformCom->Get_Matrix();
		_matrix matView = CCamera_Manager::GetInstance()->GetMatView();
		_matrix matProj = CCamera_Manager::GetInstance()->GetMatProj();

		REP tRep = {};
		tRep.m_arrInt[0] = 1;
		tRep.m_arrInt[1] = m_pAnimCom[(_uint)m_eAnimals]->GetBones()->size();
		tRep.m_arrInt[2] = g_DefferedRender;

		m_pShaderCom->SetUp_OnShader(matWorld, matView, matProj, tMainPass);

		_uint iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
			(_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);

		iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->SetData((void*)&tRep);
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
			(_uint)CONST_REGISTER::b8)->GetCBV().Get(), iOffeset, CONST_REGISTER::b8);

		//if (iSubsetNum >= 2)
		//{
		//	if (i == 0)
		//		CDevice::GetInstance()->SetTextureToShader(m_pTextureCom[0], TEXTURE_REGISTER::t0, (_uint)HORSE::HORSE_A);
		//	else
		//		CDevice::GetInstance()->SetTextureToShader(m_pTextureCom[1], TEXTURE_REGISTER::t0, (_uint)m_tPlayer.eColor);
		//}
		//else
		//{
		//	CDevice::GetInstance()->SetTextureToShader(m_pTextureCom[1], TEXTURE_REGISTER::t0, (_uint)m_tPlayer.eColor);
		//}

		m_pAnimCom[(_uint)m_eAnimals]->UpdateData(m_pMeshCom[(_uint)m_eAnimals], m_pComputeShaderCom);
		CDevice::GetInstance()->UpdateTable();
		m_pMeshCom[(_uint)m_eAnimals]->Render_Mesh(i);
	}


	m_pCollider_OBB->Render_Collider();
	m_pCollider_Attack->Render_Collider(1);
	//m_pColiider[1]->Render_Collider();



	Safe_Release(pManagement);
}

void CAnimals::Render_GameObject_Shadow()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();


	_uint iSubsetNum = m_pMeshCom[(_uint)m_eAnimals]->GetSubsetNum();
	for (_uint i = 0; i < iSubsetNum; ++i)
	{
		MAINPASS tMainPass = {};
		_matrix matWorld = m_pTransformCom->Get_Matrix();
		_matrix matView = CCamera_Manager::GetInstance()->GetShadowView();
		_matrix matProj = CCamera_Manager::GetInstance()->GetShadowMatProj();

		REP tRep = {};
		tRep.m_arrInt[0] = 1;
		tRep.m_arrInt[1] = m_pAnimCom[(_uint)m_eAnimals]->GetBones()->size();

		m_pShaderCom_Shadow->SetUp_OnShader(matWorld, matView, matProj, tMainPass);

		_uint iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
			(_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);

		iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->SetData((void*)&tRep);
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
			(_uint)CONST_REGISTER::b8)->GetCBV().Get(), iOffeset, CONST_REGISTER::b8);

		m_pAnimCom[(_uint)m_eAnimals]->UpdateData(m_pMeshCom[(_uint)m_eAnimals], m_pComputeShaderCom);
		CDevice::GetInstance()->UpdateTable();
		m_pMeshCom[(_uint)m_eAnimals]->Render_Mesh(i);
	}
	Safe_Release(pManagement);
}

void CAnimals::Render_PostEffect()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();


	_uint iSubsetNum = m_pMeshCom[(_uint)m_eAnimals]->GetSubsetNum();
	for (_uint i = 0; i < iSubsetNum; ++i)
	{
		MAINPASS tMainPass = {};
		_matrix matWorld = m_pTransformCom->Get_Matrix();
		_matrix matView = CCamera_Manager::GetInstance()->GetMatView();
		_matrix matProj = CCamera_Manager::GetInstance()->GetMatProj();


		REP tRep = {};
		tRep.m_arrInt[0] = 1;
		tRep.m_arrInt[1] = m_pAnimCom[(_uint)m_eAnimals]->GetBones()->size();

		m_pShaderCom_PostEffect->SetUp_OnShader(matWorld, matView, matProj, tMainPass);

		_uint iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
			(_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);

		iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->SetData((void*)&tRep);
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
			(_uint)CONST_REGISTER::b8)->GetCBV().Get(), iOffeset, CONST_REGISTER::b8);

		ComPtr<ID3D12DescriptorHeap>	pPostEffectTex = CManagement::GetInstance()->GetPostEffectTex()->GetSRV().Get();
		CDevice::GetInstance()->SetTextureToShader(pPostEffectTex.Get(), TEXTURE_REGISTER::t0);


		m_pAnimCom[(_uint)m_eAnimals]->UpdateData(m_pMeshCom[(_uint)m_eAnimals], m_pComputeShaderCom);
		CDevice::GetInstance()->UpdateTable();
		m_pMeshCom[(_uint)m_eAnimals]->Render_Mesh(i);
	}
	Safe_Release(pManagement);
}

void CAnimals::Render_Blur()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();


	_uint iSubsetNum = m_pMeshCom[(_uint)m_eAnimals]->GetSubsetNum();
	for (_uint i = 0; i < iSubsetNum; ++i)
	{
		MAINPASS tMainPass = {};
		_matrix matWorld = m_pTransformCom->Get_Matrix();
		_matrix matView = CCamera_Manager::GetInstance()->GetMatView();
		_matrix matProj = CCamera_Manager::GetInstance()->GetMatProj();


		REP tRep = {};
		tRep.m_arrInt[0] = 1;
		tRep.m_arrInt[1] = m_pAnimCom[(_uint)m_eAnimals]->GetBones()->size();

		tRep.m_arrMat[0] = m_matOldWorld;//OldWorld
		tRep.m_arrMat[1] = m_matOldView;//OldView

		m_pShaderCom_Blur->SetUp_OnShader(matWorld, matView, matProj, tMainPass);

		_uint iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
			(_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);

		iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->SetData((void*)&tRep);
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
			(_uint)CONST_REGISTER::b8)->GetCBV().Get(), iOffeset, CONST_REGISTER::b8);



		m_pAnimCom[(_uint)m_eAnimals]->UpdateData(m_pMeshCom[(_uint)m_eAnimals], m_pComputeShaderCom);
		CDevice::GetInstance()->UpdateTable();
		m_pMeshCom[(_uint)m_eAnimals]->Render_Mesh(i);
	}

	m_matOldWorld = m_pTransformCom->Get_Matrix();
	m_matOldView = CCamera_Manager::GetInstance()->GetMatView();

	Safe_Release(pManagement);
}

HRESULT CAnimals::CreateInputLayout()
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
	if (FAILED(m_pShaderCom_Blur->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::LESS, SHADER_TYPE::SHADER_BLUR)))
		return E_FAIL;

	return S_OK;
}

CAnimals* CAnimals::Create()
{
	CAnimals* pInstance = new CAnimals();

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MessageBox(0, L"CAnimals Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CAnimals::Clone_GameObject(void* pArg, _uint iIdx)
{
	CAnimals* pInstance = new CAnimals(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MessageBox(0, L"CAnimals Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	m_iLayerIdx = iIdx;
	return pInstance;
}

void CAnimals::Free()
{
	for (_uint i = 0; i < (_uint)ANIMALS::ANIMALS_END; ++i)
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

	Safe_Release(m_pUI_OnHead);
	CGameObject::Free();
}

HRESULT CAnimals::Ready_Component()
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
	m_pShaderCom_Shadow = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Shadow");
	NULL_CHECK_VAL(m_pShaderCom_Shadow, E_FAIL);
	if (FAILED(Add_Component(L"Com_ShadowShader", m_pShaderCom_Shadow)))
		return E_FAIL;
	m_pShaderCom_PostEffect = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_PostEffect");
	NULL_CHECK_VAL(m_pShaderCom_PostEffect, E_FAIL);
	if (FAILED(Add_Component(L"Com_PostEffectShader", m_pShaderCom_PostEffect)))
		return E_FAIL;
	m_pShaderCom_Blur = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Blur");
	NULL_CHECK_VAL(m_pShaderCom_Blur, E_FAIL);
	if (FAILED(Add_Component(L"Com_BlurShader", m_pShaderCom_Blur)))
		return E_FAIL;

	m_pMeshCom[(_uint)ANIMALS::ANIMALS_BEAR] = (CMesh*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Animals_Bear");
	NULL_CHECK_VAL(m_pMeshCom[(_uint)ANIMALS::ANIMALS_BEAR], E_FAIL);
	if (FAILED(Add_Component(L"Com_Mesh_ANIMALS_BEAR", m_pMeshCom[(_uint)ANIMALS::ANIMALS_BEAR])))
		return E_FAIL;

	m_pMeshCom[(_uint)ANIMALS::ANIMALS_BOAR] = (CMesh*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Animals_Boar");
	NULL_CHECK_VAL(m_pMeshCom[(_uint)ANIMALS::ANIMALS_BOAR], E_FAIL);
	if (FAILED(Add_Component(L"Com_Mesh_ANIMALS_BOAR", m_pMeshCom[(_uint)ANIMALS::ANIMALS_BOAR])))
		return E_FAIL;

	m_pMeshCom[(_uint)ANIMALS::ANIMALS_DEER] = (CMesh*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Animals_Deer");
	NULL_CHECK_VAL(m_pMeshCom[(_uint)ANIMALS::ANIMALS_DEER], E_FAIL);
	if (FAILED(Add_Component(L"Com_Mesh_ANIMALS_DEER", m_pMeshCom[(_uint)ANIMALS::ANIMALS_DEER])))
		return E_FAIL;

	m_pMeshCom[(_uint)ANIMALS::ANIMALS_WOLF] = (CMesh*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Animals_Wolf");
	NULL_CHECK_VAL(m_pMeshCom[(_uint)ANIMALS::ANIMALS_WOLF], E_FAIL);
	if (FAILED(Add_Component(L"Com_Mesh_ANIMALS_WOLF", m_pMeshCom[(_uint)ANIMALS::ANIMALS_WOLF])))
		return E_FAIL;

	m_pAnimCom[(_uint)ANIMALS::ANIMALS_BEAR] = (CAnimator*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Animation");
	NULL_CHECK_VAL(m_pAnimCom[(_uint)ANIMALS::ANIMALS_BEAR], E_FAIL);
	if (FAILED(Add_Component(L"Com_Anim0", m_pAnimCom[(_uint)ANIMALS::ANIMALS_BEAR])))
		return E_FAIL;
	m_pAnimCom[(_uint)ANIMALS::ANIMALS_BOAR] = (CAnimator*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Animation");
	NULL_CHECK_VAL(m_pAnimCom[(_uint)ANIMALS::ANIMALS_BOAR], E_FAIL);
	if (FAILED(Add_Component(L"Com_Anim1", m_pAnimCom[(_uint)ANIMALS::ANIMALS_BOAR])))
		return E_FAIL;
	m_pAnimCom[(_uint)ANIMALS::ANIMALS_DEER] = (CAnimator*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Animation");
	NULL_CHECK_VAL(m_pAnimCom[(_uint)ANIMALS::ANIMALS_DEER], E_FAIL);
	if (FAILED(Add_Component(L"Com_Anim2", m_pAnimCom[(_uint)ANIMALS::ANIMALS_DEER])))
		return E_FAIL;
	m_pAnimCom[(_uint)ANIMALS::ANIMALS_WOLF] = (CAnimator*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Animation");
	NULL_CHECK_VAL(m_pAnimCom[(_uint)ANIMALS::ANIMALS_WOLF], E_FAIL);
	if (FAILED(Add_Component(L"Com_Anim3", m_pAnimCom[(_uint)ANIMALS::ANIMALS_WOLF])))
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



	m_pNaviCom = (CNavigation*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_NaviMesh");
	NULL_CHECK_VAL(m_pNaviCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Navi", m_pNaviCom)))
		return E_FAIL;


	Safe_Release(pManagement);
	return S_OK;
}

void CAnimals::Set_Animation(const _float& fTimeDelta)
{
	if (m_iCurAnimIdx != m_iPreAnimIdx)
	{

		m_vecAnimCtrl[m_iCurAnimIdx].fCurTime = 0.f;
		m_iPreAnimIdx = m_iCurAnimIdx;
	}

}

void CAnimals::Set_Animals(ANIMALS eAnimals)
{
	switch (eAnimals)
	{
	case ANIMALS::ANIMALS_BEAR:
		m_vecAnimCtrl.push_back(AnimCtrl(432, 465, 0, 2));
		m_vecAnimCtrl.push_back(AnimCtrl(398, 420, 0, 0));
		m_vecAnimCtrl.push_back(AnimCtrl(234, 390, 0, 0));
		m_vecAnimCtrl.push_back(AnimCtrl(111, 233, 0, 0));
		m_vecAnimCtrl.push_back(AnimCtrl(0, 50, 0, 0));
		break;
	case ANIMALS::ANIMALS_BOAR:
		m_vecAnimCtrl.push_back(AnimCtrl(0, 29, 0, 0.91f));
		m_vecAnimCtrl.push_back(AnimCtrl(30, 47, 0.92f, 1.52f));
		m_vecAnimCtrl.push_back(AnimCtrl(231, 400, 0, 0));
		m_vecAnimCtrl.push_back(AnimCtrl(48, 230, 0, 0));
		m_vecAnimCtrl.push_back(AnimCtrl(401, 423, 0, 0));
		break;
	case ANIMALS::ANIMALS_DEER:
		m_vecAnimCtrl.push_back(AnimCtrl(393, 423, 0, 0));
		m_vecAnimCtrl.push_back(AnimCtrl(423, 442, 0, 0));
		m_vecAnimCtrl.push_back(AnimCtrl(0, 230, 0, 0));
		m_vecAnimCtrl.push_back(AnimCtrl(231, 320, 0, 0));
		m_vecAnimCtrl.push_back(AnimCtrl(321, 392, 0, 0));
		break;
	case ANIMALS::ANIMALS_WOLF:
		m_vecAnimCtrl.push_back(AnimCtrl(0, 29, 0, 0));
		m_vecAnimCtrl.push_back(AnimCtrl(30, 46, 0, 0));
		m_vecAnimCtrl.push_back(AnimCtrl(47, 212, 0, 0));
		m_vecAnimCtrl.push_back(AnimCtrl(213, 400, 0, 0));
		m_vecAnimCtrl.push_back(AnimCtrl(480, 575, 0, 0));
		break;
	}
}


void CAnimals::AnimVectorClear()
{
	m_vecAnimCtrl.clear();
	m_vecAnimCtrl.shrink_to_fit();
}

void CAnimals::Obb_Collision()
{
}

void CAnimals::Hit_Object(_float& fCnt, _vec3 vStart, _vec3 vEnd, _vec3 vMid)
{
}

void CAnimals::Resurrection()
{
}
