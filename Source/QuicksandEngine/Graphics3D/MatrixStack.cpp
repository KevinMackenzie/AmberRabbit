#include "../Stdafx.hpp"
#include "MatrixStack.hpp"

void GLMatrixStack::Push(const glm::mat4& matrix)
{
	if (mStack.size() != 0)
	{
		//if there are already things on the stack, instead of multiplying through EVERY TIME TOP IS CALLED, make it more efficient 
		//so the top is ALWAYS a concatination
		glm::mat4 transformed = matrix * Top();

		mStack.push(transformed);
	}
	else
	{
		mStack.push(matrix);
	}
}

void GLMatrixStack::Pop(void)
{
	mStack.pop();
}

const glm::mat4& GLMatrixStack::Top(void)
{
	return mStack.top();
}

size_t GLMatrixStack::Size(void)
{
	return mStack.size();
}

void GLMatrixStack::Empty(void)
{
	mStack.empty();
}