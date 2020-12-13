#include "framework.h"
#include "Buffer_Terrain_Height.h"
#include "Transform.h"
CBuffer_Terrain_Height::CBuffer_Terrain_Height(ID3D12Device* pGraphic_Device)
	: CVIBuffer(pGraphic_Device)
{
}

CBuffer_Terrain_Height::CBuffer_Terrain_Height(const CBuffer_Terrain_Height& rhs)
	: CVIBuffer(rhs)
	, m_pPixel(rhs.m_pPixel)
	, m_iNumVerticesX(rhs.m_iNumVerticesX)
	, m_iNumVerticesZ(rhs.m_iNumVerticesZ)
	, m_fInterval(rhs.m_fInterval)
	, m_pPosition(rhs.m_pPosition)
{

}

HRESULT CBuffer_Terrain_Height::Ready_VIBuffer(const _tchar* pFilePath, const _float& fInterval)
{
	_uint		iNumVerticesX, iNumVerticesZ;

	HANDLE		hFile = 0;
	_ulong		dwByte = 0;

	m_fInterval = fInterval;

	hFile = CreateFile(pFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	// 파일정보
	ReadFile(hFile, &m_fh, sizeof(BITMAPFILEHEADER), &dwByte, nullptr);

	// 이미지정보
	ReadFile(hFile, &m_ih, sizeof(BITMAPINFOHEADER), &dwByte, nullptr);

	iNumVerticesX = m_ih.biWidth;
	iNumVerticesZ = m_ih.biHeight;

	m_pPosition = new _vec3[iNumVerticesX * iNumVerticesZ];

	m_pPixel = new _ulong[iNumVerticesX * iNumVerticesZ];

	// 픽셀정보
	ReadFile(hFile, m_pPixel, sizeof(_ulong) * (iNumVerticesX * iNumVerticesZ), &dwByte, nullptr);

	CloseHandle(hFile);

	m_iNumVerticesX = iNumVerticesX;
	m_iNumVerticesZ = iNumVerticesZ;
	m_fInterval = fInterval;

	m_iVertices = iNumVerticesX * iNumVerticesZ;
	m_iStride = sizeof(VTXTEXNOR);
	m_iNumPolygons = (iNumVerticesX - 1) * (iNumVerticesZ - 1) * 2;

	m_pVertices = new VTXTEXNOR[m_iVertices];
	for (size_t i = 0; i < iNumVerticesZ; i++)
	{
		for (size_t j = 0; j < iNumVerticesX; j++)
		{
			_uint		iIndex = i * iNumVerticesX + j;

			m_pVertices[iIndex].vPos = _vec3(j * m_fInterval, (m_pPixel[iIndex] & 0x000000ff) / 10.0f, i * m_fInterval);
			m_pPosition[iIndex] = m_pVertices[iIndex].vPos;
			m_pVertices[iIndex].vTexUV = _vec2(j / (iNumVerticesX - 1.f) , i / (iNumVerticesZ - 1.f));
		}
	}

	m_iIndices = (3 * ((m_iNumVerticesX * 2) * (m_iNumVerticesZ - 1)) + ((m_iNumVerticesZ - 1) - 1));
	m_pIndices = new _uint[m_iIndices];

	_uint			iPolygonIndex = 0;
	for (size_t i = 0; i < iNumVerticesZ - 1; i++)
	{
		for (size_t j = 0; j < iNumVerticesX - 1; j++)
		{
			size_t iIndex = i * iNumVerticesX + j;
			m_pIndices[iPolygonIndex] = iIndex + iNumVerticesX;
			m_pIndices[iPolygonIndex + 1] = iIndex + iNumVerticesX + 1;
			m_pIndices[iPolygonIndex + 2] = iIndex + 1;

			m_pIndices[iPolygonIndex + 3] = iIndex + iNumVerticesX;
			m_pIndices[iPolygonIndex + 4] = iIndex + 1;
			m_pIndices[iPolygonIndex + 5] = iIndex;

			iPolygonIndex += 6;
		}
	}


	for (int i = 0; i < m_iNumPolygons * 3;)
	{
		_vec3 vSour, vDest, vNormal, vCross;
		//좌하
		
		vSour = Vector3::Subtract(m_pVertices[m_pIndices[i + 1]].vPos, m_pVertices[m_pIndices[i]].vPos);
		vDest = Vector3::Subtract(m_pVertices[m_pIndices[i + 2]].vPos, m_pVertices[m_pIndices[i]].vPos);
		vCross = Vector3::CrossProduct(vSour, vDest);
		vNormal = Vector3::Normalize(vCross);


		m_pVertices[m_pIndices[i]].vNormal = Vector3::Add(m_pVertices[m_pIndices[i]].vNormal, vNormal);
		m_pVertices[m_pIndices[i+1]].vNormal = Vector3::Add(m_pVertices[m_pIndices[i+1]].vNormal, vNormal);
		m_pVertices[m_pIndices[i+2]].vNormal = Vector3::Add(m_pVertices[m_pIndices[i+2]].vNormal, vNormal);
		++++++i;

		//우상

		vSour = Vector3::Subtract(m_pVertices[m_pIndices[i + 2]].vPos, m_pVertices[m_pIndices[i+1]].vPos);
		vDest = Vector3::Subtract(m_pVertices[m_pIndices[i]].vPos, m_pVertices[m_pIndices[i+1]].vPos);
		vCross = Vector3::CrossProduct(vSour, vDest);
		vNormal = Vector3::Normalize(vCross);

		m_pVertices[m_pIndices[i]].vNormal = Vector3::Add(m_pVertices[m_pIndices[i]].vNormal, vNormal);
		m_pVertices[m_pIndices[i + 1]].vNormal = Vector3::Add(m_pVertices[m_pIndices[i + 1]].vNormal, vNormal);
		m_pVertices[m_pIndices[i + 2]].vNormal = Vector3::Add(m_pVertices[m_pIndices[i + 2]].vNormal, vNormal);
		++++++i;
		

	}
	D3D12_HEAP_PROPERTIES	tHeap_Pro_Default = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_HEAP_PROPERTIES	tHeap_Pro_Upload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	CDevice::GetInstance()->Open();
	{
		D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(m_iStride * m_iVertices);
		if (FAILED(m_pGraphic_Device->CreateCommittedResource(&tHeap_Pro_Default, D3D12_HEAP_FLAG_NONE, &tResource_Desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_pVertexBuffer))))
			return E_FAIL;
		m_pVertexBuffer->SetName(L"VertexBuffer");
		if (FAILED(m_pGraphic_Device->CreateCommittedResource(&tHeap_Pro_Upload, D3D12_HEAP_FLAG_NONE, &tResource_Desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_pVertexUploadBuffer))))
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
		if (FAILED(m_pGraphic_Device->CreateCommittedResource(&tHeap_Pro_Upload, D3D12_HEAP_FLAG_NONE, &tResource_Desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_pIndexUploadBuffer))))
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

