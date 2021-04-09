#include "framework.h"
#include "..\Headers\Mesh.h"
#include "Texture.h"
#include "StructedBuffer.h"


CMesh::CMesh()
	: CComponent()
{
}

CMesh::CMesh(const CMesh& rhs)
	: CComponent(rhs)
	, m_tRenderInfo(rhs.m_tRenderInfo)
	, m_iSubsetNum(rhs.m_iSubsetNum)
	, m_vecTexture(rhs.m_vecTexture)
	, m_iMaxTexNum(rhs.m_iMaxTexNum)
	, m_vecMTBone(rhs.m_vecMTBone)
	, m_vecMTAnimClip(rhs.m_vecMTAnimClip)
	, m_pBoneFrameData(rhs.m_pBoneFrameData)
	, m_pBoneOffset(rhs.m_pBoneOffset)
	, m_vecContainer(rhs.m_vecContainer)

{

	m_IsClone = true;


}

HRESULT CMesh::Ready_Mesh(const wstring& pFilePath, const _tchar* pSaveFilePath)
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


	string strFilePath(pFilePath.begin(), pFilePath.end());

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


	Load_Skeleton(m_pScene->GetRootNode());
	m_pScene->FillAnimStackNameArray(m_arrAnimName);
	Load_AnimationClip();
	Triangulate(m_pScene->GetRootNode());
	Load_MeshFromNode(m_pScene->GetRootNode());


	pFbxImporter->Destroy();
	pFbxImporter = nullptr;



	if (FAILED(Ready_MeshData(&m_vecContainer[0])))
		return E_FAIL;
	Load_Texture();


	if (pSaveFilePath)
	{
		if (FAILED(Save(pSaveFilePath)))
			return E_FAIL;
	}



	return S_OK;
}

void CMesh::Load_MeshFromNode(FbxNode* pNode)
{
	FbxNodeAttribute* pAttr = pNode->GetNodeAttribute();

	if (pAttr && FbxNodeAttribute::eMesh == pAttr->GetAttributeType())
	{
		FbxMesh* pMesh = pNode->GetMesh();
		if (NULL != pMesh)
			Load_Mesh(pMesh);
	}

	_uint	iMtrlCnt = pNode->GetMaterialCount();
	if (iMtrlCnt > 0)
	{
		for (_uint i = 0; i < iMtrlCnt; ++i)
		{
			FbxSurfaceMaterial* pMtrlSur = pNode->GetMaterial(i);
			Laod_Material(pMtrlSur);
		}
	}

	_uint	iChildCnt = pNode->GetChildCount();

	for (_uint i = 0; i < iChildCnt; ++i)
	{
		Load_MeshFromNode(pNode->GetChild(i));
	}

}

void CMesh::Load_Mesh(FbxMesh* pMesh)
{
	m_vecContainer.push_back(tContainer());
	tContainer& pContainer = m_vecContainer[m_vecContainer.size() - 1];

	string strName = pMesh->GetName();

	pContainer.strName = wstring(strName.begin(), strName.end());

	_uint iVtxCnt = pMesh->GetControlPointsCount();

	pContainer.Resize(iVtxCnt);

	FbxVector4* pFbxPos = pMesh->GetControlPoints();

	for (_uint i = 0; i < iVtxCnt; ++i)
	{
		pContainer.vecPos[i].x = (float)pFbxPos[i].mData[0];
		pContainer.vecPos[i].y = (float)pFbxPos[i].mData[2];
		pContainer.vecPos[i].z = (float)pFbxPos[i].mData[1];
	}

	// 폴리곤 개수
	_uint iPolyCnt = pMesh->GetPolygonCount();


	// Subset Num
	int iMtrlCnt = pMesh->GetNode()->GetMaterialCount();
	m_iSubsetNum = iMtrlCnt;
	pContainer.vecIdx.resize(iMtrlCnt);



	FbxGeometryElementMaterial* pMtrl = pMesh->GetElementMaterial();

	_uint iPolySize = pMesh->GetPolygonSize(0);
	if (3 != iPolySize)
		return;

	_uint	iArrIdx[3] = {};
	_uint	iVtxOrder = 0;


	for (_uint i = 0; i < iPolyCnt; ++i)
	{
		for (_uint j = 0; j < iPolySize; ++j)
		{
			_uint iIdx = pMesh->GetPolygonVertex(i, j);
			iArrIdx[j] = iIdx;

			GetTangent(pMesh, &pContainer, iIdx, iVtxOrder);
			GetBinormal(pMesh, &pContainer, iIdx, iVtxOrder);
			GetNormal(pMesh, &pContainer, iIdx, iVtxOrder);
			GetUV(pMesh, &pContainer, iIdx, pMesh->GetTextureUVIndex(i, j));
			++iVtxOrder;
		}

		_uint iSubsetIdx = pMtrl->GetIndexArray().GetAt(i);
		pContainer.vecIdx[iSubsetIdx].push_back(iArrIdx[0]);
		pContainer.vecIdx[iSubsetIdx].push_back(iArrIdx[2]);
		pContainer.vecIdx[iSubsetIdx].push_back(iArrIdx[1]);
	}

	Load_AnimationData(pMesh, &pContainer);
}

void CMesh::Laod_Material(FbxSurfaceMaterial* _pMtrlSur)
{
	tFbxMaterial tMtrlInfo{};

	string str = _pMtrlSur->GetName();
	tMtrlInfo.strMtrlName = wstring(str.begin(), str.end());

	// Diff
	tMtrlInfo.tMtrl.vMtrlDiff = GetMtrlData(_pMtrlSur
		, FbxSurfaceMaterial::sDiffuse
		, FbxSurfaceMaterial::sDiffuseFactor);

	// Amb
	tMtrlInfo.tMtrl.vMtrlAmb = GetMtrlData(_pMtrlSur
		, FbxSurfaceMaterial::sAmbient
		, FbxSurfaceMaterial::sAmbientFactor);

	// Spec
	tMtrlInfo.tMtrl.vMtrlSpec = GetMtrlData(_pMtrlSur
		, FbxSurfaceMaterial::sSpecular
		, FbxSurfaceMaterial::sSpecularFactor);

	// Emisv
	tMtrlInfo.tMtrl.vMtrlEmiv = GetMtrlData(_pMtrlSur
		, FbxSurfaceMaterial::sEmissive
		, FbxSurfaceMaterial::sEmissiveFactor);

	// Texture Name
	tMtrlInfo.strDiff = GetMtrlTextureName(_pMtrlSur, FbxSurfaceMaterial::sDiffuse);
	tMtrlInfo.strNormal = GetMtrlTextureName(_pMtrlSur, FbxSurfaceMaterial::sNormalMap);
	tMtrlInfo.strSpec = GetMtrlTextureName(_pMtrlSur, FbxSurfaceMaterial::sSpecular);

	m_vecContainer.back().vecMtrl.push_back(tMtrlInfo);
}

