#include "framework.h"
#include "Animation_Controller.h"

CAnimation_Controller::CAnimation_Controller(ID3D12Device* pGraphic_Device)
{
}

HRESULT CAnimation_Controller::Ready_Animation_Controller(FbxScene* pScene)
{
	FbxArray<FbxString*>		ArrFbxAnimationStackName;
	pScene->FillAnimStackNameArray(ArrFbxAnimationStackName);

	m_iAnimationStack = ArrFbxAnimationStackName.Size();

	m_pFbxAnimStack =	new FbxAnimStack*[m_iAnimationStack];
	m_pFbxStartTime =	new FbxTime[m_iAnimationStack];
	m_pFbxStopTime =	new FbxTime[m_iAnimationStack];
	m_pFbxCurrTime =	new FbxTime[m_iAnimationStack];

	for (_uint i = 0; i < m_iAnimationStack; ++ i)
	{
		FbxString* pFbxStackName = ArrFbxAnimationStackName[i];
		FbxAnimStack* pAnimStack = pScene->FindMember<FbxAnimStack>(pFbxStackName->Buffer());

		m_pFbxAnimStack[i] = pAnimStack;

		FbxTakeInfo* pInfo = pScene->GetTakeInfo(*pFbxStackName);
		FbxTime fbxStartTime, fbxStopTime;
		if (pInfo)
		{
			fbxStartTime = pInfo->mLocalTimeSpan.GetStart();
			fbxStopTime = pInfo->mLocalTimeSpan.GetStop();
		}
		else
		{
			FbxTimeSpan fbxTimeLineTimeSpan;

			pScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(fbxTimeLineTimeSpan);
			fbxStartTime = fbxTimeLineTimeSpan.GetStart();
			fbxStopTime = fbxTimeLineTimeSpan.GetStop();
		}
		
		m_pFbxStartTime[i] =	fbxStartTime;
		m_pFbxStopTime[i] =		fbxStopTime;

		m_pFbxCurrTime[i] = FbxTime(0);

	}
	FbxArrayDelete(ArrFbxAnimationStackName);
	return S_OK;
}

