#include "framework.h"
#include "Hierachy_Loader.h"
#include "TempDX12.h"
#include "Texture.h"
CHierachy_Loader::CHierachy_Loader(ID3D12Device* pGraphic_Device)
{
	m_pGraphic_Device = pGraphic_Device;
	m_pGraphic_Device->AddRef();
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
	pFbxImporter = nullptr;

	return S_OK;
}

HRESULT CHierachy_Loader::Ready_Load_Hierachy(FbxNode* pNode, _bool IsStatic)
{
	if (IsStatic)
	{
		FbxNodeAttribute* pAttr = pNode->GetNodeAttribute();
	
		if ((pAttr && FbxNodeAttribute::eMesh == pAttr->GetAttributeType()))
		{
			FbxMesh* pMesh = pNode->GetMesh();
			if (pMesh)
			{
				RenderInfo* pInfo = new RenderInfo;
				_uint iMtrlCnt = pNode->GetMaterialCount();
				if (iMtrlCnt > 0)
				{
					for (_uint i = 0; i < iMtrlCnt; ++i)
					{
						FbxSurfaceMaterial* pMtrlSur = pNode->GetMaterial(i);
						if (nullptr == pMtrlSur)
							return E_FAIL;
						if (FAILED(LoadMaterial(pMtrlSur, pInfo)))
							return E_FAIL;
					}
				}
				/*if (FAILED(LoadTexture(pInfo)))
					return E_FAIL;*/

				if (FAILED(LoadMesh(pMesh, pInfo)))
					return E_FAIL;
				pMesh->SetUserDataPtr(pInfo);
				m_vecRenderInfo.push_back(pInfo);
			}
		}
		_uint	iChildCnt = pNode->GetChildCount();
		for (_uint i = 0; i < iChildCnt; ++i)
			Ready_Load_Hierachy(pNode->GetChild(i));
	}
	else
	{
		FbxNodeAttribute* pAttr = pNode->GetNodeAttribute();
		if ((pAttr && FbxNodeAttribute::eMesh == pAttr->GetAttributeType()))
		{
   			FbxMesh* pMesh = pNode->GetMesh();
			if (pMesh)
			{
				RenderInfo* pInfo = new RenderInfo;
				_uint iMtrlCnt = pNode->GetMaterialCount();
	/*			if (iMtrlCnt > 0)
				{
					for (_uint i = 0; i < iMtrlCnt; ++i)
					{
						MTRLINFO tInfo = {};
						FbxSurfaceMaterial* pMtrlSur = pNode->GetMaterial(i);
						if (nullptr == pMtrlSur)
							return E_FAIL;
						if (FAILED(LoadMaterial(pMtrlSur, pInfo)))
							return E_FAIL;
					}
				}*/

				if (FAILED(LoadMeshIndexBuffer_(pMesh, pInfo)))
					return E_FAIL;
				pMesh->SetUserDataPtr(pInfo);
				m_vecRenderInfo.push_back(pInfo);
			}
		}
		_uint	iChildCnt = pNode->GetChildCount();
		for (_uint i = 0; i < iChildCnt; ++i)
			Ready_Load_Hierachy(pNode->GetChild(i), false);

	}

	return S_OK;
}

