#include "framework.h"
#include "Shader.h"

CShader::CShader(ID3D12Device* pGraphic_Device)
	: CComponent(pGraphic_Device)
{
}

CShader::CShader(const CShader& rhs)
	: CComponent(rhs)
	, m_pFilePath(rhs.m_pFilePath)
	, m_pPixelShader(rhs.m_pPixelShader)
	, m_pVertexShader(rhs.m_pVertexShader)
	, m_pVSEntryPoint(rhs.m_pVSEntryPoint)
	, m_pPSEntryPoint(rhs.m_pPSEntryPoint)
{

}

HRESULT CShader::Ready_Shader(const _tchar* pFilepath, const char* VSEntry, const char* PSEntry, _uint iFlag)
{
	m_pFilePath = pFilepath;
	m_pVSEntryPoint = VSEntry;
	m_pPSEntryPoint = PSEntry;
	m_iCompileFlags = iFlag;
	return S_OK;
}

HRESULT CShader::Compile_Shader()
{
	ID3DBlob* compilationMsgs = nullptr;
	if (FAILED(D3DCompileFromFile(m_pFilePath, nullptr, nullptr, m_pVSEntryPoint, "vs_5_1", m_iCompileFlags, 0, &m_pVertexShader, &compilationMsgs)))
	{
		char* pTemp = ((char*)compilationMsgs->GetBufferPointer());
		return E_FAIL;
	}


	if (FAILED(D3DCompileFromFile(m_pFilePath, nullptr, nullptr, m_pPSEntryPoint, "ps_5_1", m_iCompileFlags, 0, &m_pPixelShader, &compilationMsgs)))
	{
		char* pTemp = ((char*)compilationMsgs->GetBufferPointer());
		return E_FAIL;
	}	
	return S_OK;
}

HRESULT CShader::SetUp_OnShader(ID3D12PipelineState* pPipeline, ID3D12Resource* pConstantBuffer, _matrix matWorld, _matrix matView, _matrix matProj, 
	MAINPASS& output, ROOT_TYPE eType)
{
	CDevice::GetInstance()->GetCommandList()->SetGraphicsRootSignature(CDevice::GetInstance()->GetRootSignature(eType));
	CDevice::GetInstance()->GetCommandList()->SetPipelineState(pPipeline);

	
	XMMATRIX	xmMatWorld = XMMatrixTranspose(XMLoadFloat4x4(&matWorld));
	XMMATRIX	xmMatView = XMMatrixTranspose(XMLoadFloat4x4(&matView));
	XMMATRIX	xmMatProj = XMMatrixTranspose(XMLoadFloat4x4(&matProj));

	output.matWorld = xmMatWorld;
	output.matView = xmMatView;
	output.matProj = xmMatProj;

	return S_OK;
}

HRESULT CShader::SetUp_OnShader(ID3D12PipelineState* pPipeline, ID3D12Resource* pConstantBuffer, _matrix matWorld, _matrix matView, _matrix matProj, MAINPASS_LIGHT& output, ROOT_TYPE eType)
{
	CDevice::GetInstance()->GetCommandList()->SetGraphicsRootSignature(CDevice::GetInstance()->GetRootSignature(eType));
	CDevice::GetInstance()->GetCommandList()->SetPipelineState(pPipeline);


	XMMATRIX	xmMatWorld = XMMatrixTranspose(XMLoadFloat4x4(&matWorld));
	XMMATRIX	xmMatView = XMMatrixTranspose(XMLoadFloat4x4(&matView));
	XMMATRIX	xmMatProj = XMMatrixTranspose(XMLoadFloat4x4(&matProj));

	output.matWorld = xmMatWorld;
	output.matView = xmMatView;
	output.matProj = xmMatProj;

	return S_OK;
}


HRESULT CShader::SetUp_OnShader(ID3D12PipelineState* pPipeline, FbxAMatrix* FbxmatWorld, _matrix matView, _matrix matProj,
	_int iPassSize, void* pData, ROOT_TYPE eType)
{
	//CDevice::GetInstance()->GetCommandList()->SetGraphicsRootSignature(CDevice::GetInstance()->GetRootSignature(eType));
	CDevice::GetInstance()->GetCommandList()->SetPipelineState(pPipeline);

	XMFLOAT4X4	xmf4x4World = FbxMatrixToXmFloat4x4Matrix(FbxmatWorld);

	XMMATRIX	xmMatWorld = XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4World));
	XMMATRIX	xmMatView = XMMatrixTranspose(XMLoadFloat4x4(&matView));
	XMMATRIX	xmMatProj = XMMatrixTranspose(XMLoadFloat4x4(&matProj));

	tagMainPass tMainPass;
	tMainPass.matWorld = xmMatWorld;
	tMainPass.matView = xmMatView;
	tMainPass.matProj = xmMatProj;

	memcpy_s(pData, iPassSize, (void*)&tMainPass, sizeof(tMainPass));

	return S_OK;
}

