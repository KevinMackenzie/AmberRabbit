#ifndef QSE_LIGHTS_HPP
#define QSE_LIGHTS_HPP


#include "GLRenderer.hpp"
#include "Geometry.hpp"
#include "Scene.hpp"
#include "SceneNode.hpp"

// Note: Light color is stored in the GLMaterial structure, which is already present in all SceneNodes.

//
// struct LightProperties				- Chapter 16, page 551
//
struct LightProperties
{
	float	m_Attenuation[3];  /* Attenuation coefficients */
	float	m_Range;
	float	m_Falloff;
	float	m_Theta;
	float	m_Phi;
};


//
// class LightNode						- Chapter 16, page 551
//
//    Note: In the book this class implements the LightNode in D3D11, but here it is a base
//    class. The derived classes make it possible to run the engine in D3D9 or D3D11.
//
class LightNode : public SceneNode
{
protected:
	LightProperties m_LightProps;

public:
	LightNode(const ActorId actorId, WeakBaseRenderComponentPtr renderComponent, const LightProperties &props, const glm::mat4 *t);
};


class GLLightNode : public LightNode
{
public:
	GLLightNode(const ActorId actorId, WeakBaseRenderComponentPtr renderComponent, const LightProperties &lightProps, const glm::mat4 *t)
		: LightNode(actorId, renderComponent, lightProps, t) { }

	virtual HRESULT VOnRestore() { return S_OK; };
	virtual HRESULT VOnUpdate(Scene *, double const fEllapsed);
};


struct ConstantBuffer_Lighting;


//
// class LightManager					- Chapter 16, 553
//
class LightManager
{
	friend class Scene;

protected:
	Lights					m_Lights;
	glm::vec4					m_vLightDir[MAXIMUM_LIGHTS_SUPPORTED];
	Color					m_vLightDiffuse[MAXIMUM_LIGHTS_SUPPORTED];
	glm::vec4					m_vLightAmbient;
public:
	void CalcLighting(Scene *pScene);
	void CalcLighting(ConstantBuffer_Lighting* pLighting, SceneNode *pNode);
	int GetLightCount(const SceneNode *node) { return m_Lights.size(); }
	const glm::vec4 *GetLightAmbient(const SceneNode *node) { return &m_vLightAmbient; }
	const glm::vec4 *GetLightDirection(const SceneNode *node) { return m_vLightDir; }
	const Color *GetLightDiffuse(const SceneNode *node) { return m_vLightDiffuse; }
};


#endif