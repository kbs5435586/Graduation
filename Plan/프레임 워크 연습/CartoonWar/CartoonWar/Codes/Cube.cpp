#include "framework.h"
#include "Cube.h"
#include "Management.h"


CCube::CCube()
	: CGameObject()
{
}

CCube::CCube(const CCube& rhs)
	: CGameObject(rhs)
{
}

HRESULT CCube::Ready_Prototype()
{

	return S_OK;
}

HRESULT CCube::Ready_GameObject(void* pArg)
{

	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(CreateInputLayout()))
		return E_FAIL;



	_vec3 vPos = _vec3(5.f,5.f, 5.f);
	m_pTransformCom->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
	m_pTransformCom->SetUp_Speed(10.f, XMConvertToRadians(30.f));

	_vec3 vColliderSize = _vec3(2.f, 2.f, 2.f);
	m_pColliderCom[0]->Clone_ColliderBox(m_pTransformCom, vColliderSize);
	m_pColliderCom[1]->Clone_ColliderBox(m_pTransformCom, vColliderSize);
	m_pColliderCom[2]->Clone_ColliderBox(m_pTransformCom, vColliderSize);


	m_tInfo = {10.f,10.f,10.f,10.f};
	
	m = *m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION);
	CManagement::GetInstance()->Add_Data(DATA_TYPE::DATA_INFO, &m_tInfo);
	CManagement::GetInstance()->Add_Data(DATA_TYPE::DATA_VECTOR, &m);
	return S_OK;
}

_int CCube::Update_GameObject(const _float& fTimeDelta)
{
 	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return -1;

	pManagement->AddRef();

	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		m_pTransformCom->BackWard(fTimeDelta);
	}
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		m_pTransformCom->Rotation_Y(-fTimeDelta);
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		m_tInfo.fHP -= 1.f;
		m_pTransformCom->Rotation_Y(fTimeDelta);
	}

	CBuffer_Terrain_Height* pTerrainBuffer = (CBuffer_Terrain_Height*)pManagement->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE, L"Layer_Terrain", L"Com_Buffer");
	if (nullptr == pTerrainBuffer)
		return -1;

	_float		fY = pTerrainBuffer->Compute_HeightOnTerrain(m_pTransformCom);

	m_pTransformCom->Set_PositionY(fY+0.5f);
	m_pColliderCom[0]->Update_Collider(m_pTransformCom);
	m_pColliderCom[1]->Update_Collider(m_pTransformCom);
	m_pColliderCom[2]->Update_Collider(m_pTransformCom);

	m = *m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION);
	pManagement->Notify(DATA_TYPE::DATA_INFO, &m_tInfo);
	pManagement->Notify(DATA_TYPE::DATA_VECTOR, &m);

	Safe_Release(pManagement);

	return _int();
}

_int CCube::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONEALPHA, this)))
		return -1;

	m = *m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION);
	CManagement::GetInstance()->Notify(DATA_TYPE::DATA_INFO, &m_tInfo);
	CManagement::GetInstance()->Notify(DATA_TYPE::DATA_VECTOR, &m);
	
	return _int();
}

void CCube::Render_GameObject()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();

	// 
	MAINPASS tMainPass = {};
	_matrix matWorld = m_pTransformCom->Get_Matrix();
	_matrix matView = CCamera_Manager::GetInstance()->GetMatView();
	_matrix matProj = CCamera_Manager::GetInstance()->GetMatProj();

	_matrix I_matView = CCamera_Manager::GetInstance()->GetIMatView();
	_matrix I_matProj = CCamera_Manager::GetInstance()->GetIMatProj();


	m_pShaderCom->SetUp_OnShader(matWorld, matView, matProj, I_matView, I_matProj, tMainPass);

	_uint iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);
	CDevice::GetInstance()->UpdateTable();


	m_pBufferCom->Render_VIBuffer();


	//MAINPASS tIMainPass = {};
	//matWorld = m_pTransformCom->Get_Matrix();
	//matView = CCamera_Manager::GetInstance()->GetMatView();
	//matProj = CCamera_Manager::GetInstance()->GetMatProj();
	//
	//m_pShaderCom->SetUp_OnShader(matWorld, matView, matProj, tIMainPass);
	//
	//iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tIMainPass);
	//CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);
	//CDevice::GetInstance()->UpdateTable();


	//m_pBufferCom->Render_VIBuffer();
	//m_pColliderCom[0]->Render_Collider();
	//m_pColliderCom[1]->Render_Collider();
	//m_pColliderCom[2]->Render_Collider();
	Safe_Release(pManagement);
}

HRESULT CCube::CreateInputLayout()
{
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

	if (FAILED(m_pShaderCom->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::LESS, SHADER_TYPE::SHADER_DEFFERED)))
		return E_FAIL;
	return S_OK;
}


CCube* CCube::Create()
{
	CCube* pInstance = new CCube();

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MessageBox(0, L"CCube Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCube::Clone_GameObject(void* pArg)
{
	CCube* pInstance = new CCube(*this);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CCube Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCube::Free()
{
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);

	Safe_Release(m_pColliderCom[0]);
	Safe_Release(m_pColliderCom[1]);
	Safe_Release(m_pColliderCom[2]);

	CGameObject::Free();
}

HRESULT CCube::Ready_Component()
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

	m_pBufferCom = (CBuffer_CubeCol*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Buffer_CubeCol");
	NULL_CHECK_VAL(m_pBufferCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Buffer", m_pBufferCom)))
		return E_FAIL;

	m_pShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Default");
	NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
		return E_FAIL;

	m_pColliderCom[0] = (CCollider*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Collider_AABB");
	NULL_CHECK_VAL(m_pColliderCom[0], E_FAIL);
	if (FAILED(Add_Component(L"Com_Collider_0", m_pColliderCom[0])))
		return E_FAIL;

	m_pColliderCom[1] = (CCollider*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Collider_OBB");
	NULL_CHECK_VAL(m_pColliderCom[1], E_FAIL);
	if (FAILED(Add_Component(L"Com_Collider_1", m_pColliderCom[1])))
		return E_FAIL;

	m_pColliderCom[2] = (CCollider*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Collider_SPHERE");
	NULL_CHECK_VAL(m_pColliderCom[2], E_FAIL);
	if (FAILED(Add_Component(L"Com_Collider_2", m_pColliderCom[2])))
		return E_FAIL;



	Safe_Release(pManagement);

	return S_OK;
}
