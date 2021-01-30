#include "framework.h"
#include "FBXLoader.h"

CFBXLoader::CFBXLoader()
{
}

HRESULT CFBXLoader::Ready_FbxLoader(string strFilePath)
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

	m_pScene = FbxScene::Create(g_FbxManager, "");
	if (!pFbxImporter->Import(m_pScene))
		return E_FAIL;
	FbxGeometryConverter		FbxGeomConverter(g_FbxManager);
	FbxGeomConverter.Triangulate(m_pScene, TRUE);

	m_pScene->GetGlobalSettings().SetAxisSystem(FbxAxisSystem::eDirectX);



	pFbxImporter->Destroy();
	pFbxImporter = nullptr;

	return S_OK;
}

HRESULT CFBXLoader::Load_FbxFile(FbxNode* pNode, _bool IsStatic)
{
	if (IsStatic)
	{
		FbxNodeAttribute* pAttr = pNode->GetNodeAttribute();
		if ((pAttr && FbxNodeAttribute::eMesh == pAttr->GetAttributeType()))
		{
			FbxMesh* pMesh = pNode->GetMesh();
			//string str = pNode->GetName();
			if (pMesh)
			{
				RenderInfo* pInfo = new RenderInfo();
				
				_uint iMtrlCnt = pNode->GetMaterialCount();
				pInfo->strNodeName = pNode->GetName();

				if (iMtrlCnt > 0)
				{
					for (_uint i = 0; i < iMtrlCnt; ++i)
					{
						FbxSurfaceMaterial* pMtrlSur = pNode->GetMaterial(i);
						if (nullptr == pMtrlSur)
							return E_FAIL;
						if (FAILED(Load_Material(pMtrlSur, pInfo)))
							return E_FAIL;
					}
				}
 				if (FAILED(Load_Mesh(pMesh, pInfo)))
					return E_FAIL;

				pMesh->SetUserDataPtr(pInfo);
				m_vecRenderInfo.push_back(pInfo);
			}
		}

		_uint	iChildCnt = pNode->GetChildCount();
		for (_uint i = 0; i < iChildCnt; ++i)
			Load_FbxFile(pNode->GetChild(i));
	}
	else
	{

	}


	return S_OK;
}

HRESULT CFBXLoader::Load_Mesh(FbxMesh* pMesh, RenderInfo* pInfo)
{
	unordered_map<MESH, _uint> indexMapping;
	if (nullptr == pMesh)
		return E_FAIL;

	_uint	iVertexControlCnt = pMesh->GetControlPointsCount();

	vector<_vec3> vecControlPoint;
	for (_uint i = 0; i < iVertexControlCnt; ++i)
	{
		_vec3 vPos;
		vPos.x = (_float)(pMesh->GetControlPointAt(i).mData[0]);
		vPos.y = (_float)(pMesh->GetControlPointAt(i).mData[2]);
		vPos.z = (_float)(pMesh->GetControlPointAt(i).mData[1]);
		vecControlPoint.push_back(vPos);
	}

	_uint	iTriangleCnt = pMesh->GetPolygonCount();
	_uint	iVtxOrder = 0;
	_uint	iCnt = 0;

	for (_uint i = 0; i < iTriangleCnt; ++i)
	{
		vector<MESH>		vecPos;
		for (_uint j = 0; j < 3; ++j)
		{
			_uint	iControlPointIdx = pMesh->GetPolygonVertex(i, j);

			_vec3 vPos = vecControlPoint[iControlPointIdx];
			_vec3 vNormal = Get_Normal(pMesh, iControlPointIdx, iVtxOrder);
			_vec2 vUV = Get_UV(pMesh, iControlPointIdx, pMesh->GetTextureUVIndex(i, j));
			_vec3 vTangent = Get_Tangent(pMesh, iControlPointIdx, iVtxOrder);
			_vec3 vBinormal = Get_BiNormal(pMesh, iControlPointIdx, iVtxOrder);


			MESH Vertex = MESH(vPos, vNormal, vUV, vTangent, vBinormal);

			// IndexBuffer Setting
		/*	auto iter_find = indexMapping.find(Vertex);
			if (iter_find != indexMapping.end())
			{
				pInfo->vecIndices.push_back(iter_find->second);
			}
			else
			{
				indexMapping[Vertex] = iCnt;
				pInfo->vecIndices.push_back(iCnt);
				++iCnt;
			}*/
			// if use IndexBuffer Under line Set Line 144


			pInfo->vecVertices.push_back(Vertex);
			vecPos.push_back(Vertex);
			iVtxOrder++;
		}

		// if Get_Tangent() return 0
		_vec3 vTangent, vBinormal, vNormal;
		CalculateTangentBinormal(vecPos[0], vecPos[1], vecPos[2], vTangent, vBinormal);
		CalculateNormal(vTangent, vBinormal, vNormal);

		pInfo->vecVertices[iVtxOrder - 3].vTangent = vTangent;
		pInfo->vecVertices[iVtxOrder - 2].vTangent = vTangent;
		pInfo->vecVertices[iVtxOrder - 1].vTangent = vTangent;

		pInfo->vecVertices[iVtxOrder - 3].vBinormal = vBinormal;
		pInfo->vecVertices[iVtxOrder - 2].vBinormal = vBinormal;
		pInfo->vecVertices[iVtxOrder - 1].vBinormal = vBinormal;

		pInfo->vecVertices[iVtxOrder - 3].vNormal = vNormal;
		pInfo->vecVertices[iVtxOrder - 2].vNormal = vNormal;
		pInfo->vecVertices[iVtxOrder - 1].vNormal = vNormal;

		vecPos.clear();
	}
	//Create Buffer
	//if (FAILED(CreateBufferView_Index(pInfo)))
	//	return E_FAIL;

	if (FAILED(CreateBufferView(pInfo)))
		return E_FAIL;
	return S_OK;
}

