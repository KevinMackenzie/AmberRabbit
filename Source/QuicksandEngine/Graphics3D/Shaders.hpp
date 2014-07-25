#ifndef QSE_SHADERS_HPP
#define QSE_SHADERS_HPP

#include "Material.hpp"

enum ShaderType
{
	SH_VERTEX_SHADER = GL_VERTEX_SHADER,
	SH_TESS_CONTROL_SHADER = GL_TESS_CONTROL_SHADER,
	SH_TESS_EVALUATION_SHADER = GL_TESS_EVALUATION_SHADER,
	SH_GEOMETRY_SHADER = GL_FRAGMENT_SHADER,
	SH_FRAGMENT_SHADER = GL_FRAGMENT_SHADER
};

enum ShaderInputType
{
	IN_POSITIONS = 0,
	IN_NORMALS = 1,
	IN_UVCOORDS = 2,
};

enum ShaderUniformType
{
	IN_M = 4,
	IN_V = 3,
	IN_P = 2,
	IN_MV = 1,
	IN_MVP = 0
};

struct GLInfoStruct
{
	bool mSuccess = false;

	std::vector<char> mLog;

	operator bool() const
	{
		return mSuccess;
	}
};

typedef GLInfoStruct GLCompileOutputStruct;
typedef GLInfoStruct GLLinkOutputStruct;


//make all of the classes opaque
class GLShader;
class GLComputeShader;
class GLProgram;
class GLSeperateProgram;

typedef shared_ptr<GLShader>  GLShaderPtr;
typedef weak_ptr<GLShader>    GLShaderPtrWeak;
typedef shared_ptr<GLProgram> GLProgramPtr;
typedef weak_ptr<GLProgram>   GLProgramPtrWeak;

typedef std::map<ShaderType, string> ShaderSourceList;
typedef std::list<std::pair<ShaderType, string> > ShaderPathList;//do a little bit of fudging to get around this being the same as the above
typedef std::vector<GLuint> ShaderIdList;
typedef std::vector<GLuint> ProgramIdList;
typedef std::vector<string> ShaderNameList;
typedef std::vector<string> ProgramNameList;
typedef std::vector<GLShaderPtr> ShaderPtrList;
typedef std::vector<GLProgramPtr> ProgramPtrList;
typedef std::vector<GLShaderPtrWeak> ShaderPtrListWeak;
typedef std::vector<GLProgramPtrWeak> ProgramPtrListWeak;

//TODO: impliment shader caching.  i.e. save shader binaries to the disk
//TODO: make a "standard" shader that is added to ALL shaders that includes base things such as the core uniforms (MVP) and standard imports/exports

//ONLY make calls to this class, but a note, only the const methods can be called from other threadss
//NOTE: all strings must be UNIQUE, and there is no checking that they will be, so if they are not, this may go VERY wrong.
//THIS ALSO MEANS that a shader and a program cannot have the same string either
class GLShaderManager
{
	//these are just temporary.
	std::map<string, GLShaderPtrWeak > mShaders;
	//std::map<GLuint, shared_ptr<GLShader> > mStickyShaders;//this are used multiple times

	//this are always sticky
	std::map<string, GLProgramPtr > mPrograms;

	//a list of logs
	std::map<string, GLInfoStruct> mCompileLogs;
	std::map<string, GLInfoStruct> mLinklogs;

	//a little helper function for creating things
	GLShaderPtr CreateShader(string name, string shad, ShaderType type, bool file);

public:

	//for creating things

	GLShaderPtr CreateShader(string name, string filePath, ShaderType type);
	GLShaderPtr CreateShader(string name, string text, ShaderType type);

	GLProgramPtr CreateProgram(string name, ShaderNameList shaders);
	GLProgramPtr CreateProgram(string name, ShaderPtrList shaders);
	GLProgramPtr CreateProgram(string name, ShaderSourceList shaderSources);
	GLProgramPtr CreateProgram(string name, ShaderPathList shaderPaths);


	//for removing things

	void DeleteShader(string shaderName);
	void DeleteShader(GLShaderPtr shader);

	void DeleteProgram(string programName);
	void DeleteProgram(GLProgramPtr program);

	void FlushRemovedShaders();
	void FlushLogs();
	void FlushPrograms();
	void FlushShaders();


	//for accessing things

	GLShaderPtr       GetShaderPtr(string shaderName) const;
	GLShaderPtrWeak   GetShaderPtrWeak(string shaderName) const;
	GLProgramPtr      GetProgramPtr(string programName) const;
	GLProgramPtrWeak  GetProgramPtrWeak(string programName) const;
	
	const GLuint	  GetShaderId(GLShaderPtr shader) const;
	const ShaderType  GetShaderType(GLShaderPtr shader) const;
	const GLuint	  GetProgramId(GLProgramPtr program) const;

	const GLuint	  GetShaderId(string shader) const;
	const ShaderType  GetShaderType(string shader) const;
	const GLuint	  GetProgramId(string program) const;

	const string      GetShaderName(GLShaderPtr shader) const;
	const string      GetProgramName(GLProgramPtr program) const;

	const GLCompileOutputStruct GetShaderLog(string shader) const;
	const GLCompileOutputStruct GetShaderLog(GLShaderPtr shaderPtr) const;

	const GLLinkOutputStruct GetProgramLog(string program) const;
	const GLLinkOutputStruct GetProgramLog(GLProgramPtr programPtr) const;


	//for using things

	void UseProgram(string programName) const;
	void UseProgram(GLProgramPtr program) const;


	//for setting things up

	void SetProgramInBuffer(GLProgramPtr program, ShaderInputType inType, GLuint bufferId, GLuint vao);
	void SetProgramInBuffer(GLProgramPtr program, GLuint BufferId, GLuint vao, GLuint location){};//TODO:


	void SetProgramUniform(GLProgramPtr program, ShaderUniformType uniformType, glm::mat4 matrix);
	void SetProgramUniform(GLProgramPtr program, GLuint uniformId, GLuint location){};//TODO:

	void SetProgramMaterial(GLProgramPtr program, Material mat);

	void ResetProgarmAttributes(GLProgramPtr program);

	GLShaderManager(){};
	~GLShaderManager(){};
};

#define ShaderManager static_pointer_cast<GLRenderer>(QuicksandEngine::g_pApp->m_Renderer)->mShaderManager


/*
Usage examples


//create the shader
GLShaderPtr shad = CreateShader("shader.glsl", ST_VERTEX_SHADER);



*/

#endif