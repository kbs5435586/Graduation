#pragma once
#include "Component.h"
class CStructedBuffer;
class CMesh;
class CAnimator :
    public CComponent
{
private:
    CAnimator();
    CAnimator(const CAnimator& rhs);
    virtual ~CAnimator() = default;
public:
    HRESULT                         Ready_Animator();
private:
    const   vector<tMTBone>*        m_pVecBones;
    const   vector<tMTAnimClip>*    m_pVecClip;
private:
    vector<_float>                  m_vecClipUpdateTime;
    vector<_matrix>                 m_vecFinalBoneMat;
    int							    m_iFrameCount=30; // 30
    _float						    m_fCurTime= 0.f;
    int							    m_iCurClip=0; // 클립 인덱스	
    int							    m_iFrameIdx=0; // 클립의 현제 프레임

    CStructedBuffer*                m_pBoneFinalMat = nullptr;  // 특정 프레임의 최종 행렬
    bool						    m_IsFinalMatUpdate = false; // 최종행렬 연산 수행여부
public:
    void                            SetBones(const vector<tMTBone>* _vecBones) { m_pVecBones = _vecBones; m_vecFinalBoneMat.resize(m_pVecBones->size()); }
    void                            SetAnimClip(const vector<tMTAnimClip>* _vecAnimClip);
public:
    void                            Update(const _float& fTimeDelta);
    void                            UpdateData(CMesh* pMesh);
    void                            SetClipTime(int _iClipIdx, float _fTime) { m_vecClipUpdateTime[_iClipIdx] = _fTime; }
    vector<_matrix>&                GetFinalBoneMat() { return m_vecFinalBoneMat; }
private:
    void                            Dispatch(int x, int y, int z);
public:
    void                            CheckMesh(CMesh* pMesh);
public:
    static CAnimator*               Create();
    virtual CComponent*             Clone_Component(void* pArg = nullptr);
private:
    virtual void                    Free();
};

