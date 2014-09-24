#include "../Stdafx.hpp"
#include "SceneNode.hpp"


#include "../Actor/ActorComponent.hpp"
#include "../Actor/RenderComponent.hpp"
#include "../Actor/TransformComponent.hpp"
#include "../Utilities/Templates.hpp"

#include "GLRenderer.hpp"
#include "Geometry.hpp"
#include "Lights.hpp"
#include "Mesh.hpp"
#include "Raycast.hpp"
#include "SceneNode.hpp"
#include "tchar.h"
#include "../ResourceCache/ResCache.hpp"
#include "Scene.hpp"


shared_ptr<ResHandle> GLGrid::m_pGridProgram(nullptr);

////////////////////////////////////////////////////
// SceneNodeProperties Implementation
////////////////////////////////////////////////////

SceneNodeProperties::SceneNodeProperties(void)
{
	m_ActorId = INVALID_ACTOR_ID;
	m_Radius = 0;
	m_RenderPass = RenderPass_0;
	m_AlphaType = AlphaOpaque;

	m_Material = QuicksandEngine::g_pApp->m_ResCache->CreateDummy<GLMaterialResourceExtraData>();
}


//
// SceneNodeProperties::Transform			- Chapter 16, page 528
//
void SceneNodeProperties::Transform(glm::mat4 *toWorld, glm::mat4 *fromWorld) const
{
	if (toWorld)
		*toWorld = m_ToWorld;

	if (fromWorld)
		*fromWorld = m_FromWorld;
}


////////////////////////////////////////////////////
// SceneNode Implementation
////////////////////////////////////////////////////

SceneNode::SceneNode(ActorId actorId, WeakBaseRenderComponentPtr renderComponent, RenderPass renderPass, const glm::mat4 *to, const glm::mat4 *from)
{
	m_pParent = NULL;
	m_Props.m_ActorId = actorId;
	m_Props.m_Name = (renderComponent) ? renderComponent->VGetName() : "SceneNode";
	m_Props.m_RenderPass = renderPass;
	m_Props.m_AlphaType = AlphaOpaque;
	m_RenderComponent = renderComponent;
	VSetTransform(to, from);
	SetRadius(0);

	// [mrmike] - these lines were moved to VOnRestore() post press
	//Color color = (renderComponent) ? renderComponent->GetColor() : g_White;
	//m_Props.m_Material.SetDiffuse(color);
}


SceneNode::~SceneNode()
{
}

//
// SceneNode::VOnRestore					- Chapter 16, page 532
//
HRESULT SceneNode::VOnRestore(Scene *pScene)
{
	Color color = (m_RenderComponent) ? m_RenderComponent->GetColor() : g_White;
	m_Props.GetMaterial()->SetDiffuse(color);

	// This is meant to be called from any class
	// that inherits from SceneNode and overloads
	// VOnRestore()

	SceneNodeList::iterator i = m_Children.begin();
	SceneNodeList::iterator end = m_Children.end();
	while (i != end)
	{
		(*i)->VOnRestore(pScene);
		++i;
	}
	return S_OK;
}


//
// SceneNode::VOnLostDevice					- Chapter X, page Y
//
HRESULT SceneNode::VOnLostDevice(Scene *pScene)
{
	// This is meant to be called from any class
	// that inherits from SceneNode and overloads
	// VOnRestore()

	SceneNodeList::iterator i = m_Children.begin();
	SceneNodeList::iterator end = m_Children.end();
	while (i != end)
	{
		(*i)->VOnLostDevice(pScene);
		++i;
	}
	return S_OK;
}

//
// SceneNode::VSetTransform					- Chapter 16, page 531
//
void SceneNode::VSetTransform(const glm::mat4 *toWorld, const glm::mat4 *fromWorld)
{
	m_Props.m_ToWorld = *toWorld;
	if (!fromWorld)
	{
		m_Props.m_FromWorld = glm::inverse(m_Props.m_ToWorld);
	}
	else
	{
		m_Props.m_FromWorld = *fromWorld;
	}
}

shared_ptr<ResHandle> SceneNode::GetShader()
{
	if (!m_pShader)
		return m_pParent->GetShader();
	else
		return m_pShader;
}

