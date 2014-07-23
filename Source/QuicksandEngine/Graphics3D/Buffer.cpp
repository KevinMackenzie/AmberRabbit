#include "../Stdafx.hpp"
#include "Buffer.hpp"
#include "Shaders.hpp"

class GLUniformBuffer
{
public:
	GLProgramPtrWeak programRef;
};

void GLBufferManager::ModifyUniform(GLUniformBufferPtr buffer, UniformType type, void* data)
{/*
	GLuint bindingPoint = 1, buffer, blockIndex;
	float myFloats[8] = { 1.0, 0.0, 0.0, 1.0, 0.4, 0.0, 0.0, 1.0 };

	blockIndex = glGetUniformBlockIndex(ShaderManager.GetProgramId(buffer->programRef.lock()), "ColorBlock");
	glUniformBlockBinding(p, blockIndex, bindingPoint);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, buffer);

	glBufferData(GL_UNIFORM_BUFFER, sizeof(myFloats), myFloats, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, buffer);*/
}

