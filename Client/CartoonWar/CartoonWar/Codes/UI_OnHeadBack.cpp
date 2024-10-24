#include "framework.h"
#include "UI_OnHeadBack.h"
#include "Management.h"

CUI_OnHeadBack::CUI_OnHeadBack()
{
}

CUI_OnHeadBack::CUI_OnHeadBack(const CUI_OnHeadBack& rhs)
{
}

HRESULT CUI_OnHeadBack::Ready_Prototype()
{
	return S_OK;
}

HRESULT CUI_OnHeadBack::Ready_GameObject(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;
	if (FAILED(CreateInputLayout()))
		return E_FAIL;

	if (nullptr == pArg)
		return E_FAIL;
	m_vPos = *(_vec3*)pArg;
	m_pTransformCom->Set_StateInfo(CTransform::STATE_POSITION, &m_vPos);
	_vec3 vSize = { 5.f, 0.25f, 1.f };
	m_pTransformCom->Scaling(vSize);

	return S_OK;
}

_int CUI_OnHeadBack::Update_GameObject(const _float& fTimeDelta)
{
	m_pTransformCom->Set_StateInfo(CTransform::STATE_POSITION, &m_vPos);


	{
		_matrix matView = CCamera_Manager::GetInstance()->GetMatView();

		matView = Matrix_::Inverse(matView);

		_vec3		vRight, vUp, vLook;

		vRight = *(_vec3*)&matView.m[0][0] * m_pTransformCom->Get_Scale().x;
		vUp = *(_vec3*)&matView.m[1][0] * m_pTransformCom->Get_Scale().y;
		vLook = *(_vec3*)&matView.m[2][0] * m_pTransformCom->Get_Scale().z;

		m_pTransformCom->Set_StateInfo(CTransform::STATE_RIGHT, &vRight);
		m_pTransformCom->Set_StateInfo(CTransform::STATE_LOOK, &vLook);
	}


	return _int();
}

_int CUI_OnHeadBack::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (m_pFrustumCom->Culling_Frustum(m_pTransformCom), 10.f)
	{
		if (m_pRendererCom != nullptr)
		{
			if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI_BACK, this)))
				return E_FAIL;
		}
	}
	return _int();
}

void CUI_OnHeadBack::Render_GameObject()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();

	MAINPASS tMainPass = {};
	_matrix matWorld = m_pTransformCom->Get_Matrix();
	_matrix matView = CCamera_Manager::GetInstance()->GetMatView();
	_matrix matProj = CCamera_Manager::GetInstance()->GetMatProj();

	REP tRep = {};
	tRep.m_arrInt[0] = m_tInfo.fHP;
	tRep.m_arrInt[1] = 0;


	m_pShaderCom->SetUp_OnShader(matWorld, matView, matProj, tMainPass);

	_uint iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
		(_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);

	iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->SetData((void*)&tRep);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
		(_uint)CONST_REGISTER::b8)->GetCBV().Get(), iOffeset, CONST_REGISTER::b8);


	CDevice::GetInstance()->UpdateTable();
	m_pBufferCom->Render_VIBuffer();
	Safe_Release(pManagement);
}

HRESULT CUI_OnHeadBack::CreateInputLayout()
{
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });


	if (FAILED(m_pShaderCom->Create_Shader(vecDesc)))
		return E_FAIL;


	return S_OK;
}

CUI_OnHeadBack* CUI_OnHeadBack::Create()
{
	CUI_OnHeadBack* pInstance = new CUI_OnHeadBack;
	if (FAILED(pInstance->Ready_Prototype()))
	{
		MessageBox(0, L"CUI_OnHeadBack Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_OnHeadBack::Clone_GameObject(void* pArg, _uint iIdx)
{
	CUI_OnHeadBack* pInstance = new CUI_OnHeadBack;
	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MessageBox(0, L"CUI_OnHeadBack Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	m_iLayerIdx = iIdx;
	return pInstance;
}

void CUI_OnHeadBack::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pFrustumCom);
	CUI::Free();
}

HRESULT CUI_OnHeadBack::Ready_Component()
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

	m_pShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_UI_OnHead");
	NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
		return E_FAIL;
	m_pFrustumCom = (CFrustum*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Frustum");
	NULL_CHECK_VAL(m_pFrustumCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Frustum", m_pFrustumCom)))
		return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}

void CUI_OnHeadBack::SetPosition(_vec3 vPos, CLASS eClass)
{
	m_vPos = vPos;
	vPos.z -= 0.1f;
	switch (eClass)
	{
	case CLASS::CLASS_ARCHER:
	case CLASS::CLASS_INFANTRY:
	case CLASS(4):
	case CLASS::CLASS_MAGE:
	case CLASS::CLASS_SPEARMAN:
	case CLASS::CLASS_WORKER:
		m_vPos.y += 8.f;
		break;
	default:
		m_vPos.y += 10.f;
		//m_vPos.z -= 2.f;
		break;
	}
}

void CUI_OnHeadBack::SetInfo(INFO tInfo)
{
	m_tInfo = tInfo;
}
