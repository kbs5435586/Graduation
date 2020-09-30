#include "framework.h"
#include "Hierachy_Loader.h"
#include "Shader.h"

_uint CHierachy_Loader::m_iCurrentIdx = 0;
CHierachy_Loader::CHierachy_Loader(ID3D12Device* pGraphic_Device)
{
	m_pGraphic_Device = pGraphic_Device;
}

void CHierachy_Loader::Ready_Hierachy_Loader(FbxNode* pFbxNode)
{
	if (nullptr == pFbxNode)
		return;

	FbxNodeAttribute* pFbxNodeAttribute = pFbxNode->GetNodeAttribute();

	if (pFbxNodeAttribute && (pFbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh))
	{
		FbxMesh* pFbxMesh = pFbxNode->GetMesh();
		if (pFbxMesh)
		{
			DYNAMIC_MESH_RENDER	tMeshRender;
			ZeroMemory(&tMeshRender, sizeof(DYNAMIC_MESH_RENDER));

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
				m_vecNumOfIndices.push_back(m_iNumIndices);
			}
			for (_uint i = 0, k = 0; i < m_iNumPolygons; ++i)
			{
				int iPolygonSize = pFbxMesh->GetPolygonSize(i);
				for (int j = 0; j < iPolygonSize; ++j)
				{
					m_vecIndecies.push_back(pFbxMesh->GetPolygonVertex(i, j));
				}
			}

			ID3D12Resource*				pTempVertexBuffer;
			ID3D12Resource*				pTempVertexUploadBuffer;
			ID3D12Resource*				pTempIndexBuffer;
			ID3D12Resource*				pTempIndexUploadBuffer;

			D3D12_VERTEX_BUFFER_VIEW	tempVertexBufferView;
			D3D12_INDEX_BUFFER_VIEW		tempIndexBufferView;
		
			// BufferView Setting
			CDevice::GetInstance()->Open();
			{
				if (FAILED(m_pGraphic_Device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
					D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(sizeof(_vec3) * m_iNumVertices), D3D12_RESOURCE_STATE_COPY_DEST,
					nullptr, IID_PPV_ARGS(&pTempVertexBuffer))))
					return;
				m_vecVertexBuffer.push_back(pTempVertexBuffer);
				if (FAILED(m_pGraphic_Device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
					D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(sizeof(_vec3) * m_iNumVertices), D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr, IID_PPV_ARGS(&pTempVertexUploadBuffer))))
					return;
				m_vecVertexUploadBuffer.push_back(pTempVertexUploadBuffer);


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
					return;
				m_vecIndexBuffer.push_back(pTempIndexBuffer);
				if (FAILED(m_pGraphic_Device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
					D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(sizeof(_uint) * m_iNumIndices), D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr, IID_PPV_ARGS(&pTempIndexUploadBuffer))))
					return;
				m_vecIndexUploadBuffer.push_back(pTempIndexUploadBuffer);

				D3D12_SUBRESOURCE_DATA indexData = {};
				indexData.pData = reinterpret_cast<BYTE*>(m_vecIndecies.data());
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

			tMeshRender.tVertexBufferView = tempVertexBufferView;
			tMeshRender.tIndexBufferView = tempIndexBufferView;
			tMeshRender.iNumOfIndices = m_iNumIndices;

			m_vecDynamic_Mesh_Render.push_back(tMeshRender);
			m_iMaxIdx = m_vecDynamic_Mesh_Render.size();

			CDevice::GetInstance()->WaitForGpuComplete();



			

			//m_VertexBufferView
			//m_IndexBufferView
			//m_iIndices		Number of Index
		}
	}





	_uint iNumChilds = pFbxNode->GetChildCount();
	for (_uint i = 0; i < iNumChilds; ++i)
		Ready_Hierachy_Loader(pFbxNode->GetChild(i));

	return;
}


CHierachy_Loader* CHierachy_Loader::Create(ID3D12Device* pGraphic_Device, FbxScene* pFbxScene)
{
	CHierachy_Loader* pInstance = new CHierachy_Loader(pGraphic_Device);
	pInstance->Ready_Hierachy_Loader(pFbxScene->GetRootNode());

	return pInstance;
}

