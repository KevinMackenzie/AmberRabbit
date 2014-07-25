#include "../Stdafx.hpp"
#include <GL/GL.h>
#include <GL/GLU.h>
#include <GL/glew.h>
#include <GL/wglew.h>
#include "Shaders.hpp"

class GLShader
{
	friend GLShaderManager;
	friend GLProgram;

	GLuint mShaderId;
	string mShaderName;

	string mTmpShaderText;

	ShaderType mShaderType;

public:

	GLShader();
	~GLShader();

	//common shader is if the shader will not be deleted after building into a program
	//this is used for things like lighting functions
	void Init(ShaderType shaderType, string shaderName);

	void Load(const char* shaderText, bool append = false);
	bool LoadFromFile(const char* filePath, bool append = false);

	void FlushText(void){ mTmpShaderText.clear(); }

	void Compile(GLInfoStruct& retStruct);


};

typedef std::pair<ShaderType, GLShaderPtr > GLShaderP;
//this is a special instance
class GLComputeShader
{
	friend GLShaderManager;
	//TODO: low priority
};

class GLProgram
{
	friend GLShaderManager;

	string mProgramName;

	GLuint mUniformBuffId;
	GLuint mProgramId;
	std::map<ShaderType, GLShaderPtr > mShaderBuff;

public:

	GLProgram();
	~GLProgram();

	void Init(string programName);

	void AttachShader(GLShaderPtr shader);
	void FlushShaders(void) { mShaderBuff.clear(); }

	void Build(GLInfoStruct& retStruct);


};

class GLSeperateProgram
{
	friend GLShaderManager;

	//this is done completely different
};



////////////////////////////////////////
//
//GLShader Methods:
//
//


GLShader::GLShader()
{
	mShaderId = 0;
}

GLShader::~GLShader()
{
	if (mShaderId != 0)
	{
		glDeleteShader(mShaderId);
	}
	mTmpShaderText.clear();
}

void GLShader::Init(ShaderType shaderType, string shaderName)
{
	mShaderName = shaderName;
	mShaderType = shaderType;
	mShaderId = 0;
}

void GLShader::Load(const char* shaderText, bool append)
{
	if (!append)
		mTmpShaderText.clear();

	mTmpShaderText = shaderText;
}

bool GLShader::LoadFromFile(const char* filePath, bool append)
{
	if (!append)
		mTmpShaderText.clear();

	std::ifstream inFile(filePath);
	if (inFile)
	{
		inFile.seekg(0, std::ios::end);
		mTmpShaderText.resize(inFile.tellg());
		inFile.seekg(0, std::ios::beg);
		inFile.read(&mTmpShaderText[0], mTmpShaderText.size());
		inFile.close();
	}
	else
	{
		return false;
	}
}

void GLShader::FlushText()
{
	mTmpShaderText.clear();
}

#define FAILED_COMPILE 'F'
#define FAILED_LINK    'F'

void GLShader::Compile(GLInfoStruct& returnStruct)
{
	//make sure we aren't trying to recompile with a previously successful one
	if (mShaderId != 0)
	{
		returnStruct.mSuccess = false;
		returnStruct.mLog.push_back(FAILED_COMPILE);
		return;
	}

	mShaderId = glCreateShader(mShaderType);

	//start by adding the strings to glShader Source.  This is done right before the compile
	//process becuase it is hard to remove it if there is any reason to flush the text
	const GLint tmpSize = mTmpShaderText.length();
	const char* text = mTmpShaderText.c_str();
	glShaderSource(mShaderId, 1, &text, &tmpSize - 1 /*BECAUSE OF NULL TERMINATED STRINGS*/);

	FlushText();

	glCompileShader(mShaderId);

	GLint isCompiled = 0;
	glGetShaderiv(mShaderId, GL_COMPILE_STATUS, &isCompiled);
	returnStruct.mSuccess = (isCompiled == GL_FALSE) ? false : true;

	GLint maxLength = 0;
	glGetShaderiv(mShaderId, GL_INFO_LOG_LENGTH, &maxLength);

	//The maxLength includes the NULL character
	returnStruct.mLog.resize(maxLength);
	glGetShaderInfoLog(mShaderId, maxLength, &maxLength, &returnStruct.mLog[0]);

	//Provide the infolog in whatever manor you deem best.
	//Exit with failure.
	
	//if it failed, delete the shader
	if (returnStruct.mSuccess == false)
	{
		glDeleteShader(mShaderId);
		mShaderId = 0;
	}
	return;
}



////////////////////////////////////////
//
//GLProgram Methods:
//
//

GLProgram::GLProgram()
{
	mProgramId = 0;
}

GLProgram::~GLProgram()
{
	glDeleteProgram(mProgramId);
}

void GLProgram::Init(string programName)
{
	//unlike with the shader, this will be created during initialization
	mProgramId = glCreateProgram();
	mProgramName = programName;

	//for uniforms
	glGenBuffers(1, &mUniformBuffId);
}

