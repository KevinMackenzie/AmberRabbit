// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the OBJGLUF_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// OBJGLUF_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#pragma once

#pragma warning (disable : 4251)

#ifdef _WIN32
#ifdef OBJGLUF_EXPORTS
#define OBJGLUF_API __declspec(dllexport)
#else
#define OBJGLUF_API __declspec(dllimport)
#endif
#else
#define OBJGLUF_API
#endif

//TODO LIST:
//
//support normal mapping
//support tangent space
//support guided lighting
//support guided materials


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef USING_ASSIMP
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#endif

#include <vector>
#include <memory>
#include <map>
#include <list>
#include <string>
#include <sstream>
#include <locale>
#include <codecvt>
#include <stdlib.h>
#include <stack>

#ifndef OBJGLUF_EXPORTS
#ifndef SUPPRESS_RADIAN_ERROR
#error "GLM is using radians as input, to suppress this error, #define SUPPRESS_RADIAN_ERROR"
#endif
#endif

#ifndef OBJGLUF_EXPORTS
#ifndef SUPPRESS_UTF8_ERROR
#error "ATTENTION, all strings MUST be in utf8 encoding"
#endif
#endif

//call this once per frame
#define GLUFStats GLUF::GLUFStats_func



namespace std
{
	//This is a very useful function that is not in the standard libraries
	template<class _Ty> inline
	const _Ty& (clamp)(const _Ty& _Value, const _Ty& _Min, const _Ty& _Max)
	{
		return std::min(std::max(_Value, _Min), _Max);
	}
}

namespace GLUF
{
//added openGL functionality

//This first generates a single buffer, then immediatly binds it
#define glGenBufferBindBuffer(target, buffer) glGenBuffers(1, buffer); glBindBuffer(target, *buffer)

//This first generates a single vertex array, then immediatly binds it
#define glGenVertexArrayBindVertexArray(vertexArray) glGenVertexArrays(1, vertexArray); glBindVertexArray(*vertexArray)

//end

typedef void(*GLUFErrorMethod)(const char* message, const char* funcName, const char* sourceFile, unsigned int lineNum);

OBJGLUF_API void GLUFRegisterErrorMethod(GLUFErrorMethod method);
OBJGLUF_API GLUFErrorMethod GLUFGetErrorMethod();

#ifndef __FUNCTION__
#define __FUNCTION__ __func__//not always defined on non-windows systems
#endif

#define GLUF_ERROR(message) GLUFGetErrorMethod()(message, __FUNCTION__, __FILE__, __LINE__);
#define GLUF_ASSERT(expr)	{ if (!(expr)) { std::stringstream ss; ss << "ASSERTION FAILURE:" << #expr; GLUF_ERROR(ss.str().c_str()) } }

#define GLUF_SAFE_DELETE(ptr) {delete(ptr); (ptr) = nullptr;}
#define GLUF_NULL(type) (std::shared_ptr<type>(nullptr))
#define GLUF_UNREFERENCED_PARAMETER(value) (value)

typedef unsigned long GLUFResult;

//GLUFResult Values
#define GR_FAILURE			0
#define GR_SUCCESS			1
#define GR_OUTOFMEMORY		2
#define GR_INVALIDARG		3
#define GR_NOTIMPL			4

#define GLUF_FAILED(result) ((result == GR_FAILURE) ? true : false)
#define GLUF_SUCCEEDED(result) ((result == GR_SUCCESS) ? true : false)
#define GLUF_V_RETURN(result) {if(result != GR_SUCCESS) return result;}

#define GLUFTRACE_ERR(str, gr) GLUFTrace(__FILE__, __FUNCTION__, (unsigned long)__LINE__, gr, str);

OBJGLUF_API GLUFResult GLUFTrace(const char*, const char*, unsigned long, GLUFResult, const char*);


//statistics methods
OBJGLUF_API void GLUFStats_func();
OBJGLUF_API const wchar_t* GLUFGetFrameStats();
OBJGLUF_API const wchar_t* GLUFGetDeviceStats();

//not defined if not windows
#ifndef _T
#define _T __T
#endif
#ifndef __T
#define __T(str) L ## str
#endif

#define GLUFGetTime() glfwGetTime()
#define GLUFGetTimef() ((float)glfwGetTime())
#define GLUFGetTimeMs() ((int)(glfwGetTime() * 1000.0))

typedef glm::u8vec4 Color;//only accepts numbers from 0 to 255
typedef glm::vec3 Color3f;
typedef glm::vec4 Color4f;


#define GLUF_PI (double)3.14159265358979
#define GLUF_PI_F (float)3.14159265358979

#define DEG_TO_RAD(value) (value *(GLUF_PI / 180))
#define DEG_TO_RAD_F(value) (value *(GLUF_PI_F / 180))


OBJGLUF_API bool GLUFInit();

//call this after calling glfwMakeContextCurrent on the window
OBJGLUF_API bool GLUFInitOpenGLExtentions();

OBJGLUF_API void GLUFTerminate();


//this loads an entire file into a binary array, path is input, rawSize and rawData are outputs
OBJGLUF_API char* GLUFLoadFileIntoMemory(const wchar_t* path, unsigned long* rawSize);
OBJGLUF_API long GLUFLoadFileIntoMemory(const wchar_t* path, char* buffer, long len = -1);
OBJGLUF_API char* GLUFLoadFileIntoMemory(const char* path, unsigned long* rawSize);
OBJGLUF_API long GLUFLoadFileIntoMemory(const char* path, char* buffer, long len = -1);

OBJGLUF_API std::string GLUFLoadBinaryArrayIntoString(char* data, long len);

typedef std::vector<glm::vec4> Vec4Array;
typedef std::vector<glm::vec3> Vec3Array;
typedef std::vector<glm::vec2> Vec2Array;
typedef std::vector<GLuint>    IndexArray;

class OBJGLUF_API GLUFMatrixStack
{
	std::stack<glm::mat4> mStack;
	static glm::mat4 mIdentity;
public:
	void Push(const glm::mat4& matrix);
	void Pop(void);
	const glm::mat4& Top();//this returns the matrix that is a concatination of all subsequent matrices
	size_t Size();
	void Empty();
};


//this rect is supposed to be used where the origin is bottom left, and upper left corner is (1,1)
struct OBJGLUF_API GLUFRect
{
	long left, top, right, bottom;



};

struct OBJGLUF_API GLUFRectf
{
	float left, top, right, bottom;
};

struct OBJGLUF_API GLUFPoint
{
	union{ long x, width;  };
	union{ long y, height; };

