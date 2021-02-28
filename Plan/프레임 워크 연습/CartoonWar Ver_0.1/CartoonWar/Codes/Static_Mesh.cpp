#include "framework.h"
#include "Static_Mesh.h"
#include "Management.h"
#include "FBXLoader.h"

CStatic_Mesh::CStatic_Mesh()
	: CComponent()
{
}

CStatic_Mesh::CStatic_Mesh(const CStatic_Mesh& rhs)
	: CComponent(rhs)
	, m_pLoader(rhs.m_pLoader)
{
	m_IsClone = true;
}

HRESULT CStatic_Mesh::Ready_Static_Mesh(string strFilePath)
{
	m_pLoader = CFBXLoader::Create(strFilePath);
	if (nullptr == m_pLoader)
		return E_FAIL;

	if (FAILED(m_pLoader->Load_FbxFile(m_pLoader->GetScene()->GetRootNode())))
		return E_FAIL;


	m_pLoader->GetRenderInfo();

	return S_OK;
}

void CStatic_Mesh::Render_Hierachy_Mesh(FbxNode* pNode, CShader* pShaderCom, _matrix matWorld, MAINPASS& tPass,
	CTexture* pTexture, const _tchar* pTextureTag)
{
	FbxNodeAttribute* pAttr = pNode->GetNodeAttribute();
	if (pAttr && pAttr->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		FbxAMatrix RootNodeMatrix = pNode->EvaluateGlobalTransform();
		FbxAMatrix GeometicOffest = GetGeometricOffsetTransform(pNode);
		FbxMesh* pMesh = pNode->GetMesh();


		Render_Mesh(pShaderCom, pMesh, RootNodeMatrix, GeometicOffest, matWorld, tPass, pTexture, pTextureTag);

	}
	
	_uint iChildCnt = pNode->GetChildCount();
	for (_uint i = 0; i <iChildCnt; ++i)
	{
		Render_Hierachy_Mesh(pNode->GetChild(i), pShaderCom, matWorld, tPass, pTexture, pTextureTag);
	}
}

void CStatic_Mesh::Render_Mesh(CShader* pShaderCom, FbxMesh* pMesh, FbxAMatrix& pRootNodeMatrix, FbxAMatrix& pGeometryMatrix,
			_matrix matWorld, MAINPASS& tPass, CTexture* pTexture, const _tchar* pTextureTag)
{
	FbxAMatrix	fbxMatrixTransform = ConvertMatrixToFbx(matWorld);
	_int		iSkinDeformers = pMesh->GetDeformerCount(FbxDeformer::eSkin);
	if (iSkinDeformers == 0)
		fbxMatrixTransform = fbxMatrixTransform * pRootNodeMatrix * pGeometryMatrix;

	_matrix		matView = CCamera_Manager::GetInstance()->GetMatView();
	_matrix		matProj = CCamera_Manager::GetInstance()->GetMatProj();
	

	if (FAILED(pShaderCom->SetUp_OnShader_FbxMesh(FbxMatrixToMatrix(&fbxMatrixTransform), matView, matProj, tPass)))
		return ;
	RenderInfo* pInfo = (RenderInfo*)pMesh->GetUserDataPtr();
	if (pInfo->strNodeName.find("Body") != string::npos)
	{
		CDevice::GetInstance()->SetTextureToShader(pTexture->GetSRV(pTextureTag,1),  TEXTURE_REGISTER::t0);
		CDevice::GetInstance()->SetTextureToShader(pTexture->GetSRV(pTextureTag, 3),  TEXTURE_REGISTER::t1);
	}
	else
	{
		CDevice::GetInstance()->SetTextureToShader(pTexture->GetSRV(pTextureTag, 0),  TEXTURE_REGISTER::t0);
		CDevice::GetInstance()->SetTextureToShader(pTexture->GetSRV(pTextureTag, 2),  TEXTURE_REGISTER::t1);
	}

	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	_uint iOffset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tPass);

	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffset, CONST_REGISTER::b0);

	CDevice::GetInstance()->UpdateTable();



	CDevice::GetInstance()->GetCmdLst()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CDevice::GetInstance()->GetCmdLst()->IASetVertexBuffers(0, 1, &(pInfo->VertexBufferView));
	CDevice::GetInstance()->GetCmdLst()->DrawInstanced(pInfo->vecVertices.size(), 1, 0, 0);

	//CDevice::GetInstance()->GetCmdLst()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//CDevice::GetInstance()->GetCmdLst()->IASetVertexBuffers(0, 1, &(pInfo->VertexBufferView));
	//CDevice::GetInstance()->GetCmdLst()->IASetIndexBuffer(&(pInfo->IndexBufferView));
	//CDevice::GetInstance()->GetCmdLst()->DrawIndexedInstanced(pInfo->vecIndices.size(), 1, 0, 0, 0);
}

FbxAMatrix CStatic_Mesh::GetGeometricOffsetTransform(FbxNode* pNode)
{
	const FbxVector4 T = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 R = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 S = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return(FbxAMatrix(T, R, S));
}

FbxAMatrix CStatic_Mesh::ConvertMatrixToFbx(_matrix matWorld)
{
	FbxAMatrix fbxmtxResult;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) fbxmtxResult[i][j] = matWorld.m[i][j];
	}
	return(fbxmtxResult);
}

_matrix CStatic_Mesh::FbxMatrixToMatrix(FbxAMatrix* pFbxMatrix)
{
	FbxVector4 S = pFbxMatrix->GetS();
	FbxVector4 R = pFbxMatrix->GetR();
	FbxVector4 T = pFbxMatrix->GetT();

	FbxAMatrix fbxmtxTransform = FbxAMatrix(T, R, S);

	XMFLOAT4X4 xmf4x4Result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) xmf4x4Result.m[i][j] = (float)fbxmtxTransform[i][j];
		//for (int j = 0; j < 4; j++) xmf4x4Result.m[i][j] = (float)(*pFbxMatrix)[i][j];
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

CStatic_Mesh* CStatic_Mesh::Create(string strFilePath)
{
	CStatic_Mesh* pInstance = new CStatic_Mesh();
	if (FAILED(pInstance->Ready_Static_Mesh(strFilePath)))
	{
		MessageBox(0, L"CStatic_Mesh Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CStatic_Mesh::Clone_Component(void* pArg)
{
	return new CStatic_Mesh(*this);
}

void CStatic_Mesh::Free()
{
	if(m_IsClone)
		Safe_Release(m_pLoader);
	CComponent::Free();
}
