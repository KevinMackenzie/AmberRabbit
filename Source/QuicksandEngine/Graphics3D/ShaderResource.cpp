#include "../Stdafx.hpp"
#include "ShaderResource.hpp"

unsigned char g_glslVersionComb = 0;


bool GLShaderResourceLoader::LoadResource(char* text, GLUFShaderType type, shared_ptr<ResHandle> handle)
{
	shared_ptr<GLShaderResourceExtraData> extra = static_pointer_cast<GLShaderResourceExtraData>(handle->GetExtra());

	extra->m_pShader = GLUFSHADERMANAGER.CreateShaderFromMemory(text, type);

	if (!extra->m_pShader)
		return false;

	return true;
}

bool GLVertShaderResourceLoader::VLoadResource(char* rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
	return GLShaderResourceLoader::LoadResource(rawBuffer, SH_VERTEX_SHADER, handle);
}

bool GLCtrlShaderResourceLoader::VLoadResource(char* rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
	return GLShaderResourceLoader::LoadResource(rawBuffer, SH_TESS_CONTROL_SHADER, handle);
}

bool GLEvalShaderResourceLoader::VLoadResource(char* rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
	return GLShaderResourceLoader::LoadResource(rawBuffer, SH_TESS_EVALUATION_SHADER, handle);
}

bool GLGeomShaderResourceLoader::VLoadResource(char* rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
	return GLShaderResourceLoader::LoadResource(rawBuffer, SH_GEOMETRY_SHADER, handle);
}

bool GLFragShaderResourceLoader::VLoadResource(char* rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
	return GLShaderResourceLoader::LoadResource(rawBuffer, SH_FRAGMENT_SHADER, handle);
}

bool GLProgramResourceLoader::VLoadResource(char* rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
	if (!XmlResourceLoader::VLoadResource(rawBuffer, rawSize, handle))
		return false;

	shared_ptr<GLProgramResourceExtraData> extra = static_pointer_cast<GLProgramResourceExtraData>(handle->GetExtra());

	tinyxml2::XMLElement* pRoot = extra->GetRoot();

	//format is as follows:
	//NOTE: there is NO limit to the number of shaders per type
	//<PROGRAM>
	//	<GLSL version = 330>
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

	if (pRoot->Name() != "PROGRAM")
		return false;

	if (g_glslVersionComb == 0)
	{
		//get the highest support shader version
		std::string version = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
		std::vector<std::string> parts = GLUFSplitStr(version, '.');
		LOG_ASSERT(parts.size() >= 2 && "Cannot Determine GLSL Version");

		std::string tmp = parts[0].c_str();
		tmp += parts[1].c_str();

		g_glslVersionComb = (unsigned char)std::stoi(tmp);
	}

	//get the highest suported shader version from the xml file
	int maxVersion = 0;
	tinyxml2::XMLElement* shaderToUse = pRoot->FirstChildElement("GLSL");
	tinyxml2::XMLElement* prev = nullptr;

	while (!shaderToUse)
	{
		if (!shaderToUse)
			break;

		const tinyxml2::XMLAttribute* number = shaderToUse->FirstAttribute();
		if (!number)
			return false;

		if (number->Name() == "version")
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
		tinyxml2::XMLElement* shader = shaderToUse->FirstChildElement("SHADER");
		while (shader)
		{
			paths.insert(std::pair<GLUFShaderType, const char*>(SH_VERTEX_SHADER, shader->GetText()));

			shader = shader->NextSiblingElement("SHADER");
		}
	}

	shaderType = shaderToUse->FirstChildElement("TESS_CTRL");
	if (shaderType)
	{
		tinyxml2::XMLElement* shader = shaderToUse->FirstChildElement("SHADER");
		while (shader)
		{
			paths.insert(std::pair<GLUFShaderType, const char*>(SH_TESS_CONTROL_SHADER, shader->GetText()));

			shader = shader->NextSiblingElement("SHADER");
		}
	}

	shaderType = shaderToUse->FirstChildElement("TESS_EVAL");
	if (shaderType)
	{
		tinyxml2::XMLElement* shader = shaderToUse->FirstChildElement("SHADER");
		while (shader)
		{
			paths.insert(std::pair<GLUFShaderType, const char*>(SH_TESS_EVALUATION_SHADER, shader->GetText()));

			shader = shader->NextSiblingElement("SHADER");
		}
	}

	shaderType = shaderToUse->FirstChildElement("GEOMETRY");
	if (shaderType)
	{
		tinyxml2::XMLElement* shader = shaderToUse->FirstChildElement("SHADER");
		while (shader)
		{
			paths.insert(std::pair<GLUFShaderType, const char*>(SH_GEOMETRY_SHADER, shader->GetText()));

			shader = shader->NextSiblingElement("SHADER");
		}
	}

	shaderType = shaderToUse->FirstChildElement("FRAGMENT");
	if (shaderType)
	if (shaderType)
	{
		tinyxml2::XMLElement* shader = shaderToUse->FirstChildElement("SHADER");
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
		Resource* res = new Resource(it.second);
		shared_ptr<GLShaderResourceExtraData> shadExtra = static_pointer_cast<GLShaderResourceExtraData>(QuicksandEngine::g_pApp->m_ResCache->GetHandle(res)->GetExtra());

		shaders.push_back(shadExtra->GetShader());
	}

	extra->m_pProgram = GLUFSHADERMANAGER.CreateProgram(shaders, false);

	if (!extra->m_pProgram)
		return false;


	return true;
}