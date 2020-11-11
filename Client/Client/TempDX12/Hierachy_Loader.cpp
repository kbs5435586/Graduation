#include "framework.h"
#include "Hierachy_Loader.h"

CHierachy_Loader::CHierachy_Loader(ID3D12Device* pGraphic_Device)
{
}

HRESULT CHierachy_Loader::Ready_Hierachy_Loader(string strFilePath, FbxScene*& pScene)
{
	_int iSDKMajor;
	_int iSDKMinor;
	_int iSDKRevision;

	_int iFileFormatMajor;
	_int iFileFormatMinor;
	_int iFileFormatRevision;

	FbxManager::GetFileFormatVersion(iSDKMajor, iSDKMinor, iSDKRevision);
	FbxImporter* pFbxImporter = FbxImporter::Create(g_FbxManager, "");

	if (nullptr == pFbxImporter)
		return E_FAIL;

	if (!pFbxImporter->Initialize(strFilePath.c_str(), -1, g_FbxManager->GetIOSettings()))
	{
		FbxStatus eStatus = pFbxImporter->GetStatus();
		return E_FAIL;
	}

	pFbxImporter->GetFileVersion(iFileFormatMajor, iFileFormatMinor, iFileFormatRevision);

	pScene = FbxScene::Create(g_FbxManager, "");
	if (!pFbxImporter->Import(pScene))
		return E_FAIL;
	FbxGeometryConverter		FbxGeomConverter(g_FbxManager);
	FbxGeomConverter.Triangulate(pScene, TRUE);

	pScene->GetGlobalSettings().SetAxisSystem(FbxAxisSystem::Max);

	pFbxImporter->Destroy();

	return S_OK;
}

HRESULT CHierachy_Loader::Ready_Load_Hierachy(FbxNode* pNode)
{
	FbxNodeAttribute* pAttr = pNode->GetNodeAttribute();
	if ((pAttr && FbxNodeAttribute::eMesh == pAttr->GetAttributeType()))
	{
		FbxMesh* pMesh = pNode->GetMesh();
		if (nullptr == pMesh)
			return E_FAIL;

		_uint	iVertices = pMesh->GetControlPointsCount();
		_uint	iIndices = 0;
		_uint	iPolygons = pMesh->GetPolygonCount();

		for (_uint i = 0; i < iPolygons; ++i)
			iIndices += pMesh->GetPolygonSize(i);


		FbxVector4* pFbxPos = pMesh->GetControlPoints();

		vector<_vec3> vecPos;
		for (_uint i = 0; i < iVertices; ++i)
		{
			_vec3	vPos;
			vPos.x = (float)pFbxPos[i].mData[0];
			vPos.y = (float)pFbxPos[i].mData[1];
			vPos.z = (float)pFbxPos[i].mData[2];
			vecPos.push_back(vPos);
		}

		_int* pIndices = new _int[iIndices];
		for (_uint i = 0, k = 0; i < iPolygons; ++i)
		{
			_uint iPolygonSize = pMesh->GetPolygonSize(i);
			for (_uint j = 0; j < iPolygonSize; ++j)
				pIndices[k++] = pMesh->GetPolygonVertex(i, j);
		}

		RenderInfo* pInfo = new RenderInfo;
		CreateBufferView(iVertices, iIndices, vecPos, pIndices,pInfo);
		m_vecRenderInfo.push_back(pInfo);
		
		pMesh->SetUserDataPtr(pInfo);
	}
	_uint	iChildCnt = pNode->GetChildCount();
	for (_uint i = 0; i < iChildCnt; ++i)
		Ready_Load_Hierachy(pNode->GetChild(i));

	return S_OK;
}

HRESULT CHierachy_Loader::CreateBufferView(_uint iVerticesNum, _uint iIndicesNum, vector<_vec3>& vecPos, _int* pIndices, RenderInfo* pInfo)
{
	m_iVertices = iVerticesNum;
	m_iIndices = iIndicesNum;
	m_iStride = sizeof(_vec3);

	CDevice::GetInstance()->Open();
	{
		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(m_iStride * m_iVertices), D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_pVertexBuffer))))
			return E_FAIL;
		m_pVertexBuffer->SetName(L"VertexBuffer");
		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(m_iStride * m_iVertices), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_pVertexUploadBuffer))))
			return E_FAIL;
		m_pVertexUploadBuffer->SetName(L"Upload VertexBuffer");


		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = (void*)(vecPos.data());
		vertexData.RowPitch = m_iStride * m_iVertices;
		vertexData.SlicePitch = m_iStride * m_iVertices;

		UpdateSubresources(CDevice::GetInstance()->GetCommandList(), m_pVertexBuffer, m_pVertexUploadBuffer, 0, 0, 1, &vertexData);
		CDevice::GetInstance()->GetCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pVertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));
	}
	{
		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(sizeof(_uint) * m_iIndices), D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_pIndexBuffer))))
			return E_FAIL;
		m_pIndexBuffer->SetName(L"IndexBuffer");
		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(sizeof(_uint) * m_iIndices), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_pIndexUploadBuffer))))
			return E_FAIL;
		m_pIndexUploadBuffer->SetName(L"Upload IndexBuffer");

		D3D12_SUBRESOURCE_DATA indexData = {};
		indexData.pData = (void*)(pIndices);
		indexData.RowPitch = sizeof(_uint) * m_iIndices;
		indexData.SlicePitch = sizeof(_uint) * m_iIndices;

		UpdateSubresources(CDevice::GetInstance()->GetCommandList(), m_pIndexBuffer, m_pIndexUploadBuffer, 0, 0, 1, &indexData);
		CDevice::GetInstance()->GetCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pIndexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));
	}
	CDevice::GetInstance()->Close();

	pInfo->VertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	pInfo->VertexBufferView.StrideInBytes = m_iStride;
	pInfo->VertexBufferView.SizeInBytes = m_iStride * m_iVertices;

	pInfo->IndexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
	pInfo->IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	pInfo->IndexBufferView.SizeInBytes = sizeof(_uint) * m_iIndices;

	pInfo->iIndices = m_iIndices;
	pInfo->vecPosition = vecPos;
	CDevice::GetInstance()->WaitForGpuComplete();
	return S_OK;
}


CHierachy_Loader* CHierachy_Loader::Create(ID3D12Device* pGraphic_Device, string strFilePath, FbxScene*& pScene)
{
	CHierachy_Loader* pInstance = new CHierachy_Loader(pGraphic_Device);
	if (FAILED(pInstance->Ready_Hierachy_Loader(strFilePath, pScene)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

void CHierachy_Loader::Free()
{
	for (auto& iter : m_vecRenderInfo)
		Safe_Delete(iter);

	Safe_Release(m_pVertexBuffer);
	Safe_Release(m_pVertexUploadBuffer);
	Safe_Release(m_pIndexBuffer);
	Safe_Release(m_pVertexBuffer);
}
