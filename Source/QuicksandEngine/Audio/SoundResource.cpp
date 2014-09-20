#include "../Stdafx.hpp"

#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

//#include    <Vorbis/vorbis/codec.h>            // from the vorbis sdk
//#include    <Vorbis/vorbis/vorbisfile.h>       // also :)

#include "SoundResource.hpp"
#include "Audio.hpp"

	
//
// SoundResource::SoundResource			- Chapter X, page 362
//


SoundResourceExtraData::SoundResourceExtraData()/*
:	m_SoundType(SOUND_TYPE_UNKNOWN),
	m_bInitialized(false),
	m_LengthMilli(0)*/
{	
	m_pSample = QSE_NEW sf::SoundBuffer;
	// don't do anything yet - timing sound Initialization is important!
}

SoundResourceExtraData::~SoundResourceExtraData()
{
	SAFE_DELETE(m_pSample);
}

bool SoundResourceLoader::VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
	shared_ptr<SoundResourceExtraData> extra = shared_ptr<SoundResourceExtraData>(QSE_NEW SoundResourceExtraData());

	return extra->m_pSample->loadFromMemory(rawBuffer, rawSize);
}


shared_ptr<IResourceLoader> CreateWAVResourceLoader()
{
	return shared_ptr<IResourceLoader>(QSE_NEW WaveResourceLoader());
}

shared_ptr<IResourceLoader> CreateOGGResourceLoader()
{
	return shared_ptr<IResourceLoader>(QSE_NEW OggResourceLoader());
}

shared_ptr<IResourceLoader> CreateMP3ResourceLoader()
{
	return shared_ptr<IResourceLoader>(QSE_NEW Mp3ResourceLoader);
}