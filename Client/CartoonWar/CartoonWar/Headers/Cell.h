#pragma once
#include "Base.h"
class CLine;
class CShader;
class CCell :
    public CBase
{
private:
    CCell();
    virtual ~CCell() = default;
public:
    HRESULT                     Ready_Cell(const _vec3* pPointA, const _vec3* pPointB, const _vec3* pPointC, const _uint& iIdx);
    _bool                       Is_inCell(_vec3 vMovedPos, LINE* pOutLine);
    _bool                       Compare_Point(_vec3* pPoint1, _vec3* pPoint2);
    void                        Clear_Neighbor();
    void                        Render_Cell();
public:
     _vec3*                Get_Point(POINT_ ePoint) { return &m_vPoint[(_uint)ePoint]; }
    CCell*                Get_Neighbor(NEIGHBOR eNeighbor) const { return m_pNeighbor[(_uint)eNeighbor]; }
    _ubyte                      Get_CellIndex()const { return m_iIndex; }
    _vec3*                      GetPoint() { return m_vPoint; }
    _bool&                      GetIsLast() { return m_IsLast; }
public:
    void                        Set_Neighbor(NEIGHBOR eNeighbor, CCell* pCell);
private:
    HRESULT                     Create_InputLayOut();
private:
    _vec3                       m_vPoint[(_uint)POINT_::POINT_END];
    _uint                       m_iIndex = 0;
    _bool                       m_IsLast = false;
private:
    CCell*                      m_pNeighbor[(_uint)NEIGHBOR::NEIGHBOR_END] = {nullptr};
    CLine*                      m_pLine[(_uint)LINE::LINE_END] = {nullptr };
public:
    static CCell*               Create(const _vec3* pPointA, const _vec3* pPointB, const _vec3* pPointC, const _uint& iIdx);
private:
    virtual void                Free();
private:
    ComPtr<ID3D12Resource>		m_pVertexBuffer = nullptr;
    ComPtr<ID3D12Resource>		m_pVertexUploadBuffer = nullptr;
    D3D12_VERTEX_BUFFER_VIEW	m_tVertexBufferView;
private:
    CShader*                    m_pShaderCom = nullptr;
};