HRESULT CHierachy_Loader::CreateBufferView_Dynamic(_uint iVerticesNum, RenderInfo* pInfo)
{
	m_iVertices = iVerticesNum;
	m_iStride = sizeof(MESH);

	D3D12_HEAP_PROPERTIES	tHeap_Pro_Default = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_HEAP_PROPERTIES	tHeap_Pro_Upload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	ID3D12Resource* pVertexBuffer = nullptr;
	ID3D12Resource* pVertexUploadBuffer = nullptr;
	ID3D12Resource* pIndexBuffer = nullptr;
	ID3D12Resource* pIndexUploadBuffer = nullptr;
	CDevice::GetInstance()->Open();
	{
		D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(m_iStride * m_iVertices);

		pVertexUploadBuffer = ::CreateBufferResource(CDevice::GetInstance()->GetDevice()
			, CDevice::GetInstance()->GetCommandList(), NULL, m_iStride * m_iVertices,
			D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

		pVertexUploadBuffer->Map(0, NULL, (void**)pInfo->vecMeshData.data());

	}
	CDevice::GetInstance()->Close();

	pInfo->VertexBufferView.BufferLocation = pVertexUploadBuffer->GetGPUVirtualAddress();
	pInfo->VertexBufferView.StrideInBytes = m_iStride;
	pInfo->VertexBufferView.SizeInBytes = m_iStride * m_iVertices;


	m_vecVertexBuffer.push_back(pVertexBuffer);
	m_vecVertexUploadBuffer.push_back(pVertexUploadBuffer);
	m_vecIndexBuffer.push_back(pIndexBuffer);
	m_vecIndexUploadBuffer.push_back(pIndexUploadBuffer);

	CDevice::GetInstance()->WaitForGpuComplete();
	return S_OK;
}

HRESULT CHierachy_Loader::CreateBufferView__(_uint iVerticesNum, _uint iIndicesNum, RenderInfo* pInfo)
{
	m_iVertices = iVerticesNum;
	m_iIndices = iIndicesNum;
	m_iStride = sizeof(VTXTEMP);

	D3D12_HEAP_PROPERTIES	tHeap_Pro_Default = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_HEAP_PROPERTIES	tHeap_Pro_Upload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	ID3D12Resource* pVertexBuffer = nullptr;
	ID3D12Resource* pVertexUploadBuffer = nullptr;
	ID3D12Resource* pIndexBuffer = nullptr;
	ID3D12Resource* pIndexUploadBuffer = nullptr;
	CDevice::GetInstance()->Open();
	{
		D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(m_iStride * m_iVertices);

		pVertexUploadBuffer = ::CreateBufferResource(CDevice::GetInstance()->GetDevice()
			, CDevice::GetInstance()->GetCommandList(), NULL, m_iStride * m_iVertices,
			D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

		pVertexUploadBuffer->Map(0, NULL, (void**)&pInfo->temp);

	}
	{
		pIndexBuffer = ::CreateBufferResource(CDevice::GetInstance()->GetDevice()
			, CDevice::GetInstance()->GetCommandList(), pInfo->vecIndices.data(), sizeof(UINT) * m_iIndices,
			D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &pIndexUploadBuffer);
	}
	CDevice::GetInstance()->Close();

	pInfo->VertexBufferView.BufferLocation = pVertexUploadBuffer->GetGPUVirtualAddress();
	pInfo->VertexBufferView.StrideInBytes = m_iStride;
	pInfo->VertexBufferView.SizeInBytes = m_iStride * m_iVertices;

	pInfo->IndexBufferView.BufferLocation = pIndexBuffer->GetGPUVirtualAddress();
	pInfo->IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	pInfo->IndexBufferView.SizeInBytes = sizeof(UINT) * m_iIndices;

	pInfo->iIndices = m_iIndices;


	m_vecVertexBuffer.push_back(pVertexBuffer);
	m_vecVertexUploadBuffer.push_back(pVertexUploadBuffer);
	m_vecIndexBuffer.push_back(pIndexBuffer);
	m_vecIndexUploadBuffer.push_back(pIndexUploadBuffer);

	CDevice::GetInstance()->WaitForGpuComplete();
	return S_OK;
}

_vec4 CHierachy_Loader::GetMtrlData(FbxSurfaceMaterial* pSurface, const char* pMtrlName, const char* pMtrlFactorName)
{
	FbxDouble3  vMtrl;
	FbxDouble	dFactor = 0.f;

	FbxProperty tMtrlProperty = pSurface->FindProperty(pMtrlName);
	FbxProperty tMtrlFactorProperty = pSurface->FindProperty(pMtrlFactorName);

	if (tMtrlProperty.IsValid() && tMtrlFactorProperty.IsValid())
	{
		vMtrl = tMtrlProperty.Get<FbxDouble3>();
		dFactor = tMtrlFactorProperty.Get<FbxDouble>();
	}

	_vec4 vRetVal = _vec4((_float)vMtrl.mData[0] * dFactor, (_float)vMtrl.mData[1] * dFactor, (_float)vMtrl.mData[2] * dFactor, dFactor);
	return vRetVal;
}

wstring CHierachy_Loader::GetMtrlTextureName(FbxSurfaceMaterial* pSurface, const char* pMtrlProperty)
{
	string strName;

	FbxProperty TextureProperty = pSurface->FindProperty(pMtrlProperty);
	if (TextureProperty.IsValid())
	{
		UINT iCnt = TextureProperty.GetSrcObjectCount();

		if (1 <= iCnt)
		{
			FbxFileTexture* pFbxTex = TextureProperty.GetSrcObject<FbxFileTexture>(0);
			if (NULL != pFbxTex)
				strName = pFbxTex->GetFileName();
		}
	}

	return wstring(strName.begin(), strName.end());
}

HRESULT CHierachy_Loader::LoadMesh(FbxMesh* pMesh, RenderInfo* pInfo)
{
	if (nullptr == pMesh)
		return E_FAIL;

	_uint iVertexControlCount = pMesh->GetControlPointsCount(); // Vertex Buffer Num
	
	vector<_vec3>	vecControlPoint;	
	for (_uint i = 0; i < iVertexControlCount; ++i)
	{
		_vec3 vPos;
		vPos.x = (float)(pMesh->GetControlPointAt(i).mData[0]);
		vPos.y = (float)(pMesh->GetControlPointAt(i).mData[1]);
		vPos.z = (float)(pMesh->GetControlPointAt(i).mData[2]);
		vecControlPoint.push_back(vPos);
	}

	_uint	iTriCount = pMesh->GetPolygonCount();
	_uint	iVtxOrder = 0;

	for (_uint i = 0; i < iTriCount; ++i)
	{
		for (_uint j = 0; j < 3; ++j)
		{
			_vec3 vPos = {};
			_vec3 vNormal = {};
			_vec2 vUV = {};
			_uint iControlPointIndex = pMesh->GetPolygonVertex(i, j);

			vPos.x = vecControlPoint[iControlPointIndex].x;
			vPos.y = vecControlPoint[iControlPointIndex].y;
			vPos.z = vecControlPoint[iControlPointIndex].z;

			FbxVector2 fbxTexCoord;
			FbxStringList UVSetNameList;
			
			vNormal = GetNormal(pMesh, iControlPointIndex, iVtxOrder);
			vUV= GetUV(pMesh, iControlPointIndex, pMesh->GetTextureUVIndex(i, j));

			pInfo->vecMeshData.push_back(MESH(vPos, vNormal, vUV));
			iVtxOrder++;
		}
	}

	CreateBufferView(pInfo->vecMeshData.size(), pInfo);

	return S_OK;
}

HRESULT CHierachy_Loader::LoadDynamicMesh(FbxMesh* pMesh, RenderInfo* pInfo)
{
	if (nullptr == pMesh)
		return E_FAIL;

	_uint iVertexControlCount = pMesh->GetControlPointsCount(); // Vertex Buffer Num

	vector<_vec3>	vecControlPoint;
	for (_uint i = 0; i < iVertexControlCount; ++i)
	{
		_vec3 vPos;
		vPos.x = (float)(pMesh->GetControlPointAt(i).mData[0]);
		vPos.y = (float)(pMesh->GetControlPointAt(i).mData[1]);
		vPos.z = (float)(pMesh->GetControlPointAt(i).mData[2]);
		vecControlPoint.push_back(vPos);
	}

	_uint	iTriCount = pMesh->GetPolygonCount();
	_uint	iVtxOrder = 0;

	for (_uint i = 0; i < iTriCount; ++i)
	{
		for (_uint j = 0; j < 3; ++j)
		{
			_vec3 vPos = {};
			_vec3 vNormal = {};
			_vec2 vUV = {};
			_uint iControlPointIndex = pMesh->GetPolygonVertex(i, j);

			vPos.x = vecControlPoint[iControlPointIndex].x;
			vPos.y = vecControlPoint[iControlPointIndex].y;
			vPos.z = vecControlPoint[iControlPointIndex].z;

			FbxVector2 fbxTexCoord;
			FbxStringList UVSetNameList;

			vNormal = GetNormal(pMesh, iControlPointIndex, iVtxOrder);
			vUV = GetUV(pMesh, iControlPointIndex, pMesh->GetTextureUVIndex(i, j));

			pInfo->vecMeshData.push_back(MESH(vPos, vNormal, vUV));
			iVtxOrder++;
		}
	}

	CreateBufferView_Dynamic(pInfo->vecMeshData.size(), pInfo);
	return S_OK;
}

HRESULT CHierachy_Loader::LoadMeshIndexBuffer(FbxMesh* pMesh, RenderInfo* pInfo)
{
	if (nullptr == pMesh)
		return E_FAIL;

	_uint iPolygonCnt = pMesh->GetPolygonCount();

	FbxVector4 vPos, vNormal;

	_uint	iIndex = 0;

	for (_uint i = 0; i < iPolygonCnt; ++i)
	{
		_uint iPolygonSize = pMesh->GetPolygonSize(i);

		for (_uint j = 0; j < iPolygonSize; ++j)
		{
			_uint idx = pMesh->GetPolygonVertex(i, j);
			pInfo->vecIndices.push_back(idx);

			vPos = pMesh->GetControlPointAt(idx);
			pMesh->GetPolygonVertexNormal(i, j, vNormal);

			pInfo->vecPosition.push_back({ (_float)vPos.mData[0], (_float)vPos.mData[1], (_float)vPos.mData[2]});
			pInfo->vecNormal.push_back({ (_float)vNormal.mData[0], (_float)vNormal.mData[1], (_float)vNormal.mData[2]});

			++iIndex;
		}
			
	}
	_int i = 0;
	// 정점들에서 같은거 솎아내기
	return S_OK;
}

HRESULT CHierachy_Loader::LoadMeshIndexBuffer_(FbxMesh* pMesh, RenderInfo* pInfo)
{
	if (nullptr == pMesh)
		return E_FAIL;

	int		iVertices = pMesh->GetControlPointsCount();
	int		iIndices = 0;
	int		iPolygons = pMesh->GetPolygonCount();

	for (int i = 0; i < iPolygons; ++i)
		iIndices += pMesh->GetPolygonSize(i);


	vector<int>	vecIndices;
	vecIndices.resize(iIndices);

	FbxVector4 vPos, vNormal;
	_vec2 vUV;
	for (int i = 0, k = 0; i < iPolygons; ++i)
	{
		int nPolygonSize = pMesh->GetPolygonSize(i);
		for (int j = 0; j < nPolygonSize; j++)
		{
			int ij = pMesh->GetPolygonVertex(i, j);
			vecIndices[k++] = pMesh->GetPolygonVertex(i, j);
			pMesh->GetPolygonVertexNormal(i, j, vNormal);	
			vUV = GetUV(pMesh, ij, pMesh->GetTextureUVIndex(i, j));

			pInfo->vecNormal.push_back(_vec3((_float)vNormal.mData[0], (_float)vNormal.mData[1], (_float)vNormal.mData[2]));
			pInfo->vecUV.push_back(vUV);
		}
	}

	

	CreateBufferView__(iVertices, iIndices, pInfo);
	return S_OK;
}

HRESULT CHierachy_Loader::LoadMaterial(FbxSurfaceMaterial* pMtrlSur, RenderInfo* pInfo)
{
	MTRLINFO	tInfo = {};
	string strName = pMtrlSur->GetName();
	tInfo.strMtrlName = wstring(strName.begin(), strName.end());

	tInfo.vMtrlDiff =	GetMtrlData(pMtrlSur, FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor);
	tInfo.vMtrlAmb =	GetMtrlData(pMtrlSur, FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor);
	tInfo.vMtrlSpec =	GetMtrlData(pMtrlSur, FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor);
	tInfo.vMtrlEmiv =	GetMtrlData(pMtrlSur, FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissiveFactor);
	
	
	tInfo.strDiff =		GetMtrlTextureName(pMtrlSur, FbxSurfaceMaterial::sDiffuse);
	tInfo.strNormal =	GetMtrlTextureName(pMtrlSur, FbxSurfaceMaterial::sNormalMap);
	tInfo.strSpec =		GetMtrlTextureName(pMtrlSur, FbxSurfaceMaterial::sSpecular);


	pInfo->vecMtrlInfo.push_back(tInfo);
	return S_OK;
}

HRESULT CHierachy_Loader::LoadTexture(RenderInfo* pInfo)
{
	if (nullptr == pInfo)
		return E_FAIL;

	for (auto& iter : pInfo->vecMtrlInfo)
	{
		CTexture* pTexture = nullptr;
		if (iter.strDiff.size()>0)
		{
			
			string strTemp = {iter.strDiff.begin(), iter.strDiff.end()};
			
			if (strTemp.find(".dds")!=string::npos)
			{
				pTexture = CTexture::Create(m_pGraphic_Device, iter.strDiff.c_str(), 0, TEXTURE_TYPE_DDS);
			}
			else
			{
				pTexture = CTexture::Create(m_pGraphic_Device, iter.strDiff.c_str(), 0, TEXTURE_TYPE_ELSE);
			}

			
		}
		if (iter.strNormal.size() > 0)
		{

		}
		if (iter.strSpec.size() > 0)
		{

		}

		m_vecTexture.push_back(pTexture);
	}
	return S_OK;
}



HRESULT CHierachy_Loader::CreateBufferView(_uint iVerticesNum, RenderInfo* pInfo)
{
	m_iVertices = iVerticesNum;
	m_iStride = sizeof(MESH);

	D3D12_HEAP_PROPERTIES	tHeap_Pro_Default = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_HEAP_PROPERTIES	tHeap_Pro_Upload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	ID3D12Resource* pVertexBuffer = nullptr;
	ID3D12Resource* pVertexUploadBuffer = nullptr;
	ID3D12Resource* pIndexBuffer = nullptr;
	ID3D12Resource* pIndexUploadBuffer = nullptr;
	CDevice::GetInstance()->Open();
	{
		D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(m_iStride * m_iVertices);

		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Default, D3D12_HEAP_FLAG_NONE, 
			&tResource_Desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&pVertexBuffer))))
			return E_FAIL;
		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Upload, D3D12_HEAP_FLAG_NONE, 
			&tResource_Desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&pVertexUploadBuffer))))
			return E_FAIL;


		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = (pInfo->vecMeshData.data());
		vertexData.RowPitch = m_iStride * m_iVertices;
		vertexData.SlicePitch = m_iStride * m_iVertices;

		UpdateSubresources(CDevice::GetInstance()->GetCommandList(), pVertexBuffer, pVertexUploadBuffer, 0, 0, 1, &vertexData);
		D3D12_RESOURCE_BARRIER	tResource_Barrier = CD3DX12_RESOURCE_BARRIER::Transition(pVertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		CDevice::GetInstance()->GetCommandList()->ResourceBarrier(1, &tResource_Barrier);
	}
	CDevice::GetInstance()->Close();

	pInfo->VertexBufferView.BufferLocation = pVertexBuffer->GetGPUVirtualAddress();
	pInfo->VertexBufferView.StrideInBytes = m_iStride;
	pInfo->VertexBufferView.SizeInBytes = m_iStride * m_iVertices;


	m_vecVertexBuffer.push_back(pVertexBuffer);
	m_vecVertexUploadBuffer.push_back(pVertexUploadBuffer);
	m_vecIndexBuffer.push_back(pIndexBuffer);
	m_vecIndexUploadBuffer.push_back(pIndexUploadBuffer);

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
	for (auto& iter : m_vecTexture)
		Safe_Release(iter);
	if (m_pGraphic_Device)
		m_pGraphic_Device->Release();
	for (auto& iter : m_vecRenderInfo)
	{
		iter->vecBinormal.clear();
		iter->vecIndices.clear();
		iter->vecMeshData.clear();
		iter->vecMtrlInfo.clear();
		iter->vecNormal.clear();
		iter->vecPosition.clear();
		iter->vecTangent.clear();
		iter->vecUV.clear();
		Safe_Delete(iter);
		
	}
	for (auto& iter : m_vecVertexBuffer)
		Safe_Release(iter);
	for (auto& iter : m_vecVertexUploadBuffer)
		Safe_Release(iter);
	for (auto& iter : m_vecIndexBuffer)
		Safe_Release(iter);
	for (auto& iter : m_vecIndexUploadBuffer)
		Safe_Release(iter);
}

