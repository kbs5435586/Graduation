#include "framework.h"
#include "GameObject.h"
#include "Component.h"

CGameObject::CGameObject(ID3D12Device* pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device)
{
	m_pGraphic_Device->AddRef();
}

CGameObject::CGameObject(const CGameObject& rhs)
	: m_pGraphic_Device(rhs.m_pGraphic_Device)
{
	m_pGraphic_Device->AddRef();
}

CComponent* CGameObject::Get_ComponentPointer(const _tchar* pComponentTag)
{
	CComponent* pComponent = Find_Component(pComponentTag);
	if (nullptr == pComponent)
		return nullptr;

	return pComponent;
}

HRESULT CGameObject::Ready_GameObject(void* pArg)
{

	return S_OK;
}

_int CGameObject::Update_GameObject(const _float& fTimeDelta)
{
	return _int();
}

_int CGameObject::LastUpdate_GameObject(const _float& fTimeDelta)
{
	return _int();
}

void CGameObject::Render_GameObject()
{
}

HRESULT CGameObject::CreatePipeLine(CShader* pShader)
{
	return E_NOTIMPL;
}

D3D12_RASTERIZER_DESC CGameObject::CreateRaterizerState()
{
	return D3D12_RASTERIZER_DESC();
}

D3D12_DEPTH_STENCIL_DESC CGameObject::CreateDepthStencilState()
{
	return D3D12_DEPTH_STENCIL_DESC();
}

D3D12_BLEND_DESC CGameObject::CreateBlendState()
{
	return D3D12_BLEND_DESC();
}

D3D12_INPUT_LAYOUT_DESC CGameObject::CreateInputLayout()
{
	return D3D12_INPUT_LAYOUT_DESC();
}

HRESULT CGameObject::Add_Component(const _tchar* pComponentTag, CComponent* pComponent)
{
	if (nullptr == pComponent)
		return E_FAIL;

	if (nullptr != Find_Component(pComponentTag))
		return E_FAIL;

	m_mapComponent.insert(MAPCOMPONENT::value_type(pComponentTag, pComponent));

	pComponent->AddRef();

	return S_OK;
}

CComponent* CGameObject::Find_Component(const _tchar* pComponentTag)
{
	auto iter = find_if(m_mapComponent.begin(), m_mapComponent.end(), CFinder_Tag(pComponentTag));

	if (iter == m_mapComponent.end())
		return nullptr;

	return iter->second;
}

CGameObject* CGameObject::Clone_GameObject(void* pArg)
{
	return nullptr;
}

void CGameObject::Free()
{
	for (auto& Pair : m_mapComponent)
		Safe_Release(Pair.second);
	m_mapComponent.clear();

	if (m_pPipeLine)
		m_pPipeLine->Release();
	Safe_Release(m_pGraphic_Device);
}