	GLUFPoint(long val1, long val2) : x(val1), y(val2){}
	GLUFPoint() : x(0L), y(0L){}
};

struct OBJGLUF_API MemStreamBuf : std::streambuf
{
	MemStreamBuf(char* data, std::ptrdiff_t length)
	{
		setg(data, data, data + length);
	}
};

inline GLUFPoint operator /(const GLUFPoint& pt0, const GLUFPoint& pt1)
{
	return{ pt0.x / pt1.x, pt0.y / pt1.y };
}

inline GLUFPoint operator /(const GLUFPoint& pt0, const long& f)
{
	return{ pt0.x / f, pt0.y / f };
}

inline GLUFPoint operator -(const GLUFPoint& pt0, const GLUFPoint& pt1)
{
	return { pt0.x - pt1.x, pt0.y - pt1.y };
}

inline bool operator ==(const GLUFRect& rc0, const GLUFRect& rc1)
{
	return
		(
		rc0.left == rc1.left && rc0.right  == rc1.right &&
		rc0.top  == rc1.top  && rc0.bottom == rc1.bottom
		);
}

inline bool operator !=(const GLUFRect& rc0, const GLUFRect& rc1)
{
	return !(rc0 == rc1);
}

OBJGLUF_API bool		GLUFPtInRect(GLUFRect rect, GLUFPoint pt);
OBJGLUF_API void		GLUFSetRectEmpty(GLUFRect& rect);
OBJGLUF_API void		GLUFSetRect(GLUFRect& rect, long left, long top, long right, long bottom);
OBJGLUF_API void		GLUFSetRect(GLUFRectf& rect, float left, float top, float right, float bottom);
OBJGLUF_API void		GLUFOffsetRect(GLUFRect& rect, long x, long y);
OBJGLUF_API long		GLUFRectHeight(GLUFRect rect);
OBJGLUF_API long		GLUFRectWidth(GLUFRect rect);
OBJGLUF_API void		GLUFInflateRect(GLUFRect& rect, long dx, long dy);
OBJGLUF_API bool		GLUFIntersectRect(GLUFRect rect0, GLUFRect rect1, GLUFRect& rectIntersect);
OBJGLUF_API GLUFPoint	GLUFMultPoints(GLUFPoint pt0, GLUFPoint pt1);

OBJGLUF_API inline std::vector<std::wstring> &GLUFSplitStr(const std::wstring &s, wchar_t delim, std::vector<std::wstring> &elems, bool keepDelim = false)
{
	std::wstringstream ss(s);
	std::wstring item;
	while (std::getline(ss, item, delim))
	{
		if (keepDelim)//OOPS forgot this earlier
			item += delim;
		elems.push_back(item);
	}
	return elems;
}

OBJGLUF_API inline std::vector<std::wstring> GLUFSplitStr(const std::wstring &s, wchar_t delim, bool keepDelim = false)
{
	std::vector<std::wstring> elems;
	GLUFSplitStr(s, delim, elems, keepDelim);
	return elems;
}

OBJGLUF_API inline std::vector<std::string> &GLUFSplitStr(const std::string &s, char delim, std::vector<std::string> &elems, bool keepDelim = false)
{
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim))
	{
		if (keepDelim)//OOPS forgot this earlier
			item += delim;
		elems.push_back(item);
	}
	return elems;
}

