#pragma once
#include "Base.h"
class CObserver;
class CObserverManager :
    public CBase
{
    _DECLARE_SINGLETON(CObserverManager);
public:
   
private:
    CObserverManager();
    virtual ~CObserverManager() = default;
public:
    HRESULT                     Ready_ObserverManager();
    void                        Add_Data(DATA_TYPE eType, void* pData);
    void                        Remove_Data(DATA_TYPE eType, void* pData);
public:
    list<void*>*                Get_List(DATA_TYPE eType);
public:
    static  CObserverManager*   Create();
private:
    virtual void                Free();

private:
    DATA_TYPE                   m_eType = DATA_TYPE::DATA_END;
private:
    map<DATA_TYPE, list<void*>> m_mapData;


public:
    void                        Subscribe(CObserver* pObserver);	// 관찰자 등록
    void                        UnSubscribe(CObserver* pObserver); // 등록 해지
    void                        Notify(DATA_TYPE eType, void* pData = nullptr);	// 관찰자들에게 알림
private:
    list<CObserver*>            m_lstSubject;
};

