#include "framework.h"
#include "Buffer_Terrain_Height.h"
#include "Transform.h"
#include "QuadTree.h"
#include "Frustum.h"
CBuffer_Terrain_Height::CBuffer_Terrain_Height()
	: CVIBuffer()
{
}

CBuffer_Terrain_Height::CBuffer_Terrain_Height(const CBuffer_Terrain_Height& rhs)
	: CVIBuffer(rhs)
	, m_iNumVerticesX(rhs.m_iNumVerticesX)
	, m_iNumVerticesZ(rhs.m_iNumVerticesZ)
	, m_fInterval(rhs.m_fInterval)
	, m_pPosition(rhs.m_pPosition)
	, m_pPixel(rhs.m_pPixel)
	, m_pQuadTree(rhs.m_pQuadTree)
	, m_pPolygonVertexIndex(rhs.m_pPolygonVertexIndex)
{
	m_IsClone = true;
}

void CBuffer_Terrain_Height::Calculate_TanBi(VTXTEXBUMP Vertex1, VTXTEXBUMP Vertex2, VTXTEXBUMP Vertex3, _vec3& vTangent, _vec3& vBinormal)
{
	_float vector1[3] = {};
	_float vector2[3] = {};

	_float tuVector[2] = {};
	_float tvVector[2] = {};


	vector1[0] = Vertex2.vPos.x = Vertex1.vPos.x;
	vector1[1] = Vertex2.vPos.y = Vertex1.vPos.y;
	vector1[2] = Vertex2.vPos.z = Vertex1.vPos.z;

	vector2[0] = Vertex3.vPos.x = Vertex1.vPos.x;
	vector2[1] = Vertex3.vPos.y = Vertex1.vPos.y;
	vector2[2] = Vertex3.vPos.z = Vertex1.vPos.z;


	tuVector[0] = Vertex2.vTexUV.x - Vertex1.vTexUV.x;
	tvVector[0] = Vertex2.vTexUV.y - Vertex1.vTexUV.y;

	tuVector[1] = Vertex3.vTexUV.x - Vertex1.vTexUV.x;
	tvVector[1] = Vertex3.vTexUV.y - Vertex1.vTexUV.y;

	_float fDen  = 1.f / (tuVector[0] * tvVector[1] - tuVector[1]* tvVector[0]);

	vTangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * fDen;
	vTangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * fDen;
	vTangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * fDen;

	vBinormal.x = (tuVector[0] * vector1[0] - tuVector[1] * vector2[0]) * fDen;
	vBinormal.y = (tuVector[0] * vector1[1] - tuVector[1] * vector2[1]) * fDen;
	vBinormal.z = (tuVector[0] * vector1[2] - tuVector[1] * vector2[2]) * fDen;

	_float fLen_Tan = (_float)sqrt((vTangent.x * vTangent.x) + (vTangent.y * vTangent.y) + (vTangent.z * vTangent.z));
	_float fLen_Bin = (_float)sqrt((vTangent.x * vTangent.x) + (vTangent.y * vTangent.y) + (vTangent.z * vTangent.z));

	vTangent.x = vTangent.x / fLen_Tan;
	vTangent.y = vTangent.y / fLen_Tan;
	vTangent.z = vTangent.z / fLen_Tan;

	vBinormal.x = vBinormal.x / fLen_Bin;
	vBinormal.y = vBinormal.y / fLen_Bin;
	vBinormal.z = vBinormal.z / fLen_Bin;
}