void CMesh::Load_Texture()
{
	// Texture Load

	/*for (UINT i = 0; i < m_vecContainer.size(); ++i)
	{
		for (UINT j = 0; j < m_vecContainer[i].vecMtrl.size(); ++j)
		{
			CTexture* pTexture = nullptr;
			wstring strPath;
			wstring strFileName;

			strPath = (m_vecContainer[i].vecMtrl[j].strDiff.c_str());
			if (!strPath.empty())
			{
				pTexture = CTexture::Create(L"Texture_Mesh", strPath.c_str());
				m_vecTexture.push_back(pTexture);
			}

			strPath = (m_vecContainer[i].vecMtrl[j].strNormal.c_str());
			if (!strPath.empty())
			{
				pTexture = CTexture::Create(L"Texture_Mesh", strPath.c_str());
				m_vecTexture.push_back(pTexture);

			}

			strPath = (m_vecContainer[i].vecMtrl[j].strSpec.c_str());
			if (!strPath.empty())
			{
				pTexture = CTexture::Create(L"Texture_Mesh", strPath.c_str());
				m_vecTexture.push_back(pTexture);
			}
		}
	}

	m_iMaxTexNum = m_vecTexture.size();*/
}

HRESULT CMesh::SetUp_Texture()
{
	if (m_iCurTexNum >= m_iMaxTexNum)
	{
		m_iCurTexNum = 0;
	}

	//CDevice::GetInstance()->SetTextureToShader(m_vecTexture[m_iCurTexNum], (TEXTURE_REGISTER)((_uint)(TEXTURE_REGISTER::t0)));
	//CDevice::GetInstance()->SetTextureToShader(m_vecTexture[m_iCurTexNum + 1], (TEXTURE_REGISTER)((_uint)(TEXTURE_REGISTER::t1)));
	//CDevice::GetInstance()->SetTextureToShader(m_vecTexture[m_iCurTexNum + 2], (TEXTURE_REGISTER)((_uint)(TEXTURE_REGISTER::t2)));


	//m_iCurTexNum+=3;
	return S_OK;
}

void CMesh::Load_Skeleton(FbxNode* pNode)
{
	_uint iChildCnt = pNode->GetChildCount();

	Load_Skeleton_(pNode, 0, 0, -1);
}

void CMesh::Load_Skeleton_(FbxNode* pNode, _int iDepth, _int iIdx, _int iParentIdx)
{
	FbxNodeAttribute* pAttr = pNode->GetNodeAttribute();
	if (pAttr && pAttr->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		tBone* pBone = new tBone;

		string strBoneName = pNode->GetName();
		pBone->strBoneName = wstring(strBoneName.begin(), strBoneName.end());

		pBone->iDepth = ++iDepth;
		pBone->iParentIndx = iParentIdx;
		m_vecBone.push_back(pBone);
	}

	_uint iChildCnt = pNode->GetChildCount();
	for (_uint i = 0; i < iChildCnt; ++i)
	{
		Load_Skeleton_(pNode->GetChild(i), iDepth, (int)m_vecBone.size(), iIdx);
	}

}

void CMesh::Load_AnimationClip()
{
	_uint iAnimCnt = m_arrAnimName.GetCount();

	for (_uint i = 0; i < iAnimCnt; ++i)
	{
		FbxAnimStack* pAnimStack = m_pScene->FindMember<FbxAnimStack>(m_arrAnimName[i]->Buffer());

		if (!pAnimStack)
			continue;

		tAnimClip* pAnimClip = new tAnimClip;

		string strClipName = pAnimStack->GetName();
		pAnimClip->strName = wstring(strClipName.begin(), strClipName.end());

		FbxTakeInfo* pTakeInfo = m_pScene->GetTakeInfo(pAnimStack->GetName());
		pAnimClip->tStartTime = pTakeInfo->mLocalTimeSpan.GetStart();
		pAnimClip->tEndTime = pTakeInfo->mLocalTimeSpan.GetStop();

		pAnimClip->eMode = m_pScene->GetGlobalSettings().GetTimeMode();
		pAnimClip->llTimeLength = pAnimClip->tEndTime.GetFrameCount(pAnimClip->eMode) - pAnimClip->tStartTime.GetFrameCount(pAnimClip->eMode);

		m_vecAnimClip.push_back(pAnimClip);
	}
}

void CMesh::Triangulate(FbxNode* pNode)
{
	FbxNodeAttribute* pAttr = pNode->GetNodeAttribute();

	if (pAttr &&
		(pAttr->GetAttributeType() == FbxNodeAttribute::eMesh
			|| pAttr->GetAttributeType() == FbxNodeAttribute::eNurbs
			|| pAttr->GetAttributeType() == FbxNodeAttribute::eNurbsSurface))
	{
		FbxGeometryConverter converter(g_FbxManager);
		converter.Triangulate(pAttr, true);
	}

	int iChildCount = pNode->GetChildCount();

	for (int i = 0; i < iChildCount; ++i)
	{
		Triangulate(pNode->GetChild(i));
	}
}

void CMesh::GetTangent(FbxMesh* pMesh, tContainer* pContainer, _int iIdx, _int iVtxOrder)
{
	int iTangentCnt = pMesh->GetElementTangentCount();
	if (1 != iTangentCnt)
		assert(NULL); // 정점 1개가 포함하는 탄젠트 정보가 2개 이상이다.
	if (iTangentCnt == 0)
		return;

	// 탄젠트 data 의 시작 주소
	FbxGeometryElementTangent* pTangent = pMesh->GetElementTangent();
	UINT iTangentIdx = 0;

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

	pContainer->vecTangent[iIdx].x = (float)vTangent.mData[0];
	pContainer->vecTangent[iIdx].y = (float)vTangent.mData[2];
	pContainer->vecTangent[iIdx].z = (float)vTangent.mData[1];
}

