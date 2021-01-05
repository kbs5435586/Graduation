#pragma once
#include "VIBuffer.h"
class CBuffer_RcTex :
	public CVIBuffer
{
private:
	explicit CBuffer_RcTex(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CBuffer_RcTex(const CBuffer_RcTex& rhs);
	virtual ~CBuffer_RcTex() = default;
public:
	HRESULT				Ready_VIBuffer();
	void				Render_VIBuffer();
public:
	virtual _bool		Picking_ToBuffer(_vec3* pOut, const CTransform* pTransformCOm, const CPicking* pPickingCom);
public:
	static CBuffer_RcTex* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone_Component(void* pArg);
	virtual void		Free();
};