HRESULT CFBXLoader::Load_Material(FbxSurfaceMaterial* pMtrlSur, RenderInfo* pInfo)
{
	MTRLINFO		tInfo = {};
	string strName = pMtrlSur->GetName();
	tInfo.strMtrlName = wstring(strName.begin(), strName.end());

	tInfo.vMtrlDiff = GetMtrlData(pMtrlSur, FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor);
	tInfo.vMtrlAmb = GetMtrlData(pMtrlSur, FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor);
	tInfo.vMtrlSpec = GetMtrlData(pMtrlSur, FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor);
	tInfo.vMtrlEmiv = GetMtrlData(pMtrlSur, FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissiveFactor);

	tInfo.strDiff = GetMtrlTextureName(pMtrlSur, FbxSurfaceMaterial::sDiffuse);
	tInfo.strNormal = GetMtrlTextureName(pMtrlSur, FbxSurfaceMaterial::sNormalMap);
	tInfo.strSpec = GetMtrlTextureName(pMtrlSur, FbxSurfaceMaterial::sSpecular);

	pInfo->vecMtrlInfo.push_back(tInfo);
	return S_OK;
}

HRESULT CFBXLoader::Load_Texture(RenderInfo* pInfo, CTexture* pTexture)
{
	if (pInfo == nullptr)
		return E_FAIL;


	for (auto& iter : pInfo->vecMtrlInfo)
	{

	}

	return S_OK;
}

_vec4 CFBXLoader::GetMtrlData(FbxSurfaceMaterial* pSurface, const char* pMtrlName, const char* pMtrlFactorName)
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

wstring CFBXLoader::GetMtrlTextureName(FbxSurfaceMaterial* pSurface, const char* pMtrlProperty)
{
	string strName;

	FbxProperty TextureProperty = pSurface->FindProperty(pMtrlProperty);
	if (TextureProperty.IsValid())
	{
		UINT iCnt = TextureProperty.GetSrcObjectCount<FbxFileTexture>();

		if (1 <= iCnt)
		{
			FbxFileTexture* pFbxTex = TextureProperty.GetSrcObject<FbxFileTexture>(0);
			if (NULL != pFbxTex)
				strName = pFbxTex->GetFileName();
		}
	}
	return wstring(strName.begin(), strName.end());
}

_vec3 CFBXLoader::Get_Normal(FbxMesh* pMesh, _uint iIdx, _uint iVtxOrder)
{
	_uint		iNormalCnt = pMesh->GetElementNormalCount();

	if (iNormalCnt < 1)
		return _vec3();

	FbxGeometryElementNormal* pNormal = pMesh->GetElementNormal();
	if (nullptr == pNormal)
		return _vec3();

	_uint	iNormalIdx = 0;

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
			iNormalIdx = pNormal->GetIndexArray().GetAt(iIdx);
	}

	FbxVector4	vNormal = pNormal->GetDirectArray().GetAt(iNormalIdx);
	_vec3 vTemp = _vec3();

	vTemp.x = vNormal.mData[0];
	vTemp.y = vNormal.mData[2];
	vTemp.z = vNormal.mData[1];

	return vTemp;
}

