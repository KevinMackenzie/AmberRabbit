#include "../Stdafx.hpp"
#include "ShaderResource.hpp"

int g_glslVersionComb = 0;


//NOTE: this must be linked into a program
class GLShaderResourceExtraData : public IResourceExtraData
{
	friend class GLShaderResourceLoader;

public:
	GLShaderResourceExtraData(){};
	virtual ~GLShaderResourceExtraData(){}
	virtual std::string VToString() { return "GLShaderResourceExtraData"; }

	GLUFShaderPtr GetShader(){ return m_pShader; }

protected:

	GLUFShaderPtr m_pShader;
};

class GLShaderResourceLoader : public IResourceLoader
{
public:
	virtual bool VUseRawFile() { return false; }
	virtual bool VDiscardRawBufferAfterLoad() { return true; }
	virtual unsigned int VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize){ return 0; }
	virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle) = 0;
	bool LoadResource(char* data, long len, GLUFShaderType type, shared_ptr<ResHandle> handle);
	//virtual std::string VGetPattern() { return "*.glsl"; }
};

class GLVertShaderResourceLoader : public GLShaderResourceLoader
{
public:
	virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
	virtual std::string VGetPattern() { return "*vert.glsl"; }
};

class GLCtrlShaderResourceLoader : public GLShaderResourceLoader
{
public:
	virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
	virtual std::string VGetPattern() { return "*ctrl.glsl"; }
};

class GLEvalShaderResourceLoader : public GLShaderResourceLoader
{
public:
	virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
	virtual std::string VGetPattern() { return "*eval.glsl"; }
};

class GLGeomShaderResourceLoader : public GLShaderResourceLoader
{
public:
	virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
	virtual std::string VGetPattern() { return "*geom.glsl"; }
};

class GLFragShaderResourceLoader : public GLShaderResourceLoader
{
public:
	virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
	virtual std::string VGetPattern() { return "*frag.glsl"; }
};

bool GLShaderResourceLoader::LoadResource(char* data, long len, GLUFShaderType type, shared_ptr<ResHandle> handle)
{
	shared_ptr<GLShaderResourceExtraData> extra = shared_ptr<GLShaderResourceExtraData>(QSE_NEW GLShaderResourceExtraData());

	extra->m_pShader = GLUFSHADERMANAGER.CreateShaderFromMemory(data, len, type);
	handle->SetExtra(extra);

	if (!extra->m_pShader)
		return false;

	if (!GLUFSHADERMANAGER.GetShaderLog(extra->m_pShader).mSuccess)
	{
		LOG_ERROR("ShaderCompilation Failed for file: " + handle->GetName());
	}

	return true;
}

bool GLVertShaderResourceLoader::VLoadResource(char* rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
	return GLShaderResourceLoader::LoadResource(rawBuffer, rawSize, SH_VERTEX_SHADER, handle);
}

bool GLCtrlShaderResourceLoader::VLoadResource(char* rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
	return GLShaderResourceLoader::LoadResource(rawBuffer, rawSize, SH_TESS_CONTROL_SHADER, handle);
}

bool GLEvalShaderResourceLoader::VLoadResource(char* rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
	return GLShaderResourceLoader::LoadResource(rawBuffer, rawSize, SH_TESS_EVALUATION_SHADER, handle);
}

bool GLGeomShaderResourceLoader::VLoadResource(char* rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
	return GLShaderResourceLoader::LoadResource(rawBuffer, rawSize, SH_GEOMETRY_SHADER, handle);
}

bool GLFragShaderResourceLoader::VLoadResource(char* rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
	return GLShaderResourceLoader::LoadResource(rawBuffer, rawSize, SH_FRAGMENT_SHADER, handle);
}


shared_ptr<IResourceLoader> CreatePROGResourceLoader()
{
	return shared_ptr<IResourceLoader>(QSE_NEW GLProgramResourceLoader());
}

shared_ptr<IResourceLoader> CreateVERTResourceLoader()
{
	return shared_ptr<IResourceLoader>(QSE_NEW GLVertShaderResourceLoader());
}

shared_ptr<IResourceLoader> CreateCTRLResourceLoader()
{
	return shared_ptr<IResourceLoader>(QSE_NEW GLCtrlShaderResourceLoader());
}

shared_ptr<IResourceLoader> CreateEVALResourceLoader()
{
	return shared_ptr<IResourceLoader>(QSE_NEW GLEvalShaderResourceLoader());
}

shared_ptr<IResourceLoader> CreateGEOMResourceLoader()
{
	return shared_ptr<IResourceLoader>(QSE_NEW GLGeomShaderResourceLoader());
}

shared_ptr<IResourceLoader> CreateFRAGResourceLoader()
{
	return shared_ptr<IResourceLoader>(QSE_NEW GLFragShaderResourceLoader());
}

