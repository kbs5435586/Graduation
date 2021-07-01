#include "framework.h"
#include "Collider.h"
#include "Shader.h"
#include "Transform.h"
#include "StructedBuffer.h"
#include "Buffer_CubeCol.h"
#include "Management.h"

CCollider::CCollider()
	: CComponent()
{
}

CCollider::CCollider(const CCollider& rhs)
	: CComponent(rhs)
	, m_eType(rhs.m_eType)
	, m_vSize(rhs.m_vSize)
	, m_fRadius(rhs.m_fRadius)
{


	m_IsClone = true;
}

HRESULT CCollider::Ready_Collider(COLLIDER_TYPE eType)
{
	m_eType = eType;
	

	return S_OK;
}

HRESULT CCollider::Ready_Collider_AABB_BOX(CTransform* pTransform, const _vec3 vSize)
{
	// 대상으로 한 객체의 회전 값 받아와서 회전 죽이기

	_matrix pTarget_matrix = pTransform->Get_Matrix();
	_matrix matTemp = Remove_Rotation(pTarget_matrix);

	m_pTransformCom->Set_Matrix(matTemp);
	m_pTransformCom->Scaling(vSize);

	m_vMin = { -vSize.x / 2.f, 0.f, -vSize.z / 2.f };
	m_vMax = { vSize.x / 2.f, vSize.y, vSize.z / 2.f };

	return S_OK;
}

HRESULT CCollider::Ready_Collider_OBB_BOX(CTransform* pTransform, const _vec3 vSize)
{
	// 대상으로 한 객체의 회전 값 받아와서 Collider Transform에 적용하기
	CTransform* pTarget_Transform = pTransform;
	_vec3		vDir[3];

	for (size_t i = 0; i < 3; ++i)
	{
		vDir[i] = *pTarget_Transform->Get_StateInfo(CTransform::STATE(i));
		vDir[i].Normalize();
	}


	vDir[CTransform::STATE_RIGHT] = vDir[CTransform::STATE_RIGHT] * vSize.x;
	vDir[CTransform::STATE_UP] = vDir[CTransform::STATE_UP] * vSize.y;
	vDir[CTransform::STATE_LOOK] = vDir[CTransform::STATE_LOOK] * vSize.z;


	m_pTransformCom->Set_StateInfo(CTransform::STATE_RIGHT, &vDir[CTransform::STATE_RIGHT]);
	m_pTransformCom->Set_StateInfo(CTransform::STATE_UP, &vDir[CTransform::STATE_UP]);
	m_pTransformCom->Set_StateInfo(CTransform::STATE_LOOK, &vDir[CTransform::STATE_LOOK]);


	m_vMin = { -vSize.x / 2.f, 0.f, -vSize.z / 2.f };
	m_vMax = { vSize.x / 2.f, vSize.y, vSize.z / 2.f };


	m_pOBB = new OBB;
	m_pOBB->vPoint[0] = _vec3(m_vMin.x, m_vMax.y, m_vMin.z);
	m_pOBB->vPoint[1] = _vec3(m_vMax.x, m_vMax.y, m_vMin.z);
	m_pOBB->vPoint[2] = _vec3(m_vMax.x, m_vMin.y, m_vMin.z);
	m_pOBB->vPoint[3] = _vec3(m_vMin.x, m_vMin.y, m_vMin.z);

	m_pOBB->vPoint[4] = _vec3(m_vMin.x, m_vMax.y, m_vMax.z);
	m_pOBB->vPoint[5] = _vec3(m_vMax.x, m_vMax.y, m_vMax.z);
	m_pOBB->vPoint[6] = _vec3(m_vMax.x, m_vMin.y, m_vMax.z);
	m_pOBB->vPoint[7] = _vec3(m_vMin.x, m_vMin.y, m_vMax.z);

	return S_OK;
}

HRESULT CCollider::Ready_Collider_SPHERE(CTransform* pTransform, const _vec3 vSize)
{
	CTransform* pTarget_Transform = pTransform;
	pTarget_Transform->Scaling(vSize.x, vSize.y, vSize.z);
	m_fRadius = vSize.x;
	m_pTransformCom->Set_Matrix(pTarget_Transform->Get_Matrix());

	return S_OK;
}

HRESULT CCollider::Ready_Collider_AABB_BOX(_matrix matWorld, const _vec3 vSize)
{

	_matrix pTarget_matrix = matWorld;
	_matrix matTemp = Remove_Rotation(pTarget_matrix);

	m_pTransformCom->Set_Matrix(matTemp);
	m_pTransformCom->Scaling(vSize);


	return S_OK;
}

