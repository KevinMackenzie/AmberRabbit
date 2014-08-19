#ifndef QSE_DIRECTSOUNDAUDIO_HPP
#define QSE_DIRECTSOUNDAUDIO_HPP

#include "Audio.hpp"

// DirectSound includes
//#include <dsound.h>
//#include <mmsystem.h>

//ssfml audio includes
#include <SFML/Audio.hpp>

//////////////////////////////////////////////////////////////////////
// DirectSoundAudioBuffer						- Chapter 13, page 420
//
// Implements the rest of the IAudioBuffer interface left out by AudioBuffer.  
// If you are interested in implementing a sound system using OpenAL 
// you'd create a class OpenALAudioBuffer from AudioBuffer.
// 
//////////////////////////////////////////////////////////////////////

class SFMLSoundBuffer : public AudioBuffer
{
protected:
	sf::SoundBuffer m_Sample;
	shared_ptr<sf::Sound> m_pSound;

public:
	SFMLSoundBuffer(sf::SoundBuffer sample, shared_ptr<ResHandle> resource);
	virtual void *VGet();
	virtual bool VOnRestore();

	virtual bool VPlay(int volume, bool looping);
	virtual bool VPause();
	virtual bool VStop();
	virtual bool VResume();

	virtual bool VTogglePause();
	virtual bool VIsPlaying();
	virtual void VSetVolume(int volume);
    virtual void VSetPosition(unsigned long newPosition);

	virtual float VGetProgress();

private:
	HRESULT FillBufferWithSound();
	//HRESULT RestoreBuffer( BOOL* pbWasRestored );
};



//////////////////////////////////////////////////////////////////////
// class DirectSoundAudio						- Chapter 13, page 414
//
// Implements the rest of the IAudio interface left out by Audio.  
// If you are interested in implementing a sound system using OpenAL 
// you'd create a class OpenALAudioBuffer from AudioBuffer.
// 
//////////////////////////////////////////////////////////////////////

class SFMLSoundAudio : public Audio
{
public:
	SFMLSoundAudio() {  }
	virtual bool VActive() { return true; }

	virtual IAudioBuffer *VInitAudioBuffer(shared_ptr<ResHandle> handle);
	virtual void VReleaseAudioBuffer(IAudioBuffer* audioBuffer);

	virtual void VShutdown();
	virtual bool VInitialize(HWND hWnd);

protected:


    //IDirectSound8* m_pDS;

	//HRESULT SetPrimaryBufferFormat( DWORD dwPrimaryChannels, 
	//								DWORD dwPrimaryFreq, 
	//								DWORD dwPrimaryBitRate );
};

//NOTE: sf::Music is not used, because our resource loading system loads directly into memory anyway

#endif