CAnimation_Controller* CAnimation_Controller::Create(ID3D12Device* pGraphic_Device, FbxScene* pScene)
{
	CAnimation_Controller* pInstance = new CAnimation_Controller(pGraphic_Device);
	if (FAILED(pInstance->Ready_Animation_Controller(pScene)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

void CAnimation_Controller::Free()
{
	Safe_Delete_Array(m_pFbxAnimStack);
	Safe_Delete_Array(m_pFbxCurrTime);
	Safe_Delete_Array(m_pFbxStopTime);
	Safe_Delete_Array(m_pFbxStartTime);

}

void CAnimation_Controller::SetPosition(_int iAnimStack, _float fPosition)
{
	m_pFbxCurrTime[iAnimStack].SetSecondDouble(fPosition);
}

void CAnimation_Controller::SetAnimationStack(FbxScene* pScene, _int iAnimStack)
{
	m_iCurrAnimationStack = iAnimStack;
	pScene->SetCurrentAnimationStack(m_pFbxAnimStack[m_iCurrAnimationStack]);
}

void CAnimation_Controller::AdvacneTime(const _float& fTimeDelta)
{
	FbxTime fbxTime;
	fbxTime.SetSecondDouble(fTimeDelta);

	m_pFbxCurrTime[m_iCurrAnimationStack] += fbxTime;
	if (m_pFbxCurrTime[m_iCurrAnimationStack] > m_pFbxStopTime[m_iCurrAnimationStack])
	{
		m_pFbxCurrTime[m_iCurrAnimationStack] = m_pFbxStartTime[m_iCurrAnimationStack];
	}
}

void CAnimation_Controller::AnimateHierachyMesh(FbxNode* pNode, FbxTime& fbxCurrTime)
{
	FbxNodeAttribute* pAttribute = pNode->GetNodeAttribute();

	if (pAttribute && (pAttribute->GetAttributeType() == FbxNodeAttribute::eMesh))
	{
		FbxMesh* pMesh = pNode->GetMesh();
		AnimateMesh(pMesh, fbxCurrTime);
	}

	_uint	iChild = pNode->GetChildCount();

	for (_uint i = 0; i < iChild; ++i)
		AnimateHierachyMesh(pNode->GetChild(i), fbxCurrTime);
}

void CAnimation_Controller::AnimateMesh(FbxMesh* pMesh, FbxTime& fbxCurrTime)
{
	_uint iVertices = pMesh->GetControlPointsCount();
	if (iVertices > 0)
	{
		FbxVector4* pVertices = new FbxVector4[iVertices];
		memcpy(pVertices, pMesh->GetControlPoints(), iVertices * sizeof(FbxVector4));

		_int iSkinDeformers = pMesh->GetDeformerCount(FbxDeformer::eSkin);
		if (iSkinDeformers > 0)
		{
			ComputeSkinDeformation(pMesh, fbxCurrTime, pVertices, iVertices);
		}

		RenderInfo* pInfo = (RenderInfo*)pMesh->GetUserDataPtr();
		for (_uint i = 0; i < iVertices; ++i)
		{
			//pInfo->vecPosition[i] = _vec4(pVertices[i][0], pVertices[i][1], pVertices[i][2],1.f);
			pInfo->vPos[i] = _vec4(pVertices[i][0], pVertices[i][1], pVertices[i][2], 1.f);

		}
	}
}



void  CAnimation_Controller::ComputeSkinDeformation(FbxMesh* pfbxMesh, FbxTime& fbxCurrentTime, FbxVector4* pfbxv4Vertices, int nVertices)
{
	FbxSkin* pfbxSkinDeformer = (FbxSkin*)pfbxMesh->GetDeformer(0, FbxDeformer::eSkin);
	FbxSkin::EType nSkinningType = pfbxSkinDeformer->GetSkinningType();

	if ((nSkinningType == FbxSkin::eLinear) || (nSkinningType == FbxSkin::eRigid))
	{
		ComputeLinearDeformation(pfbxMesh, fbxCurrentTime, pfbxv4Vertices, nVertices);
	}
	else if (nSkinningType == FbxSkin::eDualQuaternion)
	{
		ComputeDualQuaternionDeformation(pfbxMesh, fbxCurrentTime, pfbxv4Vertices, nVertices);
	}
	else if (nSkinningType == FbxSkin::eBlend)
	{
		FbxVector4* pfbxv4LinearVertices = new FbxVector4[nVertices];
		memcpy(pfbxv4LinearVertices, pfbxMesh->GetControlPoints(), nVertices * sizeof(FbxVector4));
		ComputeLinearDeformation(pfbxMesh, fbxCurrentTime, pfbxv4LinearVertices, nVertices);

		FbxVector4* pfbxv4DQVertices = new FbxVector4[nVertices];
		memcpy(pfbxv4DQVertices, pfbxMesh->GetControlPoints(), nVertices * sizeof(FbxVector4));
		ComputeDualQuaternionDeformation(pfbxMesh, fbxCurrentTime, pfbxv4DQVertices, nVertices);

		int nBlendWeights = pfbxSkinDeformer->GetControlPointIndicesCount();
		double* pfControlPointBlendWeights = pfbxSkinDeformer->GetControlPointBlendWeights();
		for (int i = 0; i < nBlendWeights; i++)
		{
			pfbxv4Vertices[i] = pfbxv4DQVertices[i] * pfControlPointBlendWeights[i] + pfbxv4LinearVertices[i] * (1 - pfControlPointBlendWeights[i]);
		}

		delete[] pfbxv4LinearVertices;
		delete[] pfbxv4DQVertices;
	}
}

void CAnimation_Controller::ComputeLinearDeformation(FbxMesh* pfbxMesh, FbxTime& fbxCurrentTime, FbxVector4* pfbxv4Vertices, int nVertices)
{
	FbxAMatrix* pfbxmtxClusterDeformations = new FbxAMatrix[nVertices];
	::memset(pfbxmtxClusterDeformations, 0, nVertices * sizeof(FbxAMatrix));

	double* pfSumOfClusterWeights = new double[nVertices];
	::memset(pfSumOfClusterWeights, 0, nVertices * sizeof(double));

	FbxCluster::ELinkMode nClusterMode = ((FbxSkin*)pfbxMesh->GetDeformer(0, FbxDeformer::eSkin))->GetCluster(0)->GetLinkMode();
	if (nClusterMode == FbxCluster::eAdditive)
	{
		for (int i = 0; i < nVertices; ++i) 
			pfbxmtxClusterDeformations[i].SetIdentity();
	}

	int nSkinDeformers = pfbxMesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int i = 0; i < nSkinDeformers; i++)
	{
		FbxSkin* pfbxSkinDeformer = (FbxSkin*)pfbxMesh->GetDeformer(i, FbxDeformer::eSkin);
		int nClusters = pfbxSkinDeformer->GetClusterCount();

		for (int j = 0; j < nClusters; j++)
		{
			FbxCluster* pfbxCluster = pfbxSkinDeformer->GetCluster(j);
			if (!pfbxCluster->GetLink()) continue;

			FbxAMatrix fbxmtxClusterDeformation = ComputeClusterDeformation(pfbxMesh, pfbxCluster, nClusterMode, fbxCurrentTime);

			int* pnIndices = pfbxCluster->GetControlPointIndices();
			double* pfWeights = pfbxCluster->GetControlPointWeights();

			int nIndices = pfbxCluster->GetControlPointIndicesCount();
			for (int k = 0; k < nIndices; k++)
			{
				int nIndex = pnIndices[k];
				double fWeight = pfWeights[k];
				if ((nIndex >= nVertices) || (fWeight == 0.0))
					continue;

				FbxAMatrix fbxmtxInfluence = fbxmtxClusterDeformation;
				MatrixScale(fbxmtxInfluence, fWeight);

				if (nClusterMode == FbxCluster::eAdditive)
				{
					MatrixAddToDiagonal(fbxmtxInfluence, 1.0 - fWeight);
					pfbxmtxClusterDeformations[nIndex] = fbxmtxInfluence * pfbxmtxClusterDeformations[nIndex];
					pfSumOfClusterWeights[nIndex] = 1.0;
				}
				else
				{
					MatrixAdd(pfbxmtxClusterDeformations[nIndex], fbxmtxInfluence);
					pfSumOfClusterWeights[nIndex] += fWeight;
				}
			}
		}
	}

	for (int i = 0; i < nVertices; i++)
	{
		if (pfSumOfClusterWeights[i] != 0.0)
		{
			FbxVector4 fbxv4Vertex = pfbxv4Vertices[i];
			pfbxv4Vertices[i] = pfbxmtxClusterDeformations[i].MultT(fbxv4Vertex);
			if (nClusterMode == FbxCluster::eNormalize)
			{
				pfbxv4Vertices[i] /= pfSumOfClusterWeights[i];
			}
			else if (nClusterMode == FbxCluster::eTotalOne)
			{
				fbxv4Vertex *= (1.0 - pfSumOfClusterWeights[i]);
				pfbxv4Vertices[i] += fbxv4Vertex;
			}
		}
	}

	delete[] pfbxmtxClusterDeformations;
	delete[] pfSumOfClusterWeights;
}
void CAnimation_Controller::ComputeDualQuaternionDeformation(FbxMesh* pfbxMesh, FbxTime& fbxCurrentTime, FbxVector4* pfbxv4Vertices, int nVertices)
{
	FbxDualQuaternion* pfbxDQClusterDeformations = new FbxDualQuaternion[nVertices];
	memset(pfbxDQClusterDeformations, 0, nVertices * sizeof(FbxDualQuaternion));
	double* pfClusterWeights = new double[nVertices];
	memset(pfClusterWeights, 0, nVertices * sizeof(double));

	FbxCluster::ELinkMode nClusterMode = ((FbxSkin*)pfbxMesh->GetDeformer(0, FbxDeformer::eSkin))->GetCluster(0)->GetLinkMode();
	int nSkinDeformers = pfbxMesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int i = 0; i < nSkinDeformers; i++)
	{
		FbxSkin* pfbxSkinDeformer = (FbxSkin*)pfbxMesh->GetDeformer(i, FbxDeformer::eSkin);
		int nClusters = pfbxSkinDeformer->GetClusterCount();
		for (int j = 0; j < nClusters; j++)
		{
			FbxCluster* pfbxCluster = pfbxSkinDeformer->GetCluster(j);
			if (!pfbxCluster->GetLink()) continue;

			FbxAMatrix fbxmtxCluster = ComputeClusterDeformation(pfbxMesh, pfbxCluster, nClusterMode, fbxCurrentTime);

			FbxQuaternion Q = fbxmtxCluster.GetQ();
			FbxVector4 T = fbxmtxCluster.GetT();
			FbxDualQuaternion fbxDualQuaternion(Q, T);

			int nIndices = pfbxCluster->GetControlPointIndicesCount();
			int* pnControlPointIndices = pfbxCluster->GetControlPointIndices();
			double* pfControlPointWeights = pfbxCluster->GetControlPointWeights();
			for (int k = 0; k < nIndices; ++k)
			{
				int nIndex = pnControlPointIndices[k];
				if (nIndex >= nVertices) continue;

				double fWeight = pfControlPointWeights[k];
				if (fWeight == 0.0) continue;

				FbxDualQuaternion fbxmtxInfluence = fbxDualQuaternion * fWeight;
				if (nClusterMode == FbxCluster::eAdditive)
				{
					pfbxDQClusterDeformations[nIndex] = fbxmtxInfluence;
					pfClusterWeights[nIndex] = 1.0;
				}
				else // FbxCluster::eNormalize || FbxCluster::eTotalOne
				{
					if (j == 0)
					{
						pfbxDQClusterDeformations[nIndex] = fbxmtxInfluence;
					}
					else
					{
						double fSign = pfbxDQClusterDeformations[nIndex].GetFirstQuaternion().DotProduct(fbxDualQuaternion.GetFirstQuaternion());
						if (fSign >= 0.0)
						{
							pfbxDQClusterDeformations[nIndex] += fbxmtxInfluence;
						}
						else
						{
							pfbxDQClusterDeformations[nIndex] -= fbxmtxInfluence;
						}
					}
					pfClusterWeights[nIndex] += fWeight;
				}
			}
		}
	}

	for (int i = 0; i < nVertices; i++)
	{
		FbxVector4 fbxv4SrcVertex = pfbxv4Vertices[i];
		double fWeightSum = pfClusterWeights[i];

		if (fWeightSum != 0.0)
		{
			pfbxDQClusterDeformations[i].Normalize();
			pfbxv4Vertices[i] = pfbxDQClusterDeformations[i].Deform(pfbxv4Vertices[i]);

			if (nClusterMode == FbxCluster::eNormalize)
			{
				pfbxv4Vertices[i] /= fWeightSum;
			}
			else if (nClusterMode == FbxCluster::eTotalOne)
			{
				fbxv4SrcVertex *= (1.0 - fWeightSum);
				pfbxv4Vertices[i] += fbxv4SrcVertex;
			}
		}
	}

	delete[] pfbxDQClusterDeformations;
	delete[] pfClusterWeights;
}

void CAnimation_Controller::MatrixScale(FbxAMatrix& fbxmatSrcMatrix, double pValue)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) 
			fbxmatSrcMatrix[i][j] *= pValue;
	}
}
void CAnimation_Controller::MatrixAdd(FbxAMatrix& fbxmatDstMatrix, FbxAMatrix& fbxmatSrcMatrix)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) 
			fbxmatDstMatrix[i][j] += fbxmatSrcMatrix[i][j];
	}
}
void CAnimation_Controller::MatrixAddToDiagonal(FbxAMatrix& fbxmatSrcMatrix, double pValue)
{
	fbxmatSrcMatrix[0][0] += pValue;
	fbxmatSrcMatrix[1][1] += pValue;
	fbxmatSrcMatrix[2][2] += pValue;
	fbxmatSrcMatrix[3][3] += pValue;
}
FbxAMatrix CAnimation_Controller::GetGeometricOffsetTransform(FbxNode* pfbxNode)
{
	const FbxVector4 T = pfbxNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 R = pfbxNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 S = pfbxNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return(FbxAMatrix(T, R, S));
}
FbxAMatrix CAnimation_Controller::ComputeClusterDeformation(FbxMesh* pfbxMesh, FbxCluster* pfbxCluster, FbxCluster::ELinkMode nClusterMode, FbxTime& fbxCurrentTime)
{
	FbxAMatrix fbxmtxVertexTransform;

	if (nClusterMode == FbxCluster::eNormalize)
	{
		FbxAMatrix fbxmtxGeometryOffset = GetGeometricOffsetTransform(pfbxMesh->GetNode());

		FbxAMatrix fbxmtxBindPoseMeshToRoot; //Cluster Transform
		pfbxCluster->GetTransformMatrix(fbxmtxBindPoseMeshToRoot);

		FbxAMatrix fbxmtxBindPoseBoneToRoot; //Cluster Link Transform
		pfbxCluster->GetTransformLinkMatrix(fbxmtxBindPoseBoneToRoot);

		FbxAMatrix fbxmtxAnimatedBoneToRoot = pfbxCluster->GetLink()->EvaluateGlobalTransform(fbxCurrentTime); //Cluster Link Node Global Transform

		fbxmtxVertexTransform = fbxmtxAnimatedBoneToRoot * fbxmtxBindPoseBoneToRoot.Inverse() * fbxmtxBindPoseMeshToRoot * fbxmtxGeometryOffset;
	}
	else
	{ //FbxCluster::eAdditive
		if (pfbxCluster->GetAssociateModel())
		{
			FbxAMatrix fbxmtxAssociateModel;
			pfbxCluster->GetTransformAssociateModelMatrix(fbxmtxAssociateModel);

			FbxAMatrix fbxmtxAssociateGeometryOffset = GetGeometricOffsetTransform(pfbxCluster->GetAssociateModel());
			fbxmtxAssociateModel *= fbxmtxAssociateGeometryOffset;

			FbxAMatrix fbxmtxAssociateModelGlobal = pfbxCluster->GetAssociateModel()->EvaluateGlobalTransform(fbxCurrentTime);

			FbxAMatrix fbxmtxClusterTransform;
			pfbxCluster->GetTransformMatrix(fbxmtxClusterTransform);

			FbxAMatrix fbxmtxGeometryOffset = GetGeometricOffsetTransform(pfbxMesh->GetNode());
			fbxmtxClusterTransform *= fbxmtxGeometryOffset;

			FbxAMatrix fbxmtxClusterLinkTransform;
			pfbxCluster->GetTransformLinkMatrix(fbxmtxClusterLinkTransform);

			FbxAMatrix fbxmtxLinkGeometryOffset = GetGeometricOffsetTransform(pfbxCluster->GetLink());
			fbxmtxClusterLinkTransform *= fbxmtxLinkGeometryOffset;

			FbxAMatrix fbxmtxClusterLinkToRoot = pfbxCluster->GetLink()->EvaluateGlobalTransform(fbxCurrentTime);

			fbxmtxVertexTransform = fbxmtxClusterTransform.Inverse() * fbxmtxAssociateModel * fbxmtxAssociateModelGlobal.Inverse() * fbxmtxClusterLinkToRoot * fbxmtxClusterLinkTransform.Inverse() * fbxmtxClusterTransform;
		}
	}
	return(fbxmtxVertexTransform);
}