bool GLProgramResourceLoader::VLoadResource(char* rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
	shared_ptr<GLProgramResourceExtraData> extra = shared_ptr<GLProgramResourceExtraData>(QSE_NEW GLProgramResourceExtraData());

	extra->ParseXml(rawBuffer, rawSize);

	tinyxml2::XMLElement* pRoot = extra->GetRoot();

	//format is as follows:
	//NOTE: there is NO limit to the number of shaders per type
	//NOTE: the only requirement is that the GLSL version are in ASCENDING ORDER
	//<PROGRAM>
	//	<GLSL version = "330">
	//		<VERTEX>
	//			<SHADER>standard.vert.glsl</SHADER>
	//		</VERTEX>
	//		<TESS_CTRL>
	//			<SHADER>standard.ctrl.glsl</SHADER>
	//		</TESS_CTRL>
	//		<TESS_EVAL>
	//			<SHADER>standard.eval.glsl</SHADER>
	//		</TESS_EVAL>
	//		<GEOMETRY>
	//			<SHADER>standard.geom.glsl</SHADER>
	//		</GEOMETRY>
	//		<FRAGMENT>
	//			<SHADER>standard.frag.glsl</SHADER>
	//		</FRAGMENT>
	//	</GLSL>
	//</PROGRAM>


	if (strcmp(pRoot->Name(), "PROGRAM") != 0)
		return false;

	if (g_glslVersionComb == 0)
	{
		//get the highest support shader version
		std::string version = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
		std::vector<std::string> parts = GLUFSplitStr(version, '.');
		LOG_ASSERT(parts.size() >= 2 && "Cannot Determine GLSL Version");

		std::string tmp = parts[0].c_str();
		tmp += parts[1][0];
		tmp += parts[1][1];

		g_glslVersionComb = std::stoi(tmp);
	}

	//get the highest suported shader version from the xml file
	int maxVersion = 0;
	tinyxml2::XMLElement* shaderToUse = pRoot->FirstChildElement("GLSL");
	tinyxml2::XMLElement* prev = nullptr;

	while (shaderToUse)
	{
		const tinyxml2::XMLAttribute* number = shaderToUse->FirstAttribute();
		if (!number)
			return false;

		if (strcmp(number->Name() , "version") == 0)
		{
			//if this element has too high of a version, then use the previous one
			if (number->IntValue() > g_glslVersionComb)
			{
				shaderToUse = prev;
				break;
			}
			if (maxVersion < number->IntValue())
			{
				maxVersion = number->IntValue();

				//if there are no more version after this, the break;
				if (shaderToUse->NextSiblingElement("GLSL") == nullptr)
					break;
			}
		}

		prev = shaderToUse;
		shaderToUse = shaderToUse->NextSiblingElement("GLSL");
	}

	//if it couldn't find a version to use
	if (maxVersion == 0 || shaderToUse == nullptr)
		return false;

	//this is just used because it uses normal characters
	GLUFShaderSourceList paths;

	tinyxml2::XMLElement* shaderType = shaderToUse->FirstChildElement("VERTEX");
	if (shaderType)
	{
		tinyxml2::XMLElement* shader = shaderType->FirstChildElement("SHADER");
		while (shader)
		{
			paths.insert(std::pair<GLUFShaderType, const char*>(SH_VERTEX_SHADER, shader->GetText()));

			shader = shader->NextSiblingElement("SHADER");
		}
	}

	shaderType = shaderToUse->FirstChildElement("TESS_CTRL");
	if (shaderType)
	{
		tinyxml2::XMLElement* shader = shaderType->FirstChildElement("SHADER");
		while (shader)
		{
			paths.insert(std::pair<GLUFShaderType, const char*>(SH_TESS_CONTROL_SHADER, shader->GetText()));

			shader = shader->NextSiblingElement("SHADER");
		}
	}

	shaderType = shaderToUse->FirstChildElement("TESS_EVAL");
	if (shaderType)
	{
		tinyxml2::XMLElement* shader = shaderType->FirstChildElement("SHADER");
		while (shader)
		{
			paths.insert(std::pair<GLUFShaderType, const char*>(SH_TESS_EVALUATION_SHADER, shader->GetText()));

			shader = shader->NextSiblingElement("SHADER");
		}
	}

	shaderType = shaderToUse->FirstChildElement("GEOMETRY");
	if (shaderType)
	{
		tinyxml2::XMLElement* shader = shaderType->FirstChildElement("SHADER");
		while (shader)
		{
			paths.insert(std::pair<GLUFShaderType, const char*>(SH_GEOMETRY_SHADER, shader->GetText()));

			shader = shader->NextSiblingElement("SHADER");
		}
	}

	shaderType = shaderToUse->FirstChildElement("FRAGMENT");
	if (shaderType)
	{
		tinyxml2::XMLElement* shader = shaderType->FirstChildElement("SHADER");
		while (shader)
		{
			paths.insert(std::pair<GLUFShaderType, const char*>(SH_FRAGMENT_SHADER, shader->GetText()));

			shader = shader->NextSiblingElement("SHADER");
		}
	}


	///now take these paths and get their data
	GLUFShaderPtrList shaders;

	for (auto it : paths)
	{
		Resource res(it.second);
		shared_ptr<ResHandle> shadHandle = QuicksandEngine::g_pApp->m_ResCache->GetHandle(&res);

		if (!shadHandle)
		{
			LOG_ERROR("Failed to load program!");
			return false;
		}
		if (!shadHandle->GetExtra())
		{
			LOG_ERROR("Failed to load program!");
			return false;
		}
		

		shaders.push_back(static_pointer_cast<GLShaderResourceExtraData>(shadHandle->GetExtra())->GetShader());
	}

	extra->m_pProgram = GLUFSHADERMANAGER.CreateProgram(shaders, false);

	handle->SetExtra(extra);

	if (!extra->m_pProgram)
		return false;


	return true;
}