//
// SceneNode::VPreRender					- Chapter 16, page 532
//
HRESULT SceneNode::VPreRender(Scene *pScene)
{
	//SetPosition(glm::vec3(0, 0, -50));
	pScene->PushAndSetMatrix(m_Props.m_ToWorld);


	// This was added post press! Is is always ok to read directly from the game logic.
	StrongActorPtr pActor = MakeStrongPtr(QuicksandEngine::g_pApp->GetGameLogic()->VGetActor(m_Props.m_ActorId));
	if (pActor)
	{
		shared_ptr<TransformComponent> pTc = MakeStrongPtr(pActor->GetComponent<TransformComponent>(TransformComponent::g_Name));
		if (pTc)
		{
			m_Props.m_ToWorld = pTc->GetTransform();
		}
	}

	//set the shader in this method, because this will render all children who have this as null
	
	GLUFProgramPtr prog = static_pointer_cast<GLProgramResourceExtraData>(GetShader()->GetExtra())->GetProgram();
	//if (m_pShader)
	//{
		//this is always safe to do
		GLUFSHADERMANAGER.UseProgram(prog);
	//}


	//apply the uniforms
	glm::mat4 model = pScene->GetTopMatrix();
	glm::mat4 view = pScene->GetCamera()->GetView();
	glm::mat4 proj = pScene->GetCamera()->GetProjection();

	glm::mat4 model_view = view * model;
	glm::mat4 model_view_proj = proj * model_view;

	GLUFVariableLocMap uniformLocations = GLUFSHADERMANAGER.GetShaderUniformLocations(prog);

	GLUFVariableLocMap::iterator it;
	
	it = uniformLocations.find("_model");
	if (it != uniformLocations.end())
		glUniformMatrix4fv(it->second, 1, GL_FALSE, &model[0][0]);

	it = uniformLocations.find("_view");
	if (it != uniformLocations.end())
		glUniformMatrix4fv(it->second, 1, GL_FALSE, &view[0][0]);
	
	it = uniformLocations.find("_proj");
	if (it != uniformLocations.end())
		glUniformMatrix4fv(it->second, 1, GL_FALSE, &proj[0][0]);
	
	it = uniformLocations.find("_mv");
	if (it != uniformLocations.end())
		glUniformMatrix4fv(it->second, 1, GL_FALSE, &model_view[0][0]);
	
	it = uniformLocations.find("_mvp");
	if (it != uniformLocations.end())
		glUniformMatrix4fv(it->second, 1, GL_FALSE, &model_view_proj[0][0]);


	//these are the material uniforms
	shared_ptr<GLMaterialResourceExtraData> mat = m_Props.GetMaterial();

	Color3f tmpColor;

	it = uniformLocations.find("m_diff");
	if (it != uniformLocations.end())
	{
		tmpColor = GLUFColorToFloat3(mat->GetDiffuse());
		glUniform3fv(it->second, 1, &tmpColor[0]);
	}
	it = uniformLocations.find("m_amb");
	if (it != uniformLocations.end())
	{
		tmpColor = GLUFColorToFloat3(mat->GetAmbient());
		glUniform3fv(it->second, 1, &tmpColor[0]);
	}

	Color specular;
	GLfloat power;
	mat->GetSpecular(specular, power);

	it = uniformLocations.find("m_spec");
	if (it != uniformLocations.end())
	{
		tmpColor = GLUFColorToFloat3(specular);
		glUniform3fv(it->second, 1, &tmpColor[0]);
	}
	it = uniformLocations.find("m_pow");
	if (it != uniformLocations.end())
		glUniform1f(it->second, power);

	it = uniformLocations.find("m_tex0");
	if (it != uniformLocations.end() && mat->GetTexture())//make sure the there is a sampler, and we have a texture
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, static_pointer_cast<GLTextureResourceExtraData>(mat->GetTexture()->GetExtra())->GetTexture());
		glUniform1i(it->second, 0);
	}
	else
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	return S_OK;
}

//
// SceneNode::VPostRender					- Chapter 16, page 532
//
HRESULT SceneNode::VPostRender(Scene *pScene)
{
	pScene->PopMatrix();
	return S_OK;
}



//
// SceneNode::VIsVisible				- Chapter 16, page 533
//	
bool SceneNode::VIsVisible(Scene *pScene) const
{
	// transform the location of this node into the camera space 
	// of the camera attached to the scene

	//TODO: get this to work properly, but for now render everything

	/*glm::mat4 toWorld, fromWorld;
	pScene->GetCamera()->VGet()->Transform(&toWorld, &fromWorld);

	glm::vec3 pos = GetWorldPosition();

	glm::vec3 fromWorldPos = Xform(fromWorld, pos);

	Frustum const &frustum = pScene->GetCamera()->GetFrustum();

	bool isVisible = frustum.Inside(fromWorldPos, VGet()->Radius());
	return isVisible;*/
	return true;
}

HRESULT SceneNode::VRender(Scene* pScene)
{
	return S_OK;
}

//
// SceneNode::GetWorldPosition			- not described in the book
//
//   This was added post press to respect any SceneNode ancestors - you have to add all 
//   their positions together to get the world position of any SceneNode.
//
const glm::vec3 SceneNode::GetWorldPosition() const
{
	glm::vec3 pos = GetPosition();
	if (m_pParent)
	{
		pos += m_pParent->GetWorldPosition();
	}
	return pos;
}


//
// SceneNode::VOnUpdate					- Chapter 16, page 532
//
HRESULT SceneNode::VOnUpdate(Scene *pScene, double const fEllapsed)
{
	// This is meant to be called from any class
	// that inherits from SceneNode and overloads
	// VOnUpdate()

	SceneNodeList::iterator i = m_Children.begin();
	SceneNodeList::iterator end = m_Children.end();

	while (i != end)
	{
		(*i)->VOnUpdate(pScene, fEllapsed);
		++i;
	}
	return S_OK;
}

//
// SceneNode::VRenderChildren					- Chapter 16, page 533
//
HRESULT SceneNode::VRenderChildren(Scene *pScene)
{
	// Iterate through the children....
	SceneNodeList::iterator i = m_Children.begin();
	SceneNodeList::iterator end = m_Children.end();

	GLUFProgramPtr prog = static_pointer_cast<GLProgramResourceExtraData>(GetShader()->GetExtra())->GetProgram();
	while (i != end)
	{
		//make sure to set this back to this program
		if (m_pShader)
		{
			GLUFSHADERMANAGER.UseProgram(prog);
		}

		if ((*i)->VPreRender(pScene) == S_OK)
		{
			// You could short-circuit rendering
			// if an object returns E_FAIL from
			// VPreRender()

			// Don't render this node if you can't see it
			if ((*i)->VIsVisible(pScene))
			{
				float alpha = (*i)->VGet()->GetMaterial()->GetAlpha();
				if (alpha == u8OPAQUE)
				{
					(*i)->VRender(pScene);
				}
				else if (alpha != u8TRANSPARENT)
				{
					// The object isn't totally transparent...
					AlphaSceneNode *asn = QSE_NEW AlphaSceneNode;
					LOG_ASSERT(asn);
					asn->m_pNode = *i;
					asn->m_Concat = pScene->GetTopMatrix();

					glm::vec4 worldPos(::GetPosition(asn->m_Concat), 1.0f /*TODO: is this correct*/);

					glm::mat4 fromWorld = pScene->GetCamera()->VGet()->FromWorld();

					glm::vec4 screenPos = glm::vec4(Xform(fromWorld, glm::vec3(worldPos)), 1.0f/*TODO: same as above*/);

					asn->m_ScreenZ = screenPos.z;

					pScene->AddAlphaSceneNode(asn);
				}

				// [mrmike] see comment just below...
				(*i)->VRenderChildren(pScene);
			}

			// [mrmike] post-press fix - if the parent is not visible, the childrend
			//           shouldn't be visible either.
			//(*i)->VRenderChildren(pScene);
		}
		(*i)->VPostRender(pScene);
		++i;
	}

	return S_OK;
}