void CMesh::GetNormal(FbxMesh* pMesh, tContainer* pContainer, _int iIdx, _int iVtxOrder)
{
	int iTangentCnt = pMesh->GetNodeCount();
	if (1 != iTangentCnt)
		assert(NULL); // 정점 1개가 포함하는 탄젠트 정보가 2개 이상이다.

	// 탄젠트 data 의 시작 주소
	FbxGeometryElementNormal* pNormal = pMesh->GetElementNormal();
	UINT iNormalIdx = 0;

	if (pNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		if (pNormal->GetReferenceMode() == FbxGeometryElement::eDirect)
			iNormalIdx = iVtxOrder;
		else
			iNormalIdx = pNormal->GetIndexArray().GetAt(iVtxOrder);
	}
	else if (pNormal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		if (pNormal->GetReferenceMode() == FbxGeometryElement::eDirect)
			iNormalIdx = iIdx;
		else
			iNormalIdx = pNormal->GetIndexArray().GetAt(iIdx);
	}

	FbxVector4 vNormal = pNormal->GetDirectArray().GetAt(iNormalIdx);

	pContainer->vecNormal[iIdx].x = (float)vNormal.mData[0];
	pContainer->vecNormal[iIdx].y = (float)vNormal.mData[2];
	pContainer->vecNormal[iIdx].z = (float)vNormal.mData[1];
}

void CMesh::GetBinormal(FbxMesh* pMesh, tContainer* pContainer, _int iIdx, _int iVtxOrder)
{
	int iBinormalCnt = pMesh->GetElementBinormalCount();
	if (1 != iBinormalCnt)
		assert(NULL); // 정점 1개가 포함하는 종법선 정보가 2개 이상이다.
	if (iBinormalCnt == 0)
		return;

	// 종법선 data 의 시작 주소
	FbxGeometryElementBinormal* pBinormal = pMesh->GetElementBinormal();
	UINT iBinormalIdx = 0;

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

	pContainer->vecBinormal[iIdx].x = (float)vBinormal.mData[0];
	pContainer->vecBinormal[iIdx].y = (float)vBinormal.mData[2];
	pContainer->vecBinormal[iIdx].z = (float)vBinormal.mData[1];
}

void CMesh::GetUV(FbxMesh* pMesh, tContainer* pContainer, _int iIdx, _int iUVIndex)
{
	FbxGeometryElementUV* pUV = pMesh->GetElementUV();
	if (nullptr == pUV)
		return;

	UINT iUVIdx = 0;
	if (pUV->GetReferenceMode() == FbxGeometryElement::eDirect)
		iUVIdx = iIdx;
	else
		iUVIdx = pUV->GetIndexArray().GetAt(iIdx);

	iUVIdx = iUVIndex;
	FbxVector2 vUV = pUV->GetDirectArray().GetAt(iUVIdx);
	pContainer->vecUV[iIdx].x = (float)vUV.mData[0];
	pContainer->vecUV[iIdx].y = 1.f - (float)vUV.mData[1]; // fbx uv 좌표계는 좌하단이 0,0
}

void CMesh::Load_AnimationData(FbxMesh* _pMesh, tContainer* _pContainer)
{
	int iSkinCount = _pMesh->GetDeformerCount(FbxDeformer::eSkin);
	if (iSkinCount <= 0 || m_vecAnimClip.empty())
		return;

	_pContainer->bAnimation = true;

	// Skin 개수만큼 반복을하며 읽는다.	
	for (int i = 0; i < iSkinCount; ++i)
	{
		FbxSkin* pSkin = (FbxSkin*)_pMesh->GetDeformer(i, FbxDeformer::eSkin);

		if (pSkin)
		{
			FbxSkin::EType eType = pSkin->GetSkinningType();
			if (FbxSkin::eRigid == eType || FbxSkin::eLinear)
			{
				// Cluster 를 얻어온다
				// Cluster == Joint == 관절
				int iClusterCount = pSkin->GetClusterCount();

				for (int j = 0; j < iClusterCount; ++j)
				{
					FbxCluster* pCluster = pSkin->GetCluster(j);

					if (!pCluster->GetLink())
						continue;

					// 현재 본 인덱스를 얻어온다.
					int iBoneIdx = FindBoneIndex(pCluster->GetLink()->GetName());
					if (-1 == iBoneIdx)
						assert(NULL);

					FbxAMatrix matNodeTransform = GetTransform(_pMesh->GetNode());

					// Weights And Indices 정보를 읽는다.
					LoadWeightsAndIndices(pCluster, iBoneIdx, _pContainer);

					// Bone 의 OffSet 행렬 구한다.
					LoadOffsetMatrix(pCluster, matNodeTransform, iBoneIdx, _pContainer);

					// Bone KeyFrame 별 행렬을 구한다.
					LoadKeyframeTransform(_pMesh->GetNode(), pCluster, matNodeTransform, iBoneIdx, _pContainer);
				}
			}
		}
	}
	CheckWeightAndIndices(_pMesh, _pContainer);
}

void CMesh::LoadWeightsAndIndices(FbxCluster* _pCluster, int _iBoneIdx, tContainer* _pContainer)
{
	int iIndicesCount = _pCluster->GetControlPointIndicesCount();

	for (int i = 0; i < iIndicesCount; ++i)
	{
		tWeightsAndIndices tWI = {};

		// 각 정점에게 본 인덱스 정보와, 가중치 값을 알린다.
		tWI.iBoneIdx = _iBoneIdx;
		tWI.dWeight = _pCluster->GetControlPointWeights()[i];

		int iVtxIdx = _pCluster->GetControlPointIndices()[i];

		_pContainer->vecWI[iVtxIdx].push_back(tWI);
	}
	_int i = 0;
}

