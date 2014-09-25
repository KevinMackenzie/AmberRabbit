#ifndef QSE_SCENENODE_HPP
#define QSE_SCENENODE_HPP

#include "../Application/Interfaces.hpp"
#include "ShaderResource.hpp"

#include "Geometry.hpp"
#include "Material.hpp"
//#include "Shaders.hpp"

// Forward declarations
class SceneNode;
class Scene;
class RayCast;
class MovementController;
class IResourceExtraData;
class ActorComponent;
class BaseRenderComponent;

// FUTURE WORK - Smart pointers don't work right....going to use a naked pointer for now!
typedef BaseRenderComponent* WeakBaseRenderComponentPtr;

////////////////////////////////////////////////////
//
// AlphaType					- Chapter X, page Y
//
//   This enum defines the different types of alpha blending
//   types that can be set on a scene node.
//
//	
////////////////////////////////////////////////////

enum AlphaType
{
	AlphaOpaque,
	AlphaTexture,
	AlphaMaterial,
	AlphaVertex
};


//////////////////////////////////////////////////////////////////////
//  class SceneNodeProperties	- Chapter 16, page 527
//
//   This class is contained in the SceneNode class, and gains
//   easy access to common scene node properties such as its ActorId
//   or render pass, with a single ISceneNode::VGet() method.
//
//////////////////////////////////////////////////////////////////////

class SceneNodeProperties
{
	friend class SceneNode;

protected:
	ActorId                 m_ActorId;
	string					m_Name;
	glm::mat4					m_ToWorld, m_FromWorld;
	float					m_Radius;
	RenderPass				m_RenderPass;
	shared_ptr<ResHandle>	m_Material;
	AlphaType				m_AlphaType;

	void SetAlpha(glm::u8 alpha)
	{
		m_AlphaType = AlphaMaterial; GetMaterial()->SetAlpha(alpha);
	}

public:
	SceneNodeProperties(void);
	const ActorId &ActorId() const { return m_ActorId; }
	glm::mat4 const &ToWorld() const { return m_ToWorld; }
	glm::mat4 const &FromWorld() const { return m_FromWorld; }
	void Transform(glm::mat4 *toWorld, glm::mat4 *fromWorld) const;

	const char * Name() const { return m_Name.c_str(); }

	bool HasAlpha() const { return GetMaterial()->HasAlpha(); }
	float Alpha() const { return GetMaterial()->GetAlpha(); }
	AlphaType AlphaType() const { return m_AlphaType; }

	RenderPass RenderPass() const { return m_RenderPass; }
	float Radius() const { return m_Radius; }

	shared_ptr<GLMaterialResourceExtraData> GetMaterial() const { return static_pointer_cast<GLMaterialResourceExtraData>(m_Material->GetExtra()); }
	shared_ptr<ResHandle> GetMaterialResource() const { return m_Material; }
	void SetMaterial(shared_ptr<ResHandle> mat){ m_Material = mat; }
};

//////////////////////////////////////////////////////////////
//
// SceneNodeList						- Chapter 16, page 529
//
//   Every scene node has a list of its children - this 
//   is implemented as a vector since it is expected that
//   we won't add/delete nodes very often, and we'll want 
//   fast random access to each child.
//
//////////////////////////////////////////////////////////////

typedef std::vector<shared_ptr<ISceneNode> > SceneNodeList;


//////////////////////////////////////////////////////////////
//
// SceneNode							- Chapter 16, page 529
//
//   Implements ISceneNode. Forms the base class for any node
//   that can exist in the 3D scene graph managed by class Scene.
//
//////////////////////////////////////////////////////////////

class SceneNode : public ISceneNode
{
	friend class Scene;

protected:
	SceneNodeList			m_Children;
	SceneNode				*m_pParent;
	SceneNodeProperties		m_Props;
	WeakBaseRenderComponentPtr	m_RenderComponent;

	shared_ptr<ResHandle> m_pShader;

public:
	SceneNode(ActorId actorId, WeakBaseRenderComponentPtr renderComponent, RenderPass renderPass, const glm::mat4 *to, const glm::mat4 *from = NULL);

	virtual ~SceneNode();

	shared_ptr<ResHandle> GetShader();

	void ApplyTransformUniforms();

	virtual const SceneNodeProperties* const VGet() const { return &m_Props; }

	virtual void VSetTransform(const glm::mat4 *toWorld, const glm::mat4 *fromWorld = NULL);

	virtual HRESULT VOnRestore(Scene *pScene);
	virtual HRESULT VOnUpdate(Scene *, double const fEllapsed);