//
// SceneNode::VAddChild					- Chapter 16, page 535
//
bool SceneNode::VAddChild(shared_ptr<ISceneNode> ikid)
{
	m_Children.push_back(ikid);

	shared_ptr<SceneNode> kid = static_pointer_cast<SceneNode>(ikid);

	kid->m_pParent = this;					// [mrmike] Post-press fix - the parent was never set!

	// The radius of the sphere should be fixed right here
	glm::vec3 kidPos = ::GetPosition(kid->VGet()->ToWorld());

	// [mrmike] - Post-press fix. This was not correct! subtracting the parents's position from the kidPos
	//            created a HUGE radius, depending on the location of the parent, which could be anywhere
	//            in the game world.

	//glm::vec3 dir = kidPos - m_Props.ToWorld().GetPosition();
	//float newRadius = dir.Length() + kid->VGet()->Radius();

	float newRadius = glm::length(kidPos) + kid->VGet()->Radius();

	if (newRadius > m_Props.m_Radius)
		m_Props.m_Radius = newRadius;

	return true;
}

//
// SceneNode::VRemoveChild					- not in the book
//
//   If an actor is destroyed it should be removed from the scene graph.
//   Generally the HumanView will recieve a message saying the actor has been
//   destroyed, and it will then call Scene::RemoveChild which will traverse 
//   the scene graph to find the child that needs removing.
//
bool SceneNode::VRemoveChild(ActorId id)
{
	for (SceneNodeList::iterator i = m_Children.begin(); i != m_Children.end(); ++i)
	{
		const SceneNodeProperties* pProps = (*i)->VGet();
		if (pProps->ActorId() != INVALID_ACTOR_ID && id == pProps->ActorId())
		{
			i = m_Children.erase(i);	//this can be expensive for vectors
			return true;
		}
	}
	return false;
}



HRESULT SceneNode::VPick(Scene *pScene, RayCast *raycast)
{
	for (SceneNodeList::const_iterator i = m_Children.begin(); i != m_Children.end(); ++i)
	{
		HRESULT hr = (*i)->VPick(pScene, raycast);

		if (hr == E_FAIL)
			return E_FAIL;
	}

	return S_OK;
}

// This was changed post press - it was convenient to be able to set alpha on a parent node and
// have it propagate to all its children, since materials are set in the SceneNodes's properties, and
// not as a SceneNode that changes renderstate by itself.
void SceneNode::SetAlpha(float alpha)
{
	m_Props.SetAlpha((glm::u8)(alpha * 255.0f));
	for (SceneNodeList::const_iterator i = m_Children.begin(); i != m_Children.end(); ++i)
	{
		shared_ptr<SceneNode> sceneNode = static_pointer_cast<SceneNode>(*i);
		sceneNode->SetAlpha(alpha);
	}
}

void SceneNode::SetShader(shared_ptr<ResHandle> pShader)
{
	m_pShader = pShader;
}


////////////////////////////////////////////////////
// RootNode Implementation
////////////////////////////////////////////////////

//
// RootNode::RootNode					- Chapter 16, page 545
//
RootNode::RootNode()
	: SceneNode(INVALID_ACTOR_ID, WeakBaseRenderComponentPtr(), RenderPass_0, &glm::mat4())
{
	m_Children.reserve(RenderPass_Last);

	//make sure there is always something to fall back on.
	Resource res("Shaders\\DefaultShading.prog");
	shared_ptr<ResHandle> defaultShading = QuicksandEngine::g_pApp->m_ResCache->GetHandle(&res);
	SetShader(defaultShading);

	shared_ptr<SceneNode> staticGroup(QSE_NEW SceneNode(INVALID_ACTOR_ID, WeakBaseRenderComponentPtr(), RenderPass_Static, &glm::mat4()));
	//m_Children.push_back(staticGroup);	// RenderPass_Static = 0
	SceneNode::VAddChild(staticGroup);//[Kevin] This was not here, but for the shading to work correctly, these have to have a parent

	shared_ptr<SceneNode> actorGroup(QSE_NEW SceneNode(INVALID_ACTOR_ID, WeakBaseRenderComponentPtr(), RenderPass_Actor, &glm::mat4()));
	//m_Children.push_back(actorGroup);	// RenderPass_Actor = 1
	SceneNode::VAddChild(actorGroup);

	shared_ptr<SceneNode> skyGroup(QSE_NEW SceneNode(INVALID_ACTOR_ID, WeakBaseRenderComponentPtr(), RenderPass_Sky, &glm::mat4()));
	//m_Children.push_back(skyGroup);	// RenderPass_Sky = 2
	SceneNode::VAddChild(skyGroup);

	shared_ptr<SceneNode> invisibleGroup(QSE_NEW SceneNode(INVALID_ACTOR_ID, WeakBaseRenderComponentPtr(), RenderPass_NotRendered, &glm::mat4()));
	//m_Children.push_back(invisibleGroup);	// RenderPass_NotRendered = 3
	SceneNode::VAddChild(invisibleGroup);
}

