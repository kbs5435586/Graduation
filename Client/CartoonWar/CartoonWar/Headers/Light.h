#pragma once
#include "Base.h"
class CShader;
class CTransform;
class CBuffer_RectTex;
class CRenderer;
class CDebug_Camera;
class CLight :
	public CBase
{
private:
	CLight();
	~CLight() = default;
public:
	HRESULT						Ready_Light(LIGHT & tLightInfo);
	void						Update();
	void						Render();
private:
	HRESULT						CreateInputLayout(_uint iType);
public:
	LIGHT&						Get_LightInfo() { return m_tLight; }
	void						Set_ArrIdx(_uint i) { m_iArridx = i; }
	_uint						Get_ArrIdx() { return m_iArridx; }
private:
	void						Set_LookAt(const _vec3& vLook);
private:
	LIGHT						m_tLight = {};
public:
	static CLight*				Create(LIGHT & tLightInfo);
private:
	virtual void				Free();
private:
	CTransform*					m_pTransformCom = nullptr;
	CShader*					m_pShader_Dir = nullptr;
	CShader*					m_pShader_Point = nullptr;
	CShader*					m_pShader_Merge = nullptr;
	CBuffer_RectTex*			m_pBufferCom_Dir = nullptr;
	CBuffer_RectTex*			m_pBufferCom_Point = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	_uint						m_iArridx = 0;
private:
	CDebug_Camera*				m_pGameObject = nullptr;
};

