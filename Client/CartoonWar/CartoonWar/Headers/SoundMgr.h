#pragma once

#include "Base.h"
//using namespace FMOD;

class CSoundMgr :
    public CBase
{
	_DECLARE_SINGLETON(CSoundMgr)

private:
	explicit CSoundMgr();
	virtual ~CSoundMgr() = default;
public:
	HRESULT						Ready_Channel();
public:
	HRESULT						Add_Sound(Sound_Character eCharacter, SoundState State, const char* pFilePath, const _float& fVolume);
	HRESULT						Add_BGSound(Sound_Character eCharacter, SoundState State, const char* pFilePath, const _float& fVolume);
public:
	void						Play_Sound(Sound_Character eCharacter, SoundState State, FMOD_MODE eMode = FMOD_LOOP_OFF);
	void						Play_BGSound(Sound_Character eCharacter, SoundState State);
	void						Pause_Sound();
	void						Pause_BGSound();
public:
	void						Play_Sound(SoundChannel eChannel, Sound_Character eCharacter, SoundState State, const _float& fVolume = 0.5f, FMOD_MODE eMode = FMOD_LOOP_OFF);
	void						SetVolume(SoundChannel eChannel, const _float& fVolume);
	void						Pause_Sound(SoundChannel eChannel);
	void						Stop_Sound(SoundChannel eChannel);
	void						Update_Sound();
public:
	_bool						IsPlaying(SoundChannel eChannel, _bool IsPlaying);
protected:
	FMOD::System*				m_pFmod = nullptr;
	FMOD::Channel*				m_pCh[CHANNEL_END];
	SOUNDDEV					m_SoundDev[SOUND_END][END];
	_float						m_fVolume = 0.5f;
public:
	static CSoundMgr*			Create();
public:
	virtual void				Free();
};

