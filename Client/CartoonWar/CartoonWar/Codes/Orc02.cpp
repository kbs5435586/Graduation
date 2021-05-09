#include "framework.h"
#include "Management.h"
#include "Orc02.h"
#include "Weapon.h"

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

	m_pTransformCom->SetUp_RotationY(XMConvertToRadians(180.f));
	_vec3 vPos = { 200.f, 0.f, 200.f };
	m_pTransformCom->Scaling(0.02f, 0.02f, 0.02f);
	m_pTransformCom->SetUp_Speed(100.f, XMConvertToRadians(90.f));
	m_pTransformCom->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
	m_pMeshCom->m_vecDiffTexturePath;
	m_pAnimCom->SetBones(m_pMeshCom->GetBones());
	m_pAnimCom->SetAnimClip(m_pMeshCom->GetAnimClip());
	SetUp_Anim();

	m_pAnimCom->LateInit();
	m_iCurAnimIdx = 14;

	for (auto& iter : m_pMeshCom->m_vecDiffTexturePath)
	{
		CTexture* pTexture = CTexture::Create(iter);
		m_vecTexture.push_back(pTexture);
	}

	return S_OK;
}

_int COrc02::Update_GameObject(const _float& fTimeDelta)
{
	m_pColiiderCom->Update_Collider(m_pTransformCom);

	if (m_pWeapon)
	{
		CTransform* pWeaponTransform = (CTransform*)m_pWeapon->Get_ComponentPointer(L"Com_Transform");
		pWeaponTransform->Set_Matrix(m_pTransformCom->Get_Matrix());
	}
	return _int();
}

_int COrc02::LastUpdate_GameObject(const _float& fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (m_pFrustumCom->Culling_Frustum(m_pTransformCom, 10.f))
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONEALPHA, this)))
			return -1;

	}

	if (CManagement::GetInstance()->Key_Pressing(KEY_LBUTTON))
	{
		m_iCurAnimIdx = rand() % 8;
	}
	if (CManagement::GetInstance()->Key_Pressing(KEY_UP))
	{
		m_iCurAnimIdx = 29;
		{
			_vec3 vLook = {};
			vLook = *m_pTransformCom->Get_StateInfo(CTransform::STATE_LOOK);
			vLook = Vector3_::Normalize(vLook);


			_vec3 vDirectionPerSec = (vLook * 5.f * fTimeDelta);
			_vec3 vSlide = {};
			if (m_pNaviCom->Move_OnNavigation(m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION), &vDirectionPerSec, &vSlide))
			{
				m_pTransformCom->BackWard(fTimeDelta);
			}
			else
			{
				m_pTransformCom->Go_There(vSlide);
			}
		}

	}
	else if (CManagement::GetInstance()->Key_Pressing(KEY_DOWN))
	{
		m_iCurAnimIdx = 30;
		//m_pTransformCom->BackWard(fTimeDelta);
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
	else if (CManagement::GetInstance()->Key_Pressing(KEY_RIGHT))
	{
		m_iCurAnimIdx = 32;
		//m_pTransformCom->Rotation_Y(fTimeDelta);
		m_pTransformCom->Go_Left(fTimeDelta);
	}
	else if (CManagement::GetInstance()->Key_Pressing(KEY_LEFT))
	{
		m_iCurAnimIdx = 34;
		//m_pTransformCom->Rotation_Y(-fTimeDelta);
		m_pTransformCom->Go_Right(fTimeDelta);
	}
	else
	{
		m_iCurAnimIdx = 14;
	}



	Set_Animation();
	if (m_pAnimCom->Update(m_vecAnimCtrl[m_iCurAnimIdx], m_fRatio, fTimeDelta) && m_IsOnce)
	{
		m_iCurAnimIdx = 16;
		m_IsOnce = false;
	}


	if (CManagement::GetInstance()->Key_Down(KEY_E))
	{
		_uint iLen = 99999999;
		_vec3 vPos = *m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION);
		for (auto& iter : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_Weapon"))
		{
			CTransform* pTargetTransform = (CTransform*)iter->Get_ComponentPointer(L"Com_Transform");
			_vec3 vTargetPos = *pTargetTransform->Get_StateInfo(CTransform::STATE_POSITION);

			_vec3 vLen = vPos - vTargetPos;
			_uint iTempLen = Vector3_::Length(vLen);

			if (iLen > iTempLen)
			{
				iLen = iTempLen;
				for (auto& iter1 : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_Weapon"))
				{
					dynamic_cast<CWeapon*>(iter1)->GetIsPicked() = false;
				}
				dynamic_cast<CWeapon*>(iter)->GetIsPicked() = true;
			}
		}


		for (auto& iter : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_Weapon"))
		{
			if (dynamic_cast<CWeapon*>(iter)->GetIsPicked())
			{
				m_pWeapon = dynamic_cast<CWeapon*>(iter);
				m_pWeapon->GetStructedBuffer() = m_pAnimCom->GetMatix();
				m_pWeapon->GetBoneIdx() = 27;

			}
		}
	}
	else if (CManagement::GetInstance()->Key_Down(KEY_Q))
	{
		m_pWeapon = nullptr;
		for (auto& iter : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_Weapon"))
		{
			dynamic_cast<CWeapon*>(iter)->GetIsPicked() = false;
		}
	}



	return _int();
}

void COrc02::Render_GameObject()
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
		tRep.m_arrInt[0] = 1;
		tRep.m_arrInt[1] = m_pAnimCom->GetBones()->size();

		m_pShaderCom->SetUp_OnShader(matWorld, matView, matProj, tMainPass);

		_uint iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
			(_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);

		iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->SetData((void*)&tRep);
		CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(
			(_uint)CONST_REGISTER::b8)->GetCBV().Get(), iOffeset, CONST_REGISTER::b8);

		CTexture* pTexture = m_vecTexture[i];
		if (pTexture)
		{
			CDevice::GetInstance()->SetTextureToShader(pTexture->GetSRV_().Get(), TEXTURE_REGISTER::t0);
		}
		m_pAnimCom->UpdateData(m_pMeshCom, m_pComputeShaderCom);

		CDevice::GetInstance()->UpdateTable();
		m_pMeshCom->Render_Mesh(i);
	}


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

	if (FAILED(m_pShaderCom->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::LESS, SHADER_TYPE::SHADER_DEFFERED)))
		return E_FAIL;

	return S_OK;
}

