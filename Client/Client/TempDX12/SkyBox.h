#pragma once
#include "GameObject.h"
class CTransform;
class CBuffer_CubeTex;
class CRenderer;
class CShader;
class CTexture;
class CSkyBox :
    public CGameObject
{
private:
    CSkyBox(ID3D12Device* pGraphic_Device);
    CSkyBox(const CSkyBox& rhs);
    virtual ~CSkyBox() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
public:
	virtual HRESULT							CreatePipeLine(CShader* pShader);
	virtual D3D12_RASTERIZER_DESC			CreateRaterizerState();
	virtual D3D12_DEPTH_STENCIL_DESC		CreateDepthStencilState();
	virtual D3D12_BLEND_DESC				CreateBlendState();
	virtual D3D12_INPUT_LAYOUT_DESC			CreateInputLayout();
private:
	HRESULT									CreateConstantBuffer();
public:
	static									CSkyBox* Create(ID3D12Device* pGraphic_Device);
	virtual									CGameObject* Clone_GameObject(void* pArg = nullptr) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component(void* pArg);
private:
	CTransform*								m_pTransformCom = nullptr;
	CBuffer_CubeTex*						m_pBufferCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CShader*								m_pShaderCom = nullptr;
	CTexture*								m_pTextureCom = nullptr;
};