OBJGLUF_API inline std::vector<std::string> GLUFSplitStr(const std::string &s, char delim, bool keepDelim = false)
{
	std::vector<std::string> elems;
	GLUFSplitStr(s, delim, elems, keepDelim);
	return elems;
}

template<typename T>
OBJGLUF_API inline size_t GLUFGetVectorSize(std::vector<T> vec)
{
	return vec.size();
}

template<typename T>
OBJGLUF_API inline std::vector<T> GLUFArrToVec(T* arr, unsigned long len)
{
	return std::vector<T>(arr, arr + len);
}

//used for getting vertices from rects 0,0 is bottom left
OBJGLUF_API glm::vec2 GLUFGetVec2FromRect(GLUFRect rect, bool x, bool y);
OBJGLUF_API glm::vec2 GLUFGetVec2FromRect(GLUFRectf rect, bool x, bool y);

//used for getting vertices from rects 0,0 is bottom left
OBJGLUF_API GLUFPoint GLUFGetPointFromRect(GLUFRect rect, bool x, bool y);


OBJGLUF_API Color4f GLUFColorToFloat(Color color);//takes 0-255 to 0.0f - 1.0f
OBJGLUF_API Color3f GLUFColorToFloat3(Color color);//takes 0-255 to 0.0f - 1.0f



enum GLUFLocationType
{
	GLT_ATTRIB = 0,
	GLT_UNIFORM,
};

struct OBJGLUF_API GLUFShaderInfoStruct
{
	bool mSuccess = false;

	char* mLog;

	operator bool() const
	{
		return mSuccess;
	}
};

enum GLUFShaderType
{
	SH_VERTEX_SHADER = GL_VERTEX_SHADER,
	SH_TESS_CONTROL_SHADER = GL_TESS_CONTROL_SHADER,
	SH_TESS_EVALUATION_SHADER = GL_TESS_EVALUATION_SHADER,
	SH_GEOMETRY_SHADER = GL_GEOMETRY_SHADER,
	SH_FRAGMENT_SHADER = GL_FRAGMENT_SHADER
};



typedef GLUFShaderInfoStruct GLUFCompileOutputStruct;
typedef GLUFShaderInfoStruct GLUFLinkOutputStruct;


//make all of the classes opaque
class OBJGLUF_API GLUFShader;
class OBJGLUF_API GLUFComputeShader;
class OBJGLUF_API GLUFProgram;
class OBJGLUF_API GLUFSeperateProgram;

typedef std::shared_ptr<GLUFShader>  GLUFShaderPtr;
typedef std::weak_ptr<GLUFShader>    GLUFShaderPtrWeak;
typedef std::shared_ptr<GLUFProgram> GLUFProgramPtr;
typedef std::weak_ptr<GLUFProgram>   GLUFProgramPtrWeak;
typedef std::shared_ptr<GLUFSeperateProgram> GLUFSepProgramPtr;
typedef std::weak_ptr<GLUFSeperateProgram> GLUFSepProgramPtrWeak;