void CMesh::LoadOffsetMatrix(FbxCluster* _pCluster, const FbxAMatrix& _matNodeTransform, int _iBoneIdx, tContainer* _pContainer)
{
	FbxAMatrix matClusterTrans;
	FbxAMatrix matClusterLinkTrans;

	_pCluster->GetTransformMatrix(matClusterTrans);
	_pCluster->GetTransformLinkMatrix(matClusterLinkTrans);

	// Reflect Matrix
	FbxVector4 V0 = { 1, 0, 0, 0 };
	FbxVector4 V1 = { 0, 0, 1, 0 };
	FbxVector4 V2 = { 0, 1, 0, 0 };
	FbxVector4 V3 = { 0, 0, 0, 1 };

	FbxAMatrix matReflect;
	matReflect[0] = V0;
	matReflect[1] = V1;
	matReflect[2] = V2;
	matReflect[3] = V3;

	FbxAMatrix matOffset;
	matOffset = matClusterLinkTrans.Inverse() * matClusterTrans * _matNodeTransform;
	matOffset = matReflect * matOffset * matReflect;

	m_vecBone[_iBoneIdx]->matOffset = matOffset;
}

void CMesh::LoadKeyframeTransform(FbxNode* _pNode, FbxCluster* _pCluster, const FbxAMatrix& _matNodeTransform, int _iBoneIdx, tContainer* _pContainer)
{
	if (m_vecAnimClip.empty())
		return;

	FbxVector4	v1 = { 1, 0, 0, 0 };
	FbxVector4	v2 = { 0, 0, 1, 0 };
	FbxVector4	v3 = { 0, 1, 0, 0 };
	FbxVector4	v4 = { 0, 0, 0, 1 };
	FbxAMatrix	matReflect;
	matReflect.mData[0] = v1;
	matReflect.mData[1] = v2;
	matReflect.mData[2] = v3;
	matReflect.mData[3] = v4;

	m_vecBone[_iBoneIdx]->matBone = _matNodeTransform;

	FbxTime::EMode eTimeMode = m_pScene->GetGlobalSettings().GetTimeMode();

	FbxLongLong llStartFrame = m_vecAnimClip[0]->tStartTime.GetFrameCount(eTimeMode);
	FbxLongLong llEndFrame = m_vecAnimClip[0]->tEndTime.GetFrameCount(eTimeMode);

	for (FbxLongLong i = llStartFrame; i < llEndFrame; ++i)
	{
		tKeyFrame tFrame = {};
		FbxTime   tTime = 0;

		tTime.SetFrame(i, eTimeMode);

		FbxAMatrix matFromNode = _pNode->EvaluateGlobalTransform(tTime) * _matNodeTransform;
		FbxAMatrix matCurTrans = matFromNode.Inverse() * _pCluster->GetLink()->EvaluateGlobalTransform(tTime);
		matCurTrans = matReflect * matCurTrans * matReflect;

		tFrame.dTime = tTime.GetSecondDouble();
		tFrame.matTransform = matCurTrans;

		m_vecBone[_iBoneIdx]->vecKeyFrame.push_back(tFrame);
	}
}

int CMesh::FindBoneIndex(string _strBoneName)
{
	wstring strBoneName = wstring(_strBoneName.begin(), _strBoneName.end());

	for (UINT i = 0; i < m_vecBone.size(); ++i)
	{
		if (m_vecBone[i]->strBoneName == strBoneName)
			return i;
	}

	return -1;
}

FbxAMatrix CMesh::GetTransform(FbxNode* _pNode)
{
	const FbxVector4 vT = _pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 vR = _pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 vS = _pNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(vT, vR, vS);
}

_bool CMesh::IsAnimation()
{
	return !m_vecMTAnimClip.empty();
}

void CMesh::CheckWeightAndIndices(FbxMesh* _pMesh, tContainer* _pContainer)
{
	vector<vector<tWeightsAndIndices>>::iterator iter = _pContainer->vecWI.begin();

	int iVtxIdx = 0;
	for (iter; iter != _pContainer->vecWI.end(); ++iter, ++iVtxIdx)
	{
		if ((*iter).size() > 1)
		{
			// 가중치 값 순으로 내림차순 정렬
			sort((*iter).begin(), (*iter).end()
				, [](const tWeightsAndIndices& left, const tWeightsAndIndices& right)
				{
					return left.dWeight > right.dWeight;
				}
			);

			double dWeight = 0.f;
			for (UINT i = 0; i < (*iter).size(); ++i)
			{
				dWeight += (*iter)[i].dWeight;
			}

			// 가중치의 합이 1이 넘어가면 처음부분에 더해준다.
			double revision = 0.f;
			if (dWeight > 1.0)
			{
				revision = 1.0 - dWeight;
				(*iter)[0].dWeight += revision;
			}

			if ((*iter).size() >= 4)
			{
				(*iter).erase((*iter).begin() + 4, (*iter).end());
			}
		}

		// 정점 정보로 변환, 
		float fWeights[4] = {};
		float fIndices[4] = {};

		for (UINT i = 0; i < (*iter).size(); ++i)
		{
			fWeights[i] = (float)(*iter)[i].dWeight;
			fIndices[i] = (float)(*iter)[i].iBoneIdx;
		}

		memcpy(&_pContainer->vecWeights[iVtxIdx], fWeights, sizeof(_vec4));
		memcpy(&_pContainer->vecIndices[iVtxIdx], fIndices, sizeof(_vec4));
	}
	_int i = 0;
}

_vec4 CMesh::GetMtrlData(FbxSurfaceMaterial* _pSurface, const char* _pMtrlName, const char* _pMtrlFactorName)
{
	FbxDouble3  vMtrl;
	FbxDouble	dFactor = 0.f;

	FbxProperty tMtrlProperty = _pSurface->FindProperty(_pMtrlName);
	FbxProperty tMtrlFactorProperty = _pSurface->FindProperty(_pMtrlFactorName);

	if (tMtrlProperty.IsValid() && tMtrlFactorProperty.IsValid())
	{
		vMtrl = tMtrlProperty.Get<FbxDouble3>();
		dFactor = tMtrlFactorProperty.Get<FbxDouble>();
	}

	_vec4 vRetVal = _vec4(vMtrl.mData[0] * dFactor, vMtrl.mData[1] * dFactor, vMtrl.mData[2] * dFactor, dFactor);
	return vRetVal;
}