void GLProgram::AttachShader(GLShaderPtr shader)
{
	mShaderBuff.insert(GLShaderP(shader->mShaderType, shader));
	glAttachShader(mProgramId, shader->mShaderId);
}

void GLProgram::FlushShaders(void)
{
	for (auto it : mShaderBuff)
	{
		glDetachShader(mProgramId, it.second->mShaderId);
	}
	mShaderBuff.clear();
}

void GLProgram::Build(GLInfoStruct& retStruct)
{
	
	//Link our program
	glLinkProgram(mProgramId);

	//Note the different functions here: glGetProgram* instead of glGetShader*.
	GLint isLinked = 0;
	glGetProgramiv(mProgramId, GL_LINK_STATUS, &isLinked);
	retStruct.mSuccess = (isLinked == GL_FALSE) ? false : true;

	GLint maxLength = 0;
	glGetProgramiv(mProgramId, GL_INFO_LOG_LENGTH, &maxLength);

	//The maxLength includes the NULL character
	retStruct.mLog.resize(maxLength);
	glGetProgramInfoLog(mProgramId, maxLength, &maxLength, &retStruct.mLog[0]);

	if (!retStruct.mSuccess)
	{
		//in the case of failure, DO NOT DELETE ANYTHING.
	}
	else
	{
		FlushShaders();//this removes the references to them from the program, but they will still exist unless they are 'common'
	}
}




/////////////////////////////////////////////////////////////////////////
//
//GLShaderManager Methods:
//
//
//
//
//

//for creating things


GLShaderPtr GLShaderManager::CreateShader(string name, string shad, ShaderType type, bool file)
{
	GLShaderPtr shader(new GLShader());
	shader->Init(type, name);

	(file) ? shader->LoadFromFile(shad.c_str()) : shader->Load(shad.c_str());

	GLInfoStruct output;
	shader->Compile(output);
	mCompileLogs.insert(std::pair<string, GLInfoStruct>(name, output));

	//log it if it failed
	if (!output)
	{
		stringstream ss;
		ss << "Shader Compilation Failed: \n" << output.mLog.data();
		LOG_ERROR(ss.str());
	}

	//finally, add it to the list
	mShaders.insert(std::pair<string, GLShaderPtrWeak>(name, GLShaderPtrWeak(shader)));

	return shader;
}


GLShaderPtr GLShaderManager::CreateShader(string name, string filePath, ShaderType type)
{
	return CreateShader(name, filePath, type, true);
}


GLShaderPtr GLShaderManager::CreateShader(string name, string text, ShaderType type)
{
	return CreateShader(name, text, type, false);
}





GLProgramPtr GLShaderManager::CreateProgram(string name, ShaderNameList shaders)
{
	//this is the simplest
	GLProgramPtr program(new GLProgram());
	program->Init(name);

	for (auto it : shaders)
	{
		GLShaderPtrWeak weakPtr = mShaders.find(it)->second;

		//sanity testing
		LOG_ASSERT(!weakPtr.expired());

		program->AttachShader(weakPtr.lock());
	}

	GLInfoStruct out;
	program->Build(out);
	mLinklogs.insert(std::pair<string, GLInfoStruct>(name, out));

	if (!out)
	{
		stringstream ss;
		ss << "Program Link Failed: \n" << out.mLog.data();
		LOG_ERROR(ss.str());
	}

	//finally insert it into the list
	mPrograms.insert(std::pair<string, GLProgramPtr>(name, program));

	return program;
}


GLProgramPtr GLShaderManager::CreateProgram(string name, ShaderPtrList shaders)
{
	ShaderNameList shaderNames;

	for (auto it : shaders)
	{
		LOG_ASSERT(it);

		shaderNames.push_back(GetShaderName(it));
	}

	return CreateProgram(name, shaderNames);
}


GLProgramPtr GLShaderManager::CreateProgram(string name, ShaderSourceList shaderSources)
{
	ShaderNameList shaders;
	for (auto it : shaderSources)
	{
		//use the counter global to get a unique name  This is temperary anyway
		string shadName = "SHADER_" + __COUNTER__;
		shaders.push_back(shadName);
		CreateShader(shadName, it.second, it.first);

		//make sure it didn't fail
		if (!GetShaderLog(shadName))
		{
			LOG_ERROR("Program Creation Failed, Reason: Shader Compilation Failed");
			return nullptr;
		}
	}

	return CreateProgram(name, shaders);
}


GLProgramPtr GLShaderManager::CreateProgram(string name, ShaderPathList shaderPaths)
{
	ShaderNameList shaders;
	for (auto it : shaderPaths)
	{
		//use the counter global to get a unique name  This is temperary anyway
		string shadName = "SHADER_" + __COUNTER__;
		shaders.push_back(shadName);
		CreateShader(shadName, it.second, it.first, true);

		//make sure it didn't fail
		if (!GetShaderLog(shadName))
		{
			LOG_ERROR("Program Creation Failed, Reason: Shader Compilation Failed");
			return nullptr;
		}
	}

	return CreateProgram(name, shaders);
}




