#pragma once
#include "VIBuffer.h"
class CBuffer_CubeTex :
	public CVIBuffer
{
private:
	CBuffer_CubeTex(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBuffer_CubeTex(const CBuffer_CubeTex& rhs);
	virtual ~CBuffer_CubeTex() = default;
public:
	HRESULT Ready_VIBuffer();
	void Render_VIBuffer();
public:
	virtual _bool Picking_ToBuffer(_vec3* pOut, const CTransform* pTransformCom, const CPicking* pPickingCom);
public:
	static CBuffer_CubeTex* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone_Component(void* pArg);
protected:
	virtual void Free();
};

