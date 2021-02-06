#include "framework.h"
#include "Buffer_Terrain.h"

CBuffer_Terrain::CBuffer_Terrain()
	: CVIBuffer()
{
}

CBuffer_Terrain::CBuffer_Terrain(const CBuffer_Terrain& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CBuffer_Terrain::Ready_VIBuffer(const _uint& iNumVerticesX, const _uint& iNumVerticesZ, const _float& fInterval)
{
	m_iNumVerticesX = iNumVerticesX;
	m_iNumVerticesZ = iNumVerticesZ;
	m_fInterval = fInterval;

	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	m_iStride = sizeof(VTXTEXNOR);
	vector<VTXTEXNOR>		vecVertices;
	vecVertices.resize(m_iNumVertices);


	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint iIdx = i * m_iNumVerticesZ + j;

			vecVertices[iIdx].vPos =_vec3(j * m_fInterval, 0.0f, i * m_fInterval);
			vecVertices[iIdx].vTexUV = _vec2(j / (m_iNumVerticesX - 1.f), i / (m_iNumVerticesZ - 1.f));
			vecVertices[iIdx].vNormal=_vec3(0.f,0.f,0.f);
		}
	}

	m_iNumIndices = (3 * ((m_iNumVerticesX * 2) * (m_iNumVerticesZ - 1)) + ((m_iNumVerticesZ - 1) - 1));
	vector<_uint>	vecIndices;
	vecIndices.resize(m_iNumIndices);

	_uint		iPolygonIdx = 0;

	for (size_t i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (size_t j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			size_t iIdx = i * m_iNumVerticesX + j;

			vecIndices[iPolygonIdx] = iIdx + m_iNumVerticesX;
			vecIndices[iPolygonIdx + 1] = iIdx + m_iNumVerticesX + 1;
			vecIndices[iPolygonIdx + 2] = iIdx + 1;
			
			vecIndices[iPolygonIdx + 3] = iIdx + m_iNumVerticesX;
			vecIndices[iPolygonIdx + 4] = iIdx + 1;
			vecIndices[iPolygonIdx + 5] = iIdx;


			iPolygonIdx += 6; // next quad
		}
	}
	D3D12_HEAP_PROPERTIES tHeap_Pro_Default = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_HEAP_PROPERTIES tHeap_Pro_Upload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CDevice::GetInstance()->Open();
	{
		D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(m_iStride * m_iNumVertices);

		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Default, D3D12_HEAP_FLAG_NONE,
			&tResource_Desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_pVertexBuffer))))
			return E_FAIL;
		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Upload, D3D12_HEAP_FLAG_NONE,
			&tResource_Desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_pVertexUploadBuffer))))
			return E_FAIL;


		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = (void*)(vecVertices.data());
		vertexData.RowPitch = m_iStride * m_iNumVertices;
		vertexData.SlicePitch = m_iStride * m_iNumVertices;

		UpdateSubresources(CDevice::GetInstance()->GetCmdLst().Get(), m_pVertexBuffer.Get(), m_pVertexUploadBuffer.Get(), 0, 0, 1, &vertexData);
		D3D12_RESOURCE_BARRIER	tResource_Barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pVertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		CDevice::GetInstance()->GetCmdLst()->ResourceBarrier(1, &tResource_Barrier);
	}
	{
		D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(_uint) * m_iNumIndices);


		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Default, D3D12_HEAP_FLAG_NONE,
			&tResource_Desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_pIndexBuffer))))
			return E_FAIL;
		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Upload, D3D12_HEAP_FLAG_NONE,
			&tResource_Desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_pIndexUploadBuffer))))
			return E_FAIL;

		D3D12_SUBRESOURCE_DATA indexData = {};
		indexData.pData = (void*)(vecIndices.data());
		indexData.RowPitch = sizeof(_uint) * m_iNumIndices;
		indexData.SlicePitch = sizeof(_uint) * m_iNumIndices;

		UpdateSubresources(CDevice::GetInstance()->GetCmdLst().Get(), m_pIndexBuffer.Get(), m_pIndexUploadBuffer.Get(), 0, 0, 1, &indexData);
		D3D12_RESOURCE_BARRIER	tResource_Barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pIndexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);
		CDevice::GetInstance()->GetCmdLst()->ResourceBarrier(1, &tResource_Barrier);
	}
	CDevice::GetInstance()->Close();


	m_tVertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_tVertexBufferView.StrideInBytes = m_iStride;
	m_tVertexBufferView.SizeInBytes = m_iStride * m_iNumVertices;

	m_tIndexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
	m_tIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_tIndexBufferView.SizeInBytes = sizeof(_uint) * m_iNumIndices;

	CDevice::GetInstance()->WaitForFenceEvent();
	return S_OK;
}

CBuffer_Terrain* CBuffer_Terrain::Create(const _uint& iNumVerticesX, const _uint& iNumVerticesZ, const _float& fInterval)
{
	CBuffer_Terrain* pInstance = new CBuffer_Terrain();

	if (FAILED(pInstance->Ready_VIBuffer(iNumVerticesX, iNumVerticesZ, fInterval)))
	{
		MessageBox(0, L"CBuffer_Terrain Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CBuffer_Terrain::Clone_Component(void* pArg)
{
	return new CBuffer_Terrain(*this);
}

void CBuffer_Terrain::Free()
{
	CVIBuffer::Free();
}
