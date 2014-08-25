#ifndef QSE_SOUNDPROCESS_HPP
#define QSE_SOUNDPROCESS_HPP

#include "..\MainLoop\Process.hpp"

#include "Audio.hpp"
#include "SoundResource.hpp"

/////////////////////////////////////////////////////////////////////////////
// class SoundProcess							- Chapter 13, page 426
//
//    A Sound Process, not to be confused with a Sound Resource (SoundResource)
//	  manages a sound as it is being played. You can use this class to manage
//    timing between sounds & animations.
//
/////////////////////////////////////////////////////////////////////////////

class  SoundProcess : public Process
{
protected:
    shared_ptr<ResHandle> m_handle;			// this is the raw sound data
    shared_ptr<IAudioBuffer> m_AudioBuffer;		// handle to the implementation dependent audio buffer (DSound, Miles) 

    int m_Volume;						// these hold the initial setting until the sound is actually launched.
    bool m_isLooping;

public:
	SoundProcess(shared_ptr<ResHandle> soundResource, int volume=100, bool looping=false);
	virtual ~SoundProcess();

    void Play(const int volume, const bool looping);
    void Stop();

    void SetVolume(int volume);
    int GetVolume();
    int GetLengthMilli();
    bool IsSoundValid() { return m_handle != NULL; }
    bool IsPlaying();
    bool IsLooping() { return m_AudioBuffer && m_AudioBuffer->VIsLooping(); }
    float GetProgress();
    void PauseSound(void);

protected:
	virtual void VOnInit();
    virtual void VOnUpdate(unsigned long deltaMs);

    void InitializeVolume();

protected:
	SoundProcess();	 // Disable Default Construction

};



/////////////////////////////////////////////////////////////////////////////
// c;ass ExplosionProcess						- Chapter 13, page 433
//
// This is an example of a process that uses a simple state machine
// to control itself. 
//
/////////////////////////////////////////////////////////////////////////////

class  ExplosionProcess : public Process
{
protected:
    int m_Stage;
    shared_ptr<SoundProcess> m_Sound;

  public:
  	ExplosionProcess() { m_Stage=0; }

protected:   
    virtual void VOnInit();
  	virtual void VOnUpdate(unsigned long deltaMs);
};

/////////////////////////////////////////////////////////////////////////////
// class FadeProcess							- Chapter 13, page 435
//
// Fades sound volume in or out over time and then kills itself.
// This should be useful for groups of sound effects, too - such as when
//   an AI barks and it must be heard above the other effects like too much freaking thunder.
//
/////////////////////////////////////////////////////////////////////////////

class  FadeProcess : public Process
{
protected:
	shared_ptr<SoundProcess> m_Sound;
	
	int m_TotalFadeTime;
	int m_ElapsedTime;
	int m_StartVolume;
	int m_EndVolume;

public:
	FadeProcess(shared_ptr<SoundProcess> sound, int fadeTime, int endVolume);
	virtual void VOnUpdate(unsigned long deltaMs);	
};




#endif