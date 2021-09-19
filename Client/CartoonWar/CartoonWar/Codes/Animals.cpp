#include "framework.h"
#include "Management.h"
#include "Animals.h"

CAnimals::CAnimals()
{
}

CAnimals::CAnimals(const CAnimals& rhs)
{
}

HRESULT CAnimals::Ready_Prototype()
{
	return S_OK;
}

HRESULT CAnimals::Ready_GameObject(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	m_eAnimals = *(ANIMALS*)pArg;
	m_IsClone = true;
	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(CreateInputLayout()))
		return E_FAIL;

	m_tInfo = INFO(1.f, 0.f, 0.f, 0.f);
	_vec3 vPos = { 120.f,0.f,120.f };
	//_vec3 vPos = { _float(rand() % 750) + 50.f,0.f,_float(rand() % 750) + 50.f };
	m_pTransformCom->Set_StateInfo(CTransform::STATE_POSITION, &vPos);

	m_pTransformCom->SetUp_Speed(10.f, XMConvertToRadians(90.f));
	if (m_eAnimals == ANIMALS::ANIMALS_DEER)
	{ 
		m_pTransformCom->Scaling(_vec3(0.1f, 0.1f, 0.1f));
		m_vOBB_Range = { 40.f ,80.f,40.f };
	}
	else if (m_eAnimals == ANIMALS::ANIMALS_WOLF)
	{
		m_pTransformCom->Scaling(_vec3(2.f, 2.f, 2.f));
		m_vOBB_Range = { 4.f ,8.f,4.f };
	}


	_vec3 vColliderSize = { 40.f ,160.f,40.f };
	m_pColliderCom_OBB->Clone_ColliderBox(m_pTransformCom, vColliderSize);
	m_pColliderCom_AABB->Clone_ColliderBox(m_pTransformCom, vColliderSize);

	m_pAnimCom->SetBones(m_pMeshCom->GetBones());
	m_pAnimCom->SetAnimClip(m_pMeshCom->GetAnimClip());
	SetUp_Anim();
	m_pAnimCom->LateInit();
	m_iCurAnimIdx = 0;

	m_eCurState = STATE::STATE_IDLE;

	return S_OK;
}

_int CAnimals::Update_GameObject(const _float& fTimeDelta)
{
	m_pColliderCom_OBB->Update_Collider(m_pTransformCom, m_vOBB_Range, m_eCurClass);
	m_pColliderCom_AABB->Update_Collider(m_pTransformCom, m_vOBB_Range, m_eCurClass);
	Obb_Collision();
	CBuffer_Terrain_Height* pTerrainBuffer = (CBuffer_Terrain_Height*)CManagement::GetInstance()->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE, L"Layer_Terrain", L"Com_Buffer");
	if (nullptr == pTerrainBuffer)
		return -1;
	_float		fY = pTerrainBuffer->Compute_HeightOnTerrain(m_pTransformCom);
	m_pTransformCom->Set_PositionY(fY);
	MeaningLess_Moving(fTimeDelta);
	Set_State();

	if (m_tInfo.fHP <= 0.f)
	{

		m_fDeathTime += fTimeDelta;
		m_eCurState = STATE::STATE_DEAD;
		if (m_fDeathTime > 2.f)
		{
			m_IsDead = true;
		}
	}

	if (m_IsDead)
	{
		if(g_iGold<9)
			g_iGold++;
		return DEAD_OBJ;
	}

	return NO_EVENT;
}

_int CAnimals::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	CTransform* pTransform = (CTransform*)CManagement::GetInstance()->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
		L"Layer_Player", L"Com_Transform", g_iPlayerIdx);
	_vec3 vPlayerPos = *pTransform->Get_StateInfo(CTransform::STATE_POSITION);
	_vec3 vPos = *m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION);
	_vec3 vLen = vPlayerPos - vPos;
	_float fLen = vLen.Length();
	CGameObject* pPlayer = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_Player", g_iPlayerIdx);

	if (m_pFrustumCom->Culling_Frustum(m_pTransformCom))
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONEALPHA, this)))
			return -1;
		if (fLen <= 250.f)
		{
			if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this)))
				return -1;
		}
		if (fLen <= 30.f && pPlayer->GetIsRun())
		{
			m_iBlurCnt += fTimeDelta;
			if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLUR, this)))
				return -1;
		}
		else
		{
			m_matOldWorld = m_pTransformCom->Get_Matrix();;
			m_matOldView = CCamera_Manager::GetInstance()->GetMatView();
		}
	}
	else
	{
		m_matOldWorld = m_pTransformCom->Get_Matrix();;
		m_matOldView = CCamera_Manager::GetInstance()->GetMatView();
	}

	if (m_pAnimCom->Update(m_vecAnimCtrl[m_iCurAnimIdx], fTimeDelta, true) && m_IsOnce)
	{
		m_IsOnce = false;
	}

	return _int();
}