//
// RootNode::VAddChild					- Chapter 16, page 546
//
bool RootNode::VAddChild(shared_ptr<ISceneNode> kid)
{
	// The Root node has children that divide the scene graph into render passes.
	// Scene nodes will get added to these children based on the value of the
	// render pass member variable.

	RenderPass pass = kid->VGet()->RenderPass();
	if ((unsigned)pass > m_Children.size() || !m_Children[pass])
	{
		LOG_ASSERT(0 && _T("There is no such render pass"));
		return false;
	}

	return m_Children[pass]->VAddChild(kid);
}

//
// RootNode::VRemoveChild						- not described in the book 
//
//   Returns false if nothing was removed
//
bool RootNode::VRemoveChild(ActorId id)
{
	bool anythingRemoved = false;
	for (int i = RenderPass_0; i<RenderPass_Last; ++i)
	{
		if (m_Children[i]->VRemoveChild(id))
		{
			anythingRemoved = true;
		}
	}
	return anythingRemoved;
}

//
// RootNode::VRenderChildren					- Chapter 16, page 547
//
HRESULT RootNode::VRenderChildren(Scene *pScene)
{
	// This code creates fine control of the render passes.

	for (int pass = RenderPass_0; pass < RenderPass_Last; ++pass)
	{
		switch (pass)
		{
		case RenderPass_Static:
		case RenderPass_Actor:
			m_Children[pass]->VRenderChildren(pScene);
			break;

		case RenderPass_Sky:
		{
			shared_ptr<IRenderState> skyPass = pScene->GetRenderer()->VPrepareSkyBoxPass();
			m_Children[pass]->VRenderChildren(pScene);
			break;
		}
		}
	}

	return S_OK;
}


////////////////////////////////////////////////////
// CameraNode Implementation
////////////////////////////////////////////////////


//
// CameraNode::VRender					- Chapter 16, page 550
//
HRESULT CameraNode::VRender(Scene *pScene)
{
	if (m_DebugCamera)
	{
		pScene->PopMatrix();

		m_Frustum.Render();

		pScene->PushAndSetMatrix(m_Props.ToWorld());
	}

	return S_OK;
}

//
// CameraNode::VOnRestore				- Chapter 16, page 550
//
HRESULT CameraNode::VOnRestore(Scene *pScene)
{
	int windowHeight, windowWidth;
	glfwGetWindowSize(QuicksandEngine::g_pApp->GLFWWindow(), &windowWidth, &windowHeight);
	m_Frustum.SetAspect((FLOAT)windowWidth / (FLOAT)windowHeight);
	m_Projection = glm::perspective(m_Frustum.m_Fov, m_Frustum.m_Aspect, m_Frustum.m_Near, m_Frustum.m_Far);
	//pScene->GetRenderer()->VSetProjectionTransform(&m_Projection);
	return S_OK;
}

//
// CameraNode::SetView					- Chapter 16, page 550
//
//    Note: this is incorrectly called CameraNode::SetView in the book
//
HRESULT CameraNode::SetViewTransform(Scene *pScene)
{
	//If there is a target, make sure the camera is
	//rigidly attached right behind the target
	if (m_pTarget)
	{
		glm::mat4 mat = m_pTarget->VGet()->ToWorld();
		glm::vec4 at = m_CamOffsetVector;
		glm::vec4 atWorld = Xform(mat,at);
		glm::vec3 pos = ::GetPosition(mat) + glm::vec3(atWorld);
		::SetPosition(mat, pos);
		VSetTransform(&mat);
	}

	m_View = VGet()->FromWorld();

	//pScene->GetRenderer()->VSetViewTransform(&m_View);
	return S_OK;
}

//
// CameraNode::GetWorldViewProjection			- not described in the book
//
//    Returns the concatenation of the world and view projection, which is generally sent into vertex shaders
//
glm::mat4 CameraNode::GetWorldViewProjection(Scene *pScene)
{
	glm::mat4 world = pScene->GetTopMatrix();
	glm::mat4 view = VGet()->FromWorld();
	glm::mat4 worldView = world * view;
	return worldView * m_Projection;
}



////////////////////////////////////////////////////
// GLGrid Implementation
//
// D3D9 classes are not covered in the 4th edition. They are included here for those 
// people that don't have D3D11 compatible video cards. 
//
// The code is somewhat similar to the D3DGrid11 class, which IS discussed in Chapter 16 
// of the 4th edition.
// 
////////////////////////////////////////////////////

//
// GLGrid::GLGrid						- 3rd Edition, Chapter 13, page 448
//

GLGrid::GLGrid(ActorId actorId, WeakBaseRenderComponentPtr renderComponent, int squares, float squareSize, const Color &diffuseColor, const glm::mat4* pMatrix)
	: SceneNode(actorId, renderComponent, RenderPass_Static, pMatrix), m_Squares(GL_LINES, GL_DYNAMIC_DRAW)//, m_nSideLength(squares), m_fSquareLength(squareSize)
{

	shared_ptr<GLMaterialResourceExtraData> material = m_Props.GetMaterial();
	material->SetDiffuse(diffuseColor);

	//m_bTextureHasAlpha = false;
	//m_pVerts = NULL;
	//m_pIndices = NULL;
	//m_VertexArray = GLUFVertexArrayPtr(new GLUFVertexArrayPtr());
	//m_VertexArray = GLUFBUFFERMANAGER.CreateVertexArray();

	if (!m_pGridProgram)
	{
		Resource* gridProg = new Resource("Shaders\\Lines.prog");
		m_pGridProgram = QuicksandEngine::g_pApp->m_ResCache->GetHandle(gridProg);
	}

	//use the predefined grid shader

	SceneNode::SetShader(m_pGridProgram);

	GLUFVariableLocMap varLocations = GLUFSHADERMANAGER.GetShaderAttribLocations(static_pointer_cast<GLProgramResourceExtraData>(m_pShader->GetExtra())->GetProgram());

	GLUFVariableLocMap::iterator it = varLocations.find("_position");

	if (it != varLocations.end())
		m_Squares.AddVertexAttrib(GLUFVertAttrib(it->second, 4, 3, GL_FLOAT));
	else
		LOG_ERROR("Failed to Load GLGrid Attribute Location");
}