typedef std::map<GLUFShaderType, const char*> GLUFShaderSourceList;
typedef std::map<GLUFShaderType, std::wstring> GLUFShaderPathList;//do a little bit of fudging to get around this being the same as the above
typedef std::vector<GLuint> GLUFShaderIdList;
typedef std::vector<GLuint> GLUFProgramIdList;
typedef std::vector<std::wstring> GLUFShaderNameList;
typedef std::vector<std::wstring> GLUFProgramNameList;
typedef std::vector<GLUFShaderPtr> GLUFShaderPtrList;
typedef std::vector<GLUFProgramPtr> GLUFProgramPtrList;
typedef std::map<GLbitfield, GLUFProgramPtr> GLUFProgramPtrStagesMap;
typedef std::pair<GLbitfield, GLUFProgramPtr> GLUFProgramPtrStagesPair;
typedef std::map<GLUFShaderType, GLUFProgramPtr> GLUFProgramPtrMap;
typedef std::vector<GLUFShaderPtrWeak> GLUFShaderPtrListWeak;
typedef std::vector<GLUFProgramPtrWeak> GLUFProgramPtrListWeak;

typedef std::map<std::string, GLuint> GLUFVariableLocMap;
typedef std::pair<std::string, GLuint> GLUFVariableLocPair;

class OBJGLUF_API GLUFShaderManager
{

	//a list of logs
	std::map<GLUFShaderPtr, GLUFShaderInfoStruct> mCompileLogs;
	std::map<GLUFProgramPtr, GLUFShaderInfoStruct> mLinklogs;

	//a little helper function for creating things
	//GLUFShaderPtr CreateShader(std::wstring shad, GLUFShaderType type, bool file, bool seperate = false);

	//friend class GLUFBufferManager;

public:


	//for creating things

	GLUFShaderPtr CreateShaderFromFile(std::wstring filePath, GLUFShaderType type);
	GLUFShaderPtr CreateShaderFromText(const char* str, GLUFShaderType type);
	GLUFShaderPtr CreateShaderFromMemory(char* data, long len, GLUFShaderType type);

	GLUFProgramPtr CreateProgram(GLUFShaderPtrList shaders, bool seperate = false);
	GLUFProgramPtr CreateProgram(GLUFShaderSourceList shaderSources, bool seperate = false);
	GLUFProgramPtr CreateProgram(GLUFShaderPathList shaderPaths, bool seperate = false);

	//program pipelines are broken
	//GLUFSepProgramPtr CreateSeperateProgram(GLUFProgramPtrStagesMap programs);

	GLuint GetShaderVariableLocation(GLUFProgramPtr prog, GLUFLocationType locType, std::string varName);
	GLUFVariableLocMap GetShaderAttribLocations(GLUFProgramPtr prog);
	GLUFVariableLocMap GetShaderUniformLocations(GLUFProgramPtr prog);

	GLUFVariableLocMap GetShaderAttribLocations(GLUFSepProgramPtr prog);
	GLUFVariableLocMap GetShaderUniformLocations(GLUFSepProgramPtr prog);

	//for removing things

	void DeleteShader(GLUFShaderPtr shader);
	void DeleteProgram(GLUFProgramPtr program);

	void FlushSavedShaders();
	void FlushSavedPrograms();
	void FlushLogs();


	//for accessing things
	const GLuint	  GetShaderId(GLUFShaderPtr shader) const;
	const GLUFShaderType  GetShaderType(GLUFShaderPtr shader) const;
	const GLuint	  GetProgramId(GLUFProgramPtr program) const;

	const GLUFCompileOutputStruct GetShaderLog(GLUFShaderPtr shaderPtr) const;

	const GLUFLinkOutputStruct GetProgramLog(GLUFProgramPtr programPtr) const;

	void AttachPrograms(GLUFSepProgramPtr ppo, GLUFProgramPtrStagesMap programs);
	void AttachProgram(GLUFSepProgramPtr ppo, GLbitfield stages, GLUFProgramPtr);

	//this clears all programs from the given stages
	void ClearPrograms(GLUFSepProgramPtr ppo, GLbitfield stages = GL_ALL_SHADER_BITS);

	//for using things

	void UseProgram(GLUFProgramPtr program);
	void UseProgram(GLUFSepProgramPtr program);
	void UseProgramNull();


};