_vec2 CFBXLoader::Get_UV(FbxMesh* pMesh, _uint iIdx, _uint iVtxOrder)
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

_vec3 CFBXLoader::Get_Tangent(FbxMesh* pMesh, _uint iIdx, _uint iVtxOrder)
{
	_uint		iTangentCnt = pMesh->GetElementTangentCount();

	if (iTangentCnt < 1)
		return _vec3();

	FbxGeometryElementTangent* pTangent = pMesh->GetElementTangent();
	if (nullptr == pTangent)
		return _vec3();

	_uint	iTangentIdx = 0;

	if (pTangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		if (pTangent->GetReferenceMode() == FbxGeometryElement::eDirect)
		{
			iTangentIdx = iVtxOrder;
		}
		else
		{
			iTangentIdx = pTangent->GetIndexArray().GetAt(iVtxOrder);
		}
	}
	else if (pTangent->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		if (pTangent->GetReferenceMode() == FbxGeometryElement::eDirect)
		{
			iTangentIdx = iIdx;
		}
		else
			iTangentIdx = pTangent->GetIndexArray().GetAt(iIdx);
	}

	FbxVector4	vTangent = pTangent->GetDirectArray().GetAt(iTangentIdx);
	_vec3 vTemp = _vec3();

	vTemp.x = vTangent.mData[0];
	vTemp.y = vTangent.mData[2];
	vTemp.z = vTangent.mData[1];

	return vTemp;
}

_vec3 CFBXLoader::Get_BiNormal(FbxMesh* pMesh, _uint iIdx, _uint iVtxOrder)
{
	_uint		iBinormalCnt = pMesh->GetElementBinormalCount();

	if (iBinormalCnt < 1)
		return _vec3();

	FbxGeometryElementBinormal* pBinormal = pMesh->GetElementBinormal();
	if (nullptr == pBinormal)
		return _vec3();

	_uint	iBinormalIdx = 0;

	if (pBinormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		if (pBinormal->GetReferenceMode() == FbxGeometryElement::eDirect)
		{
			iBinormalIdx = iVtxOrder;
		}
		else
		{
			iBinormalIdx = pBinormal->GetIndexArray().GetAt(iVtxOrder);
		}
	}
	else if (pBinormal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		if (pBinormal->GetReferenceMode() == FbxGeometryElement::eDirect)
		{
			iBinormalIdx = iIdx;
		}
		else
			iBinormalIdx = pBinormal->GetIndexArray().GetAt(iIdx);
	}

	FbxVector4	vBinormal = pBinormal->GetDirectArray().GetAt(iBinormalIdx);
	_vec3 vTemp = _vec3();

	vTemp.x = vBinormal.mData[0];
	vTemp.y = vBinormal.mData[2];
	vTemp.z = vBinormal.mData[1];

	return vTemp;
}

void CFBXLoader::CalculateTangentBinormal(MESH vPos0, MESH vPos1, MESH vPos2, _vec3& vTangent, _vec3& vBinormal)
{
	_float Vector1[3], Vector2[3];
	_float tuVector[2], tvVector[2];


	Vector1[0] = vPos1.vPosition.x - vPos0.vPosition.x;
	Vector1[1] = vPos1.vPosition.y - vPos0.vPosition.y;
	Vector1[2] = vPos1.vPosition.z - vPos0.vPosition.z;

	Vector2[0] = vPos2.vPosition.x - vPos0.vPosition.x;
	Vector2[1] = vPos2.vPosition.y - vPos0.vPosition.y;
	Vector2[2] = vPos2.vPosition.z - vPos0.vPosition.z;

	tuVector[0] = vPos1.vUV.x - vPos0.vUV.x;
	tvVector[0] = vPos1.vUV.y - vPos0.vUV.y;

	tuVector[1] = vPos2.vUV.x - vPos0.vUV.x;
	tvVector[1] = vPos2.vUV.y - vPos0.vUV.y;



	_float fDen = 1.f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

	vTangent.x = (tvVector[1] * Vector1[0] - tvVector[0] * Vector2[0]) * fDen;
	vTangent.y = (tvVector[1] * Vector1[1] - tvVector[0] * Vector2[1]) * fDen;
	vTangent.z = (tvVector[1] * Vector1[2] - tvVector[0] * Vector2[2]) * fDen;


	vBinormal.x = (tuVector[0] * Vector2[0] - tuVector[1] *	Vector1[0]) * fDen;
	vBinormal.y = (tuVector[0] * Vector2[1] - tuVector[1] * Vector1[1]) * fDen;
	vBinormal.z = (tuVector[0] * Vector2[2] - tuVector[1] * Vector1[2]) * fDen;


	_float fLen = sqrtf((vTangent.x * vTangent.x) + (vTangent.y * vTangent.y) + (vTangent.z * vTangent.z));

	vTangent.x = vTangent.x / fLen;
	vTangent.y = vTangent.y / fLen;
	vTangent.z = vTangent.z / fLen;

	fLen = sqrtf((vBinormal.x * vBinormal.x) + (vBinormal.y * vBinormal.y) + (vBinormal.z * vBinormal.z));

	vBinormal.x = vBinormal.x / fLen;
	vBinormal.y = vBinormal.y / fLen;
	vBinormal.z = vBinormal.z / fLen;
}