	virtual HRESULT VPreRender(Scene *pScene);
	virtual bool VIsVisible(Scene *pScene) const;
	virtual HRESULT VRender(Scene *pScene);
	virtual HRESULT VRenderChildren(Scene *pScene);
	virtual HRESULT VPostRender(Scene *pScene);

	virtual bool VAddChild(shared_ptr<ISceneNode> kid);
	virtual bool VRemoveChild(ActorId id);
	virtual HRESULT VOnLostDevice(Scene *pScene);
	virtual HRESULT VPick(Scene *pScene, RayCast *pRayCast);

	void SetShader(shared_ptr<ResHandle> pShader);

	void SetAlpha(float alpha);
	float GetAlpha() const { return m_Props.Alpha(); }

	glm::vec3 GetPosition() const { return ::GetPosition(m_Props.m_ToWorld); }
	void SetPosition(const glm::vec3 &pos) { ::SetPosition(m_Props.m_ToWorld, pos); }

	const glm::vec3 GetWorldPosition() const;					// [mrmike] added post-press to respect ancestor's position 

	glm::vec3 GetDirection() const { return ::GetDirection(m_Props.m_ToWorld); }

	glm::mat4 GetToWorld() const { return m_Props.m_ToWorld; }

	void SetRadius(const float radius) { m_Props.m_Radius = radius; }
	void SetMaterial(shared_ptr<GLMaterialResourceExtraData> mat) { m_Props.m_Material->SetExtra(mat); }
	void SetMaterial(shared_ptr<ResHandle> mat) { m_Props.m_Material = mat; }
	shared_ptr<GLMaterialResourceExtraData> GetMaterial(void){ return m_Props.GetMaterial(); }
	shared_ptr<ResHandle> GetMaterialResource(void){ return m_Props.GetMaterialResource(); }
};

//
// class D3DSceneNode9						- not described in the book
// class D3DSceneNode11						- not described in the book
//
// The book only describes a D3D11 render implementations. The engine code implements D3D9 as well as 
//    D3D11, by creating different renderers and scene nodes based on the settings of PlayerOptions.xml.  
//
/*
class D3DSceneNode9 : public SceneNode
{
public:
	D3DSceneNode9(const ActorId actorId,
		WeakBaseRenderComponentPtr renderComponent,
		RenderPass renderPass,
		const glm::mat4 *t)
		: SceneNode(actorId, renderComponent, renderPass, t) { }

	virtual HRESULT VRender(Scene *pScene);
};

class D3DSceneNode11 : public SceneNode
{
public:
	virtual HRESULT VRender(Scene *pScene) { return S_OK; }
};*/


////////////////////////////////////////////////////
//
// AlphaSceneNode Description						- Chapter 16, page 535
// AlphaSceneNodes Description						- Chapter 16, page 535
//
// A list of scene nodes that need to be drawn in the alpha pass;
// the list is defined as an STL list
////////////////////////////////////////////////////

struct AlphaSceneNode
{
	shared_ptr<ISceneNode> m_pNode;
	glm::mat4 m_Concat;
	float m_ScreenZ;

	// For the STL sort...
	bool const operator <(AlphaSceneNode const &other) { return m_ScreenZ < other.m_ScreenZ; }
};

typedef std::list<AlphaSceneNode *> AlphaSceneNodes;


////////////////////////////////////////////////////
//
// SceneActorMap Description
//
//   An STL map that allows fast lookup of a scene node given an ActorId.
//
////////////////////////////////////////////////////

typedef std::map<ActorId, shared_ptr<ISceneNode> > SceneActorMap;


////////////////////////////////////////////////////
//
// Scene Description
//
// A heirarchical container of scene nodes, which
// are classes that implemente the ISceneNode interface
//
////////////////////////////////////////////////////

class CameraNode;
class SkyNode;


////////////////////////////////////////////////////
//
// class RootNode					- Chapter 16, page 545
//
//    This is the root node of the scene graph.
//
////////////////////////////////////////////////////

class RootNode : public SceneNode
{
public:
	RootNode();
	virtual bool VAddChild(shared_ptr<ISceneNode> kid);
	virtual HRESULT VRenderChildren(Scene *pScene);
	virtual bool VRemoveChild(ActorId id);
	virtual bool VIsVisible(Scene *pScene) const { return true; }
};


////////////////////////////////////////////////////
//
// class CameraNode					- Chapter 16, page 548
//
//    A camera node controls the D3D view transform and holds the view
//    frustum definition
//
////////////////////////////////////////////////////