//global instances of the managers (becuase they are not static, and have to member variables)
//extern GLUFBufferManager OBJGLUF_API g_BufferManager;
extern GLUFShaderManager OBJGLUF_API g_ShaderManager;

//#define GLUFBUFFERMANAGER g_BufferManager
#define GLUFSHADERMANAGER GLUF::g_ShaderManager

/*
Usage examples


//create the shader
GLUFShaderPtr shad = CreateShader("shader.glsl", ST_VERTEX_SHADER);



*/


enum GLUFTextureFileFormat
{
	TFF_DDS = 0,//we will ONLY support dds's, because they are flexible enough, AND have mipmaps
	TTF_DDS_CUBEMAP = 1
};


GLuint OBJGLUF_API LoadTextureFromFile(std::wstring filePath, GLUFTextureFileFormat format);
GLuint OBJGLUF_API LoadTextureFromMemory(char* data, unsigned int length, GLUFTextureFileFormat format);//this is broken, WHY



struct GLUFMeshBarebones
{
	Vec3Array mVertices;
	IndexArray mIndices;
};

////////////////////////////////////////////////////////////////
// these are buffer helpers, they are meant to be flexible 
//	and revealing
//

typedef signed char* GLUFBufferData;
typedef GLuint       GLUFAttribLoc;

struct OBJGLUF_API GLUFVertexAttribInfo
{
	unsigned short BytesPerElement;//int would be 4
	unsigned short ElementsPerValue;//vec4 would be 4
	GLUFAttribLoc  VertexAttribLocation;
	GLenum         Type;//float would be GL_FLOAT
};


class OBJGLUF_API GLUFVertexArrayBase
{
protected:
	GLuint mVertexArrayId = 0;
	GLuint mVertexCount = 0;
	

	GLenum mUsageType;
	GLenum mPrimitiveType;
	std::vector<GLUFVertexAttribInfo> mAttribInfos;//these are IN ORDER

	GLuint mIndexBuffer = 0;
	GLuint mRangedIndexBuffer = 0;
	GLuint mIndexCount  = 0;

	virtual void RefreshDataBufferAttribute() = 0;
	GLUFVertexAttribInfo GetAttribInfoFromLoc(GLUFAttribLoc loc);
	GLUFVertexArrayBase(const GLUFVertexArrayBase& other){};
public:
	GLUFVertexArrayBase(GLenum PrimType = GL_TRIANGLES, GLenum buffUsage = GL_STATIC_DRAW, bool index = true);
	~GLUFVertexArrayBase();

	//this would be used to add color, or normals, or texcoords, or even positions.  NOTE: this also deletes ALL DATA in this buffer
	virtual void AddVertexAttrib(GLUFVertexAttribInfo info);
	virtual void RemoveVertexAttrib(GLUFAttribLoc loc);
	void BindVertexArray();

	void Draw();
	void DrawRange(GLuint start, GLuint count);

	void DrawInstanced(GLuint instances);

	void BufferIndices(GLuint* indices, unsigned int Count);
	void BufferIndices(std::vector<glm::u32vec2> indices);
	void BufferIndices(std::vector<glm::u32vec3> indices);
	//void BufferFaces(GLuint* indices, unsigned int FaceCount);

	virtual void EnableVertexAttributes();
	virtual void DisableVertexAttributes();
};

//NOTE: this requires all data to be on four byte bounderies
class OBJGLUF_API GLUFVertexArrayAoS : public GLUFVertexArrayBase
{
	GLuint mDataBuffer = 0;

	virtual void RefreshDataBufferAttribute();

public:
	GLUFVertexArrayAoS(GLenum PrimType = GL_TRIANGLES, GLenum buffUsage = GL_STATIC_DRAW, bool indexed = true);
	~GLUFVertexArrayAoS();
	GLUFVertexArrayAoS(const GLUFVertexArrayAoS& other);

	//This includes the padding on 4 byte bounderies
	unsigned int GetVertexSize();

	void BufferData(void* data,  unsigned int NumVertices);//this is used to add  whole vertices.  
	void BufferSubData(unsigned int ValueOffsetCount/*ie 3 to insert after the 4th element*/, unsigned int NumValues, void* data);

};


class OBJGLUF_API GLUFVertexArraySoA : public GLUFVertexArrayBase
{
protected:
	//       Attrib Location, buffer location
	std::map<GLUFAttribLoc, GLuint> mDataBuffers;

