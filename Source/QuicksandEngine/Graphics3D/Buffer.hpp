#ifndef QSE_BUFFER_HPP
#define QSE_BUFFER_HPP
#include "MatrixStack.hpp"

//typedef GLuint GLBuffer;
//typedef GLuint GLVertexBuffer;
//typedef GLuint GLIndexBuffer;

enum UniformType
{
	UT_MODEL = 0,
	UT_VIEW = 1,
	UT_PROJ = 2,
	UT_MODELVIEW = 3,
	UT_MVP = 4
};

enum VertexArrayType
{
	VAO_POSITIONS = 0,
	VAO_NORMALS = 1,
	VAO_TEXCOORDS = 2,
	VAO_INDICIES = 3
};

struct MatrixTransformBlockPtrs
{
	glm::mat4 *pM;
	glm::mat4 *pV;
	glm::mat4 *pP;
	glm::mat4 *pMV;
	glm::mat4 *pMVP;

	MatrixTransformBlockPtrs(glm::mat4 *m, glm::mat4 *v, glm::mat4 *p, glm::mat4 *mv, glm::mat4 *mvp) : pM(new glm::mat4(*m)), pV(new glm::mat4(*v)), pP(new glm::mat4(*p)), pMV(new glm::mat4(*mv)), pMVP(new glm::mat4(*mvp))
	{}
	~MatrixTransformBlockPtrs(){ delete pM, pV, pP, pMV, pMVP; }

	glm::mat4* operator [](unsigned short index)
	{
		switch (index)
		{
		case UT_MODEL:
			return pM;
		case UT_VIEW:
			return pV;
		case UT_PROJ:
			return pP;
		case UT_MODELVIEW:
			return pMV;
		case UT_MVP:
			return pMVP;
		default:
			return nullptr;
		}
	}
};

struct MatrixTransformBlock
{
	glm::mat4 pM;
	glm::mat4 pV;
	glm::mat4 pP;
protected:
	glm::mat4 pMV;
	glm::mat4 pMVP;
public:

	MatrixTransformBlock(glm::mat4 m, glm::mat4 v, glm::mat4 p) : pM(m), pV(v), pP(p), pMV(v*m), pMVP(p*pMV)
	{}

	glm::mat4 operator [](unsigned short index)
	{
		switch (index)
		{
		case UT_MODEL:
			return pM;
		case UT_VIEW:
			return pV;
		case UT_PROJ:
			return pP;
		case UT_MODELVIEW:
			return pMV;
		case UT_MVP:
			return pMVP;
		default:
			return glm::mat4();
		}
	}
};

struct GLVAOData
{
	Vec3Array* const mPositions;
	Vec3Array* const mNormals;
	Vec2Array* const mUVCoords;
	GLIndexArray* const mIndicies;
};

class GLUniformBuffer;
class GLVertexArrayObject;
class GLTextureBuffer;


typedef weak_ptr<GLUniformBuffer> GLUniformBufferPtrWeak;
typedef weak_ptr<GLVertexArrayObject> GLVertexArrayPtrWeak;
typedef weak_ptr<GLTextureBuffer> GLTexturePtrWeak;

typedef shared_ptr<GLUniformBuffer> GLUniformBufferPtr;
typedef shared_ptr<GLVertexArrayObject> GLVertexArrayPtr;
typedef shared_ptr<GLTextureBuffer> GLTexturePtr;



//this is a similar system to the shaders system
class GLBufferManager
{
	std::map<string, GLUniformBufferPtrWeak> mUniformBuffers;
	std::map<string, GLVertexArrayPtrWeak> mVertexArrayBuffers;
	friend class DdsResourceLoader;

	GLTexturePtr CreateTexture(GLuint glTexId);

	friend class Scene;
	shared_ptr<GLMatrixStack> m_pMatrixStack;

public:

	GLMatrixStack GetMatrixStackState(){ return *m_pMatrixStack; }
	void SetMatrixStackOverride(GLMatrixStack stack){ *m_pMatrixStack = stack; }
	glm::mat4 GetMatrixStackTop(){ return m_pMatrixStack->Top(); }

	void ResetBufferBindings();//this is used to "unuse" the bound textures and buffers after done drawing
	
