#pragma once


#define RUBY		0x0001
#define SAPPHIRE	0x0002
#define DIAMOND		0x0004

#define WINCX 800
#define WINCY 600

#define TILECX 130
#define TILECY 68

#define TILEX 20
#define TILEY 30

#define MIN_STR 64
#define MID_STR 128
#define MAX_STR 256

#define NO_EVENT	0
#define DEAD_OBJ	1

#define NULL_CHECK_VOID(PTR) if(nullptr == PTR) return;
#define NULL_CHECK_VAL(PTR, RETURN_VAL) if(nullptr == PTR) return RETURN_VAL;

#define ERR_MSG(message)						\
MessageBox(g_hWnd, message, L"Error!", MB_OK);

#define NO_COPY(ClassName)						\
private:										\
	ClassName(const ClassName& _Obj);			\
	ClassName& operator=(const ClassName& _Obj);

#define DECLARE_SINGLETON(ClassName)			\
		NO_COPY(ClassName)						\
public:											\
	static ClassName* GetInstance()				\
	{											\
		if(nullptr == m_pInstance)				\
			m_pInstance = new ClassName;		\
		return m_pInstance;						\
	}											\
	void DestroyInstance()						\
	{											\
		if(m_pInstance)							\
		{										\
			delete m_pInstance;					\
			m_pInstance = nullptr;				\
		}										\
	}											\
private:										\
	static ClassName*	m_pInstance;			

#define IMPLEMENT_SINGLETON(ClassName)			\
ClassName* ClassName::m_pInstance = nullptr;

#define GET_INSTANCE(ClassName)					\
ClassName::GetInstance()