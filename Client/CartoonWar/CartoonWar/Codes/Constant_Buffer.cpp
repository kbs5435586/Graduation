#include "framework.h"
#include "Constant_Buffer.h"

CConstant_Buffer::CConstant_Buffer()
{

}

HRESULT CConstant_Buffer::Ready_ConstantBuffer(_uint iBufferSize, _uint iMaxCnt, CONST_REGISTER eType)
{
	m_iBufferSize = (iBufferSize + 255 * 2) & ~255 * 2; // ���� ������� 256 ����Ʈ �������� �Ѵ�.
	m_iMaxCnt = iMaxCnt;
	m_eType= eType;

	// ���� ������۸� GPU �޸𸮿� �Ҵ�
	D3D12_HEAP_PROPERTIES properties = {};
	properties.Type = D3D12_HEAP_TYPE_UPLOAD;
	properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	properties.CreationNodeMask = 1;
	properties.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC resource = {};
	resource.Alignment = 0;
	resource.DepthOrArraySize = 1;
	resource.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resource.Flags = D3D12_RESOURCE_FLAG_NONE;
	resource.Format = DXGI_FORMAT_UNKNOWN;
	resource.Width = m_iBufferSize * m_iMaxCnt;
	resource.Height = 1;
	resource.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resource.MipLevels = 1;
	resource.SampleDesc.Count = 1;
	resource.SampleDesc.Quality = 0;

	if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource
	(
		&properties, D3D12_HEAP_FLAG_NONE,
		&resource, D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr, IID_PPV_ARGS(&m_pBuffer)
	)))
		return E_FAIL;

	// DescriptorHeap �� ���� (View �μ�, ������۶� ���� �� ������)	
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
	cbvHeapDesc.NumDescriptors = m_iMaxCnt;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	if(FAILED(CDevice::GetInstance()->GetDevice()->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_pCBV))))
		return E_FAIL;

	// DescriptorHeap �� Buffer �� ������
	m_hCBV = m_pCBV->GetCPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_hCBV;
	UINT iIncreSize = CDevice::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	for (UINT i = 0; i < m_iMaxCnt; ++i)
	{
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = m_pBuffer->GetGPUVirtualAddress() + (m_iBufferSize * i);
		cbvDesc.SizeInBytes = m_iBufferSize;   // CB size is required to be 256-byte aligned.
		CDevice::GetInstance()->GetDevice()->CreateConstantBufferView(&cbvDesc, handle);
		handle.ptr += iIncreSize;
	}

	// ������ۿ� �����ϱ� ���ؼ� ������ �صд�.
	D3D12_RANGE readRange{ 0, 0 };
	m_pBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_pData));

	return S_OK;
}

CConstant_Buffer* CConstant_Buffer::Create(_uint iBufferSize, _uint iMaxCnt, CONST_REGISTER eType)
{
	CConstant_Buffer* pInstance = new CConstant_Buffer();
	if (FAILED(pInstance->Ready_ConstantBuffer(iBufferSize, iMaxCnt, eType)))
	{
		Safe_Release(pInstance);
	}
	return pInstance;
}


_uint CConstant_Buffer::SetData(void* pArg)
{
	 assert(!(m_iCurrCnt >= m_iMaxCnt));

	 UINT iOffsetPos = m_iCurrCnt++;

	 memcpy(m_pData + (m_iBufferSize * iOffsetPos), pArg, m_iBufferSize);

	 return iOffsetPos;
}

void CConstant_Buffer::ResetCount()
{
	m_iCurrCnt = 0;
}

void CConstant_Buffer::Free()
{
	_int i = 0;
}
