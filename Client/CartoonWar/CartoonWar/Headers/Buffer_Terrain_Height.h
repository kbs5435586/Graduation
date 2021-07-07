#pragma once
#include "VIBuffer.h"
class CQuadTree;
class CFrustum;
class CTransform;
class CBuffer_Terrain_Height :
    public CVIBuffer
{
private:
    CBuffer_Terrain_Height();
    CBuffer_Terrain_Height(const CBuffer_Terrain_Height& rhs);
    virtual ~CBuffer_Terrain_Height() = default;
private:
    void                            Calculate_TanBi(VTXTEXBUMP Vertex1, VTXTEXBUMP Vertex2, VTXTEXBUMP Vertex3, _vec3& vTangent, _vec3& vBinormal);
public:
    HRESULT                         Ready_VIBuffer(const _tchar* pFilePath, const _float& fInterval);
public:
    static CBuffer_Terrain_Height*  Create(const _tchar* pFilePath, const _float& fInterval = 1.f);
    virtual CComponent*             Clone_Component(void* pArg = nullptr);
public:
    _float                          Compute_HeightOnTerrain(CTransform* pTransform);
    HRESULT                         Culling_Frustum(CFrustum* pFrustum, const _matrix& matWorld);
private:
    virtual void                    Free();
private:
    _uint                           m_iNumVerticesX = 0;
    _uint                           m_iNumVerticesZ = 0;
    _uint                           m_iNumPolygons = 0;
    _float                          m_fInterval = 0.f;
private:
    _vec3*                          m_pPosition = nullptr;
    _ulong*                         m_pPixel = nullptr;
private:
    BITMAPFILEHEADER				m_fh;
    BITMAPINFOHEADER				m_ih;
private:
    _uint*                          m_pPolygonVertexIndex = nullptr;
    CQuadTree*                      m_pQuadTree = nullptr;
};

