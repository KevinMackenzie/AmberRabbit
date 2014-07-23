#include "../Stdafx.hpp"

#include "..\Graphics3D\Mesh.hpp"
#include "..\Graphics3D\Sky.hpp"

#include "../Utilities/String.hpp"

#include "../EventManager/Events.hpp"

#include "RenderComponent.hpp"
#include "TransformComponent.hpp"

const char* MeshRenderComponent::g_Name = "MeshRenderComponent";
const char* SphereRenderComponent::g_Name = "SphereRenderComponent";
const char* TeapotRenderComponent::g_Name = "TeapotRenderComponent";
const char* GridRenderComponent::g_Name = "GridRenderComponent";
const char* LightRenderComponent::g_Name = "LightRenderComponent";
const char* SkyRenderComponent::g_Name = "SkyRenderComponent";

//---------------------------------------------------------------------------------------------------------------------
// RenderComponent
//---------------------------------------------------------------------------------------------------------------------
bool BaseRenderComponent::VInit(XMLElement* pData)
{
    // color
    XMLElement* pColorNode = pData->FirstChildElement("Color");
    if (pColorNode)
        m_color = LoadColor(pColorNode);

    return VDelegateInit(pData);
}

void BaseRenderComponent::VPostInit(void)
{
    shared_ptr<SceneNode> pSceneNode(VGetSceneNode());
    shared_ptr<EvtData_New_Render_Component> pEvent(QSE_NEW EvtData_New_Render_Component(m_pOwner->GetId(), pSceneNode));
    IEventManager::Get()->VTriggerEvent(pEvent);
}


void BaseRenderComponent::VOnChanged(void)
{
    shared_ptr<EvtData_Modified_Render_Component> pEvent(QSE_NEW EvtData_Modified_Render_Component(m_pOwner->GetId()));
    IEventManager::Get()->VTriggerEvent(pEvent);
}


XMLElement* BaseRenderComponent::VGenerateXml(XMLDocument* pDoc)
{
	XMLElement* pBaseElement = pDoc->NewElement(VGetName());

    // color
	XMLElement* pColor = pDoc->NewElement("Color");
    pColor->SetAttribute("r", ToStr(m_color.r).c_str());
    pColor->SetAttribute("g", ToStr(m_color.g).c_str());
    pColor->SetAttribute("b", ToStr(m_color.b).c_str());
    pColor->SetAttribute("a", ToStr(m_color.a).c_str());
    pBaseElement->LinkEndChild(pColor);

    // create XML for inherited classes
    VCreateInheritedXmlElements(pBaseElement);

    return pBaseElement;
}

shared_ptr<SceneNode> BaseRenderComponent::VGetSceneNode(void)
{
    if (!m_pSceneNode)
        m_pSceneNode = VCreateSceneNode();
    return m_pSceneNode;
}

Color BaseRenderComponent::LoadColor(XMLElement* pData)
{
	Color color;

    double r = 1.0;
    double g = 1.0;
    double b = 1.0;
    double a = 1.0;

    r = std::stod(pData->Attribute("r"));
	b = std::stod(pData->Attribute("g"));
	g = std::stod(pData->Attribute("b"));
	a = std::stod(pData->Attribute("a"));

    color.r = (float)r;
    color.g = (float)g;
    color.b = (float)b;
    color.a = (float)a;

	return color;
}


//---------------------------------------------------------------------------------------------------------------------
// MeshRenderComponent
//---------------------------------------------------------------------------------------------------------------------
shared_ptr<SceneNode> MeshRenderComponent::VCreateSceneNode(void)
{
    return shared_ptr<SceneNode>();
}

void MeshRenderComponent::VCreateInheritedXmlElements(XMLElement *)
{
    LOG_ERROR("MeshRenderComponent::VGenerateSubclassXml() not implemented");
}



//---------------------------------------------------------------------------------------------------------------------
// SphereRenderComponent
//---------------------------------------------------------------------------------------------------------------------
SphereRenderComponent::SphereRenderComponent(void)
{
    m_segments = 50;
}

bool SphereRenderComponent::VDelegateInit(XMLElement* pData)
{
    XMLElement* pMesh = pData->FirstChildElement("Sphere");
    int segments = 50;
	double radius = 1.0;
	radius = std::stod(pMesh->Attribute("radius"));
    segments = std::stod(pMesh->Attribute("segments"));
	m_radius = (float)radius;
    m_segments = (unsigned int)segments;

    return true;
}

