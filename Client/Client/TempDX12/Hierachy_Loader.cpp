#include "framework.h"
#include "Hierachy_Loader.h"

CHierachy_Loader::CHierachy_Loader(ID3D12Device* pGraphic_Device)
{
	m_pGraphic_Device = pGraphic_Device;
}

HRESULT CHierachy_Loader::Ready_Hierachy_Loader(FbxNode* pFbxNode)
{
	if (nullptr == pFbxNode)
		return E_FAIL;

	FbxNodeAttribute* pFbxNodeAttribute = pFbxNode->GetNodeAttribute();

	if (pFbxNodeAttribute && (pFbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh))
	{
		FbxMesh* pFbxMesh = pFbxNode->GetMesh();
		if (pFbxMesh)
		{
			m_iNumVertices = pFbxMesh->GetControlPointsCount();
			m_iNumIndices = 0;
			m_iNumPolygons = pFbxMesh->GetPolygonCount();

			for (_uint i = 0; i < m_iNumVertices; ++i)
			{
				_vec3	vTemp;
				vTemp.x = pFbxMesh->GetControlPointAt(i).mData[0];
				vTemp.y = pFbxMesh->GetControlPointAt(i).mData[1];
				vTemp.z = pFbxMesh->GetControlPointAt(i).mData[2];
				m_vecPosition.push_back(vTemp);
			}

			for (_uint i = 0; i < m_iNumPolygons; ++i)
			{
				m_iNumIndices += pFbxMesh->GetPolygonSize(i);
			}

			for (_uint i = 0, k = 0; i < m_iNumPolygons; ++i)
			{
				int iPolygonSize = pFbxMesh->GetPolygonSize(i);
				for (int j = 0; j < iPolygonSize; ++j)
				{
					m_vecIndecise.push_back(pFbxMesh->GetPolygonVertex(i, j));
				}
			}
		}
		else return E_FAIL;
	}
	else
		return E_FAIL;

	ID3D12Resource* pTempVertexBuffer;
	ID3D12Resource* pTempVertexUploadBuffer;
	ID3D12Resource* pTempIndexBuffer;
	ID3D12Resource* pTempIndexUploadBuffer;
	D3D12_VERTEX_BUFFER_VIEW	tempVertexBufferView;
	D3D12_INDEX_BUFFER_VIEW		tempIndexBufferView;

	// BufferView Setting
	CDevice::GetInstance()->Open();
	{
		if (FAILED(m_pGraphic_Device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(sizeof(_vec3) * m_iNumVertices), D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr, IID_PPV_ARGS(&pTempVertexBuffer))))
			return E_FAIL;
		m_vecVertexBuffer.push_back(pTempVertexBuffer);
		if (FAILED(m_pGraphic_Device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(sizeof(_vec3) * m_iNumVertices), D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, IID_PPV_ARGS(&pTempVertexUploadBuffer))))
			return E_FAIL;
		m_vecVertexUploadBuffer.push_back(pTempVertexBuffer);


		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = reinterpret_cast<BYTE*>(m_vecPosition.data());
		vertexData.RowPitch = sizeof(_vec3) * m_iNumVertices;
		vertexData.SlicePitch = sizeof(_vec3) * m_iNumVertices;

		UpdateSubresources(CDevice::GetInstance()->GetCommandList(), pTempVertexBuffer, pTempVertexUploadBuffer, 0, 0, 1, &vertexData);
		CDevice::GetInstance()->GetCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pTempVertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));
	}
	{
		if (FAILED(m_pGraphic_Device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(sizeof(_uint) * m_iNumIndices), D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr, IID_PPV_ARGS(&pTempIndexBuffer))))
			return E_FAIL;

		m_vecIndexBuffer.push_back(pTempIndexBuffer);
		if (FAILED(m_pGraphic_Device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(sizeof(_uint) * m_iNumIndices), D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, IID_PPV_ARGS(&pTempIndexUploadBuffer))))
			return E_FAIL;
		m_vecIndexUploadBuffer.push_back(pTempIndexUploadBuffer);

		D3D12_SUBRESOURCE_DATA indexData = {};
		indexData.pData = reinterpret_cast<BYTE*>(m_vecIndecise.data());
		indexData.RowPitch = sizeof(_uint) * m_iNumIndices;
		indexData.SlicePitch = sizeof(_uint) * m_iNumIndices;

		UpdateSubresources(CDevice::GetInstance()->GetCommandList(), pTempIndexBuffer, pTempIndexUploadBuffer, 0, 0, 1, &indexData);
		CDevice::GetInstance()->GetCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pTempIndexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));
	}
	CDevice::GetInstance()->Close();

	tempVertexBufferView.BufferLocation = pTempVertexBuffer->GetGPUVirtualAddress();
	tempVertexBufferView.StrideInBytes = sizeof(_vec3);
	tempVertexBufferView.SizeInBytes = sizeof(_vec3) * m_iNumVertices;

	tempIndexBufferView.BufferLocation = pTempIndexBuffer->GetGPUVirtualAddress();
	tempIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	tempIndexBufferView.SizeInBytes = sizeof(_uint) * m_iNumIndices;

	m_vecVertexBufferView.push_back(tempVertexBufferView);
	m_vecIndexBufferView.push_back(tempIndexBufferView);
	CDevice::GetInstance()->WaitForGpuComplete();


	_uint iNumChilds = pFbxNode->GetChildCount();
	for (_uint i = 0; i < iNumChilds; ++i)
		Ready_Hierachy_Loader(pFbxNode->GetChild(i));

	return S_OK;
}


