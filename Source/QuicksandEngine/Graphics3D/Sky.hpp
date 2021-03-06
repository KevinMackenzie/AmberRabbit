#ifndef QSE_SKY_HPP
#define QSE_SKY_HPP


#include "Geometry.hpp"

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
	shared_ptr<ResHandle>   m_Cubemap;
	shared_ptr<CameraNode>	m_camera;
	bool					m_bActive;

	//std::string GetTextureName(const int side);

public:
	SkyNode(shared_ptr<ResHandle> cubemap);
	virtual ~SkyNode() { }
	HRESULT VPreRender(Scene *pScene);
	bool VIsVisible(Scene *pScene) const { return m_bActive; }
};

class GLSkyNode : public SkyNode
{
public:
	GLSkyNode(shared_ptr<ResHandle> cubemap);
	virtual ~GLSkyNode();
	HRESULT VOnRestore(Scene *pScene);
	virtual HRESULT VPreRender(Scene* pScene);
	HRESULT VRender(Scene *pScene);
	virtual HRESULT VPostRender(Scene* pScene);

protected:

	GLUFVertexArray				m_pVertexArray;

	GLUFAttribLoc mPositionLoc;
	GLUFAttribLoc mUVLoc;

	static shared_ptr<ResHandle>		m_pSkyProgram;
};




#endif