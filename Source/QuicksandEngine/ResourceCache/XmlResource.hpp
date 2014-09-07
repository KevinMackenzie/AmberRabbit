#ifndef QSE_XMLRESOURCE_HPP
#define QSE_XMLRESOURCE_HPP


#include "ResCache.hpp"
#include <tinyxml2.h>


class XmlResourceExtraData : public IResourceExtraData
{
    tinyxml2::XMLDocument XmlDocument;

public:
    virtual string VToString() { return "XmlResourceExtraData"; }
    void ParseXml(char* pRawBuffer, long len);
	tinyxml2::XMLElement* GetRoot(void) { return XmlDocument.RootElement(); }
};


class XmlResourceLoader : public IResourceLoader
{
public:
    virtual bool VUseRawFile() { return false; }
	virtual bool VDiscardRawBufferAfterLoad() { return true; }
    virtual unsigned int VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize) { return rawSize; }
    virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
    virtual string VGetPattern() { return "*.xml"; }

    // convenience function
    static tinyxml2::XMLElement* LoadAndReturnRootXmlElement(const char* resourceString);
};


#endif