void COrc02::SetUp_Anim()
{
	m_vecAnimCtrl.push_back(AnimCtrl(0, 70, 0.f, 2.33f));					//attack01				0
	m_vecAnimCtrl.push_back(AnimCtrl(71, 141, 2.36f, 4.7f));				//attack02				1
	m_vecAnimCtrl.push_back(AnimCtrl(142, 217, 4.73f, 7.23f));				//attack03				2
	m_vecAnimCtrl.push_back(AnimCtrl(218, 288, 7.26f, 9.59f));				//attack04				3
	m_vecAnimCtrl.push_back(AnimCtrl(289, 364, 9.63f, 11.13f));				//attack05				4
	m_vecAnimCtrl.push_back(AnimCtrl(365, 440, 12.16f, 14.66f));			//attack06				5
	m_vecAnimCtrl.push_back(AnimCtrl(441, 521, 14.7f, 17.36f));				//attack07				6
	m_vecAnimCtrl.push_back(AnimCtrl(522, 597, 17.4f, 19.9f));				//attack08				7
	m_vecAnimCtrl.push_back(AnimCtrl(598, 658, 19.93f, 21.93f));			//dead01				8
	m_vecAnimCtrl.push_back(AnimCtrl(659, 719, 21.96f, 23.96f));			//dead02				9
	m_vecAnimCtrl.push_back(AnimCtrl(720, 775, 24.f, 25.83f));				//dead03				10
	m_vecAnimCtrl.push_back(AnimCtrl(776, 831, 25.86f, 27.7f));				//dead04				11
	m_vecAnimCtrl.push_back(AnimCtrl(832, 872, 27.733333f, 29.06666f));		//gethit01				12
	m_vecAnimCtrl.push_back(AnimCtrl(873, 913, 29.1f, 30.43f));				//gethit02				13
	m_vecAnimCtrl.push_back(AnimCtrl(914, 974, 30.46f, 32.46f));			//Idle					14
	m_vecAnimCtrl.push_back(AnimCtrl(975, 1155, 32.5f, 38.5f));				//Idle_Other00			15
	m_vecAnimCtrl.push_back(AnimCtrl(1156, 1276, 38.53f, 42.53f));			//Idle_Other01			16
	m_vecAnimCtrl.push_back(AnimCtrl(1277, 1337, 42.566f, 44.56f));			//Idle_sit				17
	m_vecAnimCtrl.push_back(AnimCtrl(1338, 1388, 44.6f, 46.26f));			//jump					18
	m_vecAnimCtrl.push_back(AnimCtrl(1389, 1419, 46.3f, 47.3f));			//run					19
	m_vecAnimCtrl.push_back(AnimCtrl(1420, 1450, 47.333333f, 48.33333f));	//run_back				20
	m_vecAnimCtrl.push_back(AnimCtrl(1451, 1481, 48.36f, 49.36f));			//run_left				21
	m_vecAnimCtrl.push_back(AnimCtrl(1482, 1512, 49.4f, 50.4f));			//run_right				22
	m_vecAnimCtrl.push_back(AnimCtrl(1513, 1603, 50.43f, 53.43f));			//shout					23
	m_vecAnimCtrl.push_back(AnimCtrl(1604, 1644, 53.46f, 54.8f));			//take weapon			24
	m_vecAnimCtrl.push_back(AnimCtrl(1645, 1680, 54.83f, 56.f, true));		//TurnLeft				25
	m_vecAnimCtrl.push_back(AnimCtrl(1681, 1726, 56.03f, 57.53f));			//TurnLeft_sit			26
	m_vecAnimCtrl.push_back(AnimCtrl(1727, 1762, 57.56f, 58.73f, true));	//TurnRight				27
	m_vecAnimCtrl.push_back(AnimCtrl(1763, 1808, 58.76f, 60.26f));			//TurnRight_sit			28
	m_vecAnimCtrl.push_back(AnimCtrl(1809, 1849, 60.3f, 61.63f));			//walk					29
	m_vecAnimCtrl.push_back(AnimCtrl(1850, 1890, 61.66f, 63.f));			//walk back				30
	m_vecAnimCtrl.push_back(AnimCtrl(1891, 1941, 63.03f, 64.7f));			//walk back sit			31
	m_vecAnimCtrl.push_back(AnimCtrl(1942, 1982, 64.73f, 66.06f));			//walk left				32
	m_vecAnimCtrl.push_back(AnimCtrl(1983, 2033, 66.1f, 67.76f));			//walk left sit			33
	m_vecAnimCtrl.push_back(AnimCtrl(2034, 2074, 67.8f, 69.13f));			//walk right			34
	m_vecAnimCtrl.push_back(AnimCtrl(2075, 2125, 69.16f, 70.83f));			//walk sit				35	
	m_vecAnimCtrl.push_back(AnimCtrl(2126, 2176, 70.86f, 72.53f));			//walk rightsit			36

	m_vecAnimCtrl.push_back(AnimCtrl(2177, 2237, 72.56f, 74.56f));			//WithoutDead_01		37
	m_vecAnimCtrl.push_back(AnimCtrl(2238, 2298, 74.6f, 76.6f));			//WithoutDead_02		38
	m_vecAnimCtrl.push_back(AnimCtrl(2299, 2349, 76.63f, 78.3f));			//WithoutDeadLeft		39
	m_vecAnimCtrl.push_back(AnimCtrl(2350, 2400, 78.33f, 80.f));			//WithoutDeadRight		40
	m_vecAnimCtrl.push_back(AnimCtrl(2401, 2436, 80.03333f, 81.22f));		//WithoutGetHit_01		41
	m_vecAnimCtrl.push_back(AnimCtrl(2437, 2470, 81.23f, 82.33f));			//WithoutGetHit_21		42
	m_vecAnimCtrl.push_back(AnimCtrl(2471, 2531, 82.36f, 84.36f));			//WithoutIdle_01		43
	m_vecAnimCtrl.push_back(AnimCtrl(2532, 2712, 84.4f, 90.4f));			//WithoutIdle_02		44
	m_vecAnimCtrl.push_back(AnimCtrl(2713, 2833, 90.43f, 94.43f));			//WithoutIdle_03		45
	m_vecAnimCtrl.push_back(AnimCtrl(2834, 2894, 94.46f, 96.46f));			//WithoutIdleSit		46
	m_vecAnimCtrl.push_back(AnimCtrl(2895, 2955, 96.5f, 98.5f));			//WithoutIdleSwim		47
	m_vecAnimCtrl.push_back(AnimCtrl(2956, 2996, 98.53f, 99.86f));			//WithoutJump			48
	m_vecAnimCtrl.push_back(AnimCtrl(2997, 3027, 99.9f, 100.9f));			//WithoutRun			49
	m_vecAnimCtrl.push_back(AnimCtrl(3028, 3058, 100.93f, 101.93f));		//WithoutRunBack		50
	m_vecAnimCtrl.push_back(AnimCtrl(3059, 3089, 101.96f, 102.96f));		//WithoutRunLeft		51
	m_vecAnimCtrl.push_back(AnimCtrl(3090, 3120, 103.f, 104.f));			//WithoutRunRight		52
	m_vecAnimCtrl.push_back(AnimCtrl(3121, 3211, 104.03f, 107.03f));		//WithoutShout			53
	m_vecAnimCtrl.push_back(AnimCtrl(3212, 3252, 107.06f, 108.4f));			//WithoutSwim			54
	m_vecAnimCtrl.push_back(AnimCtrl(3253, 3288, 108.43f, 109.59f));		//WithoutTurnLeft		55
	m_vecAnimCtrl.push_back(AnimCtrl(3289, 3334, 109.63f, 111.13f));		//WithoutTurnLeftSit	56
	m_vecAnimCtrl.push_back(AnimCtrl(3335, 3370, 111.16f, 112.33f));		//WithoutTurnRight		57
	m_vecAnimCtrl.push_back(AnimCtrl(3371, 3416, 112.36f, 113.86f));		//WithoutTurnRightSit	58
	m_vecAnimCtrl.push_back(AnimCtrl(3417, 3457, 113.90f, 115.23f));		//WithoutWalk			59
	m_vecAnimCtrl.push_back(AnimCtrl(3458, 3498, 115.26f, 116.60f));		//WithoutWalkBack		60
	m_vecAnimCtrl.push_back(AnimCtrl(3499, 3549, 116.63f, 118.30f));		//WithoutWalkBackSit	61
	m_vecAnimCtrl.push_back(AnimCtrl(3550, 3590, 118.33f, 119.66f));		//WithoutWalkLeft		62
	m_vecAnimCtrl.push_back(AnimCtrl(3591, 3641, 119.70f, 121.36f));		//WihoutWalkLeftSit		63
	m_vecAnimCtrl.push_back(AnimCtrl(3642, 3682, 121.40f, 122.73f));		//WithoutWalkRight		64
	m_vecAnimCtrl.push_back(AnimCtrl(3683, 3733, 122.76f, 124.43f));		//WithoutWalkRightSit	65
	m_vecAnimCtrl.push_back(AnimCtrl(3734, 3784, 124.46f, 126.13f));		//WithoutWalkSit		66


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

CGameObject* COrc02::Clone_GameObject(void* pArg, _uint iIdx)

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
	Safe_Release(m_pComputeShaderCom);
	Safe_Release(m_pAnimCom);
	Safe_Release(m_pColiiderCom);
	Safe_Release(m_pNaviCom);
	Safe_Release(m_pFrustumCom);
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

	m_pFrustumCom = (CFrustum*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Frustum");
	NULL_CHECK_VAL(m_pFrustumCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Frustum", m_pFrustumCom)))
		return E_FAIL;


	m_pNaviCom = (CNavigation*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_NaviMesh");
	NULL_CHECK_VAL(m_pNaviCom, E_FAIL);
	if (FAILED(Add_Component(L"Com_Navi", m_pNaviCom)))
		return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}

void COrc02::Set_Animation()
{
	if (m_iCurAnimIdx != m_iPreAnimIdx)
	{
		m_vecAnimCtrl[m_iCurAnimIdx].fCurTime = 0.f;
		m_iPreAnimIdx = m_iCurAnimIdx;
	}
}