HRESULT CBuffer_Terrain_Height::Ready_VIBuffer(const _tchar* pFilePath, const _float& fInterval)
{

	_uint		iNumVerticesX, iNumVerticesZ;

	HANDLE		hFile = 0;
	_ulong		dwByte = 0;

	hFile = CreateFile(pFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	// ��������
	ReadFile(hFile, &m_fh, sizeof(BITMAPFILEHEADER), &dwByte, nullptr);

	// �̹�������
	ReadFile(hFile, &m_ih, sizeof(BITMAPINFOHEADER), &dwByte, nullptr);

	iNumVerticesX = m_ih.biWidth;
	iNumVerticesZ = m_ih.biHeight;

	m_pPosition = new _vec3[iNumVerticesX * iNumVerticesZ];
	m_pPixel = new _ulong[iNumVerticesX * iNumVerticesZ];

	_float fTemp = 0.f;

	if (iNumVerticesX >= 1500.f)
		fTemp = 1.f;
	else
		fTemp = 10.f;

	// �ȼ�����
	ReadFile(hFile, m_pPixel, sizeof(_ulong) * (iNumVerticesX * iNumVerticesZ), &dwByte, nullptr);

	CloseHandle(hFile);

	m_iNumVerticesX = iNumVerticesX;
	m_iNumVerticesZ = iNumVerticesZ;
	m_fInterval = fInterval;

	m_iNumVertices = iNumVerticesX * iNumVerticesZ;
	m_iStride = sizeof(VTXTEXBUMP);
	m_iNumPolygons = (iNumVerticesX - 1) * (iNumVerticesZ - 1) * 2;

	vector<VTXTEXBUMP>		vecVertices;
	vecVertices.resize(m_iNumVertices);
	for (size_t i = 0; i < iNumVerticesZ; i++)
	{
		for (size_t j = 0; j < iNumVerticesX; j++)
		{
			_uint		iIndex = i * iNumVerticesX + j;
			_ulong		fY = (m_pPixel[iIndex] & 0x000000ff) / fTemp;

			vecVertices[iIndex].vPos = _vec3(j * m_fInterval, fY, i * m_fInterval);
			vecVertices[iIndex].vNormal = {};
			vecVertices[iIndex].vBiNormal = {};
			vecVertices[iIndex].vTangent = {};
			m_pPosition[iIndex] = vecVertices[iIndex].vPos;
			vecVertices[iIndex].vTexUV = _vec2(j / (iNumVerticesX - 1.f), i / (iNumVerticesZ - 1.f));
		}
	}
	m_iNumIndices = (3 * ((m_iNumVerticesX * 2) * (m_iNumVerticesZ - 1)) + ((m_iNumVerticesZ - 1) - 1));

	vector<_uint>		vecIndices;
	vecIndices.resize(m_iNumIndices);

	_uint			iPolygonIndex = 0;
	for (size_t i = 0; i < iNumVerticesZ - 1; i++)
	{
		for (size_t j = 0; j < iNumVerticesX - 1; j++)
		{
			size_t iIndex = i * iNumVerticesX + j;
			vecIndices[iPolygonIndex] = iIndex + iNumVerticesX;
			vecIndices[iPolygonIndex + 1] = iIndex + iNumVerticesX + 1;
			vecIndices[iPolygonIndex + 2] = iIndex + 1;

			vecIndices[iPolygonIndex + 3] = iIndex + iNumVerticesX;
			vecIndices[iPolygonIndex + 4] = iIndex + 1;
			vecIndices[iPolygonIndex + 5] = iIndex;

			iPolygonIndex += 6;
		}
	}

	m_pPolygonVertexIndex = new _uint[m_iNumPolygons];
	memcpy(m_pPolygonVertexIndex, vecIndices.data(), m_iNumPolygons);

	for (_uint i = 0; i < m_iNumPolygons * 3;)
	{
		_vec3 vSour, vDest, vNormal, vCross;
		_vec3 vTangent, vBinormal;

		//����
		vSour = Vector3_::Subtract(vecVertices[vecIndices[i + 1]].vPos, vecVertices[vecIndices[i]].vPos);
		vDest = Vector3_::Subtract(vecVertices[vecIndices[i + 2]].vPos, vecVertices[vecIndices[i]].vPos);
		vCross = Vector3_::CrossProduct(vSour, vDest);
		vNormal = Vector3_::Normalize(vCross);


		vecVertices[vecIndices[i]].vNormal = Vector3_::Add(vecVertices[vecIndices[i]].vNormal, vNormal);
		vecVertices[vecIndices[i + 1]].vNormal = Vector3_::Add(vecVertices[vecIndices[i + 1]].vNormal, vNormal);
		vecVertices[vecIndices[i + 2]].vNormal = Vector3_::Add(vecVertices[vecIndices[i + 2]].vNormal, vNormal);

		Calculate_TanBi(vecVertices[vecIndices[i]], vecVertices[vecIndices[i+1]], vecVertices[vecIndices[i+2]], vTangent, vBinormal);
		vecVertices[vecIndices[i]].vTangent = vTangent;
		vecVertices[vecIndices[i + 1]].vTangent = vTangent;
		vecVertices[vecIndices[i + 2]].vTangent = vTangent;

		vecVertices[vecIndices[i]].vBiNormal = vBinormal;
		vecVertices[vecIndices[i + 1]].vBiNormal = vBinormal;
		vecVertices[vecIndices[i + 2]].vBiNormal = vBinormal;
		++++++i;

		//���
		vSour = Vector3_::Subtract(vecVertices[vecIndices[i + 2]].vPos, vecVertices[vecIndices[i + 1]].vPos);
		vDest = Vector3_::Subtract(vecVertices[vecIndices[i]].vPos, vecVertices[vecIndices[i + 1]].vPos);
		vCross = Vector3_::CrossProduct(vSour, vDest);
		vNormal = Vector3_::Normalize(vCross);

		vecVertices[vecIndices[i]].vNormal = Vector3_::Add(vecVertices[vecIndices[i]].vNormal, vNormal);
		vecVertices[vecIndices[i + 1]].vNormal = Vector3_::Add(vecVertices[vecIndices[i + 1]].vNormal, vNormal);
		vecVertices[vecIndices[i + 2]].vNormal = Vector3_::Add(vecVertices[vecIndices[i + 2]].vNormal, vNormal);
		Calculate_TanBi(vecVertices[vecIndices[i]], vecVertices[vecIndices[i + 1]], vecVertices[vecIndices[i + 2]], vTangent, vBinormal);
		vecVertices[vecIndices[i]].vTangent = vTangent;
		vecVertices[vecIndices[i + 1]].vTangent = vTangent;
		vecVertices[vecIndices[i + 2]].vTangent = vTangent;

		vecVertices[vecIndices[i]].vBiNormal = vBinormal;
		vecVertices[vecIndices[i + 1]].vBiNormal = vBinormal;
		vecVertices[vecIndices[i + 2]].vBiNormal = vBinormal;
		++++++i;
	}
	D3D12_HEAP_PROPERTIES	tHeap_Pro_Default = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_HEAP_PROPERTIES	tHeap_Pro_Upload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

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
		vertexData.pData = (void*)vecVertices.data();
		vertexData.RowPitch = m_iStride* m_iNumVertices;
		vertexData.SlicePitch = m_iStride * m_iNumVertices;

		D3D12_RESOURCE_BARRIER	tResource_Barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pVertexBuffer.Get(), 
												D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		UpdateSubresources(CDevice::GetInstance()->GetCmdLst().Get(), m_pVertexBuffer.Get(), m_pVertexUploadBuffer.Get(), 0, 0, 1, &vertexData);
		CDevice::GetInstance()->GetCmdLst().Get()->ResourceBarrier(1, &tResource_Barrier);
	}
	{
		D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(_uint) * m_iNumIndices);
		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Default, D3D12_HEAP_FLAG_NONE, &tResource_Desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_pIndexBuffer))))
			return E_FAIL;
		m_pIndexBuffer->SetName(L"IndexBuffer");
		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Upload, D3D12_HEAP_FLAG_NONE, &tResource_Desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_pIndexUploadBuffer))))
			return E_FAIL;
		m_pIndexUploadBuffer->SetName(L"Upload IndexBuffer");

		D3D12_SUBRESOURCE_DATA indexData = {};
		indexData.pData = (void*)vecIndices.data();
		indexData.RowPitch = sizeof(_uint)*m_iNumIndices;
		indexData.SlicePitch = sizeof(_uint) * m_iNumIndices;

		D3D12_RESOURCE_BARRIER	tResource_Barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pIndexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);
		UpdateSubresources(CDevice::GetInstance()->GetCmdLst().Get(), m_pIndexBuffer.Get(), m_pIndexUploadBuffer.Get(), 0, 0, 1, &indexData);
		CDevice::GetInstance()->GetCmdLst().Get()->ResourceBarrier(1, &tResource_Barrier);
	}
	CDevice::GetInstance()->Close();

	m_tVertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_tVertexBufferView.StrideInBytes = m_iStride;
	m_tVertexBufferView.SizeInBytes = m_iStride * m_iNumVertices;

	m_tIndexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
	m_tIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_tIndexBufferView.SizeInBytes = sizeof(_uint) * m_iNumIndices;

	CDevice::GetInstance()->WaitForFenceEvent();



	//m_pQuadTree = CQuadTree::Create(m_pPosition, m_iNumVerticesX, m_iNumVerticesZ);
	//if (nullptr == m_pQuadTree)
	//	return E_FAIL;

	return S_OK;
}	

