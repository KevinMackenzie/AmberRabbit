#include "../Stdafx.hpp"
#include "XmlResource.hpp"

using namespace tinyxml2;

void XmlResourceExtraData::ParseXml(char* pRawBuffer, long len)
{
	std::string text = GLUFLoadBinaryArrayIntoString(pRawBuffer, len);
	XmlDocument.Parse(text.c_str());
}


bool XmlResourceLoader::VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
    if (rawSize <= 0)
        return false;

    shared_ptr<XmlResourceExtraData> pExtraData = shared_ptr<XmlResourceExtraData>(QSE_NEW XmlResourceExtraData());
    pExtraData->ParseXml(rawBuffer, rawSize);

    handle->SetExtra(shared_ptr<XmlResourceExtraData>(pExtraData));

    return true;
}


shared_ptr<IResourceLoader> CreateXmlResourceLoader()
{
    return shared_ptr<IResourceLoader>(QSE_NEW XmlResourceLoader());
}


tinyxml2::XMLElement* XmlResourceLoader::LoadAndReturnRootXmlElement(const char* resourceString)
{
    Resource resource(resourceString);
    shared_ptr<ResHandle> pResourceHandle = QuicksandEngine::g_pApp->m_ResCache->GetHandle(&resource);  // this actually loads the XML file from the zip file
    shared_ptr<XmlResourceExtraData> pExtraData = static_pointer_cast<XmlResourceExtraData>(pResourceHandle->GetExtra());
    return pExtraData->GetRoot();
}