HRESULT CCollider::Ready_Collider_OBB_BOX(_matrix matWorld, const _vec3 vSize)
{
	_matrix pTarget_matrix = matWorld;


	m_pTransformCom->Set_Matrix(pTarget_matrix);
	m_pTransformCom->Scaling(vSize);


	return S_OK;
}

HRESULT CCollider::Ready_Collider_SPHERE(_matrix matWorld, const _vec3 vSize)
{
	_matrix pTarget_matrix = matWorld;


	m_pTransformCom->Set_Matrix(pTarget_matrix);
	m_pTransformCom->Scaling(vSize);
	m_fRadius = vSize.x;
	m_pTransformCom->Set_Matrix(pTarget_matrix);

	return S_OK;
}

HRESULT CCollider::Clone_ColliderBox(CTransform* pTransform, const _vec3 vSize)
{
	m_pTransformCom = CTransform::Create();
	if (nullptr == m_pTransformCom)
		return E_FAIL;


	HRESULT hr = S_OK;
	m_vSize = vSize;
	switch (m_eType)
	{
	case COLLIDER_TYPE::COLLIDER_AABB:
		m_pBufferCom = CBuffer_CubeCol::Create();
		if (nullptr == m_pBufferCom)
			break;
		hr = Ready_Collider_AABB_BOX(pTransform, vSize);
		break;
	case COLLIDER_TYPE::COLLIDER_OBB:
		m_pBufferCom = CBuffer_CubeCol::Create();
		if (nullptr == m_pBufferCom)
			break;
		hr = Ready_Collider_OBB_BOX(pTransform, vSize);
		break;
	case COLLIDER_TYPE::COLLIDER_SPHERE:
		m_pBufferCom = CBuffer_Sphere::Create();
		if (nullptr == m_pBufferCom)
			break;
		hr = Ready_Collider_SPHERE(pTransform, vSize);
		break;
	}

	m_pShaderCom = CShader::Create(L"../ShaderFiles/Shader_Collider.hlsl", "VS_Main", "PS_Main");
	if (nullptr == m_pShaderCom)
		return E_FAIL;



	if (FAILED(Create_InputLayOut()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCollider::Clone_ColliderBox(_matrix matWorld, const _vec3 vSize)
{
	m_vSize = vSize;
	HRESULT hr = S_OK;
	switch (m_eType)
	{
	case COLLIDER_TYPE::COLLIDER_AABB:
		hr = Ready_Collider_AABB_BOX(matWorld, vSize);
		break;
	case COLLIDER_TYPE::COLLIDER_OBB:
		hr = Ready_Collider_OBB_BOX(matWorld, vSize);
		break;
	case COLLIDER_TYPE::COLLIDER_SPHERE:
		hr = Ready_Collider_SPHERE(matWorld, vSize);
		break;
	}

	if (FAILED(Create_InputLayOut()))
		return E_FAIL;

	return S_OK;
}

void CCollider::Change_ColliderBoxSize(CTransform* pTransform, const _vec3 vSize)
{
	m_vSize = vSize;
	if (m_eType == COLLIDER_TYPE::COLLIDER_AABB)
	{
		_matrix pTarget_matrix = pTransform->Get_Matrix();
		_matrix matTemp = Remove_Rotation(pTarget_matrix);

		m_pTransformCom->Set_Matrix(matTemp);
		m_pTransformCom->Scaling(vSize);

		m_vMin = { -vSize.x / 2.f, 0.f, -vSize.z / 2.f };
		m_vMax = { vSize.x / 2.f, vSize.y, vSize.z / 2.f };
	}
	else if (m_eType == COLLIDER_TYPE::COLLIDER_OBB)
	{
		CTransform* pTarget_Transform = pTransform;
		_vec3		vDir[3];

		for (size_t i = 0; i < 3; ++i)
		{
			vDir[i] = *pTarget_Transform->Get_StateInfo(CTransform::STATE(i));
			vDir[i].Normalize();
		}


		vDir[CTransform::STATE_RIGHT] = vDir[CTransform::STATE_RIGHT] * vSize.x;
		vDir[CTransform::STATE_UP] = vDir[CTransform::STATE_UP] * vSize.y;
		vDir[CTransform::STATE_LOOK] = vDir[CTransform::STATE_LOOK] * vSize.z;


		m_pTransformCom->Set_StateInfo(CTransform::STATE_RIGHT, &vDir[CTransform::STATE_RIGHT]);
		m_pTransformCom->Set_StateInfo(CTransform::STATE_UP, &vDir[CTransform::STATE_UP]);
		m_pTransformCom->Set_StateInfo(CTransform::STATE_LOOK, &vDir[CTransform::STATE_LOOK]);


		m_vMin = { -vSize.x / 2.f, 0.f, -vSize.z / 2.f };
		m_vMax = { vSize.x / 2.f, vSize.y, vSize.z / 2.f };

		if (m_pOBB)
		{
			m_pOBB->vPoint[0] = _vec3(m_vMin.x, m_vMax.y, m_vMin.z);
			m_pOBB->vPoint[1] = _vec3(m_vMax.x, m_vMax.y, m_vMin.z);
			m_pOBB->vPoint[2] = _vec3(m_vMax.x, m_vMin.y, m_vMin.z);
			m_pOBB->vPoint[3] = _vec3(m_vMin.x, m_vMin.y, m_vMin.z);

			m_pOBB->vPoint[4] = _vec3(m_vMin.x, m_vMax.y, m_vMax.z);
			m_pOBB->vPoint[5] = _vec3(m_vMax.x, m_vMax.y, m_vMax.z);
			m_pOBB->vPoint[6] = _vec3(m_vMax.x, m_vMin.y, m_vMax.z);
			m_pOBB->vPoint[7] = _vec3(m_vMin.x, m_vMin.y, m_vMax.z);
		}
		
	}
}

_bool CCollider::Collision_AABB(CCollider* pTargetCollider)
{

	_matrix		matSour = Compute_WorldTransform();
	_matrix		matDest = pTargetCollider->Compute_WorldTransform();

	_vec3		vSourMin, vSourMax;
	_vec3		vDestMin, vDestMax;

	vSourMin = _vec3::Transform(m_vMin, matSour);
	vSourMax = _vec3::Transform(m_vMax, matSour);

	vDestMin = _vec3::Transform(pTargetCollider->m_vMin, matDest);
	vDestMax = _vec3::Transform(pTargetCollider->m_vMax, matDest);

	m_IsColl = false;

	if (max(vSourMin.x, vDestMin.x) < min(vSourMax.x, vDestMax.x)
		&& max(vSourMin.z, vDestMin.z) < min(vSourMax.z, vDestMax.z))
	{
		_uint i = 0; 
	}

	_float	fTemp_Max= max(vSourMin.x, vDestMin.x);
	_float	fTemp_Min= min(vSourMax.x, vDestMax.x);
	if (fTemp_Max < fTemp_Min)
		return false;

	fTemp_Max = max(vSourMin.z, vDestMin.z);
	fTemp_Min = min(vSourMax.z, vDestMax.z);
	if (fTemp_Max < fTemp_Min)
		return false;

	m_IsColl = true;


	return _bool(m_IsColl = true);
}

void CCollider::Collision_AABB(CCollider* pTargetCollider, CTransform* pSourTransform, CTransform* pDestTransform)
{
	_matrix		matSour = Compute_WorldTransform();
	_matrix		matDest = pTargetCollider->Compute_WorldTransform();

	_vec3		vSourMin, vSourMax;
	_vec3		vDestMin, vDestMax;

	XMMATRIX	xmMatSour = XMLoadFloat4x4(&matSour);
	XMMATRIX	xmMatDest = XMLoadFloat4x4(&matDest);
	//XMLoadFloat4x4
	vSourMin = _vec3::Transform(m_vMin, xmMatSour);
	vSourMax = _vec3::Transform(m_vMax, xmMatSour);

	vDestMin = _vec3::Transform(pTargetCollider->m_vMin, xmMatDest);
	vDestMax = _vec3::Transform(pTargetCollider->m_vMax, xmMatDest);

	m_IsColl = false;
	if (max(vSourMin.x, vDestMin.x) < min(vSourMax.x, vDestMax.x) && max(vSourMin.z, vDestMin.z) < min(vSourMax.z, vDestMax.z))
	{
		m_IsColl = true;
		_float	fMoveX = (min(vSourMax.x, vDestMax.x) - max(vSourMin.x, vDestMin.x));
		_float	fMoveZ = (min(vSourMax.z, vDestMax.z) - max(vSourMin.z, vDestMin.z));

		_vec3	vTemp = {};
		if (fMoveX > fMoveZ)
		{
			if (fMoveZ <= 0.5f)
			{
				//fMoveZ *= 100.f;
				vTemp = { pDestTransform->Get_Matrix()._41,
						pDestTransform->Get_Matrix()._42,
						pDestTransform->Get_Matrix()._43 + fMoveZ };
				pDestTransform->Set_StateInfo(CTransform::STATE_POSITION,&vTemp);
			}
			else
			{
				//fMoveZ *= 100.f;
				vTemp = { pDestTransform->Get_Matrix()._41,
					pDestTransform->Get_Matrix()._42,
					pDestTransform->Get_Matrix()._43 - fMoveZ };
				pDestTransform->Set_StateInfo(CTransform::STATE_POSITION,&vTemp);
			}

		}
		else
		{
			if (fMoveX <= 0.5f)
			{
				//fMoveX *= 100.f;
				vTemp = { pDestTransform->Get_Matrix()._41 + fMoveX,
						pDestTransform->Get_Matrix()._42,
						pDestTransform->Get_Matrix()._43 };
				pDestTransform->Set_StateInfo(CTransform::STATE_POSITION, &vTemp);
			}
			else
			{
				//fMoveX *= 100.f;
				vTemp = { pDestTransform->Get_Matrix()._41 - fMoveX,
					pDestTransform->Get_Matrix()._42,
					pDestTransform->Get_Matrix()._43 };
				pDestTransform->Set_StateInfo(CTransform::STATE_POSITION, &vTemp);
			}

		}
	}


}

_bool CCollider::Collision_OBB(CCollider* pTargetCollider)
{
	OBB*	pTargetOBB = pTargetCollider->m_pOBB;
	if (nullptr == pTargetOBB)
		return false;


	m_IsColl = false;

	OBB			tOBB[2];
	ZeroMemory(tOBB, sizeof(OBB) * 2);

	for (size_t i = 0; i < 8; ++i)
	{
		_matrix		matWorld = Compute_WorldTransform();
		_matrix		matTargetWorld = pTargetCollider->Compute_WorldTransform();

		tOBB[0].vPoint[i] = _vec3::Transform(m_pOBB->vPoint[i], matWorld);
		tOBB[1].vPoint[i] = _vec3::Transform(pTargetOBB->vPoint[i], matTargetWorld);
			
	}

	tOBB[0].vCenter = (tOBB[0].vPoint[0] + tOBB[0].vPoint[6]) * 0.5f;
	tOBB[1].vCenter = (tOBB[1].vPoint[0] + tOBB[1].vPoint[6]) * 0.5f;

	for (size_t i = 0; i < 2; ++i)
	{
		Compute_AlignAxis(&tOBB[i]);
		Compute_ProjAxis(&tOBB[i]);
	}	

	_vec3	vAlignAxis[9];

	for (size_t i = 0; i < 3; ++i)
	{
		for (size_t j = 0; j < 3; ++j)
		{
			_uint		iIndex = i * 3 + j;
			vAlignAxis[iIndex] = tOBB[0].vAlignAxis[i].Cross(tOBB[1].vAlignAxis[j]);

		}
	}


	_float		fDistance[3] = {};
	for (size_t i = 0; i < 2; ++i)
	{
		for (size_t j = 0; j < 3; ++j)
		{
			_vec3		vCenterDir = tOBB[1].vCenter - tOBB[0].vCenter;

			fDistance[0] = fabs(vCenterDir.Dot(tOBB[i].vAlignAxis[j]));
			

			fDistance[1] =
				fabs(tOBB[0].vProjAxis[0].Dot(tOBB[i].vAlignAxis[j])) +
				fabs(tOBB[0].vProjAxis[1].Dot(tOBB[i].vAlignAxis[j])) +
				fabs(tOBB[0].vProjAxis[2].Dot(tOBB[i].vAlignAxis[j]));


			fDistance[2] =
				fabs(tOBB[1].vProjAxis[0].Dot(tOBB[i].vAlignAxis[j])) +
				fabs(tOBB[1].vProjAxis[1].Dot(tOBB[i].vAlignAxis[j])) +
				fabs(tOBB[1].vProjAxis[2].Dot(tOBB[i].vAlignAxis[j]));


			if (fDistance[1] + fDistance[2] < fDistance[0])
				return false;

		}
	}

	for (size_t i = 0; i < 9; ++i)
	{
		_vec3		vCenterDir = tOBB[1].vCenter - tOBB[0].vCenter;
		fDistance[0] = fabs(vCenterDir.Dot(vAlignAxis[i]));

		fDistance[1] =
			fabs(tOBB[0].vProjAxis[0].Dot(vAlignAxis[i])) +
			fabs(tOBB[0].vProjAxis[1].Dot(vAlignAxis[i])) +
			fabs(tOBB[0].vProjAxis[2].Dot(vAlignAxis[i]));

		fDistance[2] =
			fabs(tOBB[1].vProjAxis[0].Dot(vAlignAxis[i])) +
			fabs(tOBB[1].vProjAxis[1].Dot(vAlignAxis[i])) +
			fabs(tOBB[1].vProjAxis[2].Dot(vAlignAxis[i]));

		if (fDistance[1] + fDistance[2] < fDistance[0])
			return false;
	}


	return _bool(m_IsColl = true);
}

void CCollider::Update_Collider(CTransform* pTransform, CLASS eCurClass)
{
	_matrix pTarget_matrix = pTransform->Get_Matrix();
	m_matWorld = pTransform->Get_Matrix();
	//m_matWorld.m[3][1] += 10.f;
	switch (m_eType)
	{
	case COLLIDER_TYPE::COLLIDER_AABB:
	{
		_matrix matTemp = Remove_Rotation(pTarget_matrix);
		matTemp.m[0][0] *= m_vSize.x;
		matTemp.m[1][1] *= m_vSize.y;
		matTemp.m[2][2] *= m_vSize.z;
		matTemp.m[3][1] += 3.f;
		m_pTransformCom->Set_Matrix(matTemp);
	}
	break;
	case COLLIDER_TYPE::COLLIDER_OBB:
	{

		_matrix matTemp_Rotate = pTarget_matrix;
		_matrix matTemp;
		matTemp.m[0][0] *= m_vSize.x;
		matTemp.m[1][1] *= m_vSize.y;
		matTemp.m[2][2] *= m_vSize.z;

		matTemp = matTemp * matTemp_Rotate;
		if (eCurClass == CLASS::CLASS_MMAGE || eCurClass == CLASS::CLASS_CAVALRY)
		{
			matTemp.m[3][1] += 5.f;
		}
		else
		{
			matTemp.m[3][1] += 3.f;
		}
		m_pTransformCom->Set_Matrix(matTemp);
	}
	break;
	case COLLIDER_TYPE::COLLIDER_SPHERE:
	{
		_matrix matTemp_Rotate = pTarget_matrix;
		_matrix matTemp;
		matTemp.m[0][0] *= m_fRadius;
		matTemp.m[1][1] *= m_fRadius;
		matTemp.m[2][2] *= m_fRadius;

		matTemp = matTemp * matTemp_Rotate;
		m_pTransformCom->Set_Matrix(matTemp);
	}
	break;
	default:
		break;
	}


}

void CCollider::Update_Collider_Ex(CTransform* pTransform)
{
	_matrix pTarget_matrix = pTransform->Get_Matrix();
	m_matWorld = pTransform->Get_Matrix();
	m_matWorld.m[3][1] += 10.f;
	switch (m_eType)
	{
	case COLLIDER_TYPE::COLLIDER_AABB:
	{
		_matrix matTemp = Remove_Rotation(pTarget_matrix);
		matTemp.m[0][0] *= m_vSize.x;
		matTemp.m[1][1] *= m_vSize.y;
		matTemp.m[2][2] *= m_vSize.z;
		matTemp.m[3][1] += 3.f;
		m_pTransformCom->Set_Matrix(matTemp);
	}
	break;
	case COLLIDER_TYPE::COLLIDER_OBB:
	{

		_matrix matTemp_Rotate = pTarget_matrix;
		_matrix matTemp;
		matTemp.m[0][0] *= m_vSize.x;
		matTemp.m[1][1] *= m_vSize.y;
		matTemp.m[2][2] *= m_vSize.z;

		matTemp = matTemp * matTemp_Rotate;
		m_pTransformCom->Set_Matrix(matTemp);
	}
	break;
	case COLLIDER_TYPE::COLLIDER_SPHERE:
	{
		_matrix matTemp_Rotate = pTarget_matrix;
		_matrix matTemp;
		matTemp.m[0][0] *= m_fRadius;
		matTemp.m[1][1] *= m_fRadius;
		matTemp.m[2][2] *= m_fRadius;

		matTemp = matTemp * matTemp_Rotate;
		m_pTransformCom->Set_Matrix(matTemp);
	}
	break;
	default:
		break;
	}
}

_matrix CCollider::Compute_WorldTransform()
{
	_matrix matTransform = m_matWorld;

	if (m_eType == COLLIDER_TYPE::COLLIDER_AABB)
	{
		matTransform = Remove_Rotation(matTransform);	
	}
	return _matrix(matTransform);
}

void CCollider::Compute_AlignAxis(OBB* pOBB)
{
	pOBB->vAlignAxis[0] = (pOBB->vPoint[2] - pOBB->vPoint[3]);
	pOBB->vAlignAxis[1] = (pOBB->vPoint[0] - pOBB->vPoint[3]);
	pOBB->vAlignAxis[2] = (pOBB->vPoint[7] - pOBB->vPoint[3]);

	for (size_t i = 0; i < 3; ++i)
	{
		pOBB->vAlignAxis[i].Normalize();
	}
}

void CCollider::Compute_ProjAxis(OBB* pOBB)
{
	_vec3 vAdd[3];
	vAdd[0] = pOBB->vPoint[5]+ pOBB->vPoint[2];
	vAdd[1] = pOBB->vPoint[5]+ pOBB->vPoint[0];
	vAdd[2] = pOBB->vPoint[5]+ pOBB->vPoint[7];

	_vec3 vProd[3];
	vProd[0] = vAdd[0]* 0.5f;
	vProd[1] = vAdd[1]* 0.5f;
	vProd[2] = vAdd[2]* 0.5f;

	pOBB->vProjAxis[0] = (vProd[0] -  pOBB->vCenter);
	pOBB->vProjAxis[1] = (vProd[1] -  pOBB->vCenter);
	pOBB->vProjAxis[2] = (vProd[2] -  pOBB->vCenter);
}

_matrix CCollider::Remove_Rotation(_matrix matWorld)
{
	_vec3 vRight = _vec3(1.f, 0.f, 0.f);
	_vec3 vUp = _vec3(0.f, 1.f, 0.f);
	_vec3 vLook = _vec3(0.f, 0.f, 1.f);

	_vec3 vRightTemp = _vec3(matWorld.m[0][0], matWorld.m[0][1], matWorld.m[0][2]);
	_vec3 vUpTemp = _vec3(matWorld.m[1][0], matWorld.m[1][1], matWorld.m[1][2]);
	_vec3 vLookTemp = _vec3(matWorld.m[2][0], matWorld.m[2][1], matWorld.m[2][2]);

	vRight *= vRightTemp.Length();
	vUp *= vUpTemp.Length();
	vLook *= vLookTemp.Length();


	memcpy(&matWorld.m[0][0], &vRight, sizeof(_vec3));
	memcpy(&matWorld.m[1][0], &vUp, sizeof(_vec3));
	memcpy(&matWorld.m[2][0], &vLook, sizeof(_vec3));

	return _matrix(matWorld);
}


HRESULT CCollider::Create_InputLayOut()
{
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

	if (FAILED(m_pShaderCom->Create_Shader(vecDesc, RS_TYPE::WIREFRAME, DEPTH_STENCIL_TYPE::LESS, SHADER_TYPE::SHADER_DEFFERED)))
		return E_FAIL;


	return S_OK;
}

void CCollider::Render_Collider(CStructedBuffer* pArg)
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

	REP tRep = {};
	if (pArg)
	{
		tRep.m_arrInt[1] = 27;
		tRep.m_arrInt[0] = 1;
		pArg->Update_Data(TEXTURE_REGISTER::t8);
	}
	else
	{
		tRep.m_arrInt[0] = 0;
	}

	_uint iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(),
		iOffeset, CONST_REGISTER::b0);

	iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->SetData((void*)&tRep);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->GetCBV().Get(),
		iOffeset, CONST_REGISTER::b8);


	CDevice::GetInstance()->UpdateTable();


	m_pBufferCom->Render_VIBuffer();
	Safe_Release(pManagement);
}

CCollider* CCollider::Create(COLLIDER_TYPE eType)
{
	CCollider* pInstance = new CCollider();
	if (FAILED(pInstance->Ready_Collider(eType)))
	{
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CCollider::Clone_Component(void* pArg)
{
	return new CCollider(*this);
}

void CCollider::Free()
{
	if (m_IsClone)
	{
		Safe_Delete(m_pOBB);
	}

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTransformCom);
	CComponent::Free();
}