	GLVertexArrayPtr CreateVertexArray(GLuint vertexCount, glm::vec3* positions, glm::vec3* normals, glm::vec2* texCoords, GLuint* indicies, GLuint numFaces);
	GLUniformBufferPtr CreateUniformArray(MatrixTransformBlockPtrs* transPtrs, Material* mat/*, GLLights* lights*/);
	


	void DrawVertexArray(GLVertexArrayPtr vertArray, GLenum topology = GL_TRIANGLES);
	void DrawVertexArrayInstanced(GLVertexArrayPtr vertArray, GLuint instanceCount, GLenum topology = GL_TRIANGLES);


	void ModifyVertexArray(GLVertexArrayPtr vertArray, VertexArrayType type, GLsizei length, void* data);
	void ModifyVertexArray(GLVertexArrayPtr vertArray, GLuint vertexCount, glm::vec3* positions, glm::vec3* normals, glm::vec2* texCoords, GLuint* indicies, GLuint numFaces);
	
	GLVAOData MapVertexArray(GLVertexArrayPtr vertArray);
	void      UnMapVertexArray(GLVertexArrayPtr vertArray);

	//this is for modifying standard things such as transforms, or material
	void ModifyUniformMatrix(GLUniformBufferPtr buffer, UniformType type, glm::mat4 data);//the transform uniform block that is standard to all shaders will always be location of 1
	void ModifyUniformMatrix(GLUniformBufferPtr buffer, MatrixTransformBlockPtrs data);
	void ModifyUniformMatrix(GLUniformBufferPtr buffer, MatrixTransformBlock data);//this REQUIRES all matrices to exist
	void ModifyUniformMaterial(GLUniformBufferPtr buffer, Material mat);//todo
	void ModifyLighting(GLUniformBufferPtr buffer){ };//todo

	//for non-generic circumstances
	void ModifyUniform1f(GLUniformBufferPtr buffer, GLuint customLocation, float data);
	void ModifyUniform1d(GLUniformBufferPtr buffer, GLuint customLocation, double data);
	void ModifyUniform1i(GLUniformBufferPtr buffer, GLuint customLocation, int data);
	void ModifyUniform1ui(GLUniformBufferPtr buffer, GLuint customLocation, unsigned int data);

	void ModifyUniform2f(GLUniformBufferPtr buffer, GLuint customLocation, glm::vec2 data);
	void ModifyUniform2d(GLUniformBufferPtr buffer, GLuint customLocation, glm::dvec2 data);
	void ModifyUniform2i(GLUniformBufferPtr buffer, GLuint customLocation, glm::ivec2 data);

	void ModifyUniform3f(GLUniformBufferPtr buffer, GLuint customLocation, glm::vec3 data);
	void ModifyUniform3d(GLUniformBufferPtr buffer, GLuint customLocation, glm::dvec3 data);
	void ModifyUniform3i(GLUniformBufferPtr buffer, GLuint customLocation, glm::ivec3 data);

	void ModifyUniform4f(GLUniformBufferPtr buffer, GLuint customLocation, glm::vec4 data);
	void ModifyUniform4d(GLUniformBufferPtr buffer, GLuint customLocation, glm::dvec4 data);
	void ModifyUniform4i(GLUniformBufferPtr buffer, GLuint customLocation, glm::ivec4 data);


	void ModifyUniformMatrix3f(GLUniformBufferPtr buffer, GLuint customLocation, glm::mat3 data);
	void ModifyUniformMatrix3d(GLUniformBufferPtr buffer, GLuint customLocation, glm::dmat3 data);
	void ModifyUniformMatrix4f(GLUniformBufferPtr buffer, GLuint customLocation, glm::mat4 data);
	void ModifyUniformMatrix4d(GLUniformBufferPtr buffer, GLuint customLocation, glm::dmat4 data);

	//TODO: add support for custom uniform blocks

	void Clean();//this takes a currently unreferenced uniforms out, because they no longer exist


	//texture methods
	GLTextureBuffer CreateTextureBuffer();

	void UseTexture(GLTexturePtr texture);

	void* MapTextureData(GLTexturePtr texture, GLsizei length);
	void UnMapTextureData(GLTexturePtr texture);

	void BufferTexture(GLTexturePtr texture, GLsizei length, void* data);
	
};

#define BufferManager static_pointer_cast<GLRenderer>(QuicksandEngine::g_pApp->m_Renderer)->mBufferManager

#endif