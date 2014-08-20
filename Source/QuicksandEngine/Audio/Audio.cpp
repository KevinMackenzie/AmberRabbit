#include "../Stdafx.hpp"

#include "Audio.hpp"
#include "SoundResource.hpp"

//#pragma comment( lib, "dsound" )

//////////////////////////////////////////////////////////////////////
// Globals
//////////////////////////////////////////////////////////////////////

Audio *g_pAudio = NULL;
char *gSoundExtentions[] = { ".mp3", ".wav", ".midi", ".ogg" };

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Audio::Audio():
	m_Initialized(false),
	m_AllPaused(false)
{
}

//
// Audio::VShutdown								- Chapter 13, page 413
//
void Audio::VShutdown()
{
	AudioBufferList::iterator i=m_AllSamples.begin();

	while (i!=m_AllSamples.end())
	{
		IAudioBuffer *audioBuffer = (*i);
		audioBuffer->VStop();
		m_AllSamples.pop_front();
	}
}

//
// Audio::VPauseAllSounds						- Chapter 13, page 413
//   Pause all active sounds, including music
//
void Audio::VPauseAllSounds()
{
	AudioBufferList::iterator i;
	AudioBufferList::iterator end;
	for(i=m_AllSamples.begin(), end=m_AllSamples.end(); i!=end; ++i)
	{
		IAudioBuffer *audioBuffer = (*i);
		audioBuffer->VPause();
	}

	m_AllPaused=true;
}

//
// Audio::VResumeAllSounds						- Chapter 13, page 413
//
void Audio::VResumeAllSounds()
{
	AudioBufferList::iterator i;
	AudioBufferList::iterator end;
	for(i=m_AllSamples.begin(), end=m_AllSamples.end(); i!=end; ++i)
	{
		IAudioBuffer *audioBuffer = (*i);
		audioBuffer->VResume();
	}

	m_AllPaused=false;
}

//
// Audio::VStopAllSounds						- Chapter 13, page 413
//
void Audio::VStopAllSounds()
{
	IAudioBuffer *audioBuffer = NULL;

	AudioBufferList::iterator i;
	AudioBufferList::iterator end;
	for(i=m_AllSamples.begin(), end=m_AllSamples.end(); i!=end; ++i)
	{
		audioBuffer = (*i);
		audioBuffer->VStop();
	}

	m_AllPaused=false;
}


//
// Audio::HasSoundCard							- not described in the book
//
//   A bit of an anachronism in name - but it simply returns true if the sound system is active.
//
bool Audio::HasSoundCard(void)
{
	return (g_pAudio && g_pAudio->VActive());
}


