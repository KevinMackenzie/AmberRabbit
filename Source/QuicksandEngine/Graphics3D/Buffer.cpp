#include "../Stdafx.hpp"
#include "Buffer.hpp"
#include "Shaders.hpp"
#include "Material.hpp"
#include "MatrixStack.hpp"

class GLUniformBuffer
{
public:
	GLProgramPtrWeak programRef;
	GLuint mBufferId;

	~GLUniformBuffer(){ glDeleteBuffers(1, &mBufferId); }
};

class GLVertexArrayObject
{
	friend GLBufferManager;

	//make sure to check that each element has the SAME number of elements (except for indices)
	Vec3Array mPositions;
	Vec3Array mNormals;
	Vec2Array mUVCoords;
	GLIndexArray mIndicies;
	GLubyte mVertsPerFace = 3;

	GLuint mVAOId = 0;
	GLuint mPositionId = 0;
	GLuint mNormalId = 0;
	GLuint mUVCoordId = 0;
	GLuint mIndexId = 0;


public:

	~GLVertexArrayObject();

};

class GLTextureBuffer
{
	GLuint mTextureId;

public:
	
	~GLTextureBuffer(){ glDeleteTextures(1, &mTextureId); };
};

GLVertexArrayObject::~GLVertexArrayObject()
{
	glDeleteVertexArrays(1, &mVAOId);
	GLuint mBuffIds[4] = {mPositionId, mNormalId, mUVCoordId, mIndexId};
	glDeleteBuffers(4, mBuffIds);
}


///////////////////////////////
//Shader code for uniform block
//
//NOTE: All will be provided unless necisary ( MAYBE FOR NOW )
//
//
/*

layout(std140, binding = 3) uniform MatrixTransformations
{					//base alignment			offset			aligned offset
	glm::mat4 m;			16						0					0
	glm::mat4 v;			16						64					64
	glm::mat4 p;			16						128					128
	glm::mat4 mv;			16						192					192
	glm::mat4 mvp;			16						256					256
}


*/

GLVertexArrayPtr GLBufferManager::CreateVertexArray(GLuint vertexCount, glm::vec3* positions, glm::vec3* normals, glm::vec2* texCoords, GLuint* indicies, GLuint numFaces)
{
	
}

void GLBufferManager::ResetBufferBindings()
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindVertexArray(0);
}

void GLBufferManager::ModifyUniformMatrix(GLUniformBufferPtr buffer, UniformType type, glm::mat4 data)
{

	glBindBufferBase(GL_UNIFORM_BUFFER, 3, buffer->mBufferId);
	glm::mat4* matrix = reinterpret_cast<glm::mat4*>(glMapBufferRange(GL_UNIFORM_BUFFER, type * 64, sizeof(glm::mat4), GL_DYNAMIC_DRAW));
	*matrix = data;
	glUnmapBuffer(GL_UNIFORM_BUFFER);
}

void GLBufferManager::ModifyUniformMatrix(GLUniformBufferPtr buffer, MatrixTransformBlockPtrs data)
{
	//determine which section to map
	unsigned int offset;
	(data.pM) ? offset = 0 : (data.pV) ? offset = 1 : (data.pP) ? offset = 2 : (data.pMV) ? offset = 3 : (data.pMVP) ? offset = 4 : LOG_WARNING("Calling ModifyUniformMatrix with MatrixTransformBlock as paramters, however all are null");
	unsigned int buffEnd;
	(data.pMVP) ? buffEnd = UT_MVP + 1 : (data.pMV) ? buffEnd = UT_MODELVIEW + 1 : (data.pP) ? buffEnd = UT_PROJ + 1 : (data.pV) ? buffEnd = UT_VIEW + 1 : (data.pM) ? buffEnd = UT_MODEL + 1 : true;

	//map them
	glBindBufferBase(GL_UNIFORM_BUFFER, 3, buffer->mBufferId);
	glm::mat4* matrices = reinterpret_cast<glm::mat4*>(glMapBufferRange(GL_UNIFORM_BUFFER, offset*64, (buffEnd - offset)*64, GL_DYNAMIC_DRAW));


	//map each matrix provided
	unsigned short it = 0;
	for (unsigned short i = offset; i <= buffEnd; ++i)
	{
		glm::mat4* tmpMat = data[i];
		
		//make sure its not null, we mapped a range, who knows what is in between except for the first and last element
		if (tmpMat)
		{
			matrices[it] = *tmpMat;
		}
		++it;
	}
	
	//lastly unmap it
	glUnmapBuffer(GL_UNIFORM_BUFFER);

}

void GLBufferManager::ModifyUniformMatrix(GLUniformBufferPtr buffer, MatrixTransformBlock data)
{
	//save the time, and do NOT determine which sections to map, just jump right in
	glBindBufferBase(GL_UNIFORM_BUFFER, 3, buffer->mBufferId);
	glm::mat4* matricies = reinterpret_cast<glm::mat4*>(glMapBufferRange(GL_UNIFORM_BUFFER, 0, (UT_MVP + 1) * 64, GL_DYNAMIC_DRAW));

	for (unsigned short i = 0; i < UT_MVP; ++i)
	{
		matricies[i] = data[i];
	}
	glUnmapBuffer(GL_UNIFORM_BUFFER);
}