wstring CMesh::GetMtrlTextureName(FbxSurfaceMaterial* _pSurface, const char* _pMtrlProperty)
{
	string strName;

	FbxProperty TextureProperty = _pSurface->FindProperty(_pMtrlProperty);
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

HRESULT CMesh::Ready_MeshData(tContainer* pContainer)
{
	ComPtr<ID3D12Resource> pVB = nullptr;
	D3D12_VERTEX_BUFFER_VIEW tVtxView = {};
	UINT iVtxCount = (UINT)pContainer->vecPos.size();
	UINT iVtxSize = sizeof(MESH);

	//XMFLOAT3		vPosition;
	//XMFLOAT4		vColor;
	//XMFLOAT2		vUV;
	//XMFLOAT3		vNormal;
	//XMFLOAT3		vTangent;
	//XMFLOAT3		vBinormal;
	//XMFLOAT4		vWeight;
	//XMFLOAT4		vIndices;

	//VTX* pSysMem = new VTX[iVtxCount];
	vector<MESH>		vecMesh;
	vecMesh.resize(iVtxCount);
	for (UINT i = 0; i < iVtxCount; ++i)
	{
		vecMesh[i].vPosition = pContainer->vecPos[i];
		vecMesh[i].vColor = _vec4(1.f, 0.f, 1.f, 1.f);
		vecMesh[i].vUV = pContainer->vecUV[i];
		vecMesh[i].vNormal = pContainer->vecNormal[i];
		vecMesh[i].vTangent = pContainer->vecTangent[i];
		vecMesh[i].vBinormal = pContainer->vecBinormal[i];
		vecMesh[i].vWeight = pContainer->vecWeights[i];
		vecMesh[i].vIndices = pContainer->vecIndices[i];
	}

	D3D12_HEAP_PROPERTIES tHeapProperty = {};
	tHeapProperty.Type = D3D12_HEAP_TYPE_UPLOAD;
	tHeapProperty.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	tHeapProperty.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	tHeapProperty.CreationNodeMask = 1;
	tHeapProperty.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC tResDesc = {};
	tResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	tResDesc.Alignment = 0;
	tResDesc.Width = iVtxSize * iVtxCount;
	tResDesc.Height = 1;
	tResDesc.DepthOrArraySize = 1;
	tResDesc.MipLevels = 1;
	tResDesc.Format = DXGI_FORMAT_UNKNOWN;
	tResDesc.SampleDesc.Count = 1;
	tResDesc.SampleDesc.Quality = 0;
	tResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	tResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(
		&tHeapProperty,
		D3D12_HEAP_FLAG_NONE,
		&tResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&pVB))))
		return E_FAIL;

	UINT8* pVertexDataBegin = nullptr;
	D3D12_RANGE readRange{ 0, 0 }; // We do not intend to read from this resource on the CPU.	
	pVB->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
	memcpy(pVertexDataBegin, vecMesh.data(), (tResDesc.Width * tResDesc.Height));
	pVB->Unmap(0, nullptr);

	tVtxView.BufferLocation = pVB->GetGPUVirtualAddress();
	tVtxView.StrideInBytes = sizeof(MESH);
	tVtxView.SizeInBytes = (UINT)tResDesc.Width;

	m_tRenderInfo.pVB = pVB;
	m_tRenderInfo.iVtxSize = iVtxSize;
	m_tRenderInfo.iVtxCnt = iVtxCount;
	m_tRenderInfo.VertexBufferView = tVtxView;
	m_tRenderInfo.vecVertices = vecMesh;


	UINT iIdxBufferCount = (UINT)pContainer->vecIdx.size();
	//m_tRenderInfo.vecIdx = pContainer->vecIdx;

	for (UINT i = 0; i < iIdxBufferCount; ++i)
	{
		Indices	tIndices;
		tIndices.iIndexCnt = (UINT)pContainer->vecIdx[i].size();
		tIndices.eFormat = DXGI_FORMAT_R32_UINT;
		tIndices.pSystem = malloc(GetSizeofFormat(tIndices.eFormat) * tIndices.iIndexCnt);
		memcpy(tIndices.pSystem, &pContainer->vecIdx[i][0], GetSizeofFormat(tIndices.eFormat) * tIndices.iIndexCnt);


		tHeapProperty = {};
		tHeapProperty.Type = D3D12_HEAP_TYPE_UPLOAD;
		tHeapProperty.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		tHeapProperty.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		tHeapProperty.CreationNodeMask = 1;
		tHeapProperty.VisibleNodeMask = 1;

		tResDesc = {};
		tResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		tResDesc.Alignment = 0;
		tResDesc.Width = GetSizeofFormat(tIndices.eFormat) * tIndices.iIndexCnt;
		tResDesc.Height = 1;
		tResDesc.DepthOrArraySize = 1;
		tResDesc.MipLevels = 1;
		tResDesc.Format = DXGI_FORMAT_UNKNOWN;
		tResDesc.SampleDesc.Count = 1;
		tResDesc.SampleDesc.Quality = 0;
		tResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		tResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;


		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(
			&tHeapProperty,
			D3D12_HEAP_FLAG_NONE,
			&tResDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&tIndices.pIB))))
			return E_FAIL;

		// Copy the Index data to the Index buffer.
		UINT8* pIdxDataBegin = nullptr;
		readRange = D3D12_RANGE{ 0, 0 }; // We do not intend to read from this resource on the CPU.	
		tIndices.pIB->Map(0, &readRange, reinterpret_cast<void**>(&pIdxDataBegin));
		memcpy(pIdxDataBegin, tIndices.pSystem, (tResDesc.Width * tResDesc.Height));
		tIndices.pIB->Unmap(0, nullptr);

		// Initialize the Index buffer view.
		tIndices.IndexBufferView.BufferLocation = tIndices.pIB->GetGPUVirtualAddress();
		tIndices.IndexBufferView.Format = tIndices.eFormat;
		tIndices.IndexBufferView.SizeInBytes = (UINT)(tResDesc.Width * tResDesc.Height);

		m_tRenderInfo.vecIndices.push_back(tIndices);
	}

	if (!pContainer->bAnimation)
		return S_OK;

	m_vecBone;
	_uint iFrameCnt = 0;

	for (_uint i = 0; i < m_vecBone.size(); ++i)
	{
		tMTBone bone = {};

		bone.iDepth = m_vecBone[i]->iDepth;
		bone.iParentIndx = m_vecBone[i]->iParentIndx;
		bone.matBone = GetMatrix(m_vecBone[i]->matBone);
		bone.matOffset = GetMatrix(m_vecBone[i]->matOffset);
		bone.strBoneName = m_vecBone[i]->strBoneName;

		for (UINT j = 0; j < m_vecBone[i]->vecKeyFrame.size(); ++j)
		{
			tMTKeyFrame tKeyframe = {};
			tKeyframe.dTime = m_vecBone[i]->vecKeyFrame[j].dTime;
			tKeyframe.iFrame = j;
			tKeyframe.vTranslate.x = (float)m_vecBone[i]->vecKeyFrame[j].matTransform.GetT().mData[0];
			tKeyframe.vTranslate.y = (float)m_vecBone[i]->vecKeyFrame[j].matTransform.GetT().mData[1];
			tKeyframe.vTranslate.z = (float)m_vecBone[i]->vecKeyFrame[j].matTransform.GetT().mData[2];

			tKeyframe.vScale.x = (float)m_vecBone[i]->vecKeyFrame[j].matTransform.GetS().mData[0];
			tKeyframe.vScale.y = (float)m_vecBone[i]->vecKeyFrame[j].matTransform.GetS().mData[1];
			tKeyframe.vScale.z = (float)m_vecBone[i]->vecKeyFrame[j].matTransform.GetS().mData[2];

			tKeyframe.qRot.x = (float)m_vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[0];
			tKeyframe.qRot.y = (float)m_vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[1];
			tKeyframe.qRot.z = (float)m_vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[2];
			tKeyframe.qRot.w = (float)m_vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[3];

			bone.vecKeyFrame.push_back(tKeyframe);
		}
		iFrameCnt = (UINT)max(iFrameCnt, bone.vecKeyFrame.size());

		m_vecMTBone.push_back(bone);
	}

	for (_uint i = 0; i < m_vecAnimClip.size(); ++i)
	{
		tMTAnimClip tClip = {};

		tClip.strAnimName = m_vecAnimClip[i]->strName;
		tClip.dStartTime = m_vecAnimClip[i]->tStartTime.GetSecondDouble();
		tClip.dEndTime = m_vecAnimClip[i]->tEndTime.GetSecondDouble();
		tClip.dTimeLength = tClip.dEndTime - tClip.dStartTime;

		tClip.iStartFrame = (int)m_vecAnimClip[i]->tStartTime.GetFrameCount(m_vecAnimClip[i]->eMode);
		tClip.iEndFrame = (int)m_vecAnimClip[i]->tEndTime.GetFrameCount(m_vecAnimClip[i]->eMode);
		tClip.iFrameLength = tClip.iEndFrame - tClip.iStartFrame;
		tClip.eMode = m_vecAnimClip[i]->eMode;

		m_vecMTAnimClip.push_back(tClip);
	}



	if (IsAnimation())
	{
		vector<_matrix> vecOffset;
		vector<tFrameTrans> vecFrameTrans;
		vecFrameTrans.resize((UINT)m_vecMTBone.size() * iFrameCnt);

		for (size_t i = 0; i < m_vecMTBone.size(); ++i)
		{
			vecOffset.push_back(m_vecMTBone[i].matOffset);

			for (size_t j = 0; j < m_vecMTBone[i].vecKeyFrame.size(); ++j)
			{
				_vec4 vecTemp = _vec4(m_vecMTBone[i].vecKeyFrame[j].vTranslate.x, m_vecMTBone[i].vecKeyFrame[j].vTranslate.y, m_vecMTBone[i].vecKeyFrame[j].vTranslate.z, 0.f);
				_vec4 vecTemp_ = _vec4(m_vecMTBone[i].vecKeyFrame[j].vScale.x, m_vecMTBone[i].vecKeyFrame[j].vScale.y, m_vecMTBone[i].vecKeyFrame[j].vScale.z, 0.f);
				vecFrameTrans[(UINT)m_vecMTBone.size() * j + i]
					= tFrameTrans{ vecTemp,vecTemp_,
					m_vecMTBone[i].vecKeyFrame[j].qRot
				};
			}
		}

		m_pBoneOffset = CStructedBuffer::Create(sizeof(Matrix), (UINT)vecOffset.size(), vecOffset.data());
		m_pBoneFrameData = CStructedBuffer::Create(sizeof(tFrameTrans), (UINT)vecOffset.size() * iFrameCnt, vecFrameTrans.data());


	}



	return S_OK;
}