void CAnimals::Render_GameObject()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();


	_uint iSubsetNum = m_pMeshCom->GetSubsetNum();
	for (_uint i = 0; i < iSubsetNum; ++i)
	{

		MAINPASS tMainPass = {};
		_matrix matWorld = m_pTransformCom->Get_Matrix();
		_matrix matView = CCamera_Manager::GetInstance()->GetMatView();
		_matrix matProj = CCamera_Manager::GetInstance()->GetMatProj();

		REP tRep = {};
		tRep.m_arrInt[0] = 1;
		tRep.m_arrInt[1] = m_pAnimCom->GetBones()->size();
		tRep.m_arrInt[2] = 2;

		m_pShaderCom->SetUp_OnShader(matWorld, matView, matProj, tMainPass);

		_uint iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
			(_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);

		iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->SetData((void*)&tRep);
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
			(_uint)CONST_REGISTER::b8)->GetCBV().Get(), iOffeset, CONST_REGISTER::b8);

		CDevice::GetInstance()->SetTextureToShader(m_pTextureCom->GetSRV(0), TEXTURE_REGISTER::t0);
		CDevice::GetInstance()->SetTextureToShader(m_pTextureCom->GetSRV(1), TEXTURE_REGISTER::t1);
		CDevice::GetInstance()->SetTextureToShader(m_pTextureCom->GetSRV(2), TEXTURE_REGISTER::t2);
		CDevice::GetInstance()->SetTextureToShader(m_pTextureCom->GetSRV(3), TEXTURE_REGISTER::t3);
		CDevice::GetInstance()->SetTextureToShader(m_pTextureCom->GetSRV(4), TEXTURE_REGISTER::t4);
		CDevice::GetInstance()->SetTextureToShader(m_pTextureCom->GetSRV(5), TEXTURE_REGISTER::t5);
		CDevice::GetInstance()->SetTextureToShader(m_pTextureCom_Normal->GetSRV(0), TEXTURE_REGISTER::t8);
		CDevice::GetInstance()->SetTextureToShader(m_pTextureCom_Normal->GetSRV(1), TEXTURE_REGISTER::t9);


		m_pAnimCom->UpdateData(m_pMeshCom, m_pComputeShaderCom);
		CDevice::GetInstance()->UpdateTable();
		m_pMeshCom->Render_Mesh(i);
	}

	if (g_IsCollisionBox)
	{
		m_pColliderCom_OBB->Render_Collider();
		m_pColliderCom_AABB->Render_Collider();
	}

	if (m_iBlurCnt >= 0.2f)
	{
		m_matOldWorld = m_pTransformCom->Get_Matrix();
		m_matOldView = CCamera_Manager::GetInstance()->GetMatView();
		m_iBlurCnt = 0;
	}
	Safe_Release(pManagement);
}

void CAnimals::Render_GameObject_Shadow()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();


	_uint iSubsetNum = m_pMeshCom->GetSubsetNum();
	for (_uint i = 0; i < iSubsetNum; ++i)
	{
		MAINPASS tMainPass = {};
		_matrix matWorld = m_pTransformCom->Get_Matrix();
		_matrix matView = CCamera_Manager::GetInstance()->GetShadowView();
		_matrix matProj = CCamera_Manager::GetInstance()->GetShadowMatProj();

		REP tRep = {};
		tRep.m_arrInt[0] = 1;
		tRep.m_arrInt[1] = m_pAnimCom->GetBones()->size();

		m_pShaderCom_Shadow->SetUp_OnShader(matWorld, matView, matProj, tMainPass);

		_uint iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
			(_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);

		iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->SetData((void*)&tRep);
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
			(_uint)CONST_REGISTER::b8)->GetCBV().Get(), iOffeset, CONST_REGISTER::b8);

		m_pAnimCom->UpdateData(m_pMeshCom, m_pComputeShaderCom);
		CDevice::GetInstance()->UpdateTable();
		m_pMeshCom->Render_Mesh(i);
	}
	Safe_Release(pManagement);
}

