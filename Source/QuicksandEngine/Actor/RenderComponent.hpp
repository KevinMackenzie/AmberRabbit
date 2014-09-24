#ifndef QSE_RENDERCOMPONENT_HPP
#define QSE_RENDERCOMPONENT_HPP

#include "RenderComponentInterface.hpp"
#include "../Graphics3D/SceneNode.hpp"
#include "../Graphics3D/Lights.hpp"

//---------------------------------------------------------------------------------------------------------------------
// RenderComponent base class.  This class does most of the work except actually creating the scene, which is 
// delegated to the subclass through a factory method:
// http://en.wikipedia.org/wiki/Factory_method_pattern
//---------------------------------------------------------------------------------------------------------------------
class  BaseRenderComponent : public RenderComponentInterface
{
protected:
    Color m_color;
    shared_ptr<SceneNode> m_pSceneNode;

public:
    virtual bool VInit(tinyxml2::XMLElement* pData) override;
    virtual void VPostInit(void) override;
	virtual void VOnChanged(void) override;
    virtual tinyxml2::XMLElement* VGenerateXml(tinyxml2::XMLDocument* pDoc) override;
	const Color GetColor() const { return m_color; }

protected:
    // loads the SceneNode specific data (represented in the <SceneNode> tag)
    virtual bool VDelegateInit(tinyxml2::XMLElement* pData) { return true; }
    virtual shared_ptr<SceneNode> VCreateSceneNode(void) = 0;  // factory method to create the appropriate scene node
    Color LoadColor(tinyxml2::XMLElement* pData);

    // editor stuff
    virtual void VCreateInheritedXmlElements(tinyxml2::XMLElement* pBaseElement) = 0;

private:
    virtual shared_ptr<SceneNode> VGetSceneNode(void) override;
};


//---------------------------------------------------------------------------------------------------------------------
// This class represents a render component built from a Mesh.  In a real game, this is the one you'll use 99% of the 
// time towards the end of the project.  The other classes are important for testing since programming tends to move 
// a lot faster than art in the early stages of development.
//---------------------------------------------------------------------------------------------------------------------
class  MeshRenderComponent : public BaseRenderComponent
{
public:
	static const char *g_Name;
	virtual const char *VGetName() const { return g_Name; }

protected:
    virtual shared_ptr<SceneNode> VCreateSceneNode(void) override;  // factory method to create the appropriate scene node
    virtual void VCreateInheritedXmlElements(tinyxml2::XMLElement* pBaseElement);
};



//---------------------------------------------------------------------------------------------------------------------
// Spheres, which Teapot Wars uses as projectiles.
//---------------------------------------------------------------------------------------------------------------------
class  SphereRenderComponent : public BaseRenderComponent
{
    unsigned int m_segments;
	float m_radius;

public:
	static const char *g_Name;
	virtual const char *VGetName() const { return g_Name; }

    SphereRenderComponent(void);

protected:
    virtual bool VDelegateInit(tinyxml2::XMLElement* pData) override;
    virtual shared_ptr<SceneNode> VCreateSceneNode(void) override;  // factory method to create the appropriate scene node

    // editor stuff
    virtual void VCreateInheritedXmlElements(tinyxml2::XMLElement* pBaseElement);
};


//---------------------------------------------------------------------------------------------------------------------
// Teapots, which are the main characters in the game.  DirectX actually has a function that generates a teapot Mesh
// so we might as well use it.  We're game programmers, not artists.  ;)
//---------------------------------------------------------------------------------------------------------------------
class  TeapotRenderComponent : public BaseRenderComponent
{
public:
	static const char *g_Name;
	virtual const char *VGetName() const { return g_Name; }

protected:
    virtual shared_ptr<SceneNode> VCreateSceneNode(void) override;  // factory method to create the appropriate scene node

    // editor stuff
    virtual void VCreateInheritedXmlElements(tinyxml2::XMLElement* pBaseElement);
};


//---------------------------------------------------------------------------------------------------------------------
// Grids, which represent the world
//---------------------------------------------------------------------------------------------------------------------
class  GridRenderComponent : public BaseRenderComponent
{
    string m_textureResource;
    int m_squares;
	float m_fsquareLen;

public:
	static const char *g_Name;
	virtual const char *VGetName() const { return g_Name; }

    GridRenderComponent(void);
	const char* GetTextureResource() { return m_textureResource.c_str(); }
	const int GetDivision() { return m_squares; }
	const float GetUnitLength(){ return m_fsquareLen; }

protected:
    virtual bool VDelegateInit(tinyxml2::XMLElement* pData) override;
    virtual shared_ptr<SceneNode> VCreateSceneNode(void) override;  // factory method to create the appropriate scene node

    // editor stuff
    virtual void VCreateInheritedXmlElements(tinyxml2::XMLElement* pBaseElement);
};


//---------------------------------------------------------------------------------------------------------------------
// Lights
//---------------------------------------------------------------------------------------------------------------------
class  LightRenderComponent : public BaseRenderComponent
{
	LightProperties m_Props; 

public:
	static const char *g_Name;
	virtual const char *VGetName() const { return g_Name; }

    LightRenderComponent(void);

protected:
    virtual bool VDelegateInit(tinyxml2::XMLElement* pData) override;
    virtual shared_ptr<SceneNode> VCreateSceneNode(void) override;  // factory method to create the appropriate scene node

    // editor stuff
    virtual void VCreateInheritedXmlElements(tinyxml2::XMLElement* pBaseElement);
};


class  SkyRenderComponent : public BaseRenderComponent
{
	string m_textureResource;

public:
	static const char *g_Name;
	virtual const char *VGetName() const { return g_Name; }

    SkyRenderComponent(void);

protected:
    virtual bool VDelegateInit(tinyxml2::XMLElement* pData) override;
    virtual shared_ptr<SceneNode> VCreateSceneNode(void) override;  // factory method to create the appropriate scene node

    // editor stuff
    virtual void VCreateInheritedXmlElements(tinyxml2::XMLElement* pBaseElement);
};

#endif