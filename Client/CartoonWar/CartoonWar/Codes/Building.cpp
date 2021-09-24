#include "framework.h"
#include "Management.h"
#include "Building.h"

CBuilding::CBuilding()
	: CGameObject()
{
}

CBuilding::CBuilding(const CBuilding& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBuilding::Ready_Prototype()
{
	return S_OK;
}

HRESULT CBuilding::Ready_GameObject(void* pArg)
{
	m_IsClone = true;
	if (nullptr == pArg)
	{
		return E_FAIL;
	}
	_tchar pTempStr[128] = {};
	lstrcpy(pTempStr, (const _tchar*)pArg);
	_uint iLen = lstrlen(pTempStr);
	m_pComponentTag = new _tchar[iLen + 1];
	lstrcpy(m_pComponentTag, pTempStr);
	if (FAILED(Ready_Component(m_pComponentTag)))
		return E_FAIL;
	if (FAILED(CreateInputLayout()))
		return E_FAIL;


	return S_OK;
}

_int CBuilding::Update_GameObject(const _float& fTimeDelta)
{
	m_pTransformCom->Scaling(0.03f, 0.03f, 0.03f);
	CBuffer_Terrain_Height* pTerrainBuffer = (CBuffer_Terrain_Height*)CManagement::GetInstance()->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE, L"Layer_Terrain", L"Com_Buffer");
	if (nullptr == pTerrainBuffer)
		return -1;
	pTerrainBuffer;
	_float		fY = pTerrainBuffer->Compute_HeightOnTerrain(m_pTransformCom);
	m_pTransformCom->Set_PositionY(fY);

	return _int();
}

_int CBuilding::LastUpdate_GameObject(const _float& fTimeDelta)
{
	CTransform* pTransform = (CTransform*)CManagement::GetInstance()->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
		L"Layer_Player", L"Com_Transform", 0);
	_vec3 vPlayerPos = *pTransform->Get_StateInfo(CTransform::STATE_POSITION);
	_vec3 vPos = *m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION);
	_vec3 vLen = vPlayerPos - vPos;
	_float fLen = vLen.Length();
	CGameObject* pPlayer = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_Player", g_iPlayerIdx);

	if (m_pFrustumCom->Culling_Frustum(m_pTransformCom))
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONEALPHA, this)))
			return -1; 
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_OUTLINE, this)))
			return -1;
		if (fLen <= 250.f)
		{
			if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this)))
				return -1;
		}
		if (fLen <= 150.f && pPlayer->GetIsRun())
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

	return _int();
}

void CBuilding::Render_GameObject()
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
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);

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


	if (m_iBlurCnt >= 0.2f)
	{
		m_matOldWorld = m_pTransformCom->Get_Matrix();
		m_matOldView = CCamera_Manager::GetInstance()->GetMatView();
		m_iBlurCnt = 0;
	}
	Safe_Release(pManagement);
}

void CBuilding::Render_GameObject_Shadow()
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
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);

		CDevice::GetInstance()->UpdateTable();
		m_pMeshCom->Render_Mesh(i);
	}

	Safe_Release(pManagement);
}

void CBuilding::Render_Blur()
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

void CBuilding::Render_OutLine()
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
		tRep.m_arrFloat[0] = 9.f;

		m_pShaderCom_OutLine->SetUp_OnShader(matWorld, matView, matProj, tMainPass);

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

HRESULT CBuilding::Ready_Component(const _tchar* pComTag)
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

	m_pMeshCom = (CMesh*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, pComTag);
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

	m_pFrustumCom = (CFrustum*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Frustum");
	NULL_CHECK_VAL(m_pFrustumCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Frustum", m_pFrustumCom)))
		return E_FAIL;

	m_pShaderCom_OutLine = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_OutLine");
	NULL_CHECK_VAL(m_pShaderCom_OutLine, E_FAIL);
	if (FAILED(Add_Component(L"Com_OutLineShader", m_pShaderCom_OutLine)))
		return E_FAIL;
	Safe_Release(pManagement);
	return S_OK;
}

HRESULT CBuilding::CreateInputLayout()
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
	if (FAILED(m_pShaderCom_OutLine->Create_Shader(vecDesc, RS_TYPE::COUNTERCLOCK, DEPTH_STENCIL_TYPE::LESS_NO_WRITE)))
		return E_FAIL;

	return S_OK;
}

CBuilding* CBuilding::Create()
{
	CBuilding* pInstance = new CBuilding();
	if (FAILED(pInstance->Ready_Prototype()))
	{
		MessageBox(0, L"CBuilding Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBuilding::Clone_GameObject(void* pArg , _uint iIdx)
{
	CBuilding* pInstance = new CBuilding(*this);
	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MessageBox(0, L"CBuilding Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	m_iLayerIdx = iIdx;
	return pInstance;
}

void CBuilding::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pShaderCom_Shadow);
	Safe_Release(m_pShaderCom_Blur);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pFrustumCom);
	Safe_Release(m_pShaderCom_OutLine);
	if (m_IsClone)
		Safe_Delete_Array(m_pComponentTag);
	CGameObject::Free();
}
