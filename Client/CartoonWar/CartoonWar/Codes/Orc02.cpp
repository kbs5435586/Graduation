#include "framework.h"
#include "Management.h"
#include "Orc02.h"

COrc02::COrc02()
	: CGameObject()
{
}

COrc02::COrc02(const COrc02& rhs)
	: CGameObject(rhs)
{
}

HRESULT COrc02::Ready_Prototype()
{
	return S_OK;
}

HRESULT COrc02::Ready_GameObject(void* pArg)
{
	m_IsClone = true;
	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(CreateInputLayout()))
		return E_FAIL;

	//m_pTransformCom->Scaling(0.1f, 0.1f, 0.1f);
	m_pTransformCom->SetUp_Speed(10.f, XMConvertToRadians(90.f));

	m_pAnimCom->SetBones(m_pMeshCom->GetBones());
	m_pAnimCom->SetAnimClip(m_pMeshCom->GetAnimClip());
	SetUp_Anim();

	_matrix matTemp = { };
	m_pColiiderCom->Clone_ColliderBox(matTemp, _vec3(10.f,10.f,10.f));
	//for (auto& iter : m_pMeshCom->m_vecDiffTexturePath)
	//{
	//	CTexture* pTexture = CTexture::Create(iter);
	//	m_vecTexture.push_back(pTexture);
	//}

	m_tInfo = { 10.f,10.f,10.f,10.f };
	CManagement::GetInstance()->Add_Data(DATA_TYPE::DATA_INFO, &m_tInfo);
	m = *m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION);
	CManagement::GetInstance()->Add_Data(DATA_TYPE::DATA_VECTOR, &m);

	return S_OK;
}

_int COrc02::Update_GameObject(const _float& fTimeDelta)
{


	return _int();
}

_int COrc02::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONEALPHA, this)))
		return -1;
	if (CManagement::GetInstance()->Key_Pressing(KEY_UP))
	{
		{
			//_vec3 vLook = {};
//vLook = *m_pTransformCom->Get_StateInfo(CTransform::STATE_LOOK);
//vLook = Vector3_::Normalize(vLook);


//_vec3 vDirectionPerSec = (vLook * 5.f * fTimeDelta);
//_vec3 vSlide = {};
//if (m_pNaviCom->Move_OnNavigation(m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION), &vDirectionPerSec, &vSlide))
//{
//	m_pTransformCom->BackWard(fTimeDelta);
//}
//else
//{
//	m_pTransformCom->Go_There(vSlide);
//}
		}


		m_iCurAnimIdx = 29;
		m_pTransformCom->BackWard(fTimeDelta);
	}
	else if (CManagement::GetInstance()->Key_Pressing(KEY_DOWN))
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
	else if (CManagement::GetInstance()->Key_Pressing(KEY_LEFT))
	{
		m_pTransformCom->Rotation_Y(fTimeDelta);
	}
	else if (CManagement::GetInstance()->Key_Down(KEY_RIGHT))
	{
		m_IsOnce = true;
		m_iCurAnimIdx = 27;
	}

	if (m_pAnimCom->Update(m_vecAnimCtrl[m_iCurAnimIdx], m_fRatio, fTimeDelta) && m_IsOnce)
	{
		m_iCurAnimIdx = 16;
		m_IsOnce = false;
	}


	m = *m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION);
	CManagement::GetInstance()->Notify(DATA_TYPE::DATA_INFO, &m_tInfo);
	CManagement::GetInstance()->Notify(DATA_TYPE::DATA_VECTOR, &m);
	return _int();
}

