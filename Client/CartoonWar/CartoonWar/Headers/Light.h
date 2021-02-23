#pragma once
#include "Base.h"
class CShader;
class CLight :
	public CBase
{
private:
	CLight();
	~CLight() = default;
public:
	HRESULT						Ready_Light(const LIGHT& tLightInfo);
	void						Render_Light();
public:
	LIGHT&						Get_LightInfo() { return m_tLight; }
private:
	HRESULT						Create_InputLayOut();
private:
	LIGHT						m_tLight = {};
	CShader*					m_pShaderCom = nullptr;
private:
	ComPtr<ID3D12Resource>		m_pVertexBuffer = nullptr;
	ComPtr<ID3D12Resource>		m_pVertexUploadBuffer = nullptr;
	ComPtr<ID3D12Resource>		m_pIndexBuffer = nullptr;
	ComPtr<ID3D12Resource>		m_pIndexUploadBuffer = nullptr;
private:
	D3D12_PRIMITIVE_TOPOLOGY	m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	_uint						m_iSlot = 0;
	_uint						m_iStride = 0;
	_uint						m_iOffset = 0;
private:
	_uint						m_iNumVertices = 0;
	_uint						m_iNumIndices = 0;
private:
	D3D12_VERTEX_BUFFER_VIEW	m_tVertexBufferView;
	D3D12_INDEX_BUFFER_VIEW		m_tIndexBufferView;
public:
	static CLight*				Create(const LIGHT& tLightInfo);
private:
	virtual void				Free();
};

