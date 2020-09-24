#pragma once
#include "Component.h"
class CShader :
	public CComponent
{
private:
	CShader(ID3D12Device* pGraphic_Device);
	CShader(const CShader& rhs);
	virtual ~CShader() = default;
public:
	HRESULT							Ready_Shader(const _tchar* pFilepath, const char* VSEntry, const char* PSEntry, _uint iFlag);
	HRESULT							Compile_Shader();
	HRESULT							SetUp_OnShader(ID3D12PipelineState* pPipeline, ID3D12Resource* pConstantBuffer,
		_matrix matWorld, _matrix matView, _matrix matProj, MAINPASS& output, ROOT_TYPE eType);

public:
	static CShader*					Create(ID3D12Device* pGraphic_Device, const _tchar* pFilepath, const char* VSEntry, const char* PSEntry, _uint iFlag);
	virtual CComponent*				Clone_Component(void* pArg = nullptr);
protected:
	virtual void					Free();
public:
	ID3DBlob*						GetVertexShader() { return m_pVertexShader; }
	ID3DBlob*						GetPixelShader() { return m_pPixelShader; }
private:
	ID3DBlob*						m_pVertexShader = nullptr;
	ID3DBlob*						m_pPixelShader = nullptr;
	_uint							m_iCompileFlags = 0;
private:
	const _tchar*					m_pFilePath = nullptr;
	const char*						m_pVSEntryPoint = nullptr;
	const char*						m_pPSEntryPoint = nullptr;

};