void COrc02::Render_GameObject()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();



	_uint iSubsetNum = m_pMeshCom->GetSubsetNum() - 1;
	//_uint iSubsetNum = 1;
	for (_uint i = 0; i < iSubsetNum; ++i)
	{

		MAINPASS tMainPass = {};
		_matrix matWorld = m_pTransformCom->Get_Matrix();
		_matrix I_matWorld = m_pTransformCom->Get_Matrix();
		_matrix matView = CCamera_Manager::GetInstance()->GetMatView();
		_matrix matProj = CCamera_Manager::GetInstance()->GetMatProj();

		_matrix I_matView = CCamera_Manager::GetInstance()->GetIMatView();
		_matrix I_matProj = CCamera_Manager::GetInstance()->GetIMatProj();

		REP tRep = {};
		tRep.m_arrInt[0] = 1;
		tRep.m_arrInt[1] = m_pAnimCom->GetBones()->size();

		m_pShaderCom->SetUp_OnShader(matWorld, matView, matProj, tMainPass);

		_uint iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
			(_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);

		iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->SetData((void*)&tRep);
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
			(_uint)CONST_REGISTER::b8)->GetCBV().Get(), iOffeset, CONST_REGISTER::b8);

		//CDevice::GetInstance()->SetTextureToShader(m_vecTexture[i]->GetSRV_().Get(), TEXTURE_REGISTER::t0);

		m_pMeshCom->SetRatio(m_fRatio);

		m_pAnimCom->UpdateData(m_pMeshCom, m_pComputeShaderCom);

		CDevice::GetInstance()->UpdateTable();
		m_pMeshCom->Render_Mesh(i);
	}

	m_pShaderComT->SetUp_OnShaderT(matWorld, I_matView, I_matProj, tMainPassT);
	_uint iOffesetT = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPassT);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffesetT, CONST_REGISTER::b0);
	CDevice::GetInstance()->UpdateTable();
	m_pBufferCom->Render_VIBuffer();

	
	m_pColiiderCom->Render_Collider(m_pAnimCom->GetMatix());
	Safe_Release(pManagement);
}

