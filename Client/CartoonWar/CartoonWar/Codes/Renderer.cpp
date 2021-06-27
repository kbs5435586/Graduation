#include "framework.h"
#include "Renderer.h"
#include "GameObject.h"
#include "Management.h"
#include "MRT.h"

CRenderer::CRenderer()
{

}

HRESULT CRenderer::Ready_Renderer()
{

	
	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eGroup, CGameObject* pGameObject)
{
	if (RENDER_END <= eGroup)
		return E_FAIL;

	if (nullptr == pGameObject)
		return E_FAIL;

	m_RenderList[eGroup].push_back(pGameObject);

	pGameObject->AddRef();

	return S_OK;
}

HRESULT CRenderer::Render_RenderGroup()//106 104
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return E_FAIL;
	pManagement->AddRef();
	//pManagement->Update();



	_uint iSwapChainIdx = CDevice::GetInstance()->GetSwapChainIdx();
	pManagement->Get_RTT((_uint)MRT::MRT_SWAPCHAIN)->Clear(iSwapChainIdx);
	pManagement->Get_RTT((_uint)MRT::MRT_DEFFERD)->Clear();
 	pManagement->Get_RTT((_uint)MRT::MRT_LIGHT)->Clear();
 	pManagement->Get_RTT((_uint)MRT::MRT_SHADOW)->Clear();
	pManagement->Get_RTT((_uint)MRT::MRT_INVEN)->Clear(true);
	Render_Inventory(pManagement);

	Render_Shadow(pManagement);
	Render_Deffered(pManagement);
	Render_Light(pManagement);
	

	iSwapChainIdx = CDevice::GetInstance()->GetSwapChainIdx();
	pManagement->Get_RTT((_uint)MRT::MRT_SWAPCHAIN)->OM_Set(1, iSwapChainIdx);

	Render_Blend();
	
	
	Render_Priority();
	Render_Alpha();
	Render_UI();

	
	Safe_Release(pManagement);
	return S_OK;
}

void CRenderer::Render_Priority()
{
	for (auto& pGameObject : m_RenderList[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render_GameObject();
			Safe_Release(pGameObject);
		}
	}
	m_RenderList[RENDER_PRIORITY].clear();
}

void CRenderer::Render_NoneAlpha()
{
	for (auto& pGameObject : m_RenderList[RENDER_NONEALPHA])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render_GameObject();
			Safe_Release(pGameObject);
		}
	}
	m_RenderList[RENDER_NONEALPHA].clear();
}

void CRenderer::Render_Alpha()
{
	for (auto& pGameObject : m_RenderList[RENDER_ALPHA])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render_GameObject();
			Safe_Release(pGameObject);
		}
	}
	m_RenderList[RENDER_ALPHA].clear();
}

void CRenderer::Render_UI()
{
	for (auto& pGameObject : m_RenderList[RENDER_UI])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render_GameObject();
			Safe_Release(pGameObject);
		}
	}
	m_RenderList[RENDER_UI].clear();
}

void CRenderer::Render_Blend()
{
	for (auto& pGameObject : m_RenderList[RENDER_BLEND])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render_GameObject();
			Safe_Release(pGameObject);
		}
	}
	m_RenderList[RENDER_BLEND].clear();
}

void CRenderer::Render_Shadow(CManagement* pManagement)
{
	pManagement->Get_RTT((_uint)MRT::MRT_SHADOW)->OM_Set();

	for (auto& pGameObject : m_RenderList[RENDER_SHADOW])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render_GameObject_Shadow();
			Safe_Release(pGameObject);
		}
	}
	m_RenderList[RENDER_SHADOW].clear();
	pManagement->Get_RTT((_uint)MRT::MRT_SHADOW)->TargetToResBarrier();
}

void CRenderer::Render_Deffered(CManagement* pManagement)
{
	pManagement->Get_RTT((_uint)MRT::MRT_DEFFERD)->OM_Set();

	Render_NoneAlpha();

	pManagement->Get_RTT((_uint)MRT::MRT_DEFFERD)->TargetToResBarrier();
}

void CRenderer::Render_Light(CManagement* pManagement)
{
	pManagement->Get_RTT((_uint)MRT::MRT_LIGHT)->OM_Set();

	pManagement->Render();

	pManagement->Get_RTT((_uint)MRT::MRT_LIGHT)->TargetToResBarrier();

}

void CRenderer::Render_Inventory(CManagement* pManagement)
{
	pManagement->Get_RTT((_uint)MRT::MRT_INVEN)->OM_Set(true);
	for (auto& pGameObject : m_RenderList[RENDER_INVEN])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render_GameObject();
			Safe_Release(pGameObject);
		}
	}
	m_RenderList[RENDER_INVEN].clear();

	pManagement->Get_RTT((_uint)MRT::MRT_INVEN)->TargetToResBarrier();
}


CRenderer* CRenderer::Create( )
{
	CRenderer* pInstance = new CRenderer();

	if (FAILED(pInstance->Ready_Renderer()))
	{
		MessageBox(0, L"CRenderer Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CRenderer::Clone_Component(void* pArg)
{
	AddRef();

	return this;
}

void CRenderer::Free()
{
	CComponent::Free();
}
