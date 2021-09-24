#include "framework.h"
#include "EffectBox.h"
#include "Management.h"
#include "Particle_FireBall.h"

CEffectBox::CEffectBox()
{
}

CEffectBox::CEffectBox(const CEffectBox& rhs)
{
}

HRESULT CEffectBox::Ready_Prototype()
{
	return S_OK;
}

HRESULT CEffectBox::Ready_GameObject(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;
	if (FAILED(Ready_Component()))
		return E_FAIL;
	if (FAILED(CreateInputLayout()))
		return E_FAIL;
	_matrix pTemp = *(_matrix*)pArg;
	m_pTransformCom->Set_Matrix(pTemp);

	m_pTransformCom->Scaling(5.f, 5.f, 5.f);
	m_pTransformCom->SetUp_Speed(60.f, XMConvertToRadians(90.f));
	
	//m_pTransformCom->BackWard(1.5f);
	Create_Particle(*m_pTransformCom->Get_StateInfo(CTransform::STATE::STATE_POSITION));
	return S_OK;
}

_int CEffectBox::Update_GameObject(const _float& fTimeDelta)
{
	m_fLifeTime += fTimeDelta;
	m_fAccTime += fTimeDelta;

	if (m_fLifeTime >= 10.f)
	{
		m_fLifeTime = 0.f;
		return DEAD_OBJ;
	}

	m_pTransformCom->BackWard(fTimeDelta);

	CGameObject* pGameObject = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_Particle_FireBall", m_iLayerIdx);
	if (nullptr == pGameObject)
		return _int();
	dynamic_cast<CParticle_FireBall*>
		(pGameObject)->GetPos() = *m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION);
	return _int();
}

_int CEffectBox::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this)))
		return -1;
	return _int();
}

void CEffectBox::Render_GameObject()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();


	MAINPASS tMainPass = {};
	_matrix matWorld = m_pTransformCom->Get_Matrix();
	_matrix matView = CCamera_Manager::GetInstance()->GetMatView();
	_matrix matProj = CCamera_Manager::GetInstance()->GetMatProj();

	m_pShaderCom->SetUp_OnShader(matWorld, matView, matProj, tMainPass);


	m_tTexInfo.fFrameTime += 0.01f;

	if (m_tTexInfo.fFrameTime > 1000.0f)
	{
		m_tTexInfo.fFrameTime = 0.0f;
	}

	m_tTexInfo.vScrollSpeed = _vec3(1.3f, 2.1f, 2.3f);
	m_tTexInfo.vScale = _vec3(1.f, 2.f, 3.f);

	DISTORTION	tDistortion = {};
	tDistortion.fDistortion1 = _vec2(0.1f, 0.2f);
	tDistortion.fDistortion2 = _vec2(0.1f, 0.3f);
	tDistortion.fDistortion3 = _vec2(0.1f, 0.1f);
	tDistortion.fDistortionScale = 0.8f;
	tDistortion.fDistortionBias = 0.5f;


	_uint iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);

	iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b4)->SetData((void*)&m_tTexInfo);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b4)->GetCBV().Get(), iOffeset, CONST_REGISTER::b4);

	iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b5)->SetData((void*)&tDistortion);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b5)->GetCBV().Get(), iOffeset, CONST_REGISTER::b5);

	CDevice::GetInstance()->SetTextureToShader(m_pTextureCom[0], TEXTURE_REGISTER::t0);
	CDevice::GetInstance()->SetTextureToShader(m_pTextureCom[1], TEXTURE_REGISTER::t1);
	CDevice::GetInstance()->SetTextureToShader(m_pTextureCom[2], TEXTURE_REGISTER::t2);
	CDevice::GetInstance()->UpdateTable();


	m_pBufferCom->Render_VIBuffer();

	Safe_Release(pManagement);
}