void CAnimals::Render_Blur()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();


	_uint iSubsetNum = m_pMeshCom->GetSubsetNum();
	for (_uint i = 0; i < iSubsetNum; ++i)
	{
		MAINPASS tMainPass = {};
		_matrix matWorld = m_pTransformCom->Get_Matrix();
		_matrix matView = CCamera_Manager::GetInstance()->GetMatView();
		_matrix matProj = CCamera_Manager::GetInstance()->GetMatProj();


		REP tRep = {};
		tRep.m_arrMat[0] = m_matOldWorld;//OldWorld
		tRep.m_arrMat[1] = m_matOldView;//OldView

		m_pShaderCom_Blur->SetUp_OnShader(matWorld, matView, matProj, tMainPass);

		_uint iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
			(_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);

		iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->SetData((void*)&tRep);
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
			(_uint)CONST_REGISTER::b8)->GetCBV().Get(), iOffeset, CONST_REGISTER::b8);



		CDevice::GetInstance()->UpdateTable();
		m_pMeshCom->Render_Mesh(i);
	}
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
	if (FAILED(m_pShaderCom_Blur->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::LESS_NO_WRITE, SHADER_TYPE::SHADER_BLUR)))
		return E_FAIL;
	return S_OK;
}

void CAnimals::SetUp_Anim()
{
	if(m_eAnimals == ANIMALS::ANIMALS_DEER)
	{
		m_vecAnimCtrl.push_back(AnimCtrl(393, 423, 0.f, 16.66f));	//Walk
		m_vecAnimCtrl.push_back(AnimCtrl(423, 442, 0.f, 16.66f));	//Run
		m_vecAnimCtrl.push_back(AnimCtrl(0, 230, 0.f, 16.66f));		//Idle
		m_vecAnimCtrl.push_back(AnimCtrl(231, 320, 0.f, 16.66f));	//Eat
		m_vecAnimCtrl.push_back(AnimCtrl(321, 392, 0.f, 16.66f));	//Die
	}
	else if (m_eAnimals == ANIMALS::ANIMALS_WOLF)
	{
		m_vecAnimCtrl.push_back(AnimCtrl(0, 29, 0.f, 16.66f));		//Walk
		m_vecAnimCtrl.push_back(AnimCtrl(30, 46, 0.f, 16.66f));		//Run
		m_vecAnimCtrl.push_back(AnimCtrl(47, 212, 0.f, 16.66f));	//Idle
		m_vecAnimCtrl.push_back(AnimCtrl(213, 400, 0.f, 16.66f));	//Eat
		m_vecAnimCtrl.push_back(AnimCtrl(480, 546, 0.f, 16.66f));	//Die
		m_vecAnimCtrl.push_back(AnimCtrl(547, 575, 0.f, 16.66f));	//Attack
	}
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
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTextureCom_Normal);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pComputeShaderCom);
	Safe_Release(m_pAnimCom);
	Safe_Release(m_pShaderCom_Shadow);
	Safe_Release(m_pColliderCom_AABB);
	Safe_Release(m_pColliderCom_OBB);
	Safe_Release(m_pNaviCom);
	Safe_Release(m_pFrustumCom);
	Safe_Release(m_pShaderCom_Blur);

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
	if (m_eAnimals == ANIMALS::ANIMALS_DEER)
	{
		m_pTextureCom_Normal = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Deer");
		NULL_CHECK_VAL(m_pTextureCom_Normal, E_FAIL);
		if (FAILED(Add_Component(L"Com_Texture_Normal", m_pTextureCom_Normal)))
			return E_FAIL;
		m_pMeshCom = (CMesh*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Animal_Deer");
		NULL_CHECK_VAL(m_pMeshCom, E_FAIL);
		if (FAILED(Add_Component(L"Com_Mesh", m_pMeshCom)))
			return E_FAIL;
	}
	else if (m_eAnimals == ANIMALS::ANIMALS_WOLF)
	{
		m_pTextureCom_Normal = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Wolf");
		NULL_CHECK_VAL(m_pTextureCom_Normal, E_FAIL);
		if (FAILED(Add_Component(L"Com_Texture_Normal", m_pTextureCom_Normal)))
			return E_FAIL;
		m_pMeshCom = (CMesh*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Animal_Wolf");
		NULL_CHECK_VAL(m_pMeshCom, E_FAIL);
		if (FAILED(Add_Component(L"Com_Mesh", m_pMeshCom)))
			return E_FAIL;
	}


	m_pTextureCom = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Hatch");
	NULL_CHECK_VAL(m_pTextureCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Texture", m_pTextureCom)))
		return E_FAIL;

	m_pRendererCom = (CRenderer*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Renderer");
	NULL_CHECK_VAL(m_pRendererCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Renderer", m_pRendererCom)))
		return E_FAIL;
	m_pShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Hatching");
	NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
		return E_FAIL;
	m_pShaderCom_Shadow = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Shadow");
	NULL_CHECK_VAL(m_pShaderCom_Shadow, E_FAIL);
	if (FAILED(Add_Component(L"Com_ShadowShader", m_pShaderCom_Shadow)))
		return E_FAIL;
	m_pComputeShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Compute_Animation");
	NULL_CHECK_VAL(m_pComputeShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_ComputeShader", m_pComputeShaderCom)))
		return E_FAIL;
	m_pShaderCom_Blur = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Blur");
	NULL_CHECK_VAL(m_pShaderCom_Blur, E_FAIL);
	if (FAILED(Add_Component(L"Com_BlurShader", m_pShaderCom_Blur)))
		return E_FAIL;

	m_pAnimCom = (CAnimator*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Animation");
	NULL_CHECK_VAL(m_pAnimCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Anim", m_pAnimCom)))
		return E_FAIL;

	m_pColliderCom_AABB = (CCollider*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Collider_AABB");
	NULL_CHECK_VAL(m_pColliderCom_AABB, E_FAIL);
	if (FAILED(Add_Component(L"Com_Collider_AABB", m_pColliderCom_AABB)))
		return E_FAIL;

	m_pColliderCom_OBB = (CCollider*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Collider_OBB");
	NULL_CHECK_VAL(m_pColliderCom_OBB, E_FAIL);
	if (FAILED(Add_Component(L"Com_Collider_OBB", m_pColliderCom_OBB)))
		return E_FAIL;


	m_pNaviCom = (CNavigation*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_NaviMesh");
	NULL_CHECK_VAL(m_pNaviCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Navi", m_pNaviCom)))
		return E_FAIL;
	m_pFrustumCom = (CFrustum*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Frustum");
	NULL_CHECK_VAL(m_pFrustumCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Frustum", m_pFrustumCom)))
		return E_FAIL;


	Safe_Release(pManagement);
	return S_OK;
}

void CAnimals::Set_Animation()
{
	if (m_iCurAnimIdx != m_iPreAnimIdx)
	{

		m_iCurAnimIdx = m_iPreAnimIdx;
	}

}

void CAnimals::MeaningLess_Moving(const _float& fTimeDelta)
{
	if(!m_IsDest)
	{
		_float	fMoveX = rand() % 1000 + 1;
		_float	fMoveZ = rand() % 1000 + 1;

		m_vDest = { fMoveX , 0.f, fMoveZ };
		m_IsDest = true;
	}


	_vec3 vPos = *m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION);
	vPos.y = 0.f;
	_vec3	vLen = vPos - m_vDest;
	_float	fLen = vLen.Length();

	_vec3	vLook = {};
	vLen.Normalize(vLook);
	if (m_eAnimals == ANIMALS::ANIMALS_WOLF)
	{
		vLook *= -1.f;
		m_pTransformCom->SetLook(vLook);
	}
	else if (m_eAnimals == ANIMALS::ANIMALS_DEER)
	{

		m_pTransformCom->SetLook(vLook);
	}
	


	if (fLen >= 50.f)
	{
		_vec3 vDirectionPerSec = (vLook * 5.f * fTimeDelta);
		_vec3 vSlide = {};
		if (!m_IsSlide)
		{
			if (m_pNaviCom->Move_OnNavigation(m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION), &vDirectionPerSec, &vSlide))
			{
				m_pTransformCom->Go_ToTarget(&m_vDest, fTimeDelta);
			}
			else
			{
				//m_pTransformCom->Go_There(vSlide);
				m_IsDest = false;
			}
		}
		else
		{
			m_pTransformCom->Go_ToTarget(&m_vDest, fTimeDelta);
			m_IsSlide = false;
		}
		m_eCurState = STATE::STATE_RUN;
	}
	else if (fLen > 5.f)
	{
		_vec3 vDirectionPerSec = (vLook * 5.f * fTimeDelta);
		_vec3 vSlide = {};
		if (!m_IsSlide)
		{
			if (m_pNaviCom->Move_OnNavigation(m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION), &vDirectionPerSec, &vSlide))
			{
				m_pTransformCom->Go_ToTarget(&m_vDest, fTimeDelta);
			}
			else
			{
				m_pTransformCom->Go_There(vSlide);
			}
		}
		else
		{
			m_pTransformCom->Go_ToTarget(&m_vDest, fTimeDelta);
			m_IsSlide = false;
		}
		m_eCurState = STATE::STATE_WALK;
	}
	else if (fLen <= 2.f)
	{
		m_IsDest = false;
	}
	else if (fLen <= 5.f)
	{
		_vec3 vDirectionPerSec = (vLook * 5.f * fTimeDelta);
		_vec3 vSlide = {};
		if (!m_IsSlide)
		{
			if (m_pNaviCom->Move_OnNavigation(m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION), &vDirectionPerSec, &vSlide))
			{
				m_pTransformCom->Go_ToTarget(&m_vDest, fTimeDelta);
			}
			else
			{
				m_pTransformCom->Go_There(vSlide);
			}
		}
		else
		{
			m_pTransformCom->Go_ToTarget(&m_vDest, fTimeDelta);
			m_IsSlide = false;
		}
		m_eCurState = STATE::STATE_IDLE;
	}
}

void CAnimals::Set_State()
{
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
			// 여기서 속도 정함
		case STATE::STATE_IDLE:
			m_iCurAnimIdx=2;	
			m_pTransformCom->SetSpeed(3.f);
			break;
		case STATE::STATE_ATTACK:	//Wolf Only
			m_iCurAnimIdx = 5;
			break;
		case STATE::STATE_RUN:
			m_iCurAnimIdx = 1;
			m_pTransformCom->SetSpeed(20.f);
			break;
		case STATE::STATE_WALK:
			m_iCurAnimIdx = 0;
			m_pTransformCom->SetSpeed(10.f);
			break;
		case STATE::STATE_HITTED://EAT
			m_iCurAnimIdx = 3;
			m_pTransformCom->SetSpeed(3.f);
			break;
		case STATE::STATE_DEAD:
			m_iCurAnimIdx = 4;
			break;
		}
		m_vecAnimCtrl[m_iCurAnimIdx].iCurFrm = m_vecAnimCtrl[m_iCurAnimIdx].iStartFrm;

		m_ePreState = m_eCurState;
	}
	

}

void CAnimals::Obb_Collision()
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

void CAnimals::Hit_Object(_float& fCnt, _vec3 vStart, _vec3 vEnd, _vec3 vMid)
{
	_float fX = (pow((1.f - fCnt), 2) * vStart.x) + (2 * fCnt * (1.f - fCnt) * vMid.x) + (pow(fCnt, 2) * vEnd.x);
	_float fY = (pow((1.f - fCnt), 2) * vStart.y) + (2 * fCnt * (1.f - fCnt) * vMid.y) + (pow(fCnt, 2) * vEnd.y);
	_float fZ = (pow((1.f - fCnt), 2) * vStart.z) + (2 * fCnt * (1.f - fCnt) * vMid.z) + (pow(fCnt, 2) * vEnd.z);

	_vec3 vPos = { fX, fY, fZ };
	m_pTransformCom->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
	fCnt += 0.01f;
}

void CAnimals::Create_Particle(const _vec3& vPoistion)
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
