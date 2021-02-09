#pragma once
#include "Component.h"
class CShader :
    public CComponent
{
private:
    CShader();
    CShader(const CShader& rhs);
    virtual ~CShader() = default;
public:
	HRESULT								Ready_Shader(const _tchar* pFilePath, const char* VSEntry, const char* PSEntry,
											const char*HSEntry = nullptr, const char* DSEntry = nullptr, const char* GSEntry = nullptr);
	HRESULT								Create_Shader(vector< D3D12_INPUT_ELEMENT_DESC> vecDesc, RS_TYPE eType = RS_TYPE::DEFAULT, SHADER_TYPE eShaderType = SHADER_TYPE::SHADER_FORWARD);
public:
	HRESULT								SetUp_OnShader(_matrix matWorld, _matrix matView, 
											_matrix matProj, MAINPASS& output);
	HRESULT								SetUp_OnShader_FbxMesh(_matrix matWorld, _matrix matView, _matrix matProj, MAINPASS& tPass);
public:
	ComPtr<ID3D12PipelineState>			GetPipeLine() { return m_pPipeLineState; }
private:
	ComPtr<ID3DBlob>					m_pVSBlob = nullptr;
	ComPtr<ID3DBlob>					m_pHSBlob = nullptr;
	ComPtr<ID3DBlob>					m_pDSBlob = nullptr;
	ComPtr<ID3DBlob>					m_pGSBlob = nullptr;
	ComPtr<ID3DBlob>					m_pPSBlob = nullptr;
	ComPtr<ID3DBlob>					m_pErrBlob = nullptr;
	ComPtr<ID3DBlob>					m_pCSBlob = nullptr;
private:
	ComPtr<ID3D12PipelineState>			m_pPipeLineState = nullptr;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC  m_tPipeline = {};
public:
	static CShader*						Create(const _tchar* pFilepath, const char* VSEntry,
											const char* PSEntry, const char* HSEntry = nullptr, const char* DSEntry = nullptr, const char* GSEntry = nullptr);
	virtual CComponent*					Clone_Component(void* pArg = nullptr);
private:
	virtual void						Free();
};