HRESULT CHierachy_Loader::GetTangent(FbxMesh* pMesh, RenderInfo* pInfo, _uint iIdx, _uint iVtxOrder)
{

	_uint	iTangentCnt = pMesh->GetElementTangentCount();

	if (1 != iTangentCnt)
		return E_FAIL;

	FbxGeometryElementTangent* pTangent = pMesh->GetElementTangent();
	if (nullptr == pTangent)
		return E_FAIL;

	_uint		iTangentIdx = 0;

	if (pTangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		if (pTangent->GetReferenceMode() == FbxGeometryElement::eDirect)
			iTangentIdx = iVtxOrder;
		else
			iTangentIdx = pTangent->GetIndexArray().GetAt(iVtxOrder);
	}
	else if (pTangent->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		if (pTangent->GetReferenceMode() == FbxGeometryElement::eDirect)
			iTangentIdx = iIdx;
		else
			iTangentIdx = pTangent->GetIndexArray().GetAt(iIdx);
	}

	FbxVector4 vTangent = pTangent->GetDirectArray().GetAt(iTangentIdx);

	pInfo->vecTangent.push_back(_vec3((_float)vTangent.mData[0], (_float)vTangent.mData[1], (_float)vTangent.mData[2]));

	return S_OK;
}

_vec3 CHierachy_Loader::GetNormal(FbxMesh* pMesh, _uint iIdx, _uint iVtxOrder)
{
	_uint	iNormalCnt = pMesh->GetElementNormalCount();

	if (iNormalCnt != 1)
		return  _vec3();

	FbxGeometryElementNormal* pNormal = pMesh->GetElementNormal();

	if (nullptr == pNormal)
		return _vec3();

	_uint iNormalIdx = 0;

	if (pNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		if (pNormal->GetReferenceMode() == FbxGeometryElement::eDirect)
		{
			iNormalIdx = iVtxOrder;
		}
		else
		{
			iNormalIdx = pNormal->GetIndexArray().GetAt(iVtxOrder);
		}
	}
	else if (pNormal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		if (pNormal->GetReferenceMode() == FbxGeometryElement::eDirect)
		{
			iNormalIdx = iIdx;
		}
		else
		{
			iNormalIdx = pNormal->GetIndexArray().GetAt(iIdx);
		}
	}

	FbxVector4	vNormal = pNormal->GetDirectArray().GetAt(iNormalIdx);
	_vec3		vTemp;
	vTemp.x = vNormal.mData[0];
	vTemp.y = vNormal.mData[1];
	vTemp.z = vNormal.mData[2];

	return vTemp;
}

