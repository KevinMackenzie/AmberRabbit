#ifndef QSE_BUFFER_HPP
#define QSE_BUFFER_HPP

//typedef GLuint GLBuffer;
//typedef GLuint GLVertexBuffer;
//typedef GLuint GLIndexBuffer;
enum UniformType
{

};

class GLUniformBuffer;
class GLVertexArrayObject;


typedef weak_ptr<GLUniformBuffer> GLUniformBufferPtrWeak;
typedef weak_ptr<GLVertexArrayObject> GLVertexArrayPtrWeak;

typedef shared_ptr<GLUniformBuffer> GLUniformBufferPtr;
typedef shared_ptr<GLVertexArrayObject> GLVertexArrayPtr;


//this is a similar system to the shaders system
class GLBufferManager
{
	std::map<string, GLUniformBufferPtrWeak> mUniformBuffers;
	std::map<string, GLVertexArrayPtrWeak> mVertexArrayBuffers;


public:

	
	GLVertexArrayPtr CreateVertexArray(GLuint vertexCount, vec3* positions, vec3* normals, vec2* texCoords, GLuint* indicies, GLuint mNumFaces);
	GLUniformBufferPtr CreateUniformArray();

	void ModifyVertexArray(GLuint vertexCount, vec3* positions, vec3* normals, vec2* texCoords, GLuint* indicies, GLuint numFaces);
	
	void ModifyUniform(GLUniformBufferPtr buffer, UniformType type, void* Data);


	void Clean();//this takes a currently unreferenced uniforms out, because they no longer exist
};

#endif