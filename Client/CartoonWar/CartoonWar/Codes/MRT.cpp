#include "framework.h"
#include "MRT.h"

CMRT::CMRT()
{
}

HRESULT CMRT::Ready_MRT(_uint iCnt, tRtt* arrRT, CRTT* pDsTex)
{
	assert(nullptr != pDsTex);

	m_iRTCnt = iCnt;
	memcpy(m_tArr, arrRT, sizeof(tRtt) * iCnt);
	m_pDsTex = pDsTex;

	D3D12_DESCRIPTOR_HEAP_DESC tDesc = {};
	tDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	tDesc.NumDescriptors = m_iRTCnt;
	tDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	tDesc.NodeMask = 0;
	CDevice::GetInstance()->GetDevice()->CreateDescriptorHeap(&tDesc, IID_PPV_ARGS(&m_pRTV));
	D3D12_CPU_DESCRIPTOR_HANDLE hRTVHeap = m_pRTV->GetCPUDescriptorHandleForHeapStart();

	UINT iRTVSize = CDevice::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	for (UINT i = 0; i < m_iRTCnt; ++i)
	{
		UINT iDestRange = 1;
		UINT iSrcRange = 1;

		D3D12_CPU_DESCRIPTOR_HANDLE hDescHandle = m_pRTV->GetCPUDescriptorHandleForHeapStart();
		hDescHandle.ptr += (iRTVSize * i);

		ComPtr<ID3D12DescriptorHeap> pRTVHeap = m_tArr[i].pRtt->GetRTV();
		D3D12_CPU_DESCRIPTOR_HANDLE hSrcHandle = pRTVHeap->GetCPUDescriptorHandleForHeapStart();

		CDevice::GetInstance()->GetDevice()->CopyDescriptors(1, &hDescHandle, &iDestRange
			, 1, &hSrcHandle, &iSrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	for (_uint i = 0; i < m_iRTCnt; ++i)
	{
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = m_tArr[i].pRtt->GetTex2D().Get();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;	// 타겟에서
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;			// 일반 리소스로
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		m_TargetToRes[i] = barrier;

		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;		// 리소스
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;	// 렌더 타겟으로
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		m_ResToTarget[i] = barrier;
	}
	return S_OK;
}

void CMRT::OM_Set(_uint iCnt, _uint iOffset)
{
	UINT iRTVSize = CDevice::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE hRTVHandle = m_pRTV->GetCPUDescriptorHandleForHeapStart();
	hRTVHandle.ptr += (iRTVSize * iOffset);

	D3D12_CPU_DESCRIPTOR_HANDLE hDSVHandle = m_pDsTex->GetDSV()->GetCPUDescriptorHandleForHeapStart();

	// 타겟 지정	
	CDevice::GetInstance()->GetCmdLst()->OMSetRenderTargets(iCnt, &hRTVHandle, FALSE, &hDSVHandle);
}

void CMRT::OM_Set()
{
	D3D12_CPU_DESCRIPTOR_HANDLE hRTVHandle = m_pRTV->GetCPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE hDSVHandle = m_pDsTex->GetDSV()->GetCPUDescriptorHandleForHeapStart();

	// 타겟 지정	
	CDevice::GetInstance()->GetCmdLst()->OMSetRenderTargets(m_iRTCnt, &hRTVHandle, TRUE/*DescHeap 에 연속적으로 있다*/, &hDSVHandle);
}


void CMRT::Clear()
{
	ResToTargetBarrier();
	UINT iRTVSize = CDevice::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	for (UINT i = 0; i < m_iRTCnt; ++i)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE hRTVHandle = m_pRTV->GetCPUDescriptorHandleForHeapStart();
		hRTVHandle.ptr += iRTVSize * i;
		float arrClearColor[4] = { 
			m_tArr[i].vClear_Color.x,
			m_tArr[i].vClear_Color.y,
			m_tArr[i].vClear_Color.z,
			m_tArr[i].vClear_Color.w };
		CDevice::GetInstance()->GetCmdLst()->ClearRenderTargetView(hRTVHandle, arrClearColor, 0, nullptr);
	}

	if (nullptr != m_pDsTex)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE hDSVHandle = m_pDsTex->GetDSV()->GetCPUDescriptorHandleForHeapStart();
		CDevice::GetInstance()->GetCmdLst()->ClearDepthStencilView(hDSVHandle, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);
	}
}
void CMRT::Clear(_uint iRtIdx)
{
	UINT iRTVSize = CDevice::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE hRTVHandle = m_pRTV->GetCPUDescriptorHandleForHeapStart();

	hRTVHandle.ptr += (iRTVSize * iRtIdx);
	float arrClearColor[4] = {
		  m_tArr[iRtIdx].vClear_Color.x
		, m_tArr[iRtIdx].vClear_Color.y
		, m_tArr[iRtIdx].vClear_Color.z
		, m_tArr[iRtIdx].vClear_Color.w };

	CDevice::GetInstance()->GetCmdLst()->ClearRenderTargetView(hRTVHandle, arrClearColor, 0, nullptr);

	if (nullptr != m_pDsTex)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE hDSVHandle = m_pDsTex->GetDSV()->GetCPUDescriptorHandleForHeapStart();
		CDevice::GetInstance()->GetCmdLst()->ClearDepthStencilView(hDSVHandle, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);
	}
}

void CMRT::TargetToResBarrier()
{
	CDevice::GetInstance()->GetCmdLst()->ResourceBarrier(m_iRTCnt, m_TargetToRes);
}

void CMRT::ResToTargetBarrier()
{
	CDevice::GetInstance()->GetCmdLst()->ResourceBarrier(m_iRTCnt, m_ResToTarget);
}

CMRT* CMRT::Create(_uint iCnt, tRtt* arrRT, CRTT* pDsTex)
{
	CMRT* pInstance = new CMRT;
	if (FAILED(pInstance->Ready_MRT(iCnt, arrRT, pDsTex)))
		Safe_Release(pInstance);
	return pInstance;
}

void CMRT::Free()
{
	for (auto& iter : m_tArr)
	{
		Safe_Release(iter.pRtt);
	}
	//Safe_Release(m_pDsTex);
}
