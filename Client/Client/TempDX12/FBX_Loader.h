#pragma once
#include "Skinned_Data.h"


class CFBX_Loader
{
private:
	CFBX_Loader();
	~CFBX_Loader();
public:
	HRESULT LoadFBX(string strFileName, vector<Vertex>& outVertexVector,
					vector<uint32_t>& outIndexVector,vector<Material>& outMaterial);
public:
	HRESULT Load_Mesh(string strFileName, vector<Vertex>& outVertexVector,
					vector<uint32_t>& outIdxVector, vector<Material>* outMertarial = nullptr);
public:
	void	GetControlPoints(FbxNode* pFbxRootNode);
	void	GetVerticesAndIndice(FbxMesh* pMesh, vector<Vertex>& outVertexVector, vector<uint32_t>& outIndexVector);
	void	GetMaterials(fbxsdk::FbxNode* pNode, std::vector<Material>& outMaterial);
	void	GetMaterialAttribute(FbxSurfaceMaterial* pMaterial, Material& outMaterial);
	void	GetMaterialTexture(FbxSurfaceMaterial* pMaterial, Material& Mat);
public:
	void	ExportMesh(string fileName, vector<Vertex>& outVertexVector, vector<uint32_t>& outIndexVector,vector<Material>& outMaterial);
public:
	static	FbxManager* m_pFbxManager;
	unordered_map<unsigned int, CtrlPoint*> mControlPoints;
};

