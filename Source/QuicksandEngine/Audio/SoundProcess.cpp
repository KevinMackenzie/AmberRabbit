#include "../Stdafx.hpp"

#include "Audio.hpp"
#include "SoundProcess.hpp"
#include "SoundResource.hpp"

//////////////////////////////////////////////////////////////////////
// SoundProcess Implementation
//////////////////////////////////////////////////////////////////////

//
// SoundProcess::SoundProcess				- Chapter 13, page 428
//
SoundProcess::SoundProcess(shared_ptr<ResHandle> resource, int volume, bool looping) :
	m_handle(resource),
	m_Volume(volume),
	m_isLooping(looping)
{
	InitializeVolume();
}


//
// SoundProcess::~SoundProcess				- Chapter 13, page 428
//
SoundProcess::~SoundProcess()
{
    if (IsPlaying())
        Stop();

	if (m_AudioBuffer)
		g_pAudio->VReleaseAudioBuffer(m_AudioBuffer.get());
}


void SoundProcess::InitializeVolume()
{
	// FUTURE WORK: Somewhere set an adjusted volume based on game options
	// m_volume = g_GraphicalApp->GetVolume(typeOfSound);
}

//
// SoundProcess::GetLengthMilli					- Chapter 13, page 430
//
int SoundProcess::GetLengthMilli()
{
	if ( m_handle && m_handle->GetExtra())
	{
		shared_ptr<SoundResourceExtraData> extra = static_pointer_cast<SoundResourceExtraData>(m_handle->GetExtra());
		return extra->GetLengthMilli();
	}
	else
    {
		return 0;
    }
}

//
// SoundProcess::VOnInitialize					- Chapter 13, page 428
//    note that the book incorrectly names this SoundProcess::OnInitialize
void SoundProcess::VOnInit()
{
    Process::VOnInit();

	//If the sound has never been... you know... then Play it for the very first time
	if ( m_handle == NULL || m_handle->GetExtra() == NULL)
		return;

	//This sound will manage it's own handle in the other thread
	IAudioBuffer *buffer = g_pAudio->VInitAudioBuffer(m_handle);

	if (!buffer)
	{
        Fail();
		return;
	}

	m_AudioBuffer.reset(buffer);	

	Play(m_Volume, m_isLooping);
}

//
// SoundProcess::OnUpdate						- Chapter 13, page 429
//
void SoundProcess::VOnUpdate(unsigned long deltaMs)
{
    if (!IsPlaying())
    {
        Succeed();
    }
}

//
// SoundProcess::IsPlaying						- Chapter 13, page 429
//
bool SoundProcess::IsPlaying()
{
	if ( ! m_handle || ! m_AudioBuffer )
		return false;
	
	return m_AudioBuffer->VIsPlaying();
}

//
// SoundProcess::SetVolume						- Chapter 13, page 430
//
void SoundProcess::SetVolume(int volume)
{
	if(m_AudioBuffer==NULL)
	{
		return;
	}

	LOG_ASSERT(volume>=0 && volume<=100 && "Volume must be a number between 0 and 100");
	m_Volume = volume;
	m_AudioBuffer->VSetVolume(volume);
}

//
// SoundProcess::GetVolume						- Chapter 13, page 430
//
int SoundProcess::GetVolume()
{
	if(m_AudioBuffer==NULL)
	{
		return 0;
	}

	m_Volume = m_AudioBuffer->VGetVolume();
	return m_Volume;
}

//
// SoundProcess::PauseSound						- Chapter 13, page 430
//   NOTE: This is called TogglePause in te book
//
void SoundProcess::PauseSound()
{
	if (m_AudioBuffer)
		m_AudioBuffer->VTogglePause();
}

//
// SoundProcess::Play							- Chapter 13, page 431
//
void SoundProcess::Play(const int volume, const bool looping)
{
	LOG_ASSERT(volume>=0 && volume<=100 && "Volume must be a number between 0 and 100");

	if(!m_AudioBuffer)
	{
		return;
	}
	
	m_AudioBuffer->VPlay(volume, looping);
}

//
// SoundProcess::Stop							- Chapter 13, page 431
//
void SoundProcess::Stop()
{
	if (m_AudioBuffer)
	{
		m_AudioBuffer->VStop();
	}
}

//
// SoundProcess::GetProgress					- Chapter 13, page 431
//
float SoundProcess::GetProgress()
{
	if (m_AudioBuffer)
	{
		return m_AudioBuffer->VGetProgress();
	}

	return 0.0f;
}




//
// ExplosionProcess::VOnInit					- Chapter 13, page 433
//
void ExplosionProcess::VOnInit()
{
	Process::VOnInit();
	Resource resource("explosion.wav");
	shared_ptr<ResHandle> srh = QuicksandEngine::g_pApp->m_ResCache->GetHandle(&resource);
	m_Sound.reset(QSE_NEW SoundProcess(srh));

	// Imagine cool explosion graphics setup code here!!!!
	//
	//
	//
}

//
// ExplosionProcess::OnUpdate					- Chapter 13, page 433
//
void ExplosionProcess::VOnUpdate(unsigned long deltaMs)
{
	// Since the sound is the real pacing mechanism - we ignore deltaMilliseconds
	float progress = m_Sound->GetProgress();
	
	switch (m_Stage)
	{
		case 0:
        {
			if (progress > 0.55f)
			{
				++m_Stage;
				// Imagine secondary explosion effect launch right here!
			}
			break;
        }

		case 1:
        {
			if (progress > 0.75f)
			{
				++m_Stage;
				// Imagine tertiary explosion effect launch right here!
			}
			break;
        }

		default:
        {
			break;
        }
	}
}

/////////////////////////////////////////////////////////////////////////////
// 
// FadeProcess Implementation 
//
//////////////////////////////////////////////////////////////////////

//
// FadeProcess::FadeProcess						- Chapter 13, page 435
//
FadeProcess::FadeProcess(shared_ptr<SoundProcess> sound, int fadeTime, int endVolume)
{
	m_Sound = sound;
	m_TotalFadeTime = fadeTime;
	m_StartVolume = sound->GetVolume();
	m_EndVolume = endVolume;
	m_ElapsedTime = 0;

	VOnUpdate(0);
}

//
// FadeProcess::OnUpdate						- Chapter 13, page 435
//
void FadeProcess::VOnUpdate(unsigned long deltaMs)
{
    m_ElapsedTime += deltaMs;

	if (m_Sound->IsDead())
		Succeed();

	float cooef = (float)m_ElapsedTime / m_TotalFadeTime;
	if (cooef>1.0f)
		cooef = 1.0f;
	if (cooef<0.0f)
		cooef = 0.0f;

	int newVolume = m_StartVolume + (int)( float(m_EndVolume - m_StartVolume) * cooef);

	if (m_ElapsedTime >= m_TotalFadeTime)
	{
		newVolume = m_EndVolume;
		Succeed();
	}

	m_Sound->SetVolume(newVolume);
}

