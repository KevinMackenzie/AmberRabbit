#ifndef QSE_MATRIXSTACK_HPP
#define QSE_MATRIXSTACK_HPP

#include <stack>

//very simple, but very powerful
class GLMatrixStack
{
	std::stack<const mat4&> mStack;
public:
	void Push(const mat4& matrix);
	void Pop(void);
	const mat4& Top();//this returns the matrix that is a concatination of all subsequent matrices
	size_t Size();
	void Empty();
};


#endif