//
// GLGrid::~GLGrid						-  3rd Edition, Chapter 13, page 449
//
GLGrid::~GLGrid()
{
	//SAFE_RELEASE(m_pVerts);
	//SAFE_RELEASE(m_pIndices);
}

//
// GLGrid::VOnRestore						- 3rd Edition, Chapter 13, page 449
//   Note: the book incorrectly names this method Grid::Create
//
//	 Here's a list of changes that were made from the 3rd Edition's code:
//      1. The resource cache is used to hold the texture file.
//		2. The texture file isn't hard coded - it is stored in a member variable
//		3. SetRadius() is called with the spherical dimensions of this grid to aid culling
//		4. The input parameter was changed to a platform independent Scene object
//
HRESULT GLGrid::VOnRestore(Scene *pScene)
{
	// Call the base class's restore
	SceneNode::VOnRestore(pScene);

	GridRenderComponent* grc = static_cast<GridRenderComponent*>(m_RenderComponent);

	//read everything from the game logic
	int squares = grc->GetDivision();
	int sideLen = (int)sqrt(squares);
	float unitLen = grc->GetUnitLength();

	GLuint positionLocation = 0;
	{
		shared_ptr<GLProgramResourceExtraData> progExtra = static_pointer_cast<GLProgramResourceExtraData>(GetShader()->GetExtra());
		GLUFVariableLocMap shadLocs = GLUFSHADERMANAGER.GetShaderAttribLocations(progExtra->GetProgram());
		GLUFVariableLocMap::iterator shadLocIt = shadLocs.find("_position");

		if (shadLocIt == shadLocs.end())
		{
			GLUF_ERROR("Corrupt or invalid shader for grid");
			return E_FAIL;
		}

		positionLocation = shadLocIt->second;
	}
	//SAFE_RELEASE(m_pVerts);
	//SAFE_RELEASE(m_pIndices);

	SetRadius(sqrt(squares * squares / 2.0f));

	// Create the vertex buffer - we'll need enough verts
	// to populate the grid. If we want a 2x2 grid, we'll
	// need 3x3 set of verts.
	//m_numVerts = (squares + 1)*(squares + 1);

	/*
	if (FAILED(DXUTGetD3D9Device()->CreateVertexBuffer(
		m_numVerts*sizeof(D3D9Vertex_ColoredTextured),
		D3DUSAGE_WRITEONLY, D3D9Vertex_ColoredTextured::FVF,
		D3DPOOL_MANAGED, &m_pVerts, NULL)))
	{
		return E_FAIL;
	}*/

	// Fill the vertex buffer. We are setting the tu and tv texture
	// coordinates, which range from 0.0 to 1.0
	//D3D9Vertex_ColoredTextured* pVertices;
	//if (FAILED(m_pVerts->Lock(0, 0, (void**)&pVertices, 0)))
	//	return E_FAIL;

	//GLUFVAOData dat = GLUFBUFFERMANAGER.MapVertexArray(m_VertexArray);
	
	//get the initial x/z offset of the vertices to center the grid on the position
	float fInitial = -(sideLen * unitLen) / 2.0f;

	//this takes one row of the grid, and then expands by the length
	std::vector<glm::vec3> baseVertices;
	std::vector<glm::vec3> vertices;

	//yes, i do want it to go one past, because there is one more set of vertices than sides
	for (int i = 0; i <= sideLen; ++i)
	{
		baseVertices.push_back(glm::vec3(fInitial, 0.0f, fInitial + i * unitLen));
	}

	//fill the vertex array by 'extruding' this line
	for (int i = 0; i <= sideLen; ++i)
	{
		for (auto bv : baseVertices)
		{
			vertices.push_back(bv + glm::vec3(i * unitLen, 0.0f, 0.0f));
		}
	}

	//two vertices per line.
	std::vector<glm::u32vec2> Lines;

	//lines going in the z direction
	for (int i = 0; i <= sideLen; ++i)
	{
		for (int j = 0; j < sideLen; ++j)
		{
			Lines.push_back(glm::u32vec2(j + (i * (sideLen + 1)), j + (i * (sideLen + 1)) + 1));
		}
	}

	//lines going in the x direction
	for (int i = 0; i <= sideLen; ++i)
	{
		for (int j = 0; j < sideLen; ++j)
		{
			//remember to add the stride when doing it this way
			Lines.push_back(glm::u32vec2(j * (sideLen + 1) + i, (j + 1) * (sideLen + 1) + i));
		}
	}

	
	//center this
	//glm::vec3 pos = GetPosition();
	//this->SetPosition(pos - glm::vec3((unitLen * sideLen) / 2, 0.0f, (unitLen * sideLen) / 2));


	//buffer the data
	m_Squares.BufferData(positionLocation, vertices.size(), &vertices[0][0]);

	m_Squares.BufferIndices(Lines);//I cought a sneaky bug here where I forgot to multiply the size of the line buffer by 2

	return S_OK;
}

