#pragma once
#include "Base.h"
class CLoader_Test :
    public CBase
{
private:
	CLoader_Test(ID3D12Device* pGraphic_Device);
	~CLoader_Test() = default;
public:
	HRESULT								Ready_Hierachy_Loader(string strFilePath, FbxScene*& pScene);
public:
	HRESULT								Ready_Load_Hierachy(FbxNode* pNode);
public:
	static CLoader_Test*				Create(ID3D12Device* pGraphic_Device,string strFilePath,  FbxScene*& pScene);
	virtual void						Free();
public:
	vector<MeshData*>&					GetMeshData() { return m_vecMeshData; }
private:
	vector< MeshData*>					m_vecMeshData;
};