shared_ptr<SceneNode> SphereRenderComponent::VCreateSceneNode(void)
{
    // get the transform component
    shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_Name));
    if (!pTransformComponent)
    {
        // can't render without a transform
        return shared_ptr<SceneNode>();
    }

	WeakBaseRenderComponentPtr wbrcp(this);
	if (GameCodeApp::GetRendererImpl()==GameCodeApp::Renderer_D3D9)
	{
		// create the sphere Mesh
		ID3DXMesh* pSphereMesh;

		D3DXCreateSphere(DXUTGetD3D9Device(), m_radius, m_segments, m_segments, &pSphereMesh, NULL);

		shared_ptr<SceneNode> sphere(QSE_NEW D3DShaderMeshNode9(m_pOwner->GetId(), wbrcp, pSphereMesh, "Effects\\GameCode4.fx", RenderPass_Actor, &pTransformComponent->GetTransform()));
    
		SAFE_RELEASE(pSphereMesh);
	    return sphere;
	}
	else if (GameCodeApp::GetRendererImpl()==GameCodeApp::Renderer_D3D11)
	{
		shared_ptr<SceneNode> sphere(QSE_NEW D3DShaderMeshNode11(m_pOwner->GetId(), wbrcp, "art\\sphere.sdkmesh", RenderPass_Actor, &pTransformComponent->GetTransform()));
		return sphere;
	}
	else
	{
		LOG_ASSERT(0 && "Unknown Renderer Implementation in SphereRenderComponent::VCreateSceneNode");
		return shared_ptr<SceneNode>(NULL);
	}
}

void SphereRenderComponent::VCreateInheritedXmlElements(XMLElement* pBaseElement)
{
    XMLElement* pMesh = QSE_NEW XMLElement("Sphere");
	pMesh->SetAttribute("radius", ToStr(m_radius).c_str());
    pMesh->SetAttribute("segments", ToStr(m_segments).c_str());
    pBaseElement->LinkEndChild(pBaseElement);
}


//---------------------------------------------------------------------------------------------------------------------
// TeapotRenderComponent
//---------------------------------------------------------------------------------------------------------------------
shared_ptr<SceneNode> TeapotRenderComponent::VCreateSceneNode(void)
{
    // get the transform component
    shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_Name));
    if (pTransformComponent)
    {
		WeakBaseRenderComponentPtr weakThis(this);

		switch (GameCodeApp::GetRendererImpl())
		{
			case GameCodeApp::Renderer_D3D9: 
				return shared_ptr<SceneNode>(QSE_NEW D3DTeapotMeshNode9(m_pOwner->GetId(), weakThis, "Effects\\GameCode4.fx", RenderPass_Actor, &pTransformComponent->GetTransform()));

			case GameCodeApp::Renderer_D3D11: 
			{
				mat4 rot90;
				rot90.BuildRotationY(-AR_PI/2.0f);
				shared_ptr<SceneNode> parent(QSE_NEW SceneNode(m_pOwner->GetId(), weakThis, RenderPass_Actor, &pTransformComponent->GetTransform()));
				shared_ptr<SceneNode> teapot(QSE_NEW D3DTeapotMeshNode11(INVALID_ACTOR_ID, weakThis, RenderPass_Actor, &rot90));
				parent->VAddChild(teapot);
				return parent;
			}
								 
			default:
				GCC_ERROR("Unknown Renderer Implementation in TeapotRenderComponent");
		}
    }

    return shared_ptr<SceneNode>();
}

void TeapotRenderComponent::VCreateInheritedXmlElements(XMLElement *)
{
}


//---------------------------------------------------------------------------------------------------------------------
// GridRenderComponent
//---------------------------------------------------------------------------------------------------------------------
GridRenderComponent::GridRenderComponent(void)
{
    m_textureResource = "";
    m_squares = 0;
}

bool GridRenderComponent::VDelegateInit(XMLElement* pData)
{
    XMLElement* pTexture = pData->FirstChildElement("Texture");
    if (pTexture)
	{
		m_textureResource = pTexture->FirstChild()->Value();
	}

    XMLElement* pDivision = pData->FirstChildElement("Division");
    if (pDivision)
	{
		m_squares = atoi(pDivision->FirstChild()->Value());
	}

    return true;
}

shared_ptr<SceneNode> GridRenderComponent::VCreateSceneNode(void)
{
    shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_Name));
    if (pTransformComponent)
    {
		WeakBaseRenderComponentPtr weakThis(this);

		switch (GameCodeApp::GetRendererImpl())
		{
			case GameCodeApp::Renderer_D3D9: 
				return shared_ptr<SceneNode>(QSE_NEW D3DGrid9(m_pOwner->GetId(), weakThis, &(pTransformComponent->GetTransform())));  

			case GameCodeApp::Renderer_D3D11: 
				return shared_ptr<SceneNode>(QSE_NEW D3DGrid11(m_pOwner->GetId(), weakThis, &(pTransformComponent->GetTransform())));  
								 
			default:
				GCC_ERROR("Unknown Renderer Implementation in GridRenderComponent");
		}
    }

    return shared_ptr<SceneNode>();
}

void GridRenderComponent::VCreateInheritedXmlElements(XMLElement *pBaseElement)
{
    XMLElement* pTextureNode = QSE_NEW XMLElement("Texture");
    TiXmlText* pTextureText = QSE_NEW TiXmlText(m_textureResource.c_str());
    pTextureNode->LinkEndChild(pTextureText);
    pBaseElement->LinkEndChild(pTextureNode);

    XMLElement* pDivisionNode = QSE_NEW XMLElement("Division");
    TiXmlText* pDivisionText = QSE_NEW TiXmlText(ToStr(m_squares).c_str());
    pDivisionNode->LinkEndChild(pDivisionText);
    pBaseElement->LinkEndChild(pDivisionNode);
}