//
// GLGrid::VRender					- 3rd Edition, Chapter 13, page 451
//   Note: the book incorrectly names this method Grid::Render
// 
//   This function also handles alpha textures on the Grid, where the book's
//   code does not. 
//
HRESULT GLGrid::VRender(Scene *pScene)
{

	// Setup our texture. Using textures introduces the texture stage states,
	// which govern how textures get blended together (in the case of multiple
	// textures) and lighting information. In this case, we are modulating
	// (blending) our texture with the diffuse color of the vertices.

	/*GridRenderComponent* grc = static_cast<GridRenderComponent*>(m_RenderComponent);

	Resource resource(grc->GetTextureResource());
	shared_ptr<ResHandle> texture = QuicksandEngine::g_pApp->m_ResCache->GetHandle(&resource);
	shared_ptr<GLTextureResourceExtraData> extra = static_pointer_cast<GLTextureResourceExtraData>(texture->GetExtra());
	//DXUTGetD3D9Device()->SetTexture(0, extra->GetTexture());
	GLUFBUFFERMANAGER.UseTexture(extra->GetTexture(), 0, GL_TEXTURE0);*/
	/*
	if (m_bTextureHasAlpha)
	{
		//DXUTGetD3D9Device()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		//DXUTGetD3D9Device()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

		//DXUTGetD3D9Device()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

		DXUTGetD3D9Device()->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		DXUTGetD3D9Device()->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		DXUTGetD3D9Device()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	}
	else
	{
		DXUTGetD3D9Device()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		DXUTGetD3D9Device()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		DXUTGetD3D9Device()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	}*/

	//DXUTGetD3D9Device()->SetStreamSource(0, m_pVerts, 0, sizeof(D3D9Vertex_ColoredTextured));
	//DXUTGetD3D9Device()->SetIndices(m_pIndices);
	//DXUTGetD3D9Device()->SetFVF(D3D9Vertex_ColoredTextured::FVF);
	//DXUTGetD3D9Device()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_numVerts, 0, m_numPolys);

	// Notice that the render states are returned to 
	// their original settings.....
	// Could there be a better way???

	//DXUTGetD3D9Device()->SetTexture(0, NULL);
	//DXUTGetD3D9Device()->SetRenderState(D3DRS_LIGHTING, oldLightMode);
	//DXUTGetD3D9Device()->SetRenderState(D3DRS_CULLMODE, oldCullMode);
	
	/*float ratio = QuicksandEngine::g_pApp->GetScreenSize().x / QuicksandEngine::g_pApp->GetScreenSize().y;
	glm::mat4 view = pScene->GetCamera()->GetView();
	glm::mat4 proj = pScene->GetCamera()->GetProjection();
	//glm::mat4 model = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -10.0f)) * glm::toMat4(glm::quat(glm::vec3(GLUF_PI_F / 2, 0.0f, 0.0f)));
	glm::vec3 eulers(90, 0, 0);
	glm::quat tmpQuat = glm::quat(glm::vec3((float)DEGREES_TO_RADIANS(eulers.x), (float)DEGREES_TO_RADIANS(eulers.y), (float)DEGREES_TO_RADIANS(eulers.z)));
	glm::mat4 model = glm::toMat4(tmpQuat);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -10.0f));
	glm::mat4 MVP = proj * view * model;*/
	//glUniformMatrix4fv(0, 1, GL_FALSE, &MVP[0][0]);

	//this is GREATLY simplified
	m_Squares.Draw();


	return S_OK;
}


HRESULT GLGrid::VPick(Scene *pScene, RayCast *pRayCast)
{
	if (SceneNode::VPick(pScene, pRayCast) == E_FAIL)
		return E_FAIL;

	/*pScene->PushAndSetMatrix(m_Props.ToWorld());


	ActorId id = m_Props.ActorId();
	HRESULT hr = pRayCast->Pick(pScene, id, *data.mPositions, *data.mIndicies);

	pScene->PopMatrix();*/

	return E_FAIL;
}