	virtual void RefreshDataBufferAttribute();

	GLuint GetBufferIdFromAttribLocation(GLUFAttribLoc loc);

public:
	GLUFVertexArraySoA(GLenum PrimType = GL_TRIANGLES, GLenum buffUsage = GL_STATIC_DRAW, bool indexed = true);
	~GLUFVertexArraySoA();
	GLUFVertexArraySoA(const GLUFVertexArraySoA& other);

	GLUFMeshBarebones GetBarebonesMesh();
	void BufferData(GLUFAttribLoc loc, GLuint VertexCount, void* data);
	void BufferSubData(GLUFAttribLoc loc, GLuint VertexOffsetCount, GLuint VertexCount, void* data);

	//this would be used to add color, or normals, or texcoords, or even positions.  NOTE: this also deletes ALL DATA in this buffer
	virtual void AddVertexAttrib(GLUFVertexAttribInfo info);
	virtual void RemoveVertexAttrib(GLUFAttribLoc loc);

	//friend std::ostream& operator<<(std::ostream& out, const GLUFVertexArraySoA& dat);

};

/*inline std::ostream& operator << ( std::ostream& out, const GLUFVertexArraySoA& dat)
{
	out << "Vertex Array:\n" << "Attributes: {\n\t";
	for (auto it : dat.mAttribInfos)
		out << "(" << it.BytesPerElement << "," << it.ElementsPerValue << "," << it.VertexAttribLocation << "," << it.Type << "),\n\t";
	out << "}\n";
	out << "Data Buffers: {\n\t";
	for (auto it : dat.mDataBuffers)
		out << "(" << it.first << "," << it.second << "),\n\t";
	out << "}\n";
	out << "Index Buffer: " << dat.mIndexBuffer << "\n";
	out << "Index Count:  " << dat.mIndexCount << "\n";
	out << "Primitive Type: " << dat.mPrimitiveType << "\n";
	out << "Usage Type: " << dat.mUsageType << "\n";
	out << "Vertex Array Id: " << dat.mVertexArrayId << "\n";
	out << "Vertex Count: " << dat.mVertexCount << "\n";
	
	return out;
}*/


typedef GLUFVertexArraySoA GLUFVertexArray;

#ifdef USING_ASSIMP
GLUFVertexArray					OBJGLUF_API *LoadVertexArrayFromScene(const aiScene* scene, unsigned int meshNum = 0);
std::vector<GLUFVertexArray*>	OBJGLUF_API LoadVertexArraysFromScene(const aiScene* scene, unsigned int numMeshes);

typedef std::map<unsigned char, GLUFVertexAttribInfo> GLUFVertexAttribMap;
typedef std::pair<unsigned char, GLUFVertexAttribInfo> GLUFVertexAttribPair;
//the unsigned char represents the below #defines (GLUF_VERTEX_ATTRIB_*)
GLUFVertexArray					OBJGLUF_API *LoadVertexArrayFromScene(const aiScene* scene, GLUFVertexAttribMap inputs, unsigned int meshNum = 0);
#endif

//these are preprocessors that require the differet locations for different attributes(defaults)
#define GLUF_VERTEX_ATTRIB_POSITION		0
#define GLUF_VERTEX_ATTRIB_NORMAL		1
#define GLUF_VERTEX_ATTRIB_UV0			2
#define GLUF_VERTEX_ATTRIB_COLOR0		3
#define GLUF_VERTEX_ATTRIB_TAN			4
#define GLUF_VERTEX_ATTRIB_BITAN		5

#define GLUF_VERTEX_ATTRIB_UV1			10
#define GLUF_VERTEX_ATTRIB_UV2			11
#define GLUF_VERTEX_ATTRIB_UV3			12
#define GLUF_VERTEX_ATTRIB_UV4			13
#define GLUF_VERTEX_ATTRIB_UV5			14
#define GLUF_VERTEX_ATTRIB_UV6			15
#define GLUF_VERTEX_ATTRIB_UV7			16

#define GLUF_VERTEX_ATTRIB_COLOR1		17
#define GLUF_VERTEX_ATTRIB_COLOR2		18
#define GLUF_VERTEX_ATTRIB_COLOR3		19
#define GLUF_VERTEX_ATTRIB_COLOR4		20
#define GLUF_VERTEX_ATTRIB_COLOR5		21
#define GLUF_VERTEX_ATTRIB_COLOR6		22
#define GLUF_VERTEX_ATTRIB_COLOR7		23

