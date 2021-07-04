#pragma once
//#define WINCX 1920
//#define WINCY 1080

#define WINCX 1920
#define WINCY 1080

#define DEAD_OBJ 1
#define NO_EVENT 0

const float	g_Near = 0.1f;
const float g_Far = 300.f;




#define NO_COPY(CLASSNAME)									\
	private:												\
	CLASSNAME(const CLASSNAME&);							\
	CLASSNAME& operator = (const CLASSNAME&);				

#define _DECLARE_SINGLETON(CLASSNAME)						\
	NO_COPY(CLASSNAME)										\
	private:												\
	static CLASSNAME*	m_pInstance;						\
	public:													\
	static CLASSNAME*	GetInstance( void );				\
	static unsigned long DestroyInstance( void );					

#define _IMPLEMENT_SINGLETON(CLASSNAME)						\
	CLASSNAME*	CLASSNAME::m_pInstance = NULL;				\
	CLASSNAME*	CLASSNAME::GetInstance( void )	{			\
		if(NULL == m_pInstance) {							\
			m_pInstance = new CLASSNAME;					\
		}													\
		return m_pInstance;									\
	}														\
	unsigned long CLASSNAME::DestroyInstance( void ) {		\
		unsigned long dwRefCnt = 0;							\
		if(NULL != m_pInstance)	{							\
			dwRefCnt = m_pInstance->Release();				\
			if(0 == dwRefCnt) m_pInstance = NULL;			\
		}													\
		return dwRefCnt;									\
	}


#define _MSG_BOX(MESSAGE) MessageBox(0, TEXT(MESSAGE), TEXT("Error"), MB_OK);


#ifdef _DEBUG 
#define _CRTDBG_MAP_ALLOC 
#include <stdlib.h> 
#include <crtdbg.h> 

#ifndef DBG_NEW 
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 
#endif 
#endif  // _DEBUG  


#define NULL_CHECK_VOID(PTR) if(nullptr == PTR) return;
#define NULL_CHECK_VAL(PTR, RETURN_VAL) if(nullptr == PTR) return RETURN_VAL;