HRESULT CMesh::Save(const _tchar* pFilePath)
{
	FILE* pFile = nullptr;
	errno_t err = _wfopen_s(&pFile, pFilePath, L"wb");

	UINT iVtxCount = m_tRenderInfo.iVtxCnt;
	UINT iVtxSize = m_tRenderInfo.iVtxSize;

	fwrite(&iVtxCount, sizeof(int), 1, pFile);
	fwrite(&iVtxSize, sizeof(int), 1, pFile);

	for (auto& iter : m_tRenderInfo.vecVertices)
	{
		fwrite(&iter, sizeof(MESH), 1, pFile);
	}

	_uint	iMtrlCnt = m_tRenderInfo.vecIndices.size();
	fwrite(&iMtrlCnt, sizeof(int), 1, pFile);

	_uint	iIdxBufferSize = 0;

	for (_uint i = 0; i < iMtrlCnt; ++i)
	{
		fwrite(&m_tRenderInfo.vecIndices[i], sizeof(Indices), 1, pFile);
		fwrite(m_tRenderInfo.vecIndices[i].pSystem, m_tRenderInfo.vecIndices[i].iIndexCnt * GetSizeofFormat(m_tRenderInfo.vecIndices[i].eFormat), 1, pFile);
	}
	_uint iAnimCnt = m_vecMTAnimClip.size();
	fwrite(&iAnimCnt, sizeof(int), 1, pFile);

	for (_uint i = 0; i < iAnimCnt; ++i)
	{
		fwrite(&m_vecMTAnimClip[i].dStartTime, sizeof(double), 1, pFile);
		fwrite(&m_vecMTAnimClip[i].dEndTime, sizeof(double), 1, pFile);
		fwrite(&m_vecMTAnimClip[i].dTimeLength, sizeof(double), 1, pFile);
		fwrite(&m_vecMTAnimClip[i].eMode, sizeof(int), 1, pFile);
		fwrite(&m_vecMTAnimClip[i].fUpdateTime, sizeof(float), 1, pFile);
		fwrite(&m_vecMTAnimClip[i].iStartFrame, sizeof(int), 1, pFile);
		fwrite(&m_vecMTAnimClip[i].iEndFrame, sizeof(int), 1, pFile);
		fwrite(&m_vecMTAnimClip[i].iFrameLength, sizeof(int), 1, pFile);
	}

	_uint iBoneCnt = m_vecMTBone.size();

	fwrite(&iBoneCnt, sizeof(int), 1, pFile);

	for (_uint i = 0; i < iBoneCnt; ++i)
	{
		fwrite(&m_vecMTBone[i].iDepth, sizeof(int), 1, pFile);
		fwrite(&m_vecMTBone[i].iParentIndx, sizeof(int), 1, pFile);
		fwrite(&m_vecMTBone[i].matBone, sizeof(Matrix), 1, pFile);
		fwrite(&m_vecMTBone[i].matOffset, sizeof(Matrix), 1, pFile);

		int iFrameCount = (int)m_vecMTBone[i].vecKeyFrame.size();
		fwrite(&iFrameCount, sizeof(int), 1, pFile);

		for (int j = 0; j < m_vecMTBone[i].vecKeyFrame.size(); ++j)
		{
			fwrite(&m_vecMTBone[i].vecKeyFrame[j], sizeof(tMTKeyFrame), 1, pFile);
		}

	}

	fclose(pFile);


	return S_OK;
}

