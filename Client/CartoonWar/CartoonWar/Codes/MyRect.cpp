#include "framework.h"
#include "MyRect.h"
#include "Management.h"

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
	m_iPassSize = CalcConstantBufferByteSize(sizeof(MAINPASS));
	if (FAILED(Ready_Component()))
		return E_FAIL;
	if (FAILED(CreateConstantBuffer()))
		return E_FAIL;
	if (FAILED(CreateInputLayout()))
		return E_FAIL;


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

	return _int();
}

void CMyRect::Render_GameObject()
{
	MAINPASS tMainPass = {};
	_matrix matWorld = m_pTransformCom->Get_Matrix();
	_matrix matView = CCamera_Manager::GetInstance()->GetMatView();
	_matrix matProj = CCamera_Manager::GetInstance()->GetMatProj();

	m_pShaderCom->SetUp_OnShader( matWorld, matView, matProj, tMainPass);
	memcpy_s(m_pData, m_iPassSize, (void*)&tMainPass, sizeof(tMainPass));
	CDevice::GetInstance()->GetCmdLst()->SetGraphicsRootConstantBufferView(1, m_pConstBuffer->GetGPUVirtualAddress());

	m_pBufferCom->Render_VIBuffer();
}

HRESULT CMyRect::CreateInputLayout()
{
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "COLOR", 0, DXGI_FORMAT_R16G16B16A16_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	
	if (FAILED(m_pShaderCom->Create_Shader(vecDesc, RS_TYPE::WIREFRAME)))
		return E_FAIL;


	return S_OK;
}

HRESULT CMyRect::CreateConstantBuffer()
{
	D3D12_HEAP_PROPERTIES	tHeap_Pro_Upload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(m_iPassSize);

	if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(
		&tHeap_Pro_Upload,
		D3D12_HEAP_FLAG_NONE,
		&tResource_Desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_pConstBuffer))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pConstBuffer->Map(0, nullptr, &m_pData)))
		return E_FAIL;


	D3D12_GPU_VIRTUAL_ADDRESS ConstantBufferAddress = m_pConstBuffer->GetGPUVirtualAddress();

	int Idx = 0;
	ConstantBufferAddress += (Idx * m_iPassSize);

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = ConstantBufferAddress;
	cbvDesc.SizeInBytes = CalcConstantBufferByteSize(sizeof(MAINPASS));

	CDevice::GetInstance()->GetDevice()->CreateConstantBufferView(
		&cbvDesc,
		CDevice::GetInstance()->GetConstantBufferDescHeap()->GetCPUDescriptorHandleForHeapStart());


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

CGameObject* CMyRect::Clone_GameObject(void* pArg)
{
	CMyRect* pInstance = new CMyRect(*this);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CMyRect Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMyRect::Free()
{

	Safe_Release(m_pBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);

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

	m_pBufferCom = (CBuffer_RectCol*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Buffer_RectCol");
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
