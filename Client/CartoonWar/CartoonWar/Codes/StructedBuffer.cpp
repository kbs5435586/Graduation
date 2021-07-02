#include "framework.h"
#include "StructedBuffer.h"
#include "Management.h"

CStructedBuffer::CStructedBuffer()
{
}

HRESULT CStructedBuffer::Ready_StructedBuffer(_uint iElementSize, _uint iElementCnt, void* pArg)
{	
	m_iElementSize = iElementSize;
	m_iElementCnt = iElementCnt;

	if (pArg)
		m_eState = D3D12_RESOURCE_STATE_COPY_DEST;
	else
		m_eState = D3D12_RESOURCE_STATE_COMMON;

	D3D12_RESOURCE_DESC tBufferDesc = {};

	tBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	tBufferDesc.Width = m_iElementSize * m_iElementCnt;
	tBufferDesc.Height = 1;
	tBufferDesc.DepthOrArraySize = 1;
	tBufferDesc.MipLevels = 1;
	tBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
	tBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	tBufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	tBufferDesc.SampleDesc.Count = 1;
	tBufferDesc.SampleDesc.Quality = 0;


	CD3DX12_HEAP_PROPERTIES temp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	// Buffer Create
	CDevice::GetInstance()->GetDevice()->CreateCommittedResource(
		&temp,
		D3D12_HEAP_FLAG_NONE,
		&tBufferDesc,
		m_eState,
		nullptr,
		IID_PPV_ARGS(&m_pBuffer));


	if (pArg)
	{
		CDevice::GetInstance()->Open();
		
		// 佬扁 滚欺 积己
		ComPtr<ID3D12Resource> pReadBuffer = nullptr;

		// ReadBuffer Desc
		D3D12_RESOURCE_DESC tReadBufferDesc = {};
		tReadBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		tReadBufferDesc.Width = m_iElementSize * m_iElementCnt;
		tReadBufferDesc.Height = 1;
		tReadBufferDesc.DepthOrArraySize = 1;
		tReadBufferDesc.MipLevels = 1;
		tReadBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		tReadBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		tReadBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		tReadBufferDesc.SampleDesc.Count = 1;
		tReadBufferDesc.SampleDesc.Quality = 0;

		CD3DX12_HEAP_PROPERTIES temp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		// Buffer Create
		CDevice::GetInstance()->GetDevice()->CreateCommittedResource(
			&temp,
			D3D12_HEAP_FLAG_NONE,
			&tReadBufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&pReadBuffer));

		UINT8* pVertexDataBegin = nullptr;
		D3D12_RANGE readRange{ 0, 0 }; // We do not intend to read from this resource on the CPU.	
		pReadBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
		memcpy(pVertexDataBegin, pArg, (tReadBufferDesc.Width * tReadBufferDesc.Height));
		pReadBuffer->Unmap(0, nullptr);

		CD3DX12_RESOURCE_BARRIER _temp = CD3DX12_RESOURCE_BARRIER::Transition(m_pBuffer.Get()
			, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON);
		// Resource Copy
		CDevice::GetInstance()->GetCmdLst()->CopyBufferRegion(m_pBuffer.Get(), 0, pReadBuffer.Get(), 0, m_iElementSize * m_iElementCnt);
		CDevice::GetInstance()->GetCmdLst()->ResourceBarrier(1, &_temp);


	

		m_eState = D3D12_RESOURCE_STATE_COMMON;

		CDevice::GetInstance()->Close();
		CDevice::GetInstance()->WaitForFenceEvent();
	}


	// UAV 积己
	// UAV 甫 历厘且 DescriptorHeap Create
	D3D12_DESCRIPTOR_HEAP_DESC uavHeapDesc = {};
	uavHeapDesc.NumDescriptors = 1;
	uavHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	uavHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	CDevice::GetInstance()->GetDevice()->CreateDescriptorHeap(&uavHeapDesc, IID_PPV_ARGS(&m_pUAV));

	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_pUAV->GetCPUDescriptorHandleForHeapStart();

	// UAV DescriptorHeap 俊 UAV 积己
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = m_iElementCnt;
	uavDesc.Buffer.StructureByteStride = m_iElementSize;
	uavDesc.Buffer.CounterOffsetInBytes = 0;
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

	CDevice::GetInstance()->GetDevice()->CreateUnorderedAccessView(m_pBuffer.Get(), nullptr, &uavDesc, handle);


	// SRV 积己
	// SRV 甫 历厘且 DescriptorHeap Create
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	CDevice::GetInstance()->GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_pSRV));

	handle = m_pSRV->GetCPUDescriptorHandleForHeapStart();

	// SRV DescriptorHeap 俊 SRV 积己
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;

	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = m_iElementCnt;
	srvDesc.Buffer.StructureByteStride = m_iElementSize;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	CDevice::GetInstance()->GetDevice()->CreateShaderResourceView(m_pBuffer.Get(), &srvDesc, handle);
	return S_OK;
}   
HRESULT CStructedBuffer::Ready_StructedBuffer()
{
	return S_OK;
}

void CStructedBuffer::Update_Data(TEXTURE_REGISTER eRegister)
{
	CDevice::GetInstance()->SetBufferToRegister(this, eRegister);
}

void CStructedBuffer::Update_Data_CS(TEXTURE_REGISTER eRegister)
{
	CDevice::GetInstance()->SetBufferToRegister_CS(this, eRegister);
}

void CStructedBuffer::Update_RWData(UAV_REGISTER eRegister)
{
	CDevice::GetInstance()->SetUpUAVToRegister_CS(this, eRegister);
}

CStructedBuffer* CStructedBuffer::Create(_uint iElementSize, _uint iElementCnt, void* pArg)
{
	CStructedBuffer* pInstance = new CStructedBuffer;
	if (FAILED(pInstance->Ready_StructedBuffer(iElementSize, iElementCnt, pArg)))
	{
		Safe_Release(pInstance);
	}
	return pInstance;
}

CStructedBuffer* CStructedBuffer::Create()
{
	CStructedBuffer* pInstance = new CStructedBuffer;
	if (FAILED(pInstance->Ready_StructedBuffer()))
	{
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CStructedBuffer::Free()
{

}