class CameraNode : public SceneNode
{
	friend Scene;
public:
	CameraNode(glm::mat4 const *t, Frustum const &frustum)
		: SceneNode(INVALID_ACTOR_ID, WeakBaseRenderComponentPtr(), RenderPass_0, t),
		m_Frustum(frustum),
		m_bActive(true),
		m_DebugCamera(false),
		m_pTarget(shared_ptr<SceneNode>()),
		m_CamOffsetVector(0.0f, 1.0f, -10.0f, 0.0f)
	{
		VOnRestore(nullptr);
	}

	virtual HRESULT VRender(Scene *pScene);
	virtual HRESULT VOnRestore(Scene *pScene);
	virtual bool VIsVisible(Scene *pScene) const { return m_bActive; }

	const Frustum &GetFrustum() { return m_Frustum; }
	void SetTarget(shared_ptr<SceneNode> pTarget)
	{
		m_pTarget = pTarget;
	}
	void ClearTarget() { m_pTarget = shared_ptr<SceneNode>(); }
	shared_ptr<SceneNode> GetTarget() { return m_pTarget; }

	glm::mat4 GetWorldViewProjection(Scene *pScene);
	HRESULT SetViewTransform(Scene *pScene);

	glm::mat4 GetProjection() { return m_Projection; }
	glm::mat4 GetView() { return m_View; }

	void SetCameraOffset(const glm::vec4 & cameraOffset)
	{
		m_CamOffsetVector = cameraOffset;
	}

protected:

	Frustum			m_Frustum;
	glm::mat4			m_Projection;
	glm::mat4			m_View;
	bool			m_bActive;
	bool			m_DebugCamera;
	shared_ptr<SceneNode> m_pTarget;
	glm::vec4			m_CamOffsetVector;	//Direction of camera relative to target.
};


//TODO: SHADER FOR GRID
class GLGrid : public SceneNode
{
protected:

	//this is the number of squares on each side, ie 2 would be a 2x2 grid
	//int								m_nSideLength;

	//the length of each square
	//float							m_fSquareLength;

	GLUFVertexArray					m_Squares;
	glm::mat4						m_ModelMatrix;

	static shared_ptr<ResHandle> m_pGridProgram;

public:
	//bool					m_bTextureHasAlpha;

	GLGrid(ActorId actorId, WeakBaseRenderComponentPtr renderComponent, 
		/* const string& name, const char* textureResource,*/ 
		int squares, float squareSize, const Color &diffuseColor,  const glm::mat4* pMatrix);
	virtual ~GLGrid();
	virtual HRESULT VOnRestore(Scene *pScene);
	virtual HRESULT VRender(Scene *pScene);
	virtual HRESULT VOnUpdate(Scene *pScene, double const fEllapsed) { return S_OK; }
	virtual HRESULT VPick(Scene *pScene, RayCast *pRayCast);// { return E_FAIL; }

	//bool VHasAlpha() const { return m_bTextureHasAlpha; }
};



////////////////////////////////////////////////////
//
// ArrowNode Description
//
//   Graphical object that looks like an arrow - they
//     aren't discussed in the book per se but you see them
//     in a lot of the figures!
//
////////////////////////////////////////////////////
/*
class ArrowNode : public SceneNode
{
protected:
	 *m_shaft;
	ID3DXMesh *m_cone;
	glm::mat4 m_coneTrans;
	glm::mat4 m_shaftTrans;

public:
	ArrowNode(string name, WeakBaseRenderComponentPtr renderComponent, const float length, const glm::mat4 *t, const Color &color);

	virtual ~ArrowNode() { SAFE_RELEASE(m_shaft); SAFE_RELEASE(m_cone); }
	virtual HRESULT VRender(Scene *pScene);
	virtual HRESULT VPick(Scene *pScene, RayCast *pRayCast);
};*/


////////////////////////////////////////////////////
//
// TestObject Description
//
//    Implements a test object that has no symmetry on X, Y, and Z axes.
//    Really useful to make sure you haven't flipped left-handed versus
//    right handed coordinate systems or something like that
//
////////////////////////////////////////////////////

//class TriangleIterator;
//struct TestObjectParams;

/*
class TestObject : public SceneNode
{
protected:
	GLVertexBuffer          m_pVerts;
	DWORD					m_numVerts;
	DWORD					m_numPolys;

	DWORD					m_color;
	bool					m_squashed;

public:
	//TestObject(TestObjectParams const &p, string name);
	TestObject(string name);
	virtual ~TestObject();
	HRESULT VOnRestore(Scene *pScene);
	HRESULT VRender(Scene *pScene);

	//	TriangleIterator *CreateTriangleIterator();

	static WORD g_TestObjectIndices[][3];
	static glm::vec3 g_CubeVerts[];
	static glm::vec3 g_SquashedCubeVerts[];
};
*/


#endif