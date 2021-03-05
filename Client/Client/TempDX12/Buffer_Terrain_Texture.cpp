#include "framework.h"
#include "Buffer_Terrain_Texture.h"

CBuffer_Terrain_Texture::CBuffer_Terrain_Texture(ID3D12Device* pGraphic_Device)
	: CVIBuffer(pGraphic_Device)
{
}

CBuffer_Terrain_Texture::CBuffer_Terrain_Texture(const CBuffer_Terrain_Texture& rhs)
	: CVIBuffer(rhs)

{

}

HRESULT CBuffer_Terrain_Texture::Ready_VIBuffer(const _uint& iNumVerticesX, const _uint& iNumVerticesZ, const _float& fInterval)
{
	m_iNumVerticesX = iNumVerticesX;
	m_iNumVerticesZ = iNumVerticesZ;
	m_fInterval = fInterval;

	m_iVertices = m_iNumVerticesX * m_iNumVerticesZ;

	m_iStride = sizeof(VTXTEX);
	m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


	m_pVertices = new VTXTEX[m_iVertices];

	for (size_t i = 0; i < iNumVerticesZ; i++)
	{
		for (size_t j = 0; j < iNumVerticesX; j++)
		{
			_uint		iIndex = i * iNumVerticesX + j;

			m_pVertices[iIndex].vPosition = _vec3(j * m_fInterval, 0.0f, i * m_fInterval);
			m_pVertices[iIndex].vTex = _vec2(j / (m_iNumVerticesX - 1.f) , i / (m_iNumVerticesZ - 1.f));
		}
	}


	m_iIndices = (3 * ((m_iNumVerticesX * 2) * (m_iNumVerticesZ - 1)) + ((m_iNumVerticesZ - 1) - 1));
	m_pIndices = new _uint[m_iIndices];
	ID3D12Resource* pVertexBuffer = nullptr;;

	_uint			iPolygonIndex = 0;

	for (size_t i = 0; i < iNumVerticesZ - 1; ++i)
	{
		for (size_t j = 0; j < iNumVerticesX - 1; ++j)
		{
			size_t iIndex = i * iNumVerticesX + j;

			m_pIndices[iPolygonIndex] = iIndex + iNumVerticesX;
			m_pIndices[iPolygonIndex + 1] = iIndex + iNumVerticesX + 1;
			m_pIndices[iPolygonIndex + 2] = iIndex + 1;

			m_pIndices[iPolygonIndex + 3] = iIndex + iNumVerticesX;
			m_pIndices[iPolygonIndex + 4] = iIndex + 1;
			m_pIndices[iPolygonIndex + 5] = iIndex;


			iPolygonIndex += 6; // next quad
		}
	}
	D3D12_HEAP_PROPERTIES	tHeap_Pro_Default = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_HEAP_PROPERTIES	tHeap_Pro_Upload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	CDevice::GetInstance()->Open();
	{
		D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(m_iStride * m_iVertices);
		if (FAILED(m_pGraphic_Device->CreateCommittedResource(&tHeap_Pro_Default,D3D12_HEAP_FLAG_NONE, &tResource_Desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_pVertexBuffer))))
			return E_FAIL;
		m_pVertexBuffer->SetName(L"VertexBuffer");
		if (FAILED(m_pGraphic_Device->CreateCommittedResource(&tHeap_Pro_Upload,D3D12_HEAP_FLAG_NONE, &tResource_Desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_pVertexUploadBuffer))))
			return E_FAIL;
		m_pVertexUploadBuffer->SetName(L"Upload VertexBuffer");

		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = reinterpret_cast<BYTE*>(m_pVertices);
		vertexData.RowPitch = sizeof(m_pVertices);
		vertexData.SlicePitch = sizeof(m_pVertices);

		D3D12_RESOURCE_BARRIER	tResource_Barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pVertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		UpdateSubresources(CDevice::GetInstance()->GetCommandList(), m_pVertexBuffer, m_pVertexUploadBuffer, 0, 0, 1, &vertexData);
		CDevice::GetInstance()->GetCommandList()->ResourceBarrier(1, &tResource_Barrier);
	}
	{
		D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(_uint) * m_iIndices);
		if (FAILED(m_pGraphic_Device->CreateCommittedResource(&tHeap_Pro_Default, D3D12_HEAP_FLAG_NONE, &tResource_Desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_pIndexBuffer))))
			return E_FAIL;
		m_pIndexBuffer->SetName(L"IndexBuffer");
		if (FAILED(m_pGraphic_Device->CreateCommittedResource(&tHeap_Pro_Upload,D3D12_HEAP_FLAG_NONE, &tResource_Desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_pIndexUploadBuffer))))
			return E_FAIL;
		m_pIndexUploadBuffer->SetName(L"Upload IndexBuffer");

		D3D12_SUBRESOURCE_DATA indexData = {};
		indexData.pData = reinterpret_cast<BYTE*>(m_pIndices);
		indexData.RowPitch = sizeof(m_pIndices);
		indexData.SlicePitch = sizeof(m_pIndices);

		D3D12_RESOURCE_BARRIER	tResource_Barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pIndexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);
		UpdateSubresources(CDevice::GetInstance()->GetCommandList(), m_pIndexBuffer, m_pIndexUploadBuffer, 0, 0, 1, &indexData);
		CDevice::GetInstance()->GetCommandList()->ResourceBarrier(1, &tResource_Barrier);
	}
	CDevice::GetInstance()->Close();

	m_VertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_VertexBufferView.StrideInBytes = m_iStride;
	m_VertexBufferView.SizeInBytes = m_iStride * m_iVertices;

	m_IndexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
	m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_IndexBufferView.SizeInBytes = sizeof(_uint) * m_iIndices;

	CDevice::GetInstance()->WaitForGpuComplete();

	Safe_Delete_Array(m_pVertices);
	Safe_Delete_Array(m_pIndices);
	return S_OK;
}

void CBuffer_Terrain_Texture::Render_VIBuffer()
{
	CDevice::GetInstance()->GetCommandList()->IASetPrimitiveTopology(m_PrimitiveTopology);
	CDevice::GetInstance()->GetCommandList()->IASetVertexBuffers(m_iSlot, 1, &m_VertexBufferView);
	CDevice::GetInstance()->GetCommandList()->IASetIndexBuffer(&m_IndexBufferView);
	CDevice::GetInstance()->GetCommandList()->DrawIndexedInstanced(m_iIndices, 1, 0, 0, 0);
}

CBuffer_Terrain_Texture* CBuffer_Terrain_Texture::Create(ID3D12Device* pGraphic_Device, const _uint& iNumVerticesX, const _uint& iNumVerticesZ, const _float& fInterval)
{

	CBuffer_Terrain_Texture* pInstance = new CBuffer_Terrain_Texture(pGraphic_Device);

	if (FAILED(pInstance->Ready_VIBuffer(iNumVerticesX, iNumVerticesZ, fInterval)))
	{
		MessageBox(0, L"CBuffer_Terrain_Texture Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CBuffer_Terrain_Texture::Clone_Component(void* pArg)
{
	return new CBuffer_Terrain_Texture(*this);
}

void CBuffer_Terrain_Texture::Free()
{

	CVIBuffer::Free();
}
