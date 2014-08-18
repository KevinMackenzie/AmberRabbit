#ifndef QSE_SKY_HPP
#define QSE_SKY_HPP


#include "Geometry.hpp"
#include "GLUFMaterial.hpp"
#include "Shaders.hpp"

// Forward declarations
class SceneNode;
class Scene;

////////////////////////////////////////////////////
//
// class SkyNode					- Chapter 16, page 554
//
//    Implements a believable sky that follows
//	  the camera around - this is a base class that the D3D9 and D3D11 classes 
//    inherit from
//
////////////////////////////////////////////////////

class SkyNode : public SceneNode
{
protected:
	DWORD					m_numVerts;
	DWORD					m_sides;
	const char *			m_textureBaseName;
	shared_ptr<CameraNode>	m_camera;
	bool					m_bActive;

	std::string GetTextureName(const int side);

public:
	SkyNode(const char *textureFile);
	virtual ~SkyNode() { }
	HRESULT VPreRender(Scene *pScene);
	bool VIsVisible(Scene *pScene) const { return m_bActive; }
};

class GLSkyNode : public SkyNode
{
public:
	GLSkyNode(const char *pTextureBaseName);
	virtual ~GLSkyNode();
	HRESULT VOnRestore(Scene *pScene);
	HRESULT VRender(Scene *pScene);

protected:

	shared_ptr<ResHandle>       m_pTexture;

	GLVertexArrayPtr            m_pVertexArray;
	GLUniformBufferPtr			m_pUniformBuffer;

	GLProgramPtr				m_pShaderProgram;
};




#endif