void CFBXLoader::CalculateNormal(_vec3 vTangent, _vec3 vBinormal, _vec3& vNormal)
{
	vNormal.x = (vTangent.y * vBinormal.z) - (vTangent.z * vBinormal.y);
	vNormal.y = (vTangent.z * vBinormal.x) - (vTangent.x * vBinormal.z);
	vNormal.z = (vTangent.x * vBinormal.y) - (vTangent.y * vBinormal.x);

	_float fLen = sqrtf((vNormal.x * vNormal.x) + (vNormal.y * vNormal.y) + (vNormal.z * vNormal.z));

	vNormal.x = vNormal.x / fLen;
	vNormal.y = vNormal.y / fLen;
	vNormal.z = vNormal.z / fLen;
}

HRESULT CFBXLoader::CreateBufferView(RenderInfo* pInfo)
{
	m_iNumVertices = pInfo->vecVertices.size();
	m_iStride = sizeof(MESH);


	D3D12_HEAP_PROPERTIES	tHeap_Pro_Default = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_HEAP_PROPERTIES	tHeap_Pro_Upload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	ID3D12Resource* pVertexBuffer = nullptr;
	ID3D12Resource* pVertexUploadBuffer = nullptr;

	CDevice::GetInstance()->Open();
	{
		D3D12_RESOURCE_DESC		tDesc = CD3DX12_RESOURCE_DESC::Buffer(m_iStride * m_iNumVertices);
		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Default, D3D12_HEAP_FLAG_NONE,
			&tDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&pVertexBuffer))))
			return E_FAIL;

		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Upload, D3D12_HEAP_FLAG_NONE,
			&tDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&pVertexUploadBuffer))))
			return E_FAIL;

		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = (void*)(pInfo->vecVertices.data());
		vertexData.RowPitch = m_iStride * m_iNumVertices;
		vertexData.SlicePitch = m_iStride * m_iNumVertices;


		D3D12_RESOURCE_BARRIER	tResource_Barrier = CD3DX12_RESOURCE_BARRIER::Transition(pVertexBuffer,
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		UpdateSubresources(CDevice::GetInstance()->GetCmdLst().Get(), pVertexBuffer, pVertexUploadBuffer, 0, 0, 1, &vertexData);

		CDevice::GetInstance()->GetCmdLst().Get()->ResourceBarrier(1, &tResource_Barrier);
	}
	CDevice::GetInstance()->Close();


	pInfo->VertexBufferView.BufferLocation = pVertexBuffer->GetGPUVirtualAddress();
	pInfo->VertexBufferView.StrideInBytes = m_iStride;
	pInfo->VertexBufferView.SizeInBytes = m_iStride * m_iNumVertices;

	CDevice::GetInstance()->WaitForFenceEvent();

	m_vecVertexBuffer.push_back(pVertexBuffer);
	m_vecVertexUploadBuffer.push_back(pVertexUploadBuffer);
	return S_OK;
}

