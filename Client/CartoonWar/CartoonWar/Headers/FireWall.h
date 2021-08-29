#pragma once
#include "GameObject.h"

class CTransform;
class CRenderer;
class CBuffer_RectTex;
class CTexture;
class CShader;
class CFireWall :
    public CGameObject
{
private:
	CFireWall();
	CFireWall(const CFireWall& rhs);
	virtual ~CFireWall() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float & fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float & fTimeDelta);
	virtual void							Render_GameObject();

private:
	virtual HRESULT							CreateInputLayout();
public:
	static CFireWall*						Create();
	virtual CGameObject*					Clone_GameObject(void* pArg = nullptr, _uint iIdx = 0) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	CTransform*								m_pTransformCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CBuffer_RectTex*						m_pBufferCom = nullptr;
	CShader*								m_pShaderCom = nullptr;
	CTexture*								m_pTextureCom[3] = { nullptr };
private:
	TEXINFO									m_tTexInfo = {};

private:
	_float									m_range = {};
	_float									damageTime{};
	_float									endTime{};
	_bool									startCheck{};
	_bool									damageCheck{};
	_bool									IsGetDamage{};

	static _bool First;
public:
	void setDTime(_float _t) { damageTime = _t; }
	_float& GetDTime() { return damageTime; }

	void SetETime(_float _t) { endTime = _t; }
	_float& GetETime() { return endTime; }

	void SetSCheck(_bool _b) { startCheck = _b; }
	_bool& GetSCheck() { return startCheck; }

	void SetDCheck(_bool _b) { damageCheck = _b; }
	_bool& GetDCheck() { return damageCheck; }

	void SetDamage(_bool _b) { IsGetDamage = _b; }
	_bool& GetDamage() { return IsGetDamage; }
};

