#ifndef QSE_XMLRESOURCE_HPP
#define QSE_XMLRESOURCE_HPP


#include "ResCache.hpp"
#include <tinyxml2.h>


class XmlResourceExtraData : public IResourceExtraData
{
    tinyxml2::XMLDocument m_xmlDocument;

public:
    virtual string VToString() { return "XmlResourceExtraData"; }
    void ParseXml(char* pRawBuffer);
    tinyxml2::XMLElement* GetRoot(void) { return m_xmlDocument.RootElement(); }
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
    static XMLElement* LoadAndReturnRootXmlElement(const char* resourceString);
};


#endif