#pragma once
#include "GameObject.h"
class CTransform;
class CRenderer;
class CShader;
class CTexture;
class CMesh;
class CBuffer_RectTex;
class CFrustum;
class CTexture;
class CTeleport :
    public CGameObject
{
private:
	CTeleport();
	CTeleport(const CTeleport& rhs);
	virtual ~CTeleport() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float & fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float & fTimeDelta);
	virtual void							Render_GameObject();
	virtual void							Render_GameObject_Shadow();
	virtual void							Render_GameObject_Range();
private:
	HRESULT									Ready_Component();
	virtual HRESULT							CreateInputLayout();
public:
	static CTeleport* Create();
	virtual CGameObject* Clone_GameObject(void* pArg = nullptr, _uint iIdx = 0);
private:
	virtual void							Free();
private:
	CTransform* m_pTransformCom = nullptr;
	CTransform* m_pTransformCom_Range = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CMesh* m_pMeshCom = nullptr;
	CBuffer_RectTex* m_pBufferCom = nullptr;
	CShader* m_pShaderCom_Shadow = nullptr;
	CShader* m_pShaderCom_Range = nullptr;
	CFrustum* m_pFrustumCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
public:
	_bool									StartCheck{};
	_bool									OnceCheck{};

	_uint									m_MyID{};
	_uint									m_FriendID{};
	_float									m_range = {};
	_vec3									m_FriendPos{};

	CGameObject* m_MyFriend = {};

	CGameObject* GetMyFriend() { return m_MyFriend; };
	void SetMyFriend(CGameObject* _b) { m_MyFriend = _b; };

	_bool& GetSCheck() { return StartCheck; };
	void SetSCheck(_bool _b) { StartCheck = _b; };

	_bool& GetOnce() { return OnceCheck; };
	void SetOnce(_bool _b) { OnceCheck = _b; };

	_float& GetRange() { return m_range; };
	void SetRange(_bool _b) { m_range = _b; };

	_uint& GetID() { return m_MyID; };
	void SetID(_uint _f) { m_MyID = _f; };

	static _bool First;
	//
	//_uint& GetFriend() { return m_FriendID; };
	//void SetFriend(_uint _f) { m_FriendID = _f; };
	//
	//_vec3& GetFPos() { return m_FriendPos; };
	//void SetFPos(_vec3 _f) { m_FriendPos = _f; };
};