HRESULT CHierachy_Loader::GetBiNormal(FbxMesh* pMesh, RenderInfo* pInfo, _uint iIdx, _uint iVtxOrder)
{
	_uint iBiNormalCnt = pMesh->GetElementBinormalCount();
	if (1 != iBiNormalCnt)
		return E_FAIL;

	FbxGeometryElementBinormal* pBinormal = pMesh->GetElementBinormal();
	if (nullptr == pBinormal)
		return E_FAIL;

	_uint	iBinormalIdx = 0;

	if (pBinormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		if (pBinormal->GetReferenceMode() == FbxGeometryElement::eDirect)
			iBinormalIdx = iVtxOrder;
		else
			iBinormalIdx = pBinormal->GetIndexArray().GetAt(iVtxOrder);
	}
	else if (pBinormal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		if (pBinormal->GetReferenceMode() == FbxGeometryElement::eDirect)
			iBinormalIdx = iIdx;
		else
			iBinormalIdx = pBinormal->GetIndexArray().GetAt(iIdx);
	}

	FbxVector4 vBinormal = pBinormal->GetDirectArray().GetAt(iBinormalIdx);

	pInfo->vecBinormal.push_back(_vec3((_float)vBinormal.mData[0], (_float)vBinormal.mData[1], (_float)vBinormal.mData[2]));

	return S_OK;
}

_vec2 CHierachy_Loader::GetUV(FbxMesh* pMesh, _uint iIdx, _uint iVtxOrder)
{
	FbxGeometryElementUV* pUV = pMesh->GetElementUV();
	if (nullptr == pUV)
		return _vec2();

	_uint		iUVIdx = 0;
	if (pUV->GetReferenceMode() == FbxGeometryElement::eDirect)
		iUVIdx = iIdx;
	else
		iUVIdx = pUV->GetIndexArray().GetAt(iIdx);

	iUVIdx = iVtxOrder;

	FbxVector2 vUV = pUV->GetDirectArray().GetAt(iUVIdx);

	_vec2	TempUV;
	TempUV.x = vUV.mData[0];
	TempUV.y = 1.f - vUV.mData[1];

	return TempUV;
}





