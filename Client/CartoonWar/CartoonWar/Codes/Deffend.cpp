#include "framework.h"
#include "Deffend.h"
#include "Management.h"

CDeffend::CDeffend()
	: CGameObject()
{
}

CDeffend::CDeffend(const CDeffend& rhs)
	: CGameObject(rhs)
{
}

HRESULT CDeffend::Ready_Prototype()
{

	return S_OK;
}

HRESULT CDeffend::Ready_GameObject(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;
	if (FAILED(CreateInputLayout()))
		return E_FAIL;

	if (nullptr == pArg)
		return E_FAIL;

	m_pTransformCom->SetUp_RotationY(XMConvertToRadians(-180.f));
	m_pTransformCom->SetUp_Speed(100.f, XMConvertToRadians(90.f));

	m_tInfo = INFO(20.f,0.f,0.f,0.f);
	_matrix pTemp = *(_matrix*)pArg;
	_matrix matWorld = m_pTransformCom->Get_Matrix();
	m_pTransformCom->Set_Matrix(pTemp);
	m_pTransformCom->Scaling(0.06f, 0.06f, 0.06f);

	_vec3 vColliderSize = { 50.f,50.f,50.f };
	m_pColliderCom_OBB->Clone_ColliderBox(m_pTransformCom, vColliderSize);
	m_pColliderCom_AABB->Clone_ColliderBox(m_pTransformCom, vColliderSize);

	return S_OK;
}

_int CDeffend::Update_GameObject(const _float& fTimeDelta)
{
	m_pColliderCom_OBB->Update_Collider(m_pTransformCom, _vec3(250.f, 250.f, 250.f));
	m_pColliderCom_AABB->Update_Collider(m_pTransformCom, _vec3(250.f, 250.f, 250.f));


	Obb_Collision();
	CBuffer_Terrain_Height* pTerrainBuffer = (CBuffer_Terrain_Height*)CManagement::GetInstance()->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE, L"Layer_Terrain", L"Com_Buffer");
	if (nullptr == pTerrainBuffer)
		return NO_EVENT;

	_float		fY = pTerrainBuffer->Compute_HeightOnTerrain(m_pTransformCom);
	m_pTransformCom->Set_PositionY(fY+0.5f);


	if (m_IsDead)
		return DEAD_OBJ;
	return NO_EVENT;
}

_int CDeffend::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;
	if (m_pFrustumCom->Culling_Frustum(m_pTransformCom))
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONEALPHA, this)))
			return -1;
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this)))
			return -1;
	}
	else
	{
		m_matOldWorld = m_pTransformCom->Get_Matrix();;
		m_matOldView = CCamera_Manager::GetInstance()->GetMatView();
	}

	return _int();
}

void CDeffend::Render_GameObject()
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
		tRep.m_arrInt[2] = g_DefferedRender;

		m_pShaderCom->SetUp_OnShader(matWorld, matView, matProj, tMainPass);

		_uint iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
			(_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);
		iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->SetData((void*)&tRep);
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
			(_uint)CONST_REGISTER::b8)->GetCBV().Get(), iOffeset, CONST_REGISTER::b8);


		CTexture* pTexture = m_pMeshCom->m_pTexture[i];
		if (pTexture)
		{
			CDevice::GetInstance()->SetTextureToShader(pTexture->GetSRV_().Get(), TEXTURE_REGISTER::t0);
		}

		CDevice::GetInstance()->UpdateTable();
		m_pMeshCom->Render_Mesh(i);
	}


	//m_pColliderCom_OBB->Render_Collider();
	//m_pColliderCom_AABB->Render_Collider();
	Safe_Release(pManagement);
}

void CDeffend::Render_GameObject_Shadow()
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

		m_pShaderCom_Shadow->SetUp_OnShader(matWorld, matView, matProj, tMainPass);

		_uint iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
			(_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);

		CDevice::GetInstance()->UpdateTable();
		m_pMeshCom->Render_Mesh(i);
	}
	Safe_Release(pManagement);
}

void CDeffend::Render_Blur()
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

	m_matOldWorld = m_pTransformCom->Get_Matrix();
	m_matOldView = CCamera_Manager::GetInstance()->GetMatView();

	Safe_Release(pManagement);
}

void CDeffend::Create_Particle(const _vec3& vPoistion)
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

HRESULT CDeffend::CreateInputLayout()
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

CDeffend* CDeffend::Create()
{
	CDeffend* pInstnace = new CDeffend;
	if (FAILED(pInstnace->Ready_Prototype()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CGameObject* CDeffend::Clone_GameObject(void* pArg, _uint iIdx)
{
	CDeffend* pInstnace = new CDeffend;
	if (FAILED(pInstnace->Ready_GameObject(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

void CDeffend::Free()
{
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pShaderCom_Shadow);
	Safe_Release(m_pShaderCom_Blur);
	Safe_Release(m_pColliderCom_OBB);
	Safe_Release(m_pColliderCom_AABB);
	Safe_Release(m_pFrustumCom);

	CGameObject::Free();
}

HRESULT CDeffend::Ready_Component()
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

	m_pMeshCom = (CMesh*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_StaticMesh_Deffend");
	NULL_CHECK_VAL(m_pMeshCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Mesh", m_pMeshCom)))
		return E_FAIL;

	m_pShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Toon");
	NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
		return E_FAIL;

	m_pShaderCom_Shadow = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Shadow");
	NULL_CHECK_VAL(m_pShaderCom_Shadow, E_FAIL);
	if (FAILED(Add_Component(L"Com_ShadowShader", m_pShaderCom_Shadow)))
		return E_FAIL;

	m_pShaderCom_Blur = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Blur");
	NULL_CHECK_VAL(m_pShaderCom_Blur, E_FAIL);
	if (FAILED(Add_Component(L"Com_Shader_Blur", m_pShaderCom_Blur)))
		return E_FAIL;

	m_pColliderCom_OBB = (CCollider*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Collider_OBB");
	NULL_CHECK_VAL(m_pColliderCom_OBB, E_FAIL);
	if (FAILED(Add_Component(L"Com_Collider_OBB", m_pColliderCom_OBB)))
		return E_FAIL;
	//m_pColliderCom_AABB
	m_pColliderCom_AABB = (CCollider*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Collider_AABB");
	NULL_CHECK_VAL(m_pColliderCom_AABB, E_FAIL);
	if (FAILED(Add_Component(L"Com_Collider_AABB", m_pColliderCom_AABB)))
		return E_FAIL;
	m_pFrustumCom = (CFrustum*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Frustum");
	NULL_CHECK_VAL(m_pFrustumCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Frustum", m_pFrustumCom)))
		return E_FAIL;
	Safe_Release(pManagement);
	return S_OK;
}

void CDeffend::Obb_Collision()
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

void CDeffend::Hit_Object(_float& fCnt, _vec3 vStart, _vec3 vEnd, _vec3 vMid)
{
	_float fX = (pow((1.f - fCnt), 2) * vStart.x) + (2 * fCnt * (1.f - fCnt) * vMid.x) + (pow(fCnt, 2) * vEnd.x);
	_float fY = (pow((1.f - fCnt), 2) * vStart.y) + (2 * fCnt * (1.f - fCnt) * vMid.y) + (pow(fCnt, 2) * vEnd.y);
	_float fZ = (pow((1.f - fCnt), 2) * vStart.z) + (2 * fCnt * (1.f - fCnt) * vMid.z) + (pow(fCnt, 2) * vEnd.z);

	_vec3 vPos = { fX, fY, fZ };
	m_pTransformCom->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
	fCnt += 0.01f;
}
