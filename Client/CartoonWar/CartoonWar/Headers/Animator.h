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
    int							    m_iCurClip=0; // Ŭ�� �ε���	
    int							    m_iFrameIdx=0; // Ŭ���� ���� ������

    CStructedBuffer*                m_pBoneFinalMat = nullptr;  // Ư�� �������� ���� ���
    bool						    m_IsFinalMatUpdate = false; // ������� ���� ���࿩��
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

