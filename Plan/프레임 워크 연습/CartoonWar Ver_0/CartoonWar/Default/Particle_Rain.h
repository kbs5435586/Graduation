#pragma once
#include "GameObject.h"

class CTransform;
class CRenderer;
class CShader;
class CBuffer_RectCol;
class CMyRect;

class CParticle_Rain :
    public CGameObject
{
private:
	CParticle_Rain();
	CParticle_Rain(const CParticle_Rain& rhs);
	virtual ~CParticle_Rain() = default;
public:
	HRESULT Init();
	_int Update();
	_int LastUpdate();
	void Render();

	void Free();


private:
	vector<CMyRect> m_vRectList;
	//¿©·¯ °´Ã¼µéÀ» ´ã´Â º¤ÅÍ
	//CTransform* m_pTransformCom = nullptr;
	//CRenderer* m_pRendererCom = nullptr;
	//CBuffer_RectCol* m_pBufferCom = nullptr;
	//CShader* m_pShaderCom = nullptr;
	vector<CTransform*> m_vTransformComList;
	vector<CRenderer*> m_vRendererComList;
	vector<CBuffer_RectCol*> m_vBufferComList;
	vector<CShader*> m_vShaderComList;
};