void CHierachy_Loader::Render_Hierachy_Mesh(FbxNode* pFbxNode, FbxAMatrix matWorld, const _float& fTimeDelta)
{
	FbxNodeAttribute* pFbxNodeAttribute = pFbxNode->GetNodeAttribute();
	if (pFbxNodeAttribute && (pFbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh))
	{
		FbxAMatrix FbxMatNodeToRoot = pFbxNode->EvaluateGlobalTransform();
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

void CHierachy_Loader::Render_Hierachy_Mesh(FbxNode* pFbxNode, FbxAMatrix matWorld, const _float& fTimeDelta, ID3D12PipelineState* pPipeLine, ID3D12Resource* pConstantBuffer,
											CShader* pShader, void* pData, _uint iIdx)
{
	FbxNodeAttribute* pFbxNodeAttribute = pFbxNode->GetNodeAttribute();
	if (pFbxNodeAttribute && (pFbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh))
	{
		FbxAMatrix FbxMatNodeToRoot = pFbxNode->EvaluateGlobalTransform();
		FbxAMatrix FbxMatGeoOffset = Get_GeometricOffset(pFbxNode);

		FbxMesh* pFbxMesh = pFbxNode->GetMesh();
		Render_Mesh(pFbxMesh, FbxMatNodeToRoot, FbxMatGeoOffset, matWorld, pPipeLine, pConstantBuffer, pShader, pData, iIdx);
	}
	_uint iNumChild = pFbxNode->GetChildCount();

	for (_uint i = 0; i < iNumChild; ++i)
	{
		Render_Hierachy_Mesh(pFbxNode->GetChild(i), matWorld, fTimeDelta, pPipeLine, pConstantBuffer, pShader, pData,  iIdx);
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

		//	outMatrix = ConvertFbxToMatrix(FbxMatTransform);
		//	Set Constant Buffer
		//	CommantLst->Rendering Function



	}
}

void CHierachy_Loader::Render_Mesh(FbxMesh* pFbxMesh, FbxAMatrix& NodeToRoot, FbxAMatrix& GeometryOffset, FbxAMatrix matWorld, ID3D12PipelineState* pPipeLine,
									ID3D12Resource* pConstantBuffer, CShader* pShader, void* pData, _uint iIdx)
{
	if (nullptr == pShader)
		return;
	if (nullptr == pConstantBuffer)
		return;


	_uint iNumVertices = pFbxMesh->GetControlPointsCount();

	if (iNumVertices > 0)
	{
		FbxAMatrix		FbxMatTransform = matWorld;
		_uint			iNumSkinDeformer = pFbxMesh->GetDeformerCount(FbxDeformer::eSkin);
		if (iNumSkinDeformer == 0)
			FbxMatTransform = matWorld * NodeToRoot * GeometryOffset;

		//	outMatrix = ConvertFbxToMatrix(FbxMatTransform);
		//	Set Constant Buffer
		//	CommantLst->Rendering Function

		MAINPASS tMainPass = {};
		_matrix matWolrd = ConvertFbxToMatrix(&FbxMatTransform);
		_matrix matView = CDevice::GetInstance()->GetViewMatrix();
		_matrix matProj = CDevice::GetInstance()->GetProjectionMatrix();

		pShader->SetUp_OnShader(pPipeLine, pConstantBuffer, matWolrd, matView, matProj, tMainPass, ROOT_TYPE_COLOR);
		memcpy_s(pData, iIdx, (void*)&tMainPass, sizeof(tMainPass));

		CDevice::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(0, pConstantBuffer->GetGPUVirtualAddress());

		// m_veDynamic_Render를 인덱스 접근을 통해 BufferView들과 Num of Indices를 가져와 렌더링 해야함


		CDevice::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		CDevice::GetInstance()->GetCommandList()->IASetVertexBuffers(0, 1, &m_vecDynamic_Mesh_Render[0].tVertexBufferView);
		CDevice::GetInstance()->GetCommandList()->IASetIndexBuffer(&m_vecDynamic_Mesh_Render[0].tIndexBufferView);
		CDevice::GetInstance()->GetCommandList()->DrawIndexedInstanced(m_vecDynamic_Mesh_Render[0].iNumOfIndices, 1, 0, 0, 0);

		//MainPath
		//m_VertexBufferView
		//m_IndexBufferView
		//m_iIndices

		if (m_iCurrentIdx >= m_iMaxIdx-1)
		{
			m_iCurrentIdx = 0;
		}
		m_iCurrentIdx++;
	}
}

_matrix CHierachy_Loader::ConvertFbxToMatrix(FbxAMatrix* fbxmat)
{
	FbxVector4 S = fbxmat->GetS();
	//	FbxVector4 R = FbxVector4(0.0, 45.0, 0.0, 1.0);
	FbxVector4 R = fbxmat->GetR();
	FbxVector4 T = fbxmat->GetT();

	FbxAMatrix fbxmtxTransform = FbxAMatrix(T, R, S);

	XMFLOAT4X4 xmf4x4Result;
	for (int i = 0; i < 4; i++)
	{

		for (int j = 0; j < 4; j++) xmf4x4Result.m[i][j] = (float)(*fbxmat)[i][j];
	}

	XMFLOAT3 xmf3S = XMFLOAT3((float)S.mData[0], (float)S.mData[1], (float)S.mData[2]);
	XMFLOAT3 xmf3R = XMFLOAT3((float)R.mData[0], (float)R.mData[1], (float)R.mData[2]);
	XMFLOAT3 xmf3T = XMFLOAT3((float)T.mData[0], (float)T.mData[1], (float)T.mData[2]);

	XMMATRIX Rx = XMMatrixRotationX(XMConvertToRadians(xmf3R.x));
	XMMATRIX Ry = XMMatrixRotationY(XMConvertToRadians(xmf3R.y));
	XMMATRIX Rz = XMMatrixRotationZ(XMConvertToRadians(xmf3R.z));
	XMMATRIX xmR = XMMatrixMultiply(XMMatrixMultiply(Rx, Ry), Rz);
	XMFLOAT4X4 xmf4x4Multiply;
	XMStoreFloat4x4(&xmf4x4Multiply, XMMatrixMultiply(XMMatrixMultiply(XMMatrixScaling(xmf3S.x, xmf3S.y, xmf3S.z), xmR), XMMatrixTranslation(xmf3T.x, xmf3T.y, xmf3T.z)));

	return(xmf4x4Result);
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
		Safe_Release(iter);
	}
	for (auto& iter : m_vecVertexUploadBuffer)
	{
		Safe_Release(iter);
	}
	for (auto& iter : m_vecIndexBuffer)
	{
		Safe_Release(iter);
	}
	for (auto& iter : m_vecIndexUploadBuffer)
	{
		Safe_Release(iter);
	}
}