//for removing things

void GLShaderManager::DeleteShader(string shaderName)
{
	mShaders.erase(shaderName);
}


void GLShaderManager::DeleteShader(GLShaderPtr shader)
{
	mShaders.erase(GetShaderName(shader));
}



void GLShaderManager::DeleteProgram(string programName)
{
	mPrograms.erase(programName);
}


void GLShaderManager::DeleteProgram(GLProgramPtr program)
{
	mPrograms.erase(GetProgramName(program));
}



void GLShaderManager::FlushRemovedShaders()
{
	for (auto it : mShaders)
	{
		if (it.second.expired())
		{
			mShaders.erase(it.first);
		}
	}
}

void GLShaderManager::FlushLogs()
{
	mLinklogs.clear();
	mCompileLogs.clear();
}


void GLShaderManager::FlushPrograms()
{
	mPrograms.clear();
}


void GLShaderManager::FlushShaders()
{
	mShaders.clear();
}




//for accessing things

GLShaderPtr GLShaderManager::GetShaderPtr(string shaderName) const
{
	return mShaders.find(shaderName)->second.lock();
}


GLShaderPtrWeak GLShaderManager::GetShaderPtrWeak(string shaderName) const
{
	return mShaders.find(shaderName)->second;
}


GLProgramPtr GLShaderManager::GetProgramPtr(string programName) const
{
	return mPrograms.find(programName)->second;
}


GLProgramPtrWeak GLShaderManager::GetProgramPtrWeak(string programName) const
{
	return GLProgramPtrWeak(mPrograms.find(programName)->second);
}



const GLuint GLShaderManager::GetShaderId(GLShaderPtr shader) const
{
	LOG_ASSERT(shader);

	return shader->mShaderId;
}


const ShaderType GLShaderManager::GetShaderType(GLShaderPtr shader) const
{
	LOG_ASSERT(shader);

	return shader->mShaderType;
}


const GLuint GLShaderManager::GetProgramId(GLProgramPtr program) const
{
	LOG_ASSERT(program);

	return program->mProgramId;
}

const string GLShaderManager::GetShaderName(GLShaderPtr shader) const
{
	LOG_ASSERT(shader);

	return shader->mShaderName;
}

const string GLShaderManager::GetProgramName(GLProgramPtr program) const
{
	LOG_ASSERT(program);

	return program->mProgramName;
}

const GLuint GLShaderManager::GetShaderId(string shader) const
{
	return mShaders.find(shader)->second.lock()->mShaderId;
}


const ShaderType GLShaderManager::GetShaderType(string shader) const
{
	return mShaders.find(shader)->second.lock()->mShaderType;
}


const GLuint GLShaderManager::GetProgramId(string program) const
{
	return mPrograms.find(program)->second->mProgramId;
}



const GLCompileOutputStruct GLShaderManager::GetShaderLog(string shader) const
{
	return mCompileLogs.find(shader)->second;
}


const GLCompileOutputStruct GLShaderManager::GetShaderLog(GLShaderPtr shaderPtr) const
{
	return mCompileLogs.find(shaderPtr->mShaderName)->second;
}



const GLLinkOutputStruct GLShaderManager::GetProgramLog(string program) const
{
	return mLinklogs.find(program)->second;
}


const GLLinkOutputStruct GLShaderManager::GetProgramLog(GLProgramPtr programPtr) const
{
	return mLinklogs.find(programPtr->mProgramName)->second;
}

void GLShaderManager::SetProgramInBuffer(GLProgramPtr program, ShaderInputType inType, GLuint bufferId, GLuint vao)
{
	glUseProgram(program->mProgramId);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(bufferId);
	GLsizei size;
	switch (inType)
	{
	case IN_UVCOORDS:
		size = 2;
		break;
	default:
		size = 3;
	}
	glVertexAttribPointer(inType, size, GL_FLOAT, GL_FALSE, 0, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}

void GLShaderManager::SetProgramUniform(GLProgramPtr program, ShaderUniformType uniformType, glm::mat4 matrix)
{
	glBindBuffer(GL_UNIFORM_BUFFER, program->mUniformBuffId);
	glUniformMatrix4fv(uniformType, 1, GL_FALSE, matrix);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void GLShaderManager::SetProgramMaterial(GLProgramPtr program, Material mat)
{
	glBindBuffer(GL_UNIFORM_BUFFER, program->mUniformBuffId);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}


void GLShaderManager::ResetProgarmAttributes(GLProgramPtr program)
{
	//TODO:
}

//for using things

void GLShaderManager::UseProgram(string programName) const
{
	glUseProgram(mPrograms.find(programName)->second->mProgramId);
}


void GLShaderManager::UseProgram(GLProgramPtr program) const
{
	glUseProgram(program->mProgramId);
}