#include"framework.h"
#include "FireWall.h"
#include "Management.h"

_bool CFireWall::First = false;

CFireWall::CFireWall()
	: CGameObject()
{
}

CFireWall::CFireWall(const CFireWall& rhs)
	: CGameObject(rhs)
{
}

HRESULT CFireWall::Ready_Prototype()
{
	return S_OK;
}

HRESULT CFireWall::Ready_GameObject(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(CreateInputLayout()))
		return E_FAIL;


	_vec3 vPos = { (*(XMFLOAT2*)pArg).x,  0, (*(XMFLOAT2*)pArg).y };
	m_pTransformCom->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
	if (!First)
	{
		m_pTransformCom->Scaling(0.f, 0.f, 0.f);
		First = true;
	}
	else
		m_pTransformCom->Scaling(25.f, 25.f, 25.f);
	m_range = 50.f;

	//_vec3 vPos = { 50.f, 0.f, 50.f };
	//m_pTransformCom->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
	//m_pTransformCom->Scaling(10.f, 10.f, 10.f);
	return S_OK;

}

_int CFireWall::Update_GameObject(const _float& fTimeDelta)
{

	CBuffer_Terrain_Height* pTerrainBuffer = (CBuffer_Terrain_Height*)CManagement::GetInstance()->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE, L"Layer_Terrain", L"Com_Buffer");
	if (nullptr == pTerrainBuffer)
		return NO_EVENT;

	_float		fY = pTerrainBuffer->Compute_HeightOnTerrain(m_pTransformCom);
	m_pTransformCom->Set_PositionY(fY);

	if (startCheck)
	{
		endTime += fTimeDelta;
		if (endTime > 10.f)
			m_IsDead = true;

		if (damageCheck)
		{
			damageTime += fTimeDelta;

			if (damageTime > 0.1f)
				IsGetDamage = true;
		}
	}


	if (m_IsDead)
		return DEAD_OBJ;

	return _int();
}

_int CFireWall::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;



	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONEALPHA, this)))
		return -1;


	return _int();
}

void CFireWall::Render_GameObject()
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
	m_tTexInfo.vScale = _vec3(1.f, 1.f, 1.f);

	DISTORTION	tDistortion = {};
	tDistortion.fDistortion1 = _vec2(0.1f, 0.2f);
	tDistortion.fDistortion2 = _vec2(0.1f, 0.3f);
	tDistortion.fDistortion3 = _vec2(0.1f, 0.1f);
	tDistortion.fDistortionScale = 0;
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


HRESULT CFireWall::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc = {};
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });


	if (FAILED(m_pShaderCom->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::LESS_NO_WRITE, SHADER_TYPE::SHADER_FORWARD, BLEND_TYPE::ALPHABLEND)))
		return E_FAIL;

	return S_OK;
}

CFireWall* CFireWall::Create()
{
	CFireWall* pInstance = new CFireWall();
	if (FAILED(pInstance->Ready_Prototype()))
		Safe_Release(pInstance);
	return pInstance;
}

CGameObject* CFireWall::Clone_GameObject(void* pArg, _uint iIdx)
{
	CFireWall* pInstance = new CFireWall();
	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);
	
	return pInstance;
}

void CFireWall::Free()
{
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom[0]);
	Safe_Release(m_pTextureCom[1]);
	Safe_Release(m_pTextureCom[2]);
	CGameObject::Free();
}

HRESULT CFireWall::Ready_Component()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (pManagement == nullptr)
		return E_FAIL;
	pManagement->AddRef();

	m_pTransformCom = (CTransform*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Transform");
	NULL_CHECK_VAL(m_pTransformCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Transform", m_pTransformCom)))
		return E_FAIL;

	m_pRendererCom = (CRenderer*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Renderer");
	NULL_CHECK_VAL(m_pRendererCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Renderer", m_pRendererCom)))
		return E_FAIL;

	m_pBufferCom = (CBuffer_RectTex*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Buffer_Cylinder");
	NULL_CHECK_VAL(m_pBufferCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Buffer", m_pBufferCom)))
		return E_FAIL;

	m_pShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_FireWall");
	NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
		return E_FAIL;


	m_pTextureCom[0] = (CTexture*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Texture_FireWallAlpha");
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


	Safe_Release(pManagement);
	return S_OK;
}