HRESULT COrc02::CreateInputLayout()
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

	if (FAILED(m_pShaderCom->Create_Shader(vecDesc, RS_TYPE::WIREFRAME, DEPTH_STENCIL_TYPE::LESS, SHADER_TYPE::SHADER_DEFFERED)))
		return E_FAIL;

	vector<D3D12_INPUT_ELEMENT_DESC>  vecDescT;
	vecDescT.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDescT.push_back(D3D12_INPUT_ELEMENT_DESC{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDescT.push_back(D3D12_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	if (FAILED(m_pShaderComT->Create_Shader(vecDescT, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::LESS, SHADER_TYPE::SHADER_DEFFERED)))
		return E_FAIL;
	return S_OK;
}

void COrc02::SetUp_Anim()
{
	m_vecAnimCtrl.push_back(AnimCtrl(0, 70, 0.f, 2.3f));					//attack01		0
	m_vecAnimCtrl.push_back(AnimCtrl(71, 141, 2.3f, 4.7f));					//attack02		1
	m_vecAnimCtrl.push_back(AnimCtrl(142, 217, 4.73f, 7.23f));				//attack03		2
	m_vecAnimCtrl.push_back(AnimCtrl(218, 288, 7.26f, 9.59f));				//attack04		3
	m_vecAnimCtrl.push_back(AnimCtrl(289, 364, 9.63f, 11.13f));				//attack05		4
	m_vecAnimCtrl.push_back(AnimCtrl(365, 440, 12.16f, 14.6f));				//attack06		5
	m_vecAnimCtrl.push_back(AnimCtrl(441, 521, 14.7f, 17.3f));				//attack07		6
	m_vecAnimCtrl.push_back(AnimCtrl(522, 597, 17.3f, 19.9f));				//attack08		7
	m_vecAnimCtrl.push_back(AnimCtrl(598, 658, 19.9f, 21.9f));				//dead01		8
	m_vecAnimCtrl.push_back(AnimCtrl(659, 719, 21.9f, 23.9f));				//dead02		9
	m_vecAnimCtrl.push_back(AnimCtrl(720, 775, 24.f, 25.8f));				//dead03		10
	m_vecAnimCtrl.push_back(AnimCtrl(776, 831, 25.8f, 27.7f));				//dead04		11
	m_vecAnimCtrl.push_back(AnimCtrl(832, 872, 27.7f, 29.f));				//gethit01		12
	m_vecAnimCtrl.push_back(AnimCtrl(873, 913, 29.1f, 30.4f));				//gethit02		13
	m_vecAnimCtrl.push_back(AnimCtrl(914, 974, 30.4f, 32.4f));				//Idle			14
	m_vecAnimCtrl.push_back(AnimCtrl(975, 1155, 32.5f, 38.5f));				//Idle_Other00	15
	m_vecAnimCtrl.push_back(AnimCtrl(1156, 1276, 38.5f, 42.5f));			//Idle_Other01	16
	m_vecAnimCtrl.push_back(AnimCtrl(1277, 1337, 42.5f, 44.5f));			//Idle_sit		17
	m_vecAnimCtrl.push_back(AnimCtrl(1338, 1388, 44.6f, 46.2f));			//jump			18
	m_vecAnimCtrl.push_back(AnimCtrl(1389, 1419, 46.3f, 47.3f));			//run			19
	m_vecAnimCtrl.push_back(AnimCtrl(1420, 1450, 47.3f, 38.3f));			//run_back		20
	m_vecAnimCtrl.push_back(AnimCtrl(1451, 1481, 48.3f, 49.3f));			//run_left		21
	m_vecAnimCtrl.push_back(AnimCtrl(1482, 1512, 49.4f, 50.4f));			//run_right		22
	m_vecAnimCtrl.push_back(AnimCtrl(1513, 1603, 50.4f, 53.4f));			//shout			23
	m_vecAnimCtrl.push_back(AnimCtrl(1604, 1644, 53.4f, 54.8f));			//take weapon	24
	m_vecAnimCtrl.push_back(AnimCtrl(1645, 1680, 54.8f, 56.f, true));		//TurnLeft		25
	m_vecAnimCtrl.push_back(AnimCtrl(1681, 1726, 56.f, 57.5f));				//TurnLeft_sit	26
	m_vecAnimCtrl.push_back(AnimCtrl(1727, 1762, 57.5f, 58.7f, true));		//TurnRight		27
	m_vecAnimCtrl.push_back(AnimCtrl(1763, 1808, 58.7f, 60.2f));			//TurnRight_sit	28
	m_vecAnimCtrl.push_back(AnimCtrl(1809, 1849, 60.3f, 61.6f));			//walk			29
	m_vecAnimCtrl.push_back(AnimCtrl(1850, 1890, 61.6f, 63.f));				//walk back		30
	m_vecAnimCtrl.push_back(AnimCtrl(1891, 1941, 63.f, 64.7f));				//walk back sit	31
	m_vecAnimCtrl.push_back(AnimCtrl(1942, 1982, 64.7f, 66.f));				//walk left		32
	m_vecAnimCtrl.push_back(AnimCtrl(1983, 2033, 66.1f, 67.7f));			//walk left sit	33
	m_vecAnimCtrl.push_back(AnimCtrl(2034, 2074, 67.8f, 69.1f));			//walk right	34
	m_vecAnimCtrl.push_back(AnimCtrl(2075, 2125, 69.1f, 70.8f));			//sit			35	
	m_vecAnimCtrl.push_back(AnimCtrl(2126, 2176, 70.86f, 72.5f));			//walk right	36

}

COrc02* COrc02::Create()
{
	COrc02* pInstance = new COrc02();

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MessageBox(0, L"COrc02 Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* COrc02::Clone_GameObject(void* pArg, const _uint& iIdx)

{
	COrc02* pInstance = new COrc02(*this);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"COrc02 Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	m_iLayerIdx = iIdx;
	return pInstance;
}
void COrc02::Free()
{
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pShaderComT);
	Safe_Release(m_pComputeShaderCom);
	Safe_Release(m_pAnimCom);
	Safe_Release(m_pColiiderCom);
	//Safe_Release(m_pNaviCom);
	if (m_IsClone)
	{
		for (auto& iter : m_vecTexture)
		{
			Safe_Release(iter);
		}
	}
	CGameObject::Free();
}

HRESULT COrc02::Ready_Component()
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

	m_pMeshCom = (CMesh*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Mesh_Orc02");
	NULL_CHECK_VAL(m_pMeshCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Mesh", m_pMeshCom)))
		return E_FAIL;

	m_pShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Toon");
	NULL_CHECK_VAL(m_pShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Shader", m_pShaderCom)))
		return E_FAIL;

	m_pShaderComT = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_UUI");
	NULL_CHECK_VAL(m_pShaderComT, E_FAIL);
	if (FAILED(Add_Component(L"Com_ShaderT", m_pShaderComT)))
		return E_FAIL;

	m_pComputeShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_Compute_Animation");
	NULL_CHECK_VAL(m_pComputeShaderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_ComputeShader", m_pComputeShaderCom)))
		return E_FAIL;

	m_pAnimCom = (CAnimator*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Animation");
	NULL_CHECK_VAL(m_pAnimCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Anim", m_pAnimCom)))
		return E_FAIL;

	m_pColiiderCom = (CCollider*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Collider_OBB");
	NULL_CHECK_VAL(m_pColiiderCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Collider", m_pColiiderCom)))
		return E_FAIL;


	//m_pNaviCom = (CNavigation*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_NaviMesh_Test");
	//NULL_CHECK_VAL(m_pNaviCom, E_FAIL);
	//if (FAILED(Add_Component(L"Com_Navi", m_pNaviCom)))
	//	return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}

void COrc02::Set_Animation()
{
	if (m_iCurAnimIdx != m_iPreAnimIdx)
	{

		m_iCurAnimIdx = m_iPreAnimIdx;
	}

}
