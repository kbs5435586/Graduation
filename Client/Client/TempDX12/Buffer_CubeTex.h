#pragma once
#include "VIBuffer.h"
class CBuffer_CubeTex :
    public CVIBuffer
{
private:
	CBuffer_CubeTex(ID3D12Device* pGraphic_Device);
	CBuffer_CubeTex(const CBuffer_CubeTex& rhs);
	~CBuffer_CubeTex() = default;
public:
	HRESULT					Ready_VIBuffer();
	void					Render_VIBuffer();
public:
	static					CBuffer_CubeTex* Create(ID3D12Device * pGraphic_Device);
	virtual CComponent*		Clone_Component(void* pArg = nullptr);
protected:
	virtual void			Free();
private:
	vector<VTXTEXCUBE>			m_vecVertices;
	vector<_uint>			m_vecIndices;
};

