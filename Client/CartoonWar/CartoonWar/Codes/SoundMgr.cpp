#include "framework.h"
#include "SoundMgr.h"
#include "Management.h"

_IMPLEMENT_SINGLETON(CSoundMgr)
CSoundMgr::CSoundMgr()
{
}
HRESULT CSoundMgr::Ready_Channel()
{
	if (FMOD_OK != FMOD::System_Create(&m_pFmod))
	{
		_MSG_BOX("FMod System Create Failed!");
		return E_FAIL;
	}
	if (FMOD_OK != m_pFmod->init(10, FMOD_INIT_NORMAL, NULL))
	{
		_MSG_BOX("FMod Init Create Failed!");
		return E_FAIL;
	}

	return S_OK;
}
HRESULT CSoundMgr::Add_Sound(Sound_Character eCharacter, SoundState State, const char* pFilePath, const _float fVolume)
{
	FMOD_RESULT i = FMOD_OK;
	if (FMOD_OK != (i = m_pFmod->createSound(pFilePath, FMOD_DEFAULT, 0, &m_SoundDev[eCharacter][State].pSound)))
	{
		_MSG_BOX("Sound Add Failed!");
		return E_FAIL;
	}
	m_SoundDev[eCharacter][State].fVolume = fVolume;
	return S_OK;
}
HRESULT CSoundMgr::Add_BGSound(Sound_Character eCharacter, SoundState State, const char* pFilePath, const _float fVolume)
{
	FMOD_RESULT i = FMOD_OK;
	if (FMOD_OK != (i = m_pFmod->createSound(pFilePath, FMOD_DEFAULT, 0, &m_SoundDev[eCharacter][State].pSound)))
	{
		_MSG_BOX("Sound Add Failed!");
		return E_FAIL;
	}
	m_SoundDev[eCharacter][State].fVolume = fVolume;
	return S_OK;
}
void CSoundMgr::Play_Sound(Sound_Character eCharacter, SoundState State)
{
	if (FMOD_OK != m_pFmod->playSound(m_SoundDev[eCharacter][State].pSound, nullptr, false, &m_pCh[CHANNEL_EFEECT]))
	{
		_MSG_BOX("Sound Play Failed!");
		return;
	}
	m_pCh[CHANNEL_EFEECT]->setVolume(m_SoundDev[eCharacter][State].fVolume);
}
void CSoundMgr::Play_Sound(SoundChannel eChannel, Sound_Character eCharacter, SoundState State, const _float& fVolume)
{
	m_pFmod->update();
	if (FMOD_OK != m_pFmod->playSound(m_SoundDev[eCharacter][State].pSound, nullptr, false, &m_pCh[eChannel]))
	{
		_MSG_BOX("Sound Play Failed!");
		return;
	}
	m_pCh[State]->setVolume(fVolume);
}

void CSoundMgr::Play_BGSound(Sound_Character eCharacter, SoundState State)
{
	m_pFmod->update();
	if (FMOD_OK != m_pFmod->playSound(m_SoundDev[eCharacter][State].pSound, nullptr, false, &m_pCh[CHANNEL_BG]))
	{
		_MSG_BOX("Sound Play Failed!");
		return;
	}
	if (State == BG_STAGE)
		m_pCh[CHANNEL_BG]->setVolume(m_SoundDev[eCharacter][State].fVolume);
}
void CSoundMgr::Pause_Sound()
{
	if (FMOD_OK != m_pCh[CHANNEL_EFEECT]->setPaused(false))
	{
		_MSG_BOX("Sound Pause Failed!");
		return;
	}
}
void CSoundMgr::Pause_BGSound()
{
	FMOD_RESULT i = FMOD_OK;
	if (FMOD_OK != (i = m_pCh[CHANNEL_BG]->stop()))
	{
		_MSG_BOX("Sound Pause Failed!");
		return;
	}
	m_pFmod->update();
}

void CSoundMgr::SetVolume(SoundChannel eChannel, const _float& fVolume)
{
	m_fVolume = fVolume;
	m_pCh[eChannel]->setVolume(m_fVolume);
}
void CSoundMgr::Pause_Sound(SoundChannel eChannel)
{
	if (FMOD_OK != m_pCh[eChannel]->setPaused(false))
	{
		_MSG_BOX("Sound Pause Failed!");
		return;
	}
}
void CSoundMgr::Stop_Sound(SoundChannel eChannel)
{
	FMOD_RESULT i = FMOD_OK;
	if (FMOD_OK != (i = m_pCh[eChannel]->stop()))
	{
		_MSG_BOX("Sound stop Failed!");
		return;
	}
}
void CSoundMgr::Update_Sound()
{
	m_pFmod->update();
}
CSoundMgr* CSoundMgr::Create()
{
	return nullptr;
}

void CSoundMgr::Free()
{
	for (size_t i = 0; i < SOUND_END; i++)
	{
		for (size_t j = 0; j < END; j++)
		{
			m_SoundDev[i][j].pSound->release();
		}
	}
	m_pFmod->release();
}