//---------------------------------------------------------------------------------------------------------------------
// LightRenderComponent
//---------------------------------------------------------------------------------------------------------------------
LightRenderComponent::LightRenderComponent(void)
{
}

bool LightRenderComponent::VDelegateInit(XMLElement* pData)
{
    XMLElement* pLight = pData->FirstChildElement("Light");

	double temp;
    XMLElement* pAttenuationNode = NULL;
	pAttenuationNode = pLight->FirstChildElement("Attenuation");
    if (pAttenuationNode)
	{
		double temp;
		pAttenuationNode->Attribute("const", &temp);
		m_Props.m_Attenuation[0] = (float) temp;

		pAttenuationNode->Attribute("linear", &temp);
		m_Props.m_Attenuation[1] = (float) temp;

		pAttenuationNode->Attribute("exp", &temp);
		m_Props.m_Attenuation[2] = (float) temp;
	}

    XMLElement* pShapeNode = NULL;
	pShapeNode = pLight->FirstChildElement("Shape");
    if (pShapeNode)
	{
		pShapeNode->Attribute("range", &temp);
		m_Props.m_Range = (float) temp;
		pShapeNode->Attribute("falloff", &temp);
		m_Props.m_Falloff = (float) temp;
		pShapeNode->Attribute("theta", &temp);		
		m_Props.m_Theta = (float) temp;
		pShapeNode->Attribute("phi", &temp);
		m_Props.m_Phi = (float) temp;	
	}
    return true;
}

shared_ptr<SceneNode> LightRenderComponent::VCreateSceneNode(void)
{
    shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_Name));
    if (pTransformComponent)
    {
		WeakBaseRenderComponentPtr weakThis(this);

		switch (GameCodeApp::GetRendererImpl())
		{
			case GameCodeApp::Renderer_D3D9: 
				return shared_ptr<SceneNode>(QSE_NEW D3DLightNode9(m_pOwner->GetId(), weakThis, m_Props, &(pTransformComponent->GetTransform())));  

			case GameCodeApp::Renderer_D3D11: 
				return shared_ptr<SceneNode>(QSE_NEW D3DLightNode11(m_pOwner->GetId(), weakThis, m_Props, &(pTransformComponent->GetTransform())));  
								 
			default:
				LOG_ASSERT(0 && "Unknown Renderer Implementation in GridRenderComponent");
		}
	}
    return shared_ptr<SceneNode>();
}

void LightRenderComponent::VCreateInheritedXmlElements(XMLElement *pBaseElement)
{
    XMLElement* pSceneNode = QSE_NEW XMLElement("Light");

    // attenuation
    XMLElement* pAttenuation = QSE_NEW XMLElement("Attenuation");
    pAttenuation->SetAttribute("const", ToStr(m_Props.m_Attenuation[0]).c_str());
    pAttenuation->SetAttribute("linear", ToStr(m_Props.m_Attenuation[1]).c_str());
    pAttenuation->SetAttribute("exp", ToStr(m_Props.m_Attenuation[2]).c_str());
    pSceneNode->LinkEndChild(pAttenuation);

    // shape
    XMLElement* pShape = QSE_NEW XMLElement("Shape");
    pShape->SetAttribute("range", ToStr(m_Props.m_Range).c_str());
    pShape->SetAttribute("falloff", ToStr(m_Props.m_Falloff).c_str());
    pShape->SetAttribute("theta", ToStr(m_Props.m_Theta).c_str());
    pShape->SetAttribute("phi", ToStr(m_Props.m_Phi).c_str());
    pSceneNode->LinkEndChild(pShape);

    pBaseElement->LinkEndChild(pSceneNode);
}




SkyRenderComponent::SkyRenderComponent(void)
{
}

bool SkyRenderComponent::VDelegateInit(XMLElement* pData)
{
    XMLElement* pTexture = pData->FirstChildElement("Texture");
    if (pTexture)
	{
		m_textureResource = pTexture->FirstChild()->Value();
	}
	return true;
}

shared_ptr<SceneNode> SkyRenderComponent::VCreateSceneNode(void)
{
    shared_ptr<SkyNode> sky;
	if (GameCodeApp::GetRendererImpl()==GameCodeApp::Renderer_D3D9)
	{
		sky = shared_ptr<SkyNode>(QSE_NEW D3DSkyNode9(m_textureResource.c_str() ));
	}
	else if (GameCodeApp::GetRendererImpl()==GameCodeApp::Renderer_D3D11)
	{
		sky = shared_ptr<SkyNode>(QSE_NEW GLSkyNode(m_textureResource.c_str() ));
	}
	else
	{
		GCC_ERROR("Unknown Renderer Implementation in VLoadGameDelegate");
	}
	return sky;
}

void SkyRenderComponent::VCreateInheritedXmlElements(XMLElement *pBaseElement)
{
    XMLElement* pTextureNode = QSE_NEW XMLElement("Texture");
    TiXmlText* pTextureText = QSE_NEW TiXmlText(m_textureResource.c_str());
    pTextureNode->LinkEndChild(pTextureText);
    pBaseElement->LinkEndChild(pTextureNode);
}



