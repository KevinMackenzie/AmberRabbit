#ifndef QSE_SCRIPTRESOURCE_HPP
#define QSE_SCRIPTRESOURCE_HPP

// --------------------------------------------------------------------------
// File:        ScriptResource.h
//
// Purpose:     The declaration of a quick'n dirty ZIP file reader class.
//              Original code from Javier Arevalo.
//              Get zlib from http://www.cdrom.com/pub/infozip/zlib/
//
// class XmlFile - Chapter 7, page 186
// --------------------------------------------------------------------------

#include "../ResourceCache/ResCache.hpp"


class ScriptResourceLoader : public IResourceLoader
{
public:
    virtual bool VUseRawFile() { return false; }
    virtual bool VDiscardRawBufferAfterLoad() { return true; }
    virtual bool VAddNullZero() { return true; }
    virtual unsigned int VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize) { return rawSize; }
    virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
    virtual string VGetPattern() { return "*.lua"; }
};


#endif