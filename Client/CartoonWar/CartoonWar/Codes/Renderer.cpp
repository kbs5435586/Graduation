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


	_uint iSwapChainIdx = CDevice::GetInstance()->GetSwapChainIdx();
	pManagement->Get_RTT((_uint)MRT::MRT_SWAPCHAIN)->Clear(iSwapChainIdx);
	pManagement->Get_RTT((_uint)MRT::MRT_DEFFERD)->Clear();
 	pManagement->Get_RTT((_uint)MRT::MRT_LIGHT)->Clear();
 	pManagement->Get_RTT((_uint)MRT::MRT_SHADOW)->Clear();
	pManagement->Get_RTT((_uint)MRT::MRT_BLUR)->Clear();
	//pManagement->Get_RTT((_uint)MRT::MRT_OUTLINE)->Clear();

	pManagement->Get_RTT((_uint)MRT::MRT_INVEN)->Clear();
	//pManagement->Get_RTT((_uint)MRT::MRT_MAP)->Clear();
	
	Render_Inventory(pManagement);
	//Render_Deffered_Map(pManagement);

	Render_Shadow(pManagement);
	Render_Blur();
	Render_Deffered(pManagement);
	Render_Light(pManagement);

	iSwapChainIdx = CDevice::GetInstance()->GetSwapChainIdx();
	pManagement->Get_RTT((_uint)MRT::MRT_SWAPCHAIN)->OM_Set(1, iSwapChainIdx);

	Render_Blend();
	Render_Priority();
	Render_OutLine();
	Render_Alpha();
	Render_Post_Effect();
	pManagement->Render_Font();
	Render_UI();
	Render_UI_Back();


	Safe_Release(pManagement);
	return S_OK;
}

void CRenderer::CopySwapToPosteffect()
{
	static ComPtr<ID3D12Resource>	pPostEffectTex = CManagement::GetInstance()->GetPostEffectTex()->GetTex2D().Get();
	_uint iIdx = CDevice::GetInstance()->GetSwapChainIdx();

	CD3DX12_RESOURCE_BARRIER temp = CD3DX12_RESOURCE_BARRIER::Transition(CManagement::GetInstance()->Get_RTT((_uint)MRT::MRT_SWAPCHAIN)->Get_RTT(iIdx)->pRtt->GetTex2D().Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_SOURCE);

	CDevice::GetInstance()->GetCmdLst()->ResourceBarrier(1, &temp);

	CDevice::GetInstance()->GetCmdLst()->CopyResource(pPostEffectTex.Get(), CManagement::GetInstance()->Get_RTT((_uint)MRT::MRT_SWAPCHAIN)->Get_RTT(iIdx)->pRtt->GetTex2D().Get());

	temp = CD3DX12_RESOURCE_BARRIER::Transition(CManagement::GetInstance()->Get_RTT((_uint)MRT::MRT_SWAPCHAIN)->Get_RTT(iIdx)->pRtt->GetTex2D().Get(),
		D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	CDevice::GetInstance()->GetCmdLst()->ResourceBarrier(1, &temp);
}

void CRenderer::CopySwapToBlur()
{
	static ComPtr<ID3D12Resource>	pBlurTex = CManagement::GetInstance()->Get_RTT((_uint)MRT::MRT_BLUR)->Get_RTT(0)->pRtt->GetTex2D().Get();
	_uint iIdx = CDevice::GetInstance()->GetSwapChainIdx();

	CD3DX12_RESOURCE_BARRIER temp = CD3DX12_RESOURCE_BARRIER::Transition(CManagement::GetInstance()->Get_RTT((_uint)MRT::MRT_SWAPCHAIN)->Get_RTT(iIdx)->pRtt->GetTex2D().Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_SOURCE);

	CDevice::GetInstance()->GetCmdLst()->ResourceBarrier(1, &temp);

	CDevice::GetInstance()->GetCmdLst()->CopyResource(pBlurTex.Get(), CManagement::GetInstance()->Get_RTT((_uint)MRT::MRT_SWAPCHAIN)->Get_RTT(iIdx)->pRtt->GetTex2D().Get());

	temp = CD3DX12_RESOURCE_BARRIER::Transition(CManagement::GetInstance()->Get_RTT((_uint)MRT::MRT_SWAPCHAIN)->Get_RTT(iIdx)->pRtt->GetTex2D().Get(),
		D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	CDevice::GetInstance()->GetCmdLst()->ResourceBarrier(1, &temp);

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

void CRenderer::Render_NoneAlpha_PRO()
{
	for (auto& pGameObject : m_RenderList[RENDER_NONEALPHA_PRO])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render_GameObject();
			Safe_Release(pGameObject);
		}
	}
	m_RenderList[RENDER_NONEALPHA_PRO].clear();
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

void CRenderer::Render_UI_Back()
{
	//RENDER_UI_BACK
	for (auto& pGameObject : m_RenderList[RENDER_UI_BACK])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render_GameObject();
			Safe_Release(pGameObject);
		}
	}
	m_RenderList[RENDER_UI_BACK].clear();
}

void CRenderer::Render_UI_Map()
{
	for (auto& pGameObject : m_RenderList[RENDER_UI_MAP])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render_GameObject();
			Safe_Release(pGameObject);
		}
	}
	m_RenderList[RENDER_UI_MAP].clear();
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

void CRenderer::Render_Post_Effect()
{
	CopySwapToPosteffect();
	for (auto& pGameObject : m_RenderList[RENDER_POST])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render_PostEffect();
			Safe_Release(pGameObject);
		}
	}
	m_RenderList[RENDER_POST].clear();
}

void CRenderer::Render_Blur()
{
	CManagement::GetInstance()->Get_RTT((_uint)MRT::MRT_BLUR)->OM_Set();
	for (auto& pGameObject : m_RenderList[RENDER_BLUR])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render_Blur();
			Safe_Release(pGameObject);
		}
	}
	m_RenderList[RENDER_BLUR].clear();

	CManagement::GetInstance()->Get_RTT((_uint)MRT::MRT_BLUR)->TargetToResBarrier();

}

void CRenderer::Render_OutLine()
{
	//CManagement::GetInstance()->Get_RTT((_uint)MRT::MRT_OUTLINE)->OM_Set();
	for (auto& pGameObject : m_RenderList[RENDER_OUTLINE])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render_OutLine();
			Safe_Release(pGameObject);
		}
	}
	m_RenderList[RENDER_OUTLINE].clear();

	//CManagement::GetInstance()->Get_RTT((_uint)MRT::MRT_OUTLINE)->TargetToResBarrier();

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
	//Render_NoneAlpha_PRO();
	Render_NoneAlpha();
	
	pManagement->Get_RTT((_uint)MRT::MRT_DEFFERD)->TargetToResBarrier();
}



void CRenderer::Render_Light(CManagement* pManagement)
{
	pManagement->Get_RTT((_uint)MRT::MRT_LIGHT)->OM_Set();

	pManagement->Update();
	pManagement->Render();

	pManagement->Get_RTT((_uint)MRT::MRT_LIGHT)->TargetToResBarrier();

}

void CRenderer::Render_Inventory(CManagement* pManagement)
{
	pManagement->Get_RTT((_uint)MRT::MRT_INVEN)->OM_Set();
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


CRenderer* CRenderer::Create()
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