//these are premade GLUFVertexAttribInfo for the standard inputs
extern const GLUFVertexAttribInfo OBJGLUF_API g_attribPOS;
extern const GLUFVertexAttribInfo OBJGLUF_API g_attribNORM;
extern const GLUFVertexAttribInfo OBJGLUF_API g_attribUV0;
extern const GLUFVertexAttribInfo OBJGLUF_API g_attribUV1;
extern const GLUFVertexAttribInfo OBJGLUF_API g_attribUV2;
extern const GLUFVertexAttribInfo OBJGLUF_API g_attribUV3;
extern const GLUFVertexAttribInfo OBJGLUF_API g_attribUV4;
extern const GLUFVertexAttribInfo OBJGLUF_API g_attribUV5;
extern const GLUFVertexAttribInfo OBJGLUF_API g_attribUV6;
extern const GLUFVertexAttribInfo OBJGLUF_API g_attribUV7;
extern const GLUFVertexAttribInfo OBJGLUF_API g_attribCOLOR0;
extern const GLUFVertexAttribInfo OBJGLUF_API g_attribCOLOR1;
extern const GLUFVertexAttribInfo OBJGLUF_API g_attribCOLOR2;
extern const GLUFVertexAttribInfo OBJGLUF_API g_attribCOLOR3;
extern const GLUFVertexAttribInfo OBJGLUF_API g_attribCOLOR4;
extern const GLUFVertexAttribInfo OBJGLUF_API g_attribCOLOR5;
extern const GLUFVertexAttribInfo OBJGLUF_API g_attribCOLOR6;
extern const GLUFVertexAttribInfo OBJGLUF_API g_attribCOLOR7;
extern const GLUFVertexAttribInfo OBJGLUF_API g_attribTAN;
extern const GLUFVertexAttribInfo OBJGLUF_API g_attribBITAN;

extern std::map<unsigned char, GLUFVertexAttribInfo> OBJGLUF_API g_stdAttrib;

#define GLUFVertAttrib(location, bytes, count, type) {bytes, count, location, type}



extern GLuint OBJGLUF_API g_GLVersionMajor;
extern GLuint OBJGLUF_API g_GLVersionMinor;



//GLUF exception system
typedef unsigned int GLUFException;

#define GLUF_EXCEPTION_INVALID_LOCATION 0x00000001

}

#ifdef USING_ASSIMP
inline glm::vec2 AssimpToGlm(aiVector2D v)
{
	return glm::vec2(v.x, v.y);
}
inline glm::vec2 AssimpToGlm3_2(aiVector3D v)
{
	return glm::vec2(v.x, v.y);
}
inline glm::vec2* AssimpToGlm(aiVector2D* v, unsigned int count)
{
	glm::vec2* ret = new glm::vec2[count];
	for (unsigned int i = 0; i < count; ++i)
		ret[i] = AssimpToGlm(v[i]);
	return ret;
}
inline glm::vec2* AssimpToGlm3_2(aiVector3D* v, unsigned int count)
{
	glm::vec2* ret = new glm::vec2[count];
	for (unsigned int i = 0; i < count; ++i)
		ret[i] = AssimpToGlm3_2(v[i]);
	return ret;
}



inline glm::vec3 AssimpToGlm(aiVector3D v)
{
	return glm::vec3(v.x, v.y, v.z);
}
inline glm::vec3* AssimpToGlm(aiVector3D* v, unsigned int count)
{
	glm::vec3* ret = new glm::vec3[count];
	for (unsigned int i = 0; i < count; ++i)
		ret[i] = AssimpToGlm(v[i]);
	return ret;
}
inline glm::vec3 AssimpToGlm(aiColor3D v)
{
	return glm::vec3(v.r, v.g, v.b);
}

inline glm::vec4 AssimpToGlm(aiColor4D v)
{
	return glm::vec4(v.r, v.g, v.b, v.a);
}

inline GLUF::Color AssimpToGlm4_3u8(aiColor4D col)
{
	return GLUF::Color(col.r * 255, col.g * 255, col.b * 255, col.a * 255);
}

#endif