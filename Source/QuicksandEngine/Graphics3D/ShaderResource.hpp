#ifndef QSE_SHADERRESOURCE_HPP
#define QSE_SHADERRESOURCE_HPP

#include "../ResourceCache/ResCache.hpp"
#include "../ResourceCache/XmlResource.hpp"

//NOTE: this must be linked into a program
class GLShaderResourceExtraData : public IResourceExtraData
{
	friend class GLShaderResourceLoader;

public:
	GLShaderResourceExtraData(){};
	virtual ~GLShaderResourceExtraData(){}
	virtual std::string VToString() { return "GLShaderResourceExtraData"; }

	GLUFShaderPtr GetShader(){	return m_pShader;	}

protected:

	GLUFShaderPtr m_pShader;
};

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

class GLShaderResourceLoader : public IResourceLoader
{
public:
	virtual bool VUseRawFile() { return false; }
	virtual bool VDiscardRawBufferAfterLoad() { return true; }
	virtual unsigned int VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize){ return 0; }
	virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle) = 0;
	bool LoadResource(char* text, GLUFShaderType type, shared_ptr<ResHandle> handle);
	virtual std::string VGetPattern() { return "*.glsl"; }
};

class GLVertShaderResourceLoader : GLShaderResourceLoader
{
public:
	virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
	virtual std::string VGetPattern() { return "*.vert.glsl"; }
};

class GLCtrlShaderResourceLoader : GLShaderResourceLoader
{
public:
	virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
	virtual std::string VGetPattern() { return "*.ctrl.glsl"; }
};

class GLEvalShaderResourceLoader : GLShaderResourceLoader
{
public:
	virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
	virtual std::string VGetPattern() { return "*.eval.glsl"; }
};

class GLGeomShaderResourceLoader : GLShaderResourceLoader
{
public:
	virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
	virtual std::string VGetPattern() { return "*.geom.glsl"; }
};

class GLFragShaderResourceLoader : GLShaderResourceLoader
{
public:
	virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
	virtual std::string VGetPattern() { return "*.frag.glsl"; }
};


class GLProgramResourceLoader : XmlResourceLoader
{
public:
	virtual bool VUseRawFile() { return false; }
	virtual bool VDiscardRawBufferAfterLoad() { return true; }
	virtual unsigned int VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize){ return rawSize; }
	virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
};

#endif