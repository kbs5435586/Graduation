#include "framework.h"
#include "Particle_Default.h"
#include "Management.h"
#include "UAV.h"

CParticle_Default::CParticle_Default()
	: CGameObject()
{
}

CParticle_Default::CParticle_Default(const CParticle_Default& rhs)
	: CGameObject(rhs)
{
}

HRESULT CParticle_Default::Ready_Prototype()
{
	return S_OK;
}

HRESULT CParticle_Default::Ready_GameObject(void* pArg)
{
	PARTICLESET tParticleSet = {};
	if (nullptr != pArg)
	{
		tParticleSet = *(PARTICLESET*)pArg;
	}
	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(CreateInputLayout()))
		return E_FAIL;
	m_pTransformCom->Set_StateInfo(CTransform::STATE_POSITION, &tParticleSet.vPos);
	m_pTransformCom->SetUp_Speed(10.f, XMConvertToRadians(30.f));

	Set_Particle(tParticleSet);
	m_fMaxLifeTime = tParticleSet.fMaxLifeTime;
	
	return S_OK;
}

_int CParticle_Default::Update_GameObject(const _float& fTimeDelta)
{
	m_fLifeTime += fTimeDelta;

	if (m_fLifeTime >= m_fMaxLifeTime)
	{
		m_fLifeTime = 0.f;
		m_IsDead = true;
	}

	CBuffer_Terrain_Height* pTerrainBuffer = (CBuffer_Terrain_Height*)CManagement::GetInstance()->
		Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE, L"Layer_Terrain", L"Com_Buffer");

	if (nullptr == pTerrainBuffer)
		return -1;
	_float		fY = pTerrainBuffer->Compute_HeightOnTerrain(m_pTransformCom);
	fY += 10.f;
	//m_pTransformCom->Set_PositionY(fY);

	if (m_IsDead)
		return DEAD_OBJ;
	return NO_EVENT;
}

_int CParticle_Default::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHA, this)))
		return -1;

	m_pParticleCom->Update_Particle(fTimeDelta);

	return _int();
}

void CParticle_Default::Render_GameObject()
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

	_uint iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(),
		iOffeset, CONST_REGISTER::b0);
	CDevice::GetInstance()->SetTextureToShader(m_pTextureCom, TEXTURE_REGISTER::t0);


	REP		tRep_Basic;
	m_pParticleCom->SetUp_OnShader(tRep_Basic);
	iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->SetData((void*)&tRep_Basic);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->GetCBV().Get(),
		iOffeset, CONST_REGISTER::b8);




	CDevice::GetInstance()->ClearDummyDesc_CS();
	REP		tRep_Update;
	m_pParticleCom->Update_Particle_Shader();
	if (FAILED(m_pParticleCom->SetUp_OnUpdateShader(tRep_Update)))
		return;

	tRep_Update.m_arrVec2[0]= m_pTextureCom_Noise->GetTextureResolution();
	iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->SetData((void*)&tRep_Update);
	CDevice::GetInstance()->SetUpContantBufferToShader_CS(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->GetCBV().Get(),
		iOffeset, CONST_REGISTER::b8);
	CDevice::GetInstance()->SetTextureToShader_CS(m_pTextureCom_Noise->GetSRV(), TEXTURE_REGISTER::t0);

	CDevice::GetInstance()->GetCsCmdLst()->SetPipelineState(m_pShaderCom[1]->GetCSPipeLine().Get());
	m_pParticleCom->DisPatch(1, 1, 1);


	CDevice::GetInstance()->UpdateTable();
	m_pBufferCom->Render_VIBuffer(m_pParticleCom->GetMaxParticle());
	Safe_Release(pManagement);
}

HRESULT CParticle_Default::CreateInputLayout()
{
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

	if (FAILED(m_pShaderCom[0]->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::LESS_NO_WRITE, SHADER_TYPE::SHADER_FORWARD, BLEND_TYPE::ALPHABLEND, D3D_PRIMITIVE_TOPOLOGY_POINTLIST)))
		return E_FAIL;


	return S_OK;
}

CParticle_Default* CParticle_Default::Create()
{
	CParticle_Default* pInstance = new CParticle_Default();

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MessageBox(0, L"CParticle_Default Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CParticle_Default::Clone_GameObject(void* pArg , _uint iIdx)
{
	CParticle_Default* pInstance = new CParticle_Default(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MessageBox(0, L"CParticle_Default Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	m_iLayerIdx = iIdx;
	return pInstance;
}

void CParticle_Default::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom[0]);
	Safe_Release(m_pShaderCom[1]);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTextureCom_Noise);
	Safe_Release(m_pParticleCom);
	CGameObject::Free();
}

HRESULT CParticle_Default::Ready_Component()
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

	m_pBufferCom = (CBuffer_Point*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Buffer_Point");
	NULL_CHECK_VAL(m_pBufferCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Buffer", m_pBufferCom)))
		return E_FAIL;

	m_pShaderCom[0] = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Particle");
	NULL_CHECK_VAL(m_pShaderCom[0], E_FAIL);
	if (FAILED(Add_Component(L"Com_Shader0", m_pShaderCom[0])))
		return E_FAIL;

	m_pShaderCom[1] = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Compute_Particle");
	NULL_CHECK_VAL(m_pShaderCom[1], E_FAIL);
	if (FAILED(Add_Component(L"Com_Shader1", m_pShaderCom[1])))
		return E_FAIL;

	m_pTextureCom = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Particle_Hit");
	NULL_CHECK_VAL(m_pTextureCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_TextureCom", m_pTextureCom)))
		return E_FAIL;

	m_pTextureCom_Noise = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Noise");
	NULL_CHECK_VAL(m_pTextureCom_Noise, E_FAIL);
	if (FAILED(Add_Component(L"Com_TextureCom_Noise", m_pTextureCom_Noise)))
		return E_FAIL;

	m_pParticleCom = (CParticle*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Particle");
	NULL_CHECK_VAL(m_pParticleCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Particle", m_pParticleCom)))
		return E_FAIL;

	Safe_Release(pManagement);

	return S_OK;
}

void CParticle_Default::Set_Particle(PARTICLESET tParticleSet)
{
	if (m_pParticleCom)
	{
		m_pParticleCom->Set_Particle(tParticleSet);
	}
}
