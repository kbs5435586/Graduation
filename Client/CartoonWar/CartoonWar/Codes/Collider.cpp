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
	, m_pShaderCom(rhs.m_pShaderCom)
	, m_pTransformCom(rhs.m_pTransformCom)
	, m_pBufferCom(rhs.m_pBufferCom)
	, m_eType(rhs.m_eType)
	, m_vSize(rhs.m_vSize)
	, m_fRadius(rhs.m_fRadius)
{
	m_pShaderCom->AddRef();
	m_pBufferCom->AddRef();
	m_pTransformCom->AddRef();
}

HRESULT CCollider::Ready_Collider(COLLIDER_TYPE eType)
{
	m_eType = eType;
	switch (m_eType)
	{
	case COLLIDER_TYPE::COLLIDER_AABB:
		m_pBufferCom = CBuffer_CubeCol::Create();
		if (nullptr == m_pBufferCom)
			break;
		break;
	case COLLIDER_TYPE::COLLIDER_OBB:

		m_pBufferCom = CBuffer_CubeCol::Create();
		if (nullptr == m_pBufferCom)
			break;
		break;
	case COLLIDER_TYPE::COLLIDER_SPHERE:
		m_pBufferCom = CBuffer_Sphere::Create();
		if (nullptr == m_pBufferCom)
			break;
		break;
	}

	m_pShaderCom = CShader::Create(L"../ShaderFiles/Shader_Collider.hlsl", "VS_Main", "PS_Main");
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	m_pTransformCom = CTransform::Create();
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	return S_OK;
}

HRESULT CCollider::Ready_Collider_AABB_BOX(CTransform* pTransform, const _vec3 vSize)
{
	// 대상으로 한 객체의 회전 값 받아와서 회전 죽이기

	_matrix pTarget_matrix = pTransform->Get_Matrix();
	_matrix matTemp = Remove_Rotation(pTarget_matrix);

	m_pTransformCom->Set_Matrix(matTemp);
	m_pTransformCom->Scaling(vSize);


	return S_OK;
}

HRESULT CCollider::Ready_Collider_OBB_BOX(CTransform* pTransform, const _vec3 vSize)
{
	// 대상으로 한 객체의 회전 값 받아와서 Collider Transform에 적용하기
	CTransform* pTarget_Transform = pTransform;
	pTarget_Transform->Scaling(vSize);

	m_pTransformCom->Set_Matrix(pTarget_Transform->Get_Matrix());
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
	m_vSize = vSize;
	HRESULT hr = S_OK;
	switch (m_eType)
	{
	case COLLIDER_TYPE::COLLIDER_AABB:
		hr = Ready_Collider_AABB_BOX(pTransform, vSize);
		break;
	case COLLIDER_TYPE::COLLIDER_OBB:
		hr = Ready_Collider_OBB_BOX(pTransform, vSize);
		break;
	case COLLIDER_TYPE::COLLIDER_SPHERE:
		hr = Ready_Collider_SPHERE(pTransform, vSize);
		break;
	}

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

void CCollider::Update_Collider(CTransform* pTransform)
{
	_matrix pTarget_matrix = pTransform->Get_Matrix();

	switch (m_eType)
	{
	case COLLIDER_TYPE::COLLIDER_AABB:
	{
		_matrix matTemp = Remove_Rotation(pTarget_matrix);
		matTemp.m[0][0] *= m_vSize.x;
		matTemp.m[1][1] *= m_vSize.y;
		matTemp.m[2][2] *= m_vSize.z;
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
		m_pTransformCom->Set_Matrix(matTemp_Rotate);
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

_matrix CCollider::Remove_Rotation(_matrix matWorld)
{
	_vec3 vRight = _vec3(1.f, 0.f, 0.f);
	_vec3 vUp = _vec3(0.f, 1.f, 0.f);
	_vec3 vLook = _vec3(0.f, 0.f, 1.f);

	_vec3 vRightTemp = _vec3(matWorld.m[0][0], matWorld.m[0][1], matWorld.m[0][2]);
	_vec3 vUpTemp = _vec3(matWorld.m[1][0], matWorld.m[1][1], matWorld.m[1][2]);
	_vec3 vLookTemp = _vec3(matWorld.m[2][0], matWorld.m[2][1], matWorld.m[2][2]);

	vRight *= Vector3_::Length(vRightTemp);
	vUp *= Vector3_::Length(vUpTemp);
	vLook *= Vector3_::Length(vLookTemp);


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

	tRep.m_arrInt[0] = 7;
	_uint iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(),
		iOffeset, CONST_REGISTER::b0);

	iOffeset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->SetData((void*)&tRep);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->GetCBV().Get(), 
		iOffeset, CONST_REGISTER::b8);

	pArg->Update_Data(TEXTURE_REGISTER::t8);
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
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTransformCom);
	CComponent::Free();
}
