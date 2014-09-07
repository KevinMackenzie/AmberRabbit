#ifndef QSE_SHADERRESOURCE_HPP
#define QSE_SHADERRESOURCE_HPP

#include "../ResourceCache/ResCache.hpp"
#include "../ResourceCache/XmlResource.hpp"


class GLProgramResourceExtraData : public XmlResourceExtraData
{
	friend class GLProgramResourceLoader;

public:
	GLProgramResourceExtraData(){}
	virtual ~GLProgramResourceExtraData(){}
	virtual std::string VToString(){ return "GLProgramResourceExtraData"; }

	GLUFProgramPtr GetProgram(){ return m_pProgram; }

protected:

	GLUFProgramPtr m_pProgram;
};


class GLProgramResourceLoader : public IResourceLoader
{
public:
	virtual bool VUseRawFile() { return false; }
	virtual bool VDiscardRawBufferAfterLoad() { return true; }
	virtual unsigned int VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize){ return rawSize; }
	virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
	virtual std::string VGetPattern(){ return "*.prog"; }
};

#endif