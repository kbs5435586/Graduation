#pragma once
#define WINCX 800
#define WINCY 600

const float	g_Near = 0.1f;
const float g_Far = 1000.f;

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

//------ Server
#define WM_SOCKET (WM_USER+1)
#define IDX_TO_ID(x) x + NPC_ID_START
#define ID_TO_IDX(x) x - NPC_ID_START
//------



enum SCENEID { SCENE_STATIC, SCENE_LOGO, SCENE_STAGE, SCENE_END };
enum TEXTURE_TYPE{TEXTURE_TYPE_DDS, TEXTURE_TYPE_ELSE,  TEXTURE_TGA, TEXTURE_TYPE_END};

enum ROOT_TYPE {ROOT_TYPE_COLOR=0, ROOT_TYPE_TEXTURE=1,ROOT_TYPE_MULTI_TEXTURE, ROOT_TYPE_END};

enum LIGHT_TYPE {LIGHT_POINT, LIGHT_SPOT, LIGHT_DIRECTIONAL, LIGHT_END};