HRESULT CShader::SetUp_OnShader(ID3D12PipelineState* pPipeline, FbxAMatrix* FbxmatWorld, _matrix matView, _matrix matProj, MAINPASS_LIGHT& tPass, _int iPassSize, void* pData, ROOT_TYPE eType)
{
	CDevice::GetInstance()->GetCommandList()->SetPipelineState(pPipeline);

	XMFLOAT4X4	xmf4x4World = FbxMatrixToXmFloat4x4Matrix(FbxmatWorld);

	XMMATRIX	xmMatWorld = XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4World));
	XMMATRIX	xmMatView = XMMatrixTranspose(XMLoadFloat4x4(&matView));
	XMMATRIX	xmMatProj = XMMatrixTranspose(XMLoadFloat4x4(&matProj));

	tPass.matWorld = xmMatWorld;
	tPass.matView = xmMatView;
	tPass.matProj = xmMatProj;

	memcpy_s(pData, iPassSize, (void*)&tPass, sizeof(MAINPASS_LIGHT));

	return S_OK;
}

XMFLOAT4X4 CShader::FbxMatrixToXmFloat4x4Matrix(FbxAMatrix* pfbxmtxSource)
{
	FbxVector4 S = pfbxmtxSource->GetS();
	//	FbxVector4 R = FbxVector4(0.0, 45.0, 0.0, 1.0);
	FbxVector4 R = pfbxmtxSource->GetR();
	FbxVector4 T = pfbxmtxSource->GetT();

	FbxAMatrix fbxmtxTransform = FbxAMatrix(T, R, S);

	XMFLOAT4X4 xmf4x4Result;
	for (int i = 0; i < 4; i++)
	{
		//		for (int j = 0; j < 4; j++) xmf4x4Result.m[i][j] = (float)fbxmtxTransform[i][j];
		for (int j = 0; j < 4; j++) xmf4x4Result.m[i][j] = (float)(*pfbxmtxSource)[i][j];
	}

	XMFLOAT3 xmf3S = XMFLOAT3((float)S.mData[0], (float)S.mData[1], (float)S.mData[2]);
	XMFLOAT3 xmf3R = XMFLOAT3((float)R.mData[0], (float)R.mData[1], (float)R.mData[2]);
	XMFLOAT3 xmf3T = XMFLOAT3((float)T.mData[0], (float)T.mData[1], (float)T.mData[2]);
	//	XMFLOAT4X4 xmf4x4Transform;
	//	XMStoreFloat4x4(&xmf4x4Transform, XMMatrixAffineTransformation(XMLoadFloat3(&xmf3S), XMVectorZero(), XMQuaternionRotationRollPitchYaw(XMConvertToRadians(xmf3R.x), XMConvertToRadians(xmf3R.y), XMConvertToRadians(xmf3R.z)), XMLoadFloat3(&xmf3T)));

		//XMFLOAT4X4 xmf4x4Translation;
		//XMMATRIX xmT = XMMatrixTranslation(xmf3T.x, xmf3T.y, xmf3T.z);
		//XMStoreFloat4x4(&xmf4x4Translation, xmT);
		//XMFLOAT4X4 xmf4x4Rotation;
		//XMMATRIX xmR = XMMatrixRotationRollPitchYaw(XMConvertToRadians(xmf3R.x), XMConvertToRadians(xmf3R.y), XMConvertToRadians(xmf3R.z));
		//XMStoreFloat4x4(&xmf4x4Rotation, xmR);
		//XMStoreFloat4x4(&xmf4x4Rotation, ));
		//XMFLOAT4X4 xmf4x4Scale;
		//XMStoreFloat4x4(&xmf4x4Scale, XMMatrixScaling(xmf3S.x, xmf3S.y, xmf3S.z));

	XMMATRIX Rx = XMMatrixRotationX(XMConvertToRadians(xmf3R.x));
	XMMATRIX Ry = XMMatrixRotationY(XMConvertToRadians(xmf3R.y));
	XMMATRIX Rz = XMMatrixRotationZ(XMConvertToRadians(xmf3R.z));
	XMMATRIX xmR = XMMatrixMultiply(XMMatrixMultiply(Rx, Ry), Rz);
	XMFLOAT4X4 xmf4x4Multiply;
	XMStoreFloat4x4(&xmf4x4Multiply, XMMatrixMultiply(XMMatrixMultiply(XMMatrixScaling(xmf3S.x, xmf3S.y, xmf3S.z), xmR), XMMatrixTranslation(xmf3T.x, xmf3T.y, xmf3T.z)));

	return(xmf4x4Result);
}

CShader* CShader::Create(ID3D12Device* pGraphic_Device, const _tchar* pFilepath, const char* VSEntry, const char* PSEntry, _uint iFlag)
{
	CShader* pInstance = new CShader(pGraphic_Device);

	if (FAILED(pInstance->Ready_Shader(pFilepath, VSEntry, PSEntry, iFlag)))
	{
		MessageBox(0, L"CShader Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CShader::Clone_Component(void* pArg)
{
	return new CShader(*this);
}

void CShader::Free()
{
	CComponent::Free();
}
