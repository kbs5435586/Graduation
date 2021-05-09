#include "framework.h"
#include "MyRect.h"
#include "Management.h"
#include "UAV.h"


CMyRect::CMyRect()
	: CGameObject()
{
}

CMyRect::CMyRect(const CMyRect& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMyRect::Ready_Prototype()
{


	return S_OK;
}

HRESULT CMyRect::Ready_GameObject(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;
	if (FAILED(CreateInputLayout()))
		return E_FAIL;

	_vec3 vPos = _vec3(0.f, 10.f, 0.f);
	m_pTransformCom->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
	//m_pTransformCom->SetUp_RotationX(XMConvertToRadians(90.f));
	m_pTransformCom->Scaling(4.f, 4.f, 1.f);



	m_eTeam = TEAM::TEAM_A;


	return S_OK;
}

_int CMyRect::Update_GameObject(const _float& fTimeDelta)
{


	return _int();
}

_int CMyRect::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONEALPHA, this)))
		return -1;
	
	m_fTempTime += fTimeDelta * 100.f;
	if (m_fTempTime<=1000.f)
	{
		m_tRep.m_arrInt[2] = 0;
		m_tRep.m_arrFloat[0] += fTimeDelta;

	}
	else
	{
		m_tRep.m_arrInt[2] = 1;
		m_tRep.m_arrFloat[0] -= fTimeDelta;

	}

	return _int();
}

void CMyRect::Render_GameObject()
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
	_uint iOffset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffset, CONST_REGISTER::b0);
	CDevice::GetInstance()->SetTextureToShader(pManagement->Get_UAV(L"UAV_Default")->GetSRV().Get(), TEXTURE_REGISTER::t0);

	CDevice::GetInstance()->UpdateTable();



	m_tRep.m_arrInt[0] = 1;
	m_tRep.m_arrInt[1] = (_uint)m_eTeam;

	CDevice::GetInstance()->ClearDummyDesc_CS();
	iOffset = CManagement::GetInstance()->GetConstantBuffer((_uint)CONST_REGISTER::b8)->SetData((void*)&m_tRep);

	CDevice::GetInstance()->SetUpContantBufferToShader_CS(
		pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->GetCBV().Get(),
		iOffset, CONST_REGISTER::b8);
	CDevice::GetInstance()->SetUpUAVToRegister(pManagement->Get_UAV(L"UAV_Default"), UAV_REGISTER::u0);


	m_pShaderCom[1]->UpdateData_CS();


	CManagement::GetInstance()->Get_UAV(L"UAV_Default")->Dispatch(1, 1000, 1);


	m_pBufferCom->Render_VIBuffer();
	Safe_Release(pManagement);
}

HRESULT CMyRect::CreateInputLayout()
{
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	//vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });


	if (FAILED(m_pShaderCom[0]->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::LESS, SHADER_TYPE::SHADER_DEFFERED)))
		return E_FAIL;


	return S_OK;
}

CMyRect* CMyRect::Create()
{
	CMyRect* pInstance = new CMyRect();

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MessageBox(0, L"CMyRect Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMyRect::Clone_GameObject(void* pArg, _uint iIdx)
{
	CMyRect* pInstance = new CMyRect(*this);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CMyRect Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	m_iLayerIdx = iIdx;
	return pInstance;
}

void CMyRect::Free()
{

	Safe_Release(m_pBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom[0]);
	Safe_Release(m_pShaderCom[1]);

	CGameObject::Free();
}

HRESULT CMyRect::Ready_Component()
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

	m_pShaderCom[0] = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_UI");
	NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Shader0", m_pShaderCom[0])))
		return E_FAIL;

	m_pShaderCom[1] = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Compute_Default");
	NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Shader1", m_pShaderCom[1])))
		return E_FAIL;


	Safe_Release(pManagement);

	return S_OK;
}
