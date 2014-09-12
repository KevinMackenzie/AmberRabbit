#ifndef QSE_SCENE_HPP
#define QSE_SCENE_HPP


#include "Geometry.hpp"
#include "SceneNode.hpp"
#include "../EventManager/EventManager.hpp"

// Forward declarations
////////////////////////////////////////////////////
//
// SceneActorMap Description				- Chapter 16, page Y
//
//   An STL map that allows fast lookup of a scene node given an ActorId.
//
////////////////////////////////////////////////////

typedef std::map<ActorId, shared_ptr<ISceneNode> > SceneActorMap;


////////////////////////////////////////////////////
//
// Scene Description						- Chapter 16, page 536
//
// A heirarchical container of scene nodes, which
// are classes that implemente the ISceneNode interface
//
////////////////////////////////////////////////////

class CameraNode;
class SkyNode;
class LightNode;
class LightManager;

extern shared_ptr<GLUFMatrixStack> g_pMatrixStack;

class Scene
{
protected:
	shared_ptr<SceneNode>  	m_Root;
	shared_ptr<CameraNode> 	m_Camera;
	shared_ptr<IRenderer>	m_Renderer;

	shared_ptr<GLUFMatrixStack> m_MatrixStack;
	AlphaSceneNodes 		m_AlphaSceneNodes;
	SceneActorMap 			m_ActorMap;

	LightManager			*m_LightManager;

	void RenderAlphaPass();

public:
	Scene(shared_ptr<IRenderer> renderer);
	virtual ~Scene();

	HRESULT OnRender();
	HRESULT OnRestore();
	HRESULT OnLostDevice();
	HRESULT OnUpdate(const int deltaMilliseconds);
	shared_ptr<ISceneNode> FindActor(ActorId id);
	bool AddChild(ActorId id, shared_ptr<ISceneNode> kid);
	bool RemoveChild(ActorId id);

	// event delegates
	void NewRenderComponentDelegate(IEventDataPtr pEventData);
	void ModifiedRenderComponentDelegate(IEventDataPtr pEventData);			// added post-press!
	void DestroyActorDelegate(IEventDataPtr pEventData);
	void MoveActorDelegate(IEventDataPtr pEventData);

	void SetCamera(shared_ptr<CameraNode> camera) { m_Camera = camera; }
	const shared_ptr<CameraNode> GetCamera() const { return m_Camera; }


	void PushAndSetMatrix(const glm::mat4 &toWorld)
	{
		m_MatrixStack->Push(toWorld);
		glm::mat4 mat = GetTopMatrix();
		//m_Renderer->VSetWorldTransform(&mat);
	}

	void PopMatrix()
	{
		// Scene::PopMatrix - Chapter 16, page 541
		m_MatrixStack->Pop();
		glm::mat4 mat = GetTopMatrix();
		//m_Renderer->VSetWorldTransform(&mat);
	}

	const glm::mat4 GetTopMatrix()
	{
		return m_MatrixStack->Top();
	}

	LightManager *GetLightManager() { return m_LightManager; }

	void AddAlphaSceneNode(AlphaSceneNode *asn) { m_AlphaSceneNodes.push_back(asn); }

	HRESULT Pick(RayCast *pRayCast) { return m_Root->VPick(this, pRayCast); }

	shared_ptr<IRenderer> GetRenderer() { return m_Renderer; }
};




#endif