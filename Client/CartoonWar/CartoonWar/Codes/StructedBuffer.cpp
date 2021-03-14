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

	// UAV ����
	// UAV �� ������ DescriptorHeap Create
	D3D12_DESCRIPTOR_HEAP_DESC uavHeapDesc = {};
	uavHeapDesc.NumDescriptors = 1;
	uavHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	uavHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	CDevice::GetInstance()->GetDevice()->CreateDescriptorHeap(&uavHeapDesc, IID_PPV_ARGS(&m_pUAV));

	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_pUAV->GetCPUDescriptorHandleForHeapStart();

	// UAV DescriptorHeap �� UAV ����
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = m_iElementCnt;
	uavDesc.Buffer.StructureByteStride = m_iElementSize;
	uavDesc.Buffer.CounterOffsetInBytes = 0;
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

	CDevice::GetInstance()->GetDevice()->CreateUnorderedAccessView(m_pBuffer.Get(), nullptr, &uavDesc, handle);


	// SRV ����
	// SRV �� ������ DescriptorHeap Create
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	CDevice::GetInstance()->GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_pSRV));

	handle = m_pSRV->GetCPUDescriptorHandleForHeapStart();

	// SRV DescriptorHeap �� SRV ����
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

void CStructedBuffer::Free()
{
}
