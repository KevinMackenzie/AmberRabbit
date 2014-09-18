#ifndef QSE_SOUNDRESOURCE_HPP
#define QSE_SOUNDRESOURCE_HPP

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include <mmsystem.h>

#include "../ResourceCache/ResCache.hpp"
#include <SFML/Audio.hpp>

//
// class SoundResourceExtraData				- Chapter 13, page 399
//
class  SoundResourceExtraData : public IResourceExtraData
{
	friend class  SoundResourceLoader;

public: 	
	SoundResourceExtraData();
	virtual ~SoundResourceExtraData() { }
	virtual std::string VToString() { return "SoundResourceExtraData"; }
	sf::SoundBuffer* GetSoundBuffer(){ return m_pSample; }
	//enum SoundType GetSoundType() { return m_SoundType; }
	//WAVEFORMATEX const *GetFormat() { return &m_WavFormatEx; }
	//int GetLengthMilli() const { return m_LengthMilli; }

protected:
	//enum SoundType m_SoundType;			// is this an Ogg, WAV, etc.?
	//bool m_bInitialized;				// has the sound been initialized
	//WAVEFORMATEX m_WavFormatEx;			// description of the PCM format
	//int m_LengthMilli;					// how long the sound is in milliseconds

	sf::SoundBuffer *m_pSample;
};


class SoundResourceLoader : public IResourceLoader
{
public:
	virtual bool VUseRawFile() { return false; }
	virtual bool VDiscardRawBufferAfterLoad() { return true; }
	virtual unsigned int VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize){ return rawSize; }
	virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
};

//
// class WaveResourceLoader						- Chapter 13, page 399
//
class  WaveResourceLoader : public SoundResourceLoader
{
public:
	virtual std::string VGetPattern() { return "*.wav"; }
};


//
// class OggResourceLoader						- Chapter 13, page 399
//
class  OggResourceLoader : public SoundResourceLoader
{
public:
	virtual std::string VGetPattern() { return "*.ogg"; }
};

class  Mp3ResourceLoader : public SoundResourceLoader
{
public:
	virtual std::string VGetPattern() { return "*.mp3"; }
};

#endif