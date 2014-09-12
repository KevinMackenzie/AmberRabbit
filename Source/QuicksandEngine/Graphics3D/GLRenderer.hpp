#ifndef QSE_GLRENDERER_HPP
#define QSE_GLRENDERER_HPP

#include "../../ObjGLUF/GLUFGui.h"
//#include "../UserInterface/GLGui.hpp"
//#include "Shaders.hpp"

//
// struct ConstantBuffer_Matrices				- Chapter X, page Y
//
struct ConstantBuffer_Matrices
{
	glm::mat4 m_WorldViewProj;
	glm::mat4 m_World;
};

//
// struct ConstantBuffer_Material				- Chapter X, page Y
//
struct ConstantBuffer_Material
{
	glm::vec4 m_vDiffuseObjectColor;
	glm::vec4 m_vAmbientObjectColor;
	BOOL m_bHasTexture;
	glm::vec3 m_vUnused;
};

#define MAXIMUM_LIGHTS_SUPPORTED (8)

//
// struct ConstantBuffer_Lighting				- Chapter X, page Y
//
struct ConstantBuffer_Lighting
{
	glm::vec4 m_vLightDiffuse[MAXIMUM_LIGHTS_SUPPORTED];
	glm::vec4 m_vLightDir[MAXIMUM_LIGHTS_SUPPORTED];
	glm::vec4 m_vLightAmbient;
	UINT m_nNumLights;
	glm::vec3 m_vUnused;
};

//
// class D3DRenderer11								- Chapter 10, page 270
//
//   The D3DRenderer and D3DRenderer9 classes are not discussed in the book. The D3DRenderer class is designed to 
//   implement the IRenderer interface, which abstracts the implentation of the renderer technology, which for this
//   engine can be either D3D9 or D3D11. It also encapsulates the usefulness of CDXUTDialogResourceManager 
//   and CDXUTTextHelper for user interface tasks whether D3D9 or D3D11 is being used.
//
class GLRenderer_Base : public IRenderer
{
protected:
	Color  m_BackgroundColor;
public:
	//TODO: make my own dialog manager and text helper
	// You should leave this global - it does wacky things otherwise.
	static GLUFDialogResourceManager *g_pDialogResourceManager;
	static GLUFTextHelper* g_pTextHelper;

	Color GetColor(){ return m_BackgroundColor; }
	//virtual void VSetWorldTransform(const glm::mat4 *m){};
	//virtual void VSetViewTransform(const glm::mat4 *m){};
	//virtual void VSetProjectionTransform(const glm::mat4 *m){};

	virtual void VSetBackgroundColor(glm::u8 bgA, glm::u8 bgR, glm::u8 bgG, glm::u8 bgB)
	{
		m_BackgroundColor = Color(bgR, bgG, bgB, bgA);
	}

	virtual void VSetBackgroundColor(Color col){ m_BackgroundColor = col; }

	virtual HRESULT VOnRestore() { return S_OK; }
	virtual void VShutdown() { SAFE_DELETE(g_pTextHelper); }
};





//
// class D3DLineDrawer11 - not described in the book, it is a helper class to draw lines in D3D11
//
class GLLineDrawer
{
public:
	GLLineDrawer();
	~GLLineDrawer() {  }

	void DrawLine(const glm::vec3& from, const glm::vec3& to, const Color& color);
	HRESULT OnRestore();

protected:
	static shared_ptr<ResHandle>  m_LineDrawerShader;
	GLUFVertexArray         m_pVertexBuffer;

	GLuint m_PositionLocation;
	GLuint m_ColorLocation;
};


class GLRenderer : public GLRenderer_Base
{
public:
	GLRenderer() { m_pLineDrawer = NULL; }
	virtual void VShutdown()  { GLRenderer_Base::VShutdown(); SAFE_DELETE(m_pLineDrawer); }


	virtual bool VPreRender();
	virtual bool VPostRender();
	virtual HRESULT VOnRestore();
	virtual void VCalcLighting(Lights *lights, int maximumLights) { }

	virtual void VDrawLine(const glm::vec3& from, const glm::vec3& to, const Color& color);

	virtual shared_ptr<IRenderState> VPrepareAlphaPass();
	virtual shared_ptr<IRenderState> VPrepareSkyBoxPass();

protected:

	GLLineDrawer						*m_pLineDrawer;
};



#endif