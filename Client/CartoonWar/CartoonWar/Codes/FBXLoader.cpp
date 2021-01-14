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

	m_pScene->GetGlobalSettings().SetAxisSystem(FbxAxisSystem::Max);



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
			if (pMesh)
			{
				RenderInfo* pInfo = new RenderInfo();

				_uint iMtrlCnt = pNode->GetMaterialCount();

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
	if (nullptr == pMesh)
		return E_FAIL;

	_uint	iVertexControlCnt = pMesh->GetControlPointsCount();

	vector<_vec3> vecControlPoint;
	for (_uint i = 0; i < iVertexControlCnt; ++i)
	{
		_vec3 vPos;
		vPos.x = (_float)(pMesh->GetControlPointAt(i).mData[0]);
		vPos.y = (_float)(pMesh->GetControlPointAt(i).mData[1]);
		vPos.z = (_float)(pMesh->GetControlPointAt(i).mData[2]);
		vecControlPoint.push_back(vPos);
	}

	_uint	iTriangleCnt = pMesh->GetPolygonCount();
	_uint	iVtxOrder = 0;

	for (_uint i = 0; i < iTriangleCnt; ++i)
	{
		for (_uint j = 0; j < 3; ++j)
		{
			_vec3 vPos = {};

			_uint	iControlPointIdx = pMesh->GetPolygonVertex(i, j);

			vPos.x = vecControlPoint[iControlPointIdx].x;
			vPos.y = vecControlPoint[iControlPointIdx].y;
			vPos.z = vecControlPoint[iControlPointIdx].z;

			FbxVector2 fbxTexCoord;
			FbxStringList UVSetNameList;


			_vec3 vNormal = Get_Normal(pMesh, iControlPointIdx, iVtxOrder);
			_vec2 vUV = Get_UV(pMesh, iControlPointIdx, pMesh->GetTextureUVIndex(i, j));


			pInfo->vecMeshData.push_back(MESH(vPos, vNormal, vUV));
			iVtxOrder++;
		}
	}


	//Create Buffer
	if (FAILED(CreateBufferView(pInfo->vecMeshData.size(), pInfo)))
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

	return S_OK;
}

HRESULT CFBXLoader::Load_Texture(RenderInfo* pInfo)
{
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

_vec3 CFBXLoader::Get_Normal(FbxMesh* pMesh, _uint iIdx, _uint iVtxOrder)
{
	_uint		iNormalCnt = pMesh->GetElementNormalCount();

	if (iNormalCnt != 1)
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
	vTemp.y = vNormal.mData[1];
	vTemp.z = vNormal.mData[2];

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

HRESULT CFBXLoader::CreateBufferView(_uint iNumVertices, RenderInfo* pInfo)
{
	m_iNumVertices = iNumVertices;
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
		//m_pVertexBuffer.Get()->SetName(L"VertexBuffer");

		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Upload, D3D12_HEAP_FLAG_NONE,
			&tDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&pVertexUploadBuffer))))
			return E_FAIL;
		//m_pVertexUploadBuffer.Get()->SetName(L"Upload");

		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = (void*)(pInfo->vecMeshData.data());
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
	for (auto& iter : m_vecRenderInfo)
		Safe_Delete(iter);
	if (m_pScene)
		m_pScene->Destroy();
}
