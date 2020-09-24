#pragma once
#include "VIBuffer.h"
class CBuffer_RectTex :
	public CVIBuffer
{
private:
	CBuffer_RectTex(ID3D12Device* pGraphic_Device);
	CBuffer_RectTex(const CBuffer_RectTex& rhs);
	CBuffer_RectTex()=default;
public:
	HRESULT					Ready_VIBuffer();
	void					Render_VIBuffer();
public:
	static					CBuffer_RectTex* Create(ID3D12Device* pGraphic_Device);
	virtual CComponent*		Clone_Component(void* pArg = nullptr);
protected:
	virtual void			Free();
};