void CBuffer_Terrain_Height::Render_VIBuffer()
{
	CDevice::GetInstance()->GetCommandList()->IASetPrimitiveTopology(m_PrimitiveTopology);
	CDevice::GetInstance()->GetCommandList()->IASetVertexBuffers(m_iSlot, 1, &m_VertexBufferView);
	CDevice::GetInstance()->GetCommandList()->IASetIndexBuffer(&m_IndexBufferView);
	CDevice::GetInstance()->GetCommandList()->DrawIndexedInstanced(m_iIndices, 1, 0, 0, 0);
}

CBuffer_Terrain_Height* CBuffer_Terrain_Height::Create(ID3D12Device* pGraphic_Device, const _tchar* pFilePath, const _float& fInterval)
{

	CBuffer_Terrain_Height* pInstance = new CBuffer_Terrain_Height(pGraphic_Device);

	if (FAILED(pInstance->Ready_VIBuffer(pFilePath, fInterval)))
	{
		MessageBox(0, L"CBuffer_Terrain_Height Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CBuffer_Terrain_Height::Clone_Component(void* pArg)
{
	return new CBuffer_Terrain_Height(*this);
}

_float CBuffer_Terrain_Height::Compute_HeightOnTerrain(CTransform* pTransform)
{
	const _vec3* pTargetPos = pTransform->Get_StateInfo(CTransform::STATE_POSITION);

	_uint		iCurrentIdx = _uint(pTargetPos->z / m_fInterval) * m_iNumVerticesX + _uint(pTargetPos->x / m_fInterval);

	_float		fRatioX = (pTargetPos->x - m_pPosition[iCurrentIdx + m_iNumVerticesX].x) / m_fInterval;
	_float		fRatioZ = (m_pPosition[iCurrentIdx + m_iNumVerticesX].z - pTargetPos->z) / m_fInterval;

	_float		fHeight[4] = {
		m_pPosition[iCurrentIdx + m_iNumVerticesX].y,
		m_pPosition[iCurrentIdx + m_iNumVerticesX + 1].y,
		m_pPosition[iCurrentIdx + 1].y,
		m_pPosition[iCurrentIdx].y
	};

	// 오른쪽위에있는삼각형
	if (fRatioX >= fRatioZ)
	{
		return fHeight[0] + (fHeight[1] - fHeight[0]) * fRatioX + (fHeight[2] - fHeight[1]) * fRatioZ;
	}
	// 왼쪽 아래에있는삼각형
	else
	{
		return fHeight[0] + (fHeight[3] - fHeight[0]) * fRatioZ + (fHeight[2] - fHeight[3]) * fRatioX;
	}
}

void CBuffer_Terrain_Height::Free()
{

	CVIBuffer::Free();
}