HRESULT CFBXLoader::CreateBufferView_Index(RenderInfo* pInfo)
{
	m_iStride = sizeof(MESH);
	m_iNumIndices = pInfo->vecIndices.size();
	m_iNumVertices = pInfo->vecVertices.size();


	D3D12_HEAP_PROPERTIES	tHeap_Pro_Default = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_HEAP_PROPERTIES	tHeap_Pro_Upload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	ID3D12Resource* pVertexBuffer = nullptr;
	ID3D12Resource* pVertexUploadBuffer = nullptr;

	ID3D12Resource* pIndexBuffer = nullptr;
	ID3D12Resource* pIndexUploadBuffer = nullptr;

	CDevice::GetInstance()->Open();
	{
		D3D12_RESOURCE_DESC		tDesc = CD3DX12_RESOURCE_DESC::Buffer(m_iStride * m_iNumVertices);
		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Default, D3D12_HEAP_FLAG_NONE,
			&tDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&pVertexBuffer))))
			return E_FAIL;

		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Upload, D3D12_HEAP_FLAG_NONE,
			&tDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&pVertexUploadBuffer))))
			return E_FAIL;

		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = (void*)(pInfo->vecVertices.data());
		vertexData.RowPitch = m_iStride * m_iNumVertices;
		vertexData.SlicePitch = m_iStride * m_iNumVertices;


		D3D12_RESOURCE_BARRIER	tResource_Barrier = CD3DX12_RESOURCE_BARRIER::Transition(pVertexBuffer,
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		UpdateSubresources(CDevice::GetInstance()->GetCmdLst().Get(), pVertexBuffer, pVertexUploadBuffer, 0, 0, 1, &vertexData);

		CDevice::GetInstance()->GetCmdLst().Get()->ResourceBarrier(1, &tResource_Barrier);
	}
	{
		D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(_uint) * m_iNumIndices);

		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Default, D3D12_HEAP_FLAG_NONE,
			&tResource_Desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&pIndexBuffer))))
			return E_FAIL;
		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Upload, D3D12_HEAP_FLAG_NONE,
			&tResource_Desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&pIndexUploadBuffer))))
			return E_FAIL;

		D3D12_SUBRESOURCE_DATA indexData = {};
		indexData.pData = (void*)(pInfo->vecIndices.data());
		indexData.RowPitch = sizeof(_uint) * m_iNumIndices;
		indexData.SlicePitch = sizeof(_uint) * m_iNumIndices;

		UpdateSubresources(CDevice::GetInstance()->GetCmdLst().Get(), pIndexBuffer, pIndexUploadBuffer, 0, 0, 1, &indexData);
		D3D12_RESOURCE_BARRIER	tResource_Barrier = CD3DX12_RESOURCE_BARRIER::Transition(pIndexBuffer,
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);
		CDevice::GetInstance()->GetCmdLst()->ResourceBarrier(1, &tResource_Barrier);
	}
	CDevice::GetInstance()->Close();


	pInfo->VertexBufferView.BufferLocation = pVertexBuffer->GetGPUVirtualAddress();
	pInfo->VertexBufferView.StrideInBytes = m_iStride;
	pInfo->VertexBufferView.SizeInBytes = m_iStride * m_iNumVertices;

	pInfo->IndexBufferView.BufferLocation = pIndexBuffer->GetGPUVirtualAddress();
	pInfo->IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	pInfo->IndexBufferView.SizeInBytes = sizeof(_uint) * m_iNumIndices;

	m_vecVertexBuffer.push_back(pVertexBuffer);
	m_vecVertexUploadBuffer.push_back(pVertexUploadBuffer);
	m_vecIndexBuffer.push_back(pIndexBuffer);
	m_vecIndexUploadBuffer.push_back(pIndexUploadBuffer);

	CDevice::GetInstance()->WaitForFenceEvent();
	return S_OK;
}

CFBXLoader* CFBXLoader::Create(string strFilePath)
{
	CFBXLoader* pInstance = new CFBXLoader();
	if (FAILED(pInstance->Ready_FbxLoader(strFilePath)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

void CFBXLoader::Free()
{
	for (auto& iter : m_vecVertexBuffer)
		iter->Release();
	for (auto& iter : m_vecVertexUploadBuffer)
		iter->Release();
	for (auto& iter : m_vecIndexBuffer)
		iter->Release();
	for (auto& iter : m_vecIndexUploadBuffer)
		iter->Release();


	for (auto& iter : m_vecRenderInfo)
		Safe_Delete(iter);


	if (m_pScene)
	{
		m_pScene->Destroy();
		m_pScene = nullptr;
	}



}