CHierachy_Loader* CHierachy_Loader::Create(ID3D12Device* pGraphic_Device, FbxScene* pFbxScene)
{
	CHierachy_Loader* pInstance = new CHierachy_Loader(pGraphic_Device);
	if (FAILED(pInstance->Ready_Hierachy_Loader(pFbxScene->GetRootNode())))
	{
		MessageBox(0, L"CHierachy_Loader Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return nullptr;
}

void CHierachy_Loader::Render_Hierachy_Mesh(FbxNode* pFbxNode, FbxAMatrix matWorld, const _float& fTimeDelta)
{ 
	FbxNodeAttribute* pFbxNodeAttribute = pFbxNode->GetNodeAttribute();
	if (pFbxNodeAttribute && (pFbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh))
	{
		FbxAMatrix FbxMatNodeToRoot = pFbxNode->EvaluateGlobalTransform(fTimeDelta);
		FbxAMatrix FbxMatGeoOffset = Get_GeometricOffset(pFbxNode);

		FbxMesh* pFbxMesh = pFbxNode->GetMesh();
		Render_Mesh(pFbxMesh, FbxMatNodeToRoot, FbxMatGeoOffset, matWorld);
	}
	_uint iNumChild = pFbxNode->GetChildCount();

	for (_uint i = 0; i < iNumChild; ++i)
	{
		Render_Hierachy_Mesh(pFbxNode->GetChild(i), matWorld, fTimeDelta);
	}
}

void CHierachy_Loader::Render_Mesh(FbxMesh* pFbxMesh, FbxAMatrix& NodeToRoot, FbxAMatrix& GeometryOffset, FbxAMatrix matWorld)
{
	_uint iNumVertices = pFbxMesh->GetControlPointsCount();

	if (iNumVertices > 0)
	{
		FbxAMatrix		FbxMatTransform = matWorld;
		_uint			iNumSkinDeformer = pFbxMesh->GetDeformerCount(FbxDeformer::eSkin);
		if (iNumSkinDeformer == 0)
			FbxMatTransform = matWorld * NodeToRoot * GeometryOffset;

		//Constant Buffer Create


		//Render


	}
}

FbxAMatrix CHierachy_Loader::Get_GeometricOffset(FbxNode* pFbxNode)
{
	const FbxVector4 T = pFbxNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 R = pFbxNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 S = pFbxNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return(FbxAMatrix(T, R, S));
}

void CHierachy_Loader::Free()
{
	for (auto& iter : m_vecVertexBuffer)
	{
		if (iter)
			iter->Release();
	}
	for (auto& iter : m_vecVertexUploadBuffer)
	{
		if (iter)
			iter->Release();
	}
	for (auto& iter : m_vecIndexBuffer)
	{
		if (iter)
			iter->Release();
	}
	for (auto& iter : m_vecIndexUploadBuffer)
	{
		if (iter)
			iter->Release();
	}
}