CBuffer_Terrain_Height* CBuffer_Terrain_Height::Create(const _tchar* pFilePath, const _float& fInterval)
{
	CBuffer_Terrain_Height* pInstance = new CBuffer_Terrain_Height();

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

	_float		fHeight[4] = 
	{
		m_pPosition[iCurrentIdx + m_iNumVerticesX].y,
		m_pPosition[iCurrentIdx + m_iNumVerticesX + 1].y,
		m_pPosition[iCurrentIdx + 1].y,
		m_pPosition[iCurrentIdx].y
	};

	// �����������ִ»ﰢ��
	if (fRatioX >= fRatioZ)
	{
		return fHeight[0] + (fHeight[1] - fHeight[0]) * fRatioX + (fHeight[2] - fHeight[1]) * fRatioZ;
	}
	// ���� �Ʒ����ִ»ﰢ��
	else
	{
		return fHeight[0] + (fHeight[3] - fHeight[0]) * fRatioZ + (fHeight[2] - fHeight[3]) * fRatioX;
	}
}

HRESULT CBuffer_Terrain_Height::Culling_Frustum(CFrustum* pFrustum, const _matrix& matWorld)
{
	Plane	tPlane[6];


	if (FAILED(pFrustum->Get_LocalPlane(tPlane, &matWorld)))
		return E_FAIL;

	_uint			iNumPolygon = 0;
	m_pQuadTree->Culling_ToQuadTree(pFrustum, tPlane, m_pPolygonVertexIndex, iNumPolygon);

	vector<_uint>		vecIndices;
	vecIndices.resize(iNumPolygon);
	D3D12_HEAP_PROPERTIES	tHeap_Pro_Default = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_HEAP_PROPERTIES	tHeap_Pro_Upload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	memcpy(vecIndices.data(), m_pPolygonVertexIndex, iNumPolygon);

	CDevice::GetInstance()->Open();
	{
		D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(_uint) * iNumPolygon);
		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Default, D3D12_HEAP_FLAG_NONE, &tResource_Desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_pIndexBuffer))))
			return E_FAIL;
		m_pIndexBuffer->SetName(L"IndexBuffer");
		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Upload, D3D12_HEAP_FLAG_NONE, &tResource_Desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_pIndexUploadBuffer))))
			return E_FAIL;
		m_pIndexUploadBuffer->SetName(L"Upload IndexBuffer");

		D3D12_SUBRESOURCE_DATA indexData = {};
		indexData.pData = (void*)vecIndices.data();
		indexData.RowPitch = sizeof(_uint) * iNumPolygon;
		indexData.SlicePitch = sizeof(_uint) * iNumPolygon;

		D3D12_RESOURCE_BARRIER	tResource_Barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pIndexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);
		UpdateSubresources(CDevice::GetInstance()->GetCmdLst().Get(), m_pIndexBuffer.Get(), m_pIndexUploadBuffer.Get(), 0, 0, 1, &indexData);
		CDevice::GetInstance()->GetCmdLst().Get()->ResourceBarrier(1, &tResource_Barrier);
	}
	CDevice::GetInstance()->Close();
	CDevice::GetInstance()->WaitForFenceEvent();

	m_tIndexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
	m_tIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_tIndexBufferView.SizeInBytes = sizeof(_uint) * iNumPolygon;

	m_iNumIndices = iNumPolygon;

	return S_OK;
}

void CBuffer_Terrain_Height::Free()
{
	if (m_IsClone)
	{
		Safe_Delete_Array(m_pPosition);
		Safe_Delete_Array(m_pPixel);
		Safe_Delete_Array(m_pPolygonVertexIndex);
	}
	//Safe_Release(m_pQuadTree);
	
	CVIBuffer::Free();
}
