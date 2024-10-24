#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CBuffer_RectTex;
class CShader;
class CTexture;
class CTextureRect :
    public CGameObject
{
private:
	CTextureRect(ID3D12Device* pGraphic_Device);
	CTextureRect(const CTextureRect& rhs);
	virtual ~CTextureRect() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float & fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float & fTimeDelta);
	virtual void							Render_GameObject();
public:
	virtual HRESULT							CreatePipeLine(CShader * pShader);
	virtual D3D12_RASTERIZER_DESC			CreateRaterizerState();
	virtual D3D12_DEPTH_STENCIL_DESC		CreateDepthStencilState();
	virtual D3D12_BLEND_DESC				CreateBlendState();
	virtual D3D12_INPUT_LAYOUT_DESC			CreateInputLayout();
private:
	HRESULT									CreateConstantBuffer();
public:
	static CTextureRect*					Create(ID3D12Device * pGraphic_Device);
	virtual CGameObject*					Clone_GameObject(void* pArg = nullptr, const _uint& iIdx = 0) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component(void* pArg);
private:
	CTransform*								m_pTransformCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CBuffer_RectTex*						m_pBufferCom = nullptr;
	CShader*								m_pShaderCom = nullptr;
	CTexture*								m_pTextureCom = nullptr;


};

