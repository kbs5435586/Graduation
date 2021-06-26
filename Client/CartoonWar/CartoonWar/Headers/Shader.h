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
											const char* GSEntry = nullptr, const char* DSEntry = nullptr, const char* HSEntry = nullptr);

	HRESULT								Create_Shader(vector< D3D12_INPUT_ELEMENT_DESC> vecDesc, RS_TYPE eType = RS_TYPE::DEFAULT, 
													DEPTH_STENCIL_TYPE eDepthType = DEPTH_STENCIL_TYPE::LESS, 
													SHADER_TYPE eShaderType = SHADER_TYPE::SHADER_FORWARD,
													BLEND_TYPE eBlendType = BLEND_TYPE::DEFAULT, D3D_PRIMITIVE_TOPOLOGY eTopology = D3D_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	HRESULT								Ready_Shader(const _tchar* pFilePath, const char* CSEntry);
public:
	HRESULT								SetUp_OnShader(_matrix matWorld, _matrix matView, 
											_matrix matProj, MAINPASS& output);
	HRESULT								SetUp_OnShaderT(_matrix matWorld, _matrix matView,
		_matrix matProj, MAINPASS& output);
	HRESULT								SetUp_OnShader_FbxMesh(_matrix matWorld, _matrix matView, _matrix matProj, MAINPASS& tPass);
public:
	void								UpdateData_CS();
public:
	ComPtr<ID3D12PipelineState>			GetPipeLine() { return m_pPipeLineState; }
	ComPtr<ID3D12PipelineState>			GetCSPipeLine() { return m_pPilelineState_CS; }
private:
	ComPtr<ID3DBlob>					m_pVSBlob = nullptr;
	ComPtr<ID3DBlob>					m_pHSBlob = nullptr;
	ComPtr<ID3DBlob>					m_pDSBlob = nullptr;
	ComPtr<ID3DBlob>					m_pGSBlob = nullptr;
	ComPtr<ID3DBlob>					m_pPSBlob = nullptr;
	ComPtr<ID3DBlob>					m_pCSBlob = nullptr;
	ComPtr<ID3DBlob>					m_pErrBlob = nullptr;
private:
	ComPtr<ID3D12PipelineState>			m_pPipeLineState = nullptr;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC  m_tPipeline = {};
private:
	ComPtr<ID3D12PipelineState>			m_pPilelineState_CS;
	D3D12_COMPUTE_PIPELINE_STATE_DESC   m_tPipeline_CS;
public:
	static CShader*						Create(const _tchar* pFilepath, const char* VSEntry,
											const char* PSEntry, const char* GSEntry = nullptr, const char* DSEntry = nullptr, const char* HSEntry = nullptr);
	static CShader*						Create(const _tchar* pFilePath, const char* CSEntry);
	virtual CComponent*					Clone_Component(void* pArg = nullptr);
private:
	virtual void						Free();
};

