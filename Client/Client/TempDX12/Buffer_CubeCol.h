#pragma once
#include "VIBuffer.h"
class CBuffer_CubeCol :
	public CVIBuffer
{
private:
	CBuffer_CubeCol(ID3D12Device* pGraphic_Device);
	CBuffer_CubeCol(const CBuffer_CubeCol& rhs);
	~CBuffer_CubeCol() = default;
public:
	HRESULT					Ready_VIBuffer();
	void					Render_VIBuffer();
public:
	static					CBuffer_CubeCol* Create(ID3D12Device* pGraphic_Device);
	virtual CComponent*		Clone_Component(void* pArg = nullptr);
protected:
	virtual void			Free();
private:
	vector<VTXCOL>			m_vecVertex;
};