HRESULT CMesh::Load(const _tchar* pFilePath)
{
	FILE* pFile = nullptr;
	errno_t err = _wfopen_s(&pFile, pFilePath, L"rb");

	fread(&m_tRenderInfo.iVtxCnt, sizeof(int), 1, pFile);
	fread(&m_tRenderInfo.iVtxSize, sizeof(int), 1, pFile);

	m_tRenderInfo.vecVertices.resize(m_tRenderInfo.iVtxCnt);

	for (_uint i = 0; i < (_uint)m_tRenderInfo.iVtxCnt; ++i)
		fread(&m_tRenderInfo.vecVertices[i], sizeof(MESH), 1, pFile);		


	CDevice::GetInstance()->Open();
	D3D12_HEAP_PROPERTIES tHeapProperty = {};
	tHeapProperty.Type = D3D12_HEAP_TYPE_UPLOAD;
	tHeapProperty.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	tHeapProperty.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	tHeapProperty.CreationNodeMask = 1;
	tHeapProperty.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC tResDesc = {};
	tResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	tResDesc.Alignment = 0;
	tResDesc.Width = m_tRenderInfo.iVtxSize * m_tRenderInfo.iVtxCnt;
	tResDesc.Height = 1;
	tResDesc.DepthOrArraySize = 1;
	tResDesc.MipLevels = 1;
	tResDesc.Format = DXGI_FORMAT_UNKNOWN;
	tResDesc.SampleDesc.Count = 1;
	tResDesc.SampleDesc.Quality = 0;
	tResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	tResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;


	CDevice::GetInstance()->Open();
	CDevice::GetInstance()->GetDevice()->CreateCommittedResource(
		&tHeapProperty,
		D3D12_HEAP_FLAG_NONE,
		&tResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_tRenderInfo.pVB));
	CDevice::GetInstance()->Close();
	CDevice::GetInstance()->WaitForFenceEvent();



	// Copy the triangle data to the vertex buffer.
	UINT8* pVertexDataBegin = nullptr;
	D3D12_RANGE readRange{ 0, 0 }; // We do not intend to read from this resource on the CPU.	
	m_tRenderInfo.pVB->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
	memcpy(pVertexDataBegin, m_tRenderInfo.vecVertices.data(), (tResDesc.Width * tResDesc.Height));
	m_tRenderInfo.pVB->Unmap(0, nullptr);

	// Initialize the vertex buffer view.
	m_tRenderInfo.VertexBufferView.BufferLocation = m_tRenderInfo.pVB->GetGPUVirtualAddress();
	m_tRenderInfo.VertexBufferView.StrideInBytes = sizeof(MESH);
	m_tRenderInfo.VertexBufferView.SizeInBytes = (UINT)tResDesc.Width;

	_uint iMtrlCount = 0;
	fread(&iMtrlCount, sizeof(int), 1, pFile);

	//m_tRenderInfo.vecIndices.resize(iMtrlCount);
	m_iSubsetNum = iMtrlCount;
	for (_uint i = 0; i < iMtrlCount; ++i)
	{
		Indices info = {};
		fread(&info, sizeof(Indices), 1, pFile);
		_uint iByteWidth = info.iIndexCnt * GetSizeofFormat(info.eFormat);
		
		void* pSysMem = malloc(iByteWidth);
		info.pSystem = pSysMem;

		fread(info.pSystem, iByteWidth, 1, pFile);


		tHeapProperty = {};
		tHeapProperty.Type = D3D12_HEAP_TYPE_UPLOAD;
		tHeapProperty.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		tHeapProperty.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		tHeapProperty.CreationNodeMask = 1;
		tHeapProperty.VisibleNodeMask = 1;

		tResDesc = {};
		tResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		tResDesc.Alignment = 0;
		tResDesc.Width = GetSizeofFormat(info.eFormat) * info.iIndexCnt;
		tResDesc.Height = 1;
		tResDesc.DepthOrArraySize = 1;
		tResDesc.MipLevels = 1;
		tResDesc.Format = DXGI_FORMAT_UNKNOWN;
		tResDesc.SampleDesc.Count = 1;
		tResDesc.SampleDesc.Quality = 0;
		tResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		tResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		memset(&info, 0, sizeof(info.pIB));
		CDevice::GetInstance()->Open();

		CDevice::GetInstance()->GetDevice()->CreateCommittedResource(
			&tHeapProperty,
			D3D12_HEAP_FLAG_NONE,
			&tResDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&info.pIB));



		CDevice::GetInstance()->Close();
		CDevice::GetInstance()->WaitForFenceEvent();


		UINT8* pIdxDataBegin = nullptr;
		readRange = D3D12_RANGE{ 0, 0 }; // We do not intend to read from this resource on the CPU.	
		info.pIB->Map(0, &readRange, reinterpret_cast<void**>(&pIdxDataBegin));
		memcpy(pIdxDataBegin, info.pSystem, (tResDesc.Width * tResDesc.Height));
		info.pIB->Unmap(0, nullptr);

		// Initialize the Index buffer view.
		info.IndexBufferView.BufferLocation = info.pIB->GetGPUVirtualAddress();
		info.IndexBufferView.Format = info.eFormat;
		info.IndexBufferView.SizeInBytes = (UINT)(tResDesc.Width * tResDesc.Height);

		m_tRenderInfo.vecIndices.push_back(info);
	}

	int iCount = 0;
	fread(&iCount, sizeof(int), 1, pFile);
	for (int i = 0; i < iCount; ++i)
	{
		tMTAnimClip tClip = {};

		fread(&tClip.dStartTime, sizeof(double), 1, pFile);
		fread(&tClip.dEndTime, sizeof(double), 1, pFile);
		fread(&tClip.dTimeLength, sizeof(double), 1, pFile);
		fread(&tClip.eMode, sizeof(int), 1, pFile);
		fread(&tClip.fUpdateTime, sizeof(float), 1, pFile);
		fread(&tClip.iStartFrame, sizeof(int), 1, pFile);
		fread(&tClip.iEndFrame, sizeof(int), 1, pFile);
		fread(&tClip.iFrameLength, sizeof(int), 1, pFile);

		m_vecMTAnimClip.push_back(tClip);
	}
	iCount = 0;
	fread(&iCount, sizeof(int), 1, pFile);
	m_vecMTBone.resize(iCount);


	UINT _iFrameCount = 0;
	for (int i = 0; i < iCount; ++i)
	{

		fread(&m_vecMTBone[i].iDepth, sizeof(int), 1, pFile);
		fread(&m_vecMTBone[i].iParentIndx, sizeof(int), 1, pFile);
		fread(&m_vecMTBone[i].matBone, sizeof(Matrix), 1, pFile);
		fread(&m_vecMTBone[i].matOffset, sizeof(Matrix), 1, pFile);

		UINT iFrameCount = 0;
		fread(&iFrameCount, sizeof(int), 1, pFile);
		m_vecMTBone[i].vecKeyFrame.resize(iFrameCount);
		_iFrameCount = (UINT)max(_iFrameCount, iFrameCount);
		for (UINT j = 0; j < iFrameCount; ++j)
		{
			fread(&m_vecMTBone[i].vecKeyFrame[j], sizeof(tMTKeyFrame), 1, pFile);
		}
	}

	if (m_vecMTBone.size() > 0 && m_vecMTBone.size() > 0)
	{
		// BoneOffet 행렬
		vector<Matrix> vecOffset;
		vector<tFrameTrans> vecFrameTrans;
		vecFrameTrans.resize((UINT)m_vecMTBone.size() * _iFrameCount);

		for (size_t i = 0; i < m_vecMTBone.size(); ++i)
		{
			vecOffset.push_back(m_vecMTBone[i].matOffset);

			for (size_t j = 0; j < m_vecMTBone[i].vecKeyFrame.size(); ++j)
			{
				_vec4 vecTemp = _vec4(m_vecMTBone[i].vecKeyFrame[j].vTranslate.x, m_vecMTBone[i].vecKeyFrame[j].vTranslate.y, m_vecMTBone[i].vecKeyFrame[j].vTranslate.z, 0.f);
				_vec4 vecTemp_ = _vec4(m_vecMTBone[i].vecKeyFrame[j].vScale.x, m_vecMTBone[i].vecKeyFrame[j].vScale.y, m_vecMTBone[i].vecKeyFrame[j].vScale.z, 0.f);
				vecFrameTrans[(UINT)m_vecMTBone.size() * j + i]
					= tFrameTrans{ vecTemp,vecTemp_,
					m_vecMTBone[i].vecKeyFrame[j].qRot
				};
			}
		}
		m_pBoneOffset = CStructedBuffer::Create(sizeof(Matrix), (UINT)vecOffset.size(), vecOffset.data());
		m_pBoneFrameData = CStructedBuffer::Create(sizeof(tFrameTrans), (UINT)vecOffset.size() * _iFrameCount, vecFrameTrans.data());
	}

	fclose(pFile);

	return S_OK;
}

