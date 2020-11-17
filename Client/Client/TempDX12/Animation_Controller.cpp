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
			//pInfo->vecPosition[i] = _vec3(pVertices[i][0], pVertices[i][1], pVertices[i][2]);

			_vec3 vtemp = _vec3(pVertices[i][0], pVertices[i][1], pVertices[i][2]);
			pInfo->vPos = &vtemp;

		}
		// 문제: 원래 바인딩된 Position을 가져와 애니메이션을 돌리는데 
		//	     바인딩 된 Position의 값을 못바꾸고  헛도는 Position 값만 바꾼다. 
		//		 UploadBuffer를 사용해 GPU에서 CPU로 정점 정보를 가져오는게 가능하다.


		//CreateBufferView(iVertices, pInfo->iIndices, pInfo->vecPosition, pInfo);
	}
}



void CAnimation_Controller::ComputeSkinDeformation(FbxMesh* pMesh, FbxTime& fbxCurrTime, FbxVector4* pVertices, _int iNumVertices)
{
	FbxSkin* pSkinDeformer = (FbxSkin*)pMesh->GetDeformer(0, FbxDeformer::eSkin);
	FbxSkin::EType eSkinningType = pSkinDeformer->GetSkinningType();

	if ((eSkinningType == FbxSkin::eLinear) || (eSkinningType == FbxSkin::eRigid))
	{
		//ComputeLinear
		ComputeLinearDeformation(pMesh, fbxCurrTime, pVertices, iNumVertices);
	}
	else if (eSkinningType == FbxSkin::eDualQuaternion)
	{
		// ComputeQuaternion
		ComputeDualQuaternionDeformation(pMesh, fbxCurrTime, pVertices, iNumVertices);
	}
	else if (eSkinningType == FbxSkin::eBlend)
	{
		FbxVector4* pfbxv4LinearVertices = new FbxVector4[iNumVertices];
		memcpy(pfbxv4LinearVertices, pMesh->GetControlPoints(), iNumVertices * sizeof(FbxVector4));
		ComputeLinearDeformation(pMesh, fbxCurrTime, pfbxv4LinearVertices, iNumVertices);

		FbxVector4* pfbxv4DQVertices = new FbxVector4[iNumVertices];
		memcpy(pfbxv4DQVertices, pMesh->GetControlPoints(), iNumVertices * sizeof(FbxVector4));
		ComputeDualQuaternionDeformation(pMesh, fbxCurrTime, pfbxv4DQVertices, iNumVertices);

		int nBlendWeights = pSkinDeformer->GetControlPointIndicesCount();
		double* pfControlPointBlendWeights = pSkinDeformer->GetControlPointBlendWeights();
		for (int i = 0; i < nBlendWeights; i++)
		{
			pVertices[i] = pfbxv4DQVertices[i] * pfControlPointBlendWeights[i] + pfbxv4LinearVertices[i] * (1 - pfControlPointBlendWeights[i]);
		}

		delete[] pfbxv4LinearVertices;
		delete[] pfbxv4DQVertices;
	}
}
void CAnimation_Controller::ComputeLinearDeformation(FbxMesh* pMesh, FbxTime& fbxCurrTime, FbxVector4* pVertices, _int iNumVertices)
{
	FbxAMatrix* pClusterDeformation = new FbxAMatrix[iNumVertices];
	memset(pClusterDeformation, 0, iNumVertices * sizeof(FbxAMatrix));

	double* pSumOfClusterWeight = new double[iNumVertices];
	memset(pSumOfClusterWeight, 0, iNumVertices * sizeof(double));

	FbxCluster::ELinkMode iClusterMode = ((FbxSkin*)pMesh->GetDeformer(0, FbxDeformer::eSkin))->GetCluster(0)->GetLinkMode();
	if (iClusterMode == FbxCluster::eAdditive)
		for (_uint i = 0; i < iNumVertices; ++i)
			pClusterDeformation[i].SetIdentity();


	_uint	iSkinDeformers = pMesh->GetDeformerCount(FbxDeformer::eSkin);
	for (_uint i = 0; i < iSkinDeformers; ++i)
	{
		FbxSkin* pSkinDeformers = (FbxSkin*)pMesh->GetDeformer(i, FbxDeformer::eSkin);

		_uint	iClusters = pSkinDeformers->GetClusterCount();
		
		for (_uint j = 0; j < iClusters; ++j)
		{
			FbxCluster* pCluster = pSkinDeformers->GetCluster(j);
			if (!pCluster->GetLink())
				continue;

			FbxAMatrix fbxClusterDeformation = ComputeClusterDeformation(pMesh, pCluster, iClusterMode, fbxCurrTime);

			int*	pIndices = pCluster->GetControlPointIndices();
			double* pWeight = pCluster->GetControlPointWeights();

			_uint	iNumIndices = pCluster->GetControlPointIndicesCount();

			for (_uint k = 0; k < iNumIndices; ++k)
			{
				_int	iIdx = pIndices[k];
				double	fWeight = pWeight[k];

				if ((iIdx >= iNumIndices) || (fWeight == 0.0))
					continue;

				FbxAMatrix fbxInfluence = fbxClusterDeformation;
				MatrixScale(fbxInfluence, fWeight);

				if (iClusterMode == FbxCluster::eAdditive)
				{
					MatrixAddToDiagonal(fbxInfluence, 1.0 - fWeight);
					pClusterDeformation[iIdx] = fbxInfluence * pClusterDeformation[iIdx];
					pSumOfClusterWeight[iIdx] = 1.f;
				}
				else
				{
					MatrixAdd(pClusterDeformation[iIdx], fbxInfluence);
					pSumOfClusterWeight[iIdx] += fWeight;
				}

			}

		}

	}

	for (_uint i = 0; i < iNumVertices; ++i)
	{
		if (pSumOfClusterWeight[i] != 0.f)
		{
			FbxVector4 fbxVec4 = pVertices[i];
			pVertices[i] = pClusterDeformation[i].MultT(fbxVec4);

			if (iClusterMode == FbxCluster::eNormalize)
			{
				pVertices[i] /= pSumOfClusterWeight[i];
			}
			else if (iClusterMode == FbxCluster::eTotalOne)
			{
				fbxVec4 *= (1.f - pSumOfClusterWeight[i]);
				pVertices[i] += fbxVec4;
			}
		}
	}

	Safe_Delete_Array(pClusterDeformation);
	Safe_Delete_Array(pSumOfClusterWeight);

}
void CAnimation_Controller::ComputeDualQuaternionDeformation(FbxMesh* pMesh, FbxTime& fbxCurrTime, FbxVector4* pVertices, _int iNumVertices)
{
	FbxDualQuaternion* pDualQuaternionClusterDeformation = new FbxDualQuaternion[iNumVertices];
	memset(pDualQuaternionClusterDeformation, 0, iNumVertices * sizeof(FbxDualQuaternion));
	double* pClusterWeight = new double[iNumVertices];
	memset(pClusterWeight, 0, iNumVertices * sizeof(double));


	FbxCluster::ELinkMode iClusterMode = ((FbxSkin*)pMesh->GetDeformer(0, FbxDeformer::eSkin))->GetCluster(0)->GetLinkMode();

	_uint iSkinDeformer = pMesh->GetDeformerCount(FbxDeformer::eSkin);

	for (_uint i = 0; i < iSkinDeformer; ++i)
	{
		FbxSkin*	pSkinDeformer = (FbxSkin*)pMesh->GetDeformer(i, FbxDeformer::eSkin);
		_uint		iClusters = pSkinDeformer->GetClusterCount();

		for (_uint j = 0; j < iClusters; ++j)
		{
			FbxCluster* pCluster = pSkinDeformer->GetCluster(j);

			if (!pCluster->GetLink())
				continue;

			FbxAMatrix fbxCluster= ComputeClusterDeformation(pMesh, pCluster, iClusterMode, fbxCurrTime);

			FbxQuaternion	Q = fbxCluster.GetQ();
			FbxVector4		T = fbxCluster.GetT();
			FbxDualQuaternion	fbxDualQuaternion(Q, T);

			_uint	iIndices = pCluster->GetControlPointIndicesCount();
			_int*	pControlPointIndices = pCluster->GetControlPointIndices();
			double* pControlPointWeight = pCluster->GetControlPointWeights();

			for (_uint k = 0; k < iIndices; ++k)
			{
				_uint iIdx = pControlPointIndices[k];
				if (iIdx >= iNumVertices)
					continue;

				double fWeight = pControlPointWeight[k];
				if (fWeight == 0.f)
					continue;

				FbxDualQuaternion	fbxInfluence = fbxDualQuaternion * fWeight;

				if (iClusterMode == FbxCluster::eAdditive)
				{
					pDualQuaternionClusterDeformation[iIdx] = fbxInfluence;
					pClusterWeight[iIdx] = 1.f;
				}
				else
				{
					if (j == 0)
					{
						pDualQuaternionClusterDeformation[iIdx] = fbxInfluence;
					}
					else
					{
						double fSign = pDualQuaternionClusterDeformation[iIdx].GetFirstQuaternion().DotProduct(fbxDualQuaternion.GetFirstQuaternion());
						if (fSign >= 0.f)
						{
							pDualQuaternionClusterDeformation[iIdx] += fbxInfluence;
						}
						else
						{
							pDualQuaternionClusterDeformation[iIdx] -= fbxInfluence;
						}

					}

					pClusterWeight[iIdx] += fWeight;
				}
			}

		}
	}


	for (_uint i = 0; i < iNumVertices; ++i)
	{
		FbxVector4 fbxSrcVec4 = pVertices[i];

		double fWeightSum = pClusterWeight[i];

		if (fWeightSum != 0.f)
		{
			pDualQuaternionClusterDeformation[i].Normalize();
			pVertices[i] = pDualQuaternionClusterDeformation[i].Deform(pVertices[i]);

			if (iClusterMode == FbxCluster::eNormalize)
			{
				pVertices[i] /= fWeightSum;
			}
			else if (iClusterMode == FbxCluster::eTotalOne)
			{
				fbxSrcVec4 *= (1.f - fWeightSum);
				pVertices[i] += fbxSrcVec4;
			}
		}
	}


	Safe_Delete_Array(pDualQuaternionClusterDeformation);
	Safe_Delete_Array(pClusterWeight);
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
FbxAMatrix CAnimation_Controller::GetGeomatryOffsetTransform(FbxNode* pNode)
{
	const FbxVector4 T = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 R = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 S = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return(FbxAMatrix(T, R, S));
}

HRESULT CAnimation_Controller::CreateBufferView(_uint iVerticesNum, _uint iIndicesNum, vector<_vec3>& vecPos, RenderInfo* pInfo)
{

	int m_iVertices = iVerticesNum;
	int m_iIndices = iIndicesNum;
	int m_iStride = sizeof(_vec3);

	D3D12_HEAP_PROPERTIES	tHeap_Pro_Default = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_HEAP_PROPERTIES	tHeap_Pro_Upload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	ID3D12Resource* pVertexBuffer = nullptr;
	ID3D12Resource* pVertexUploadBuffer = nullptr;
	ID3D12Resource* pIndexBuffer = nullptr;
	ID3D12Resource* pIndexUploadBuffer = nullptr;
	CDevice::GetInstance()->Open();
	{
		D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(m_iStride * m_iVertices);


		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Default, D3D12_HEAP_FLAG_NONE, &tResource_Desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&pVertexBuffer))))
			return E_FAIL;
		pVertexBuffer->SetName(L"VertexBuffer");
		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Upload, D3D12_HEAP_FLAG_NONE, &tResource_Desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&pVertexUploadBuffer))))
			return E_FAIL;
		pVertexUploadBuffer->SetName(L"Upload VertexBuffer");


		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = (void*)(vecPos.data());
		vertexData.RowPitch = m_iStride * m_iVertices;
		vertexData.SlicePitch = m_iStride * m_iVertices;

		UpdateSubresources(CDevice::GetInstance()->GetCommandList(), pVertexBuffer, pVertexUploadBuffer, 0, 0, 1, &vertexData);
		D3D12_RESOURCE_BARRIER	tResource_Barrier = CD3DX12_RESOURCE_BARRIER::Transition(pVertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		CDevice::GetInstance()->GetCommandList()->ResourceBarrier(1, &tResource_Barrier);
	}
	{
		D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(_uint) * m_iIndices);
		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Default, D3D12_HEAP_FLAG_NONE, &tResource_Desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&pIndexBuffer))))
			return E_FAIL;
		pIndexBuffer->SetName(L"IndexBuffer");
		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Upload, D3D12_HEAP_FLAG_NONE, &tResource_Desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&pIndexUploadBuffer))))
			return E_FAIL;
		pIndexUploadBuffer->SetName(L"Upload IndexBuffer");

		D3D12_SUBRESOURCE_DATA indexData = {};
		indexData.pData = (void*)(pInfo->vecIndices.data());
		indexData.RowPitch = sizeof(_uint) * m_iIndices;
		indexData.SlicePitch = sizeof(_uint) * m_iIndices;

		UpdateSubresources(CDevice::GetInstance()->GetCommandList(), pIndexBuffer, pIndexUploadBuffer, 0, 0, 1, &indexData);
		D3D12_RESOURCE_BARRIER	tResource_Barrier = CD3DX12_RESOURCE_BARRIER::Transition(pIndexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);
		CDevice::GetInstance()->GetCommandList()->ResourceBarrier(1, &tResource_Barrier);
	}
	CDevice::GetInstance()->Close();

	pInfo->VertexBufferView.BufferLocation = pVertexBuffer->GetGPUVirtualAddress();
	pInfo->VertexBufferView.StrideInBytes = m_iStride;
	pInfo->VertexBufferView.SizeInBytes = m_iStride * m_iVertices;

	pInfo->IndexBufferView.BufferLocation = pIndexBuffer->GetGPUVirtualAddress();
	pInfo->IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	pInfo->IndexBufferView.SizeInBytes = sizeof(_uint) * m_iIndices;

	pInfo->iIndices = m_iIndices;
	pInfo->vecPosition = vecPos;



	CDevice::GetInstance()->WaitForGpuComplete();
	return S_OK;
}
FbxAMatrix CAnimation_Controller::ComputeClusterDeformation(FbxMesh* pMesh, FbxCluster* pCluster, FbxCluster::ELinkMode iClusterMode, FbxTime& fbxCurrTime)
{
	FbxAMatrix	fbxMatVertexTransform;

	if (iClusterMode == FbxCluster::eNormalize)
	{
		FbxAMatrix	fbxMatGeomatryOffset = GetGeomatryOffsetTransform(pMesh->GetNode());

		FbxAMatrix	fbxMatBindPoseMeshToRoot;
		pCluster->GetTransformMatrix(fbxMatBindPoseMeshToRoot);

		FbxAMatrix	fbxMatBindPoseBoneToRoot;
		pCluster->GetTransformLinkMatrix(fbxMatBindPoseBoneToRoot);

		FbxAMatrix	fbxMatAnimateBoneToRoot = pCluster->GetLink()->EvaluateGlobalTransform(fbxCurrTime);
		fbxMatVertexTransform = fbxMatAnimateBoneToRoot * fbxMatBindPoseBoneToRoot * fbxMatBindPoseMeshToRoot * fbxMatGeomatryOffset;

	}
	else
	{
		if (pCluster->GetAssociateModel())
		{

			FbxAMatrix fbxmtxAssociateModel;
			pCluster->GetTransformAssociateModelMatrix(fbxmtxAssociateModel);

			FbxAMatrix fbxmtxAssociateGeometryOffset = GetGeomatryOffsetTransform(pCluster->GetAssociateModel());
			fbxmtxAssociateModel *= fbxmtxAssociateGeometryOffset;

			FbxAMatrix fbxmtxAssociateModelGlobal = pCluster->GetAssociateModel()->EvaluateGlobalTransform(fbxCurrTime);

			FbxAMatrix fbxmtxClusterTransform;
			pCluster->GetTransformMatrix(fbxmtxClusterTransform);

			FbxAMatrix fbxmtxGeometryOffset = GetGeomatryOffsetTransform(pMesh->GetNode());
			fbxmtxClusterTransform *= fbxmtxGeometryOffset;

			FbxAMatrix fbxmtxClusterLinkTransform;
			pCluster->GetTransformLinkMatrix(fbxmtxClusterLinkTransform);

			FbxAMatrix fbxmtxLinkGeometryOffset = GetGeomatryOffsetTransform(pCluster->GetLink());
			fbxmtxClusterLinkTransform *= fbxmtxLinkGeometryOffset;

			FbxAMatrix fbxmtxClusterLinkToRoot = pCluster->GetLink()->EvaluateGlobalTransform(fbxCurrTime);

			fbxMatVertexTransform = fbxmtxClusterTransform.Inverse() * fbxmtxAssociateModel * fbxmtxAssociateModelGlobal.Inverse() * fbxmtxClusterLinkToRoot * fbxmtxClusterLinkTransform.Inverse() * fbxmtxClusterTransform;
		}
	}
	return(fbxMatVertexTransform);
}