/*
GLGrid::GLGrid(ActorId actorId, WeakBaseRenderComponentPtr renderComponent, const glm::mat4* pMatrix)
	: SceneNode(actorId, renderComponent, RenderPass_0, pMatrix)
{
	m_bTextureHasAlpha = false;
	m_numVerts = m_numPolys = 0;
	m_VertexBuffer = NULL;
	m_IndexBuffer = NULL;
}

GLGrid::~GLGrid()
{
	glDeleteBuffers(1,&m_VertexBuffer);
	glDeleteBuffers(1, &m_IndexBuffer);
}



HRESULT GLGrid::VOnRestore(Scene *pScene)
{
	HRESULT hr;

	V_RETURN(SceneNode::VOnRestore(pScene));

	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);

	V_RETURN(m_VertexShader.OnRestore(pScene));
	V_RETURN(m_PixelShader.OnRestore(pScene));

	GridRenderComponent* grc = static_cast<GridRenderComponent*>(m_RenderComponent);

	int squares = grc->GetDivision();

	SetRadius(sqrt(squares * squares / 2.0f));

	// Create the vertex buffer - we'll need enough verts
	// to populate the grid. If we want a 2x2 grid, we'll
	// need 3x3 set of verts.
	m_numVerts = (squares + 1)*(squares + 1);    // Create vertex buffer

	// Fill the vertex buffer. We are setting the tu and tv texture
	// coordinates, which range from 0.0 to 1.0
	D3D11Vertex_UnlitTextured *pVerts = QSE_NEW D3D11Vertex_UnlitTextured[m_numVerts];
	LOG_ASSERT(pVerts && "Out of memory in D3DGrid11::VOnRestore()");
	if (!pVerts)
		return E_FAIL;

	for (int j = 0; j<(squares + 1); j++)
	{
		for (int i = 0; i<(squares + 1); i++)
		{
			// Which vertex are we setting?
			int index = i + (j * (squares + 1));
			D3D11Vertex_UnlitTextured *vert = &pVerts[index];

			// Default position of the grid is centered on the origin, flat on
			// the XZ plane.
			float x = (float)i - (squares / 2.0f);
			float y = (float)j - (squares / 2.0f);
			vert->Pos = glm::vec3(x, 0.f, y);
			vert->Normal = glm::vec3(0.0f, 1.0f, 0.0f);

			// The texture coordinates are set to x,y to make the
			// texture tile along with units - 1.0, 2.0, 3.0, etc.
			vert->Uv.x = x;
			vert->Uv.y = y;
		}
	}

	// The number of indicies equals the number of polygons times 3
	// since there are 3 indicies per polygon. Each grid square contains
	// two polygons. The indicies are 16 bit, since our grids won't
	// be that big!

	m_numPolys = squares * squares * 2;

	WORD *pIndices = QSE_NEW WORD[m_numPolys * 3];

	LOG_ASSERT(pIndices && "Out of memory in D3DGrid11::VOnRestore()");
	if (!pIndices)
		return E_FAIL;

	// Loop through the grid squares and calc the values
	// of each index. Each grid square has two triangles:
	//
	//		A - B
	//		| / |
	//		C - D

	WORD *current = pIndices;
	for (int j = 0; j<squares; j++)
	{
		for (int i = 0; i<squares; i++)
		{
			// Triangle #1  ACB
			*(current) = WORD(i + (j*(squares + 1)));
			*(current + 1) = WORD(i + ((j + 1)*(squares + 1)));
			*(current + 2) = WORD((i + 1) + (j*(squares + 1)));

			// Triangle #2  BCD
			*(current + 3) = WORD((i + 1) + (j*(squares + 1)));
			*(current + 4) = WORD(i + ((j + 1)*(squares + 1)));
			*(current + 5) = WORD((i + 1) + ((j + 1)*(squares + 1)));
			current += 6;
		}
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(D3D11Vertex_UnlitTextured) * m_numVerts;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = pVerts;
	hr = DXUTGetD3D11Device()->CreateBuffer(&bd, &InitData, &m_pVertexBuffer);
	if (SUCCEEDED(hr))
	{
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(WORD) * m_numPolys * 3;        // 36 vertices needed for 12 triangles in a triangle list
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		InitData.pSysMem = pIndices;
		hr = DXUTGetD3D11Device()->CreateBuffer(&bd, &InitData, &m_pIndexBuffer);
	}

	SAFE_DELETE_ARRAY(pVerts);
	SAFE_DELETE_ARRAY(pIndices);

	return hr;
}


HRESULT GLGrid::VRender(Scene *pScene)
{
	HRESULT hr;

	GridRenderComponent* grc = static_cast<GridRenderComponent*>(m_RenderComponent);
	m_PixelShader.SetTexture(grc->GetTextureResource());

	V_RETURN(m_VertexShader.SetupRender(pScene, this));
	V_RETURN(m_PixelShader.SetupRender(pScene, this));

	// Set vertex buffer
	UINT stride = sizeof(D3D11Vertex_UnlitTextured);
	UINT offset = 0;
	DXUTGetD3D11DeviceContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// Set index buffer
	DXUTGetD3D11DeviceContext()->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Set primitive topology
	DXUTGetD3D11DeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DXUTGetD3D11DeviceContext()->DrawIndexed(m_numPolys * 3, 0, 0);

	return S_OK;
}*/



////////////////////////////////////////////////////
// ArrowNode Implementation - added post press
////////////////////////////////////////////////////
/*
ArrowNode::ArrowNode(std::string name, WeakBaseRenderComponentPtr renderComponent, const float length, const glm::mat4 *t, const Color &diffuseColor)
	: SceneNode(INVALID_ACTOR_ID, renderComponent, RenderPass_0, t)
{
	D3DXCreateCylinder(DXUTGetD3D9Device(), 0.1f * length, 0.0f, 0.3f * length, 16, 1, &m_cone, NULL);
	D3DXCreateCylinder(DXUTGetD3D9Device(), 0.05f * length, 0.05f * length, 0.7f * length, 16, 2, &m_shaft, NULL);

	m_coneTrans.BuildTranslation(0, 0, (0.7f + 0.15f) * length);
	m_shaftTrans.BuildTranslation(0, 0, (0.35f * length));

	// The anchor point is at 0,0,0 - so the radius must incorporate the whole length.
	SetRadius(length);
}

HRESULT ArrowNode::VRender(Scene *pScene)
{
	if (S_OK != SceneNode::VRender(pScene))
		return E_FAIL;

	pScene->PushAndSetMatrix(m_shaftTrans);

	m_shaft->DrawSubset(0);
	pScene->PopMatrix();

	pScene->PushAndSetMatrix(m_coneTrans);
	m_cone->DrawSubset(0);

	pScene->PopMatrix();
	return S_OK;
}


HRESULT ArrowNode::VPick(Scene *pScene, RayCast *pRayCast)
{
	if (SceneNode::VPick(pScene, pRayCast) == E_FAIL)
		return E_FAIL;

	pScene->PushAndSetMatrix(m_shaftTrans);
	HRESULT hr = pRayCast->Pick(pScene, m_Props.ActorId(), m_shaft);
	pScene->PopMatrix();

	if (hr == E_FAIL)
		return hr;

	pScene->PushAndSetMatrix(m_coneTrans);
	hr = pRayCast->Pick(pScene, m_Props.ActorId(), m_cone);
	pScene->PopMatrix();

	return hr;
}


*/