void CEffectBox::Render_PostEffect()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();


	MAINPASS tMainPass = {};
	_matrix matWorld = m_pTransformCom->Get_Matrix();
	_matrix matView = CCamera_Manager::GetInstance()->GetMatView();
	_matrix matProj = CCamera_Manager::GetInstance()->GetMatProj();


	m_pShaderCom_PostEffect->SetUp_OnShader(matWorld, matView, matProj, tMainPass);



	_uint iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);

	ComPtr<ID3D12DescriptorHeap>	pPostEffectTex = CManagement::GetInstance()->GetPostEffectTex()->GetSRV().Get();
	CDevice::GetInstance()->SetTextureToShader(pPostEffectTex.Get(), TEXTURE_REGISTER::t0);
	CDevice::GetInstance()->UpdateTable();
	m_pBufferCom->Render_VIBuffer();

	Safe_Release(pManagement);
}


HRESULT CEffectBox::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc = {};
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

	if (FAILED(m_pShaderCom->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::LESS_NO_WRITE, SHADER_TYPE::SHADER_FORWARD, BLEND_TYPE::ALPHABLEND)))
		return E_FAIL;

	return S_OK;
}

CEffectBox* CEffectBox::Create()
{
	CEffectBox* pInstance = new CEffectBox();
	if (FAILED(pInstance->Ready_Prototype()))
		Safe_Release(pInstance);
	return pInstance;
}

CGameObject* CEffectBox::Clone_GameObject(void* pArg, _uint iIdx)
{
	CEffectBox* pInstance = new CEffectBox();
	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);
	m_iLayerIdx = iIdx;

	return pInstance;
}

void CEffectBox::Create_Particle(const _vec3& vPoistion)
{
	if (!m_IsParticle)
	{
		_vec3 vTemp = vPoistion;
		vTemp.y += 5.f;
		PARTICLESET tParticleSet;
		tParticleSet.vPos = vTemp;
		tParticleSet.iMaxParticle = 30;
		tParticleSet.fMaxLifeTime = 10.f;
		tParticleSet.iMinLifeTime = 0.5f;

		tParticleSet.fStartScale = 0.2f;
		tParticleSet.fEndScale = 0.1f;

		tParticleSet.fMaxSpeed = 30.f;
		tParticleSet.fMinSpeed = 50.f;
		if (FAILED(CManagement::GetInstance()->Add_GameObjectToLayer(L"GameObject_Particle_FireBall", 
			(_uint)SCENEID::SCENE_STAGE, L"Layer_Particle_FireBall", nullptr, (void*)&tParticleSet)))
			return;

		int i = 0;
	}
}

void CEffectBox::Free()
{
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom[0]);
	Safe_Release(m_pTextureCom[1]);
	Safe_Release(m_pTextureCom[2]);
	Safe_Release(m_pFrustumCom);
	CGameObject::Free();
}

HRESULT CEffectBox::Ready_Component()
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

	m_pShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Fire");
	NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
		return E_FAIL;



	m_pTextureCom[0] = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_FireAlpha");
	NULL_CHECK_VAL(m_pTextureCom[0], E_FAIL);
	if (FAILED(Add_Component(L"Com_Texture_0", m_pTextureCom[0])))
		return E_FAIL;

	m_pTextureCom[1] = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Firefire");
	NULL_CHECK_VAL(m_pTextureCom[1], E_FAIL);
	if (FAILED(Add_Component(L"Com_Texture_1", m_pTextureCom[1])))
		return E_FAIL;

	m_pTextureCom[2] = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_Firenoise");
	NULL_CHECK_VAL(m_pTextureCom[2], E_FAIL);
	if (FAILED(Add_Component(L"Com_Texture_2", m_pTextureCom[2])))
		return E_FAIL;

	m_pFrustumCom = (CFrustum*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Frustum");
	NULL_CHECK_VAL(m_pFrustumCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Frustum", m_pFrustumCom)))
		return E_FAIL;
	Safe_Release(pManagement);
	return S_OK;
}
