#pragma once
#include "Component.h"
class CStructedBuffer;
class CMesh;
class CShader;
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
     vector<tMTAnimClip>            m_vecClip;
public:
    const vector<tMTBone>*          GetBones(){return m_pVecBones;}
    vector<tMTAnimClip>             GetAnim(){return m_vecClip;}
public:
    int&                            GetCurClip() { return m_iCurClip; }
private:
    vector<_float>                  m_vecClipUpdateTime;
    vector<_matrix>                 m_vecFinalBoneMat;
    int							    m_iFrameCount=30; // 30
    _float						    m_fCurTime= 0.f;
    int							    m_iCurClip=0; // Ŭ�� �ε���	
    int							    m_iFrameIdx=0; // Ŭ���� ���� ������
private:
    int							    m_iNextFrameIdx; // ���� ������ �ε���
    float						    m_fRatio;	// ������ ���� ����

    CStructedBuffer*                m_pBoneFinalMat = nullptr;  // Ư�� �������� ���� ���
    bool						    m_IsFinalMatUpdate = false; // ������� ���� ���࿩��
public:
    void                            SetBones(const vector<tMTBone>* _vecBones);
    void                            SetAnimClip(vector<tMTAnimClip> _vecAnimClip);
public:
    _bool                           Update(AnimCtrl& tCtrl, const _float& fTimeDelta);
    void                            UpdateData(CMesh* pMesh, CShader* pShader);
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