void CMesh::Render_Mesh(_uint iIdx)
{
	CDevice::GetInstance()->GetCmdLst()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CDevice::GetInstance()->GetCmdLst()->IASetVertexBuffers(0, 1, &m_tRenderInfo.VertexBufferView);
	CDevice::GetInstance()->GetCmdLst()->IASetIndexBuffer(&m_tRenderInfo.vecIndices[iIdx].IndexBufferView);
	CDevice::GetInstance()->GetCmdLst()->DrawIndexedInstanced(m_tRenderInfo.vecIndices[iIdx].iIndexCnt, 1, 0, 0, 0);

}


CMesh* CMesh::Create(const wstring& pFilePath, const _tchar* pSaveFilePath)
{
	CMesh* pInstance = new CMesh();
	if (FAILED(pInstance->Ready_Mesh(pFilePath, pSaveFilePath)))
		Safe_Release(pInstance);
	return pInstance;
}

CMesh* CMesh::Create_Load(const _tchar* pFilePath)
{
	CMesh* pInstance = new CMesh();
	if (FAILED(pInstance->Load(pFilePath)))
		Safe_Release(pInstance);
	return pInstance;
}

CComponent* CMesh::Clone_Component(void* pArg)
{
	return new CMesh(*this);
}

void CMesh::Free()
{
	if (m_pScene)
	{
		m_pScene->Destroy();

	}
	for (size_t i = 0; i < m_vecBone.size(); ++i)
	{
		Safe_Delete(m_vecBone[i]);
	}
	for (size_t i = 0; i < m_vecAnimClip.size(); ++i)
	{
		Safe_Delete(m_vecAnimClip[i]);
	}
	for (int i = 0; i < m_arrAnimName.Size(); ++i)
	{
		Safe_Delete(m_arrAnimName[i]);
	}

	if (!m_IsClone)
	{
		for (size_t i = 0; i < m_tRenderInfo.vecIndices.size(); ++i)
		{
			if (m_tRenderInfo.vecIndices[i].pSystem)
				Safe_Delete(m_tRenderInfo.vecIndices[i].pSystem);
		}

		Safe_Release(m_pBoneFrameData);
		Safe_Release(m_pBoneOffset);
	}



	//for (auto& iter : m_vecTexture)
	//	Safe_Release(iter);


	CComponent::Free();
}