////////////////////////////////////////////////////
// TestObject Implementation
//
//        - 3rd Edition, Chapter 14, page 494
////////////////////////////////////////////////////
/*
// FUTURE WORK: Fix me for D3D11
TestObject::TestObject(std::string name)
	: SceneNode(0, WeakBaseRenderComponentPtr(), RenderPass_0, NULL)
{
	//m_color = p.m_Color;
	m_color = 0xffffffff;

	m_pVerts = NULL;
	m_numVerts = m_numPolys = 0;
}


TestObject::~TestObject()
{
	SAFE_RELEASE(m_pVerts);
}


// TestObject::g_CubeVerts
//        - Chapter 14, page 495

glm::vec3 TestObject::g_CubeVerts[] =
{
	glm::vec3(0.5, 0.5, -0.5),		// Vertex 0.
	glm::vec3(-0.5, 0.5, -0.5),		// Vertex 1.
	glm::vec3(-0.5, 0.5, 0.5),  		// And so on.
	glm::vec3(0.5, 0.5, 0.5),
	glm::vec3(0.5, -0.5, -0.5),
	glm::vec3(-0.5, -0.5, -0.5),
	glm::vec3(-0.5, -0.5, 0.5),
	glm::vec3(0.5, -0.5, 0.5)
};


// TestObject::g_SquashedCubeVerts
//        - 3rd Edition, Chapter 14, page 495

glm::vec3 TestObject::g_SquashedCubeVerts[] =
{
	glm::vec3(0.5f, 0.5f, -0.25f),		// Vertex 0.
	glm::vec3(-0.5f, 0.5f, -0.25f),		// Vertex 1.
	glm::vec3(-0.5f, 0.5f, 0.5f),			// And so on.
	glm::vec3(0.75f, 0.5f, 0.5f),
	glm::vec3(0.75f, -0.5f, -0.5f),
	glm::vec3(-0.5f, -0.5f, -0.5f),
	glm::vec3(-0.5f, -0.3f, 0.5f),
	glm::vec3(0.5f, -0.3f, 0.5f)
};


// TestObject::g_TestObjectIndices
//        - 3rd Edition, Chapter 14, page 495

WORD TestObject::g_TestObjectIndices[][3] =
{
	{ 0, 1, 2 },                // Face 0 has three vertices.
	{ 0, 2, 3 },                // And so on.
	{ 0, 4, 5 },
	{ 0, 5, 1 },
	{ 1, 5, 6 },
	{ 1, 6, 2 },
	{ 2, 6, 7 },
	{ 2, 7, 3 },
	{ 3, 7, 4 },
	{ 3, 4, 0 },
	{ 4, 7, 6 },
	{ 4, 6, 5 }
};

// TestObject::VOnRestore
//        - 3rd Edition, Chapter 14, page 496

HRESULT TestObject::VOnRestore(Scene *pScene)
{
	// Call the base class's restore
	SceneNode::VOnRestore(pScene);

	glm::vec3 center;
	glm::vec3 *verts = m_squashed ? g_SquashedCubeVerts : g_CubeVerts;
	float radius;
	HRESULT hr = D3DXComputeBoundingSphere(
		static_cast<D3DXVECTOR3*>(verts), 8,
		D3DXGetFVFVertexSize(D3DFVF_XYZ),
		&center, &radius);

	SetRadius(radius);

	// Create the vertex buffer - this object is essentailly 
	// a squashed cube, but since we want each face to be flat shaded
	// each face needs its own set of verts - because each vert has a normal
	// and thus can't have any vert shared by adjacent faces.
	m_numPolys = 12;
	m_numVerts = m_numPolys * 3;

	SAFE_RELEASE(m_pVerts);
	if (FAILED(DXUTGetD3D9Device()->CreateVertexBuffer(
		m_numVerts*sizeof(D3D9Vertex_UnlitColored),
		D3DUSAGE_WRITEONLY, D3D9Vertex_UnlitColored::FVF,
		D3DPOOL_MANAGED, &m_pVerts, NULL)))
	{
		return E_FAIL;
	}

	// Fill the vertex buffer. We are setting the tu and tv texture
	// coordinates, which range from 0.0 to 1.0
	D3D9Vertex_UnlitColored* pVertices;
	if (FAILED(m_pVerts->Lock(0, 0, (void**)&pVertices, 0)))
		return E_FAIL;

	static Color colors[6] = { g_White, g_Gray65, g_Cyan, g_Red, g_Green, g_Blue };

	for (DWORD face = 0; face<m_numPolys; ++face)
	{
		D3D9Vertex_UnlitColored* v = &pVertices[face * 3];
		v->position = verts[g_TestObjectIndices[face][0]];
		v->diffuse = colors[face / 2];
		v->specular = colors[face / 2];
		(v + 1)->position = verts[g_TestObjectIndices[face][1]];
		(v + 1)->diffuse = colors[face / 2];
		(v + 1)->specular = colors[face / 2];
		(v + 2)->position = verts[g_TestObjectIndices[face][2]];
		(v + 2)->diffuse = colors[face / 2];
		(v + 2)->specular = colors[face / 2];

		glm::vec3 a = v->position - (v + 1)->position;
		glm::vec3 b = (v + 2)->position - (v + 1)->position;

		glm::vec3 cross = a.glm::cross(b);
		cross /= cross.Length();
		v->normal = cross;
		(v + 1)->normal = cross;
		(v + 2)->normal = cross;
	}

	m_pVerts->Unlock();

	return S_OK;
}

//
// TestObject::VOnRestore				- 3rd Edition, Chapter 14, page 497
//
HRESULT TestObject::VRender(Scene *pScene)
{
	if (S_OK != SceneNode::VRender(pScene))
		return E_FAIL;

	DXUTGetD3D9Device()->SetRenderState(D3DRS_LIGHTING, TRUE);
	DXUTGetD3D9Device()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	DXUTGetD3D9Device()->SetTexture(0, NULL);
	DXUTGetD3D9Device()->SetRenderState(D3DRS_COLORVERTEX, TRUE);
	DXUTGetD3D9Device()->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);

	// Setup our texture. Using textures introduces the texture stage states,
	// which govern how textures get blended together (in the case of multiple
	// textures) and lighting information. In this case, we are modulating
	// (blending) our texture with the diffuse color of the vertices.

	DXUTGetD3D9Device()->SetStreamSource(0, m_pVerts, 0, sizeof(D3D9Vertex_UnlitColored));
	DXUTGetD3D9Device()->SetFVF(D3D9Vertex_UnlitColored::FVF);

	DXUTGetD3D9Device()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 12);

	return S_OK;
}

*/