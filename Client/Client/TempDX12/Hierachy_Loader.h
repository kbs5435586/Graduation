#pragma once
#include "Base.h"
class CShader;
class CHierachy_Loader
	: public CBase
{
private:
	CHierachy_Loader(ID3D12Device* pGraphic_Device);
	~CHierachy_Loader() = default;
public:
	void								Ready_Hierachy_Loader(FbxNode* pFbxNode);
public:
	static CHierachy_Loader*			Create(ID3D12Device* pGraphic_Device, FbxScene* pFbxScene);
public:
	void								Render_Hierachy_Mesh(FbxNode* pFbxNode, FbxAMatrix matWorld,const _float& fTimeDelta);
	void								Render_Hierachy_Mesh(FbxNode* pFbxNode, FbxAMatrix matWorld, const _float& fTimeDelta, ID3D12PipelineState* pPipeLine,
																ID3D12Resource* pConstantBuffer,  CShader* pShader, void* pData, _uint iIdx);
public:
	void								Render_Mesh(FbxMesh* pFbxMesh, FbxAMatrix& NodeToRoot, 
													FbxAMatrix& GeometryOffset, FbxAMatrix matWorld);
	void								Render_Mesh(FbxMesh* pFbxMesh, FbxAMatrix& NodeToRoot,
													FbxAMatrix& GeometryOffset, FbxAMatrix matWorld, ID3D12PipelineState* pPipeLine,
													ID3D12Resource* pConstantBuffer, CShader* pShader, void* pData, _uint iIdx);
private:
	_matrix								ConvertFbxToMatrix(FbxAMatrix* fbxmat);
private:
	FbxAMatrix							Get_GeometricOffset(FbxNode* pFbxNode);
private:
	ID3D12Device*						m_pGraphic_Device = nullptr;
private:
	_uint								m_iNumVertices = 0;
	_uint								m_iNumIndices = 0;
	_uint								m_iNumPolygons = 0;
private:
	_uint								m_iMaxIdx = 0;
	static _uint						m_iCurrentIdx;
private:
	vector<ID3D12Resource*>				m_vecVertexBuffer;
	vector<ID3D12Resource*>				m_vecVertexUploadBuffer;
	vector<ID3D12Resource*>				m_vecIndexBuffer;
	vector<ID3D12Resource*>				m_vecIndexUploadBuffer;
private:
	vector<DYNAMIC_MESH_RENDER>			m_vecDynamic_Mesh_Render;
private:
	vector<_vec3>						m_vecPosition;
	vector<_uint>						m_vecIndecies;
	vector<_uint>						m_vecNumOfIndices;

private:
	virtual void						Free();
};