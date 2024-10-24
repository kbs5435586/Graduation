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


	_vec3 vPos = _vec3(0.f,5.f,0.f);
	m_pTransformCom->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
	m_pTransformCom->SetUp_Speed(10.f, XMConvertToRadians(30.f));
	return S_OK;
}

_int CCube::Update_GameObject(const _float& fTimeDelta)
{
	if (GetAsyncKeyState('T') & 0x8000)
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
	if (GetAsyncKeyState('F') & 0x8000)
	{
		m_pTransformCom->Go_Left(fTimeDelta);
	}
	if (GetAsyncKeyState('G') & 0x8000)
	{
		m_pTransformCom->BackWard(fTimeDelta);
	}
	if (GetAsyncKeyState('H') & 0x8000)
	{
		m_pTransformCom->Go_Right(fTimeDelta);
	}

	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return -1;

	pManagement->AddRef();

	CBuffer_Terrain_Height* pTerrainBuffer = (CBuffer_Terrain_Height*)pManagement->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE, L"Layer_Terrain", L"Com_Buffer");
	if (nullptr == pTerrainBuffer)
		return -1;

	_float		fY = pTerrainBuffer->Compute_HeightOnTerrain(m_pTransformCom);

	m_pTransformCom->Set_PositionY(fY);

	Safe_Release(pManagement);


	return _int();
}

_int CCube::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONEALPHA, this)))
		return -1;


	return _int();
}

void CCube::Render_GameObject()
{
	MAINPASS tMainPass = {};
	_matrix matWorld = m_pTransformCom->Get_Matrix();
	_matrix matView = CCamera_Manager::GetInstance()->GetMatView();
	_matrix matProj = CCamera_Manager::GetInstance()->GetMatProj();


	m_pBufferCom->Render_VIBuffer();
}

HRESULT CCube::CreateInputLayout()
{
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

	if (FAILED(m_pShaderCom->Create_Shader(vecDesc, RS_TYPE::DEFAULT)))
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


	Safe_Release(pManagement);

	return S_OK;
}
