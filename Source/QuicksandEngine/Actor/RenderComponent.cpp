#include "../Stdafx.hpp"

#include "..\Graphics3D\Mesh.hpp"
#include "..\Graphics3D\Sky.hpp"

#include "../Utilities/String.hpp"

#include "../EventManager/Events.hpp"

#include "RenderComponent.hpp"
#include "TransformComponent.hpp"

const char* MeshRenderComponent::g_Name = "MeshRenderComponent";
const char* SphereRenderComponent::g_Name = "SphereRenderComponent";
//const char* TeapotRenderComponent::g_Name = "TeapotRenderComponent";
const char* GridRenderComponent::g_Name = "GridRenderComponent";
const char* LightRenderComponent::g_Name = "LightRenderComponent";
const char* SkyRenderComponent::g_Name = "SkyRenderComponent";

//---------------------------------------------------------------------------------------------------------------------
// RenderComponent
//---------------------------------------------------------------------------------------------------------------------
bool BaseRenderComponent::VInit(tinyxml2::XMLElement* pData)
{
    // color
    tinyxml2::XMLElement* pColorNode = pData->FirstChildElement("Color");
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


tinyxml2::XMLElement* BaseRenderComponent::VGenerateXml(tinyxml2::XMLDocument* pDoc)
{
	tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(VGetName());

    // color
	tinyxml2::XMLElement* pColor = pDoc->NewElement("Color");
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

Color BaseRenderComponent::LoadColor(tinyxml2::XMLElement* pData)
{
	Color color;

	glm::u8 r = 255;
    glm::u8 g = 255;
	glm::u8 b = 255;
	glm::u8 a = 255;

    r = (glm::u8)std::stoi(pData->Attribute("r"));
	b = (glm::u8)std::stoi(pData->Attribute("g"));
	g = (glm::u8)std::stoi(pData->Attribute("b"));
	a = (glm::u8)std::stoi(pData->Attribute("a"));

	color = Color(r, g, b, a);

	return color;
}


//---------------------------------------------------------------------------------------------------------------------
// MeshRenderComponent
//---------------------------------------------------------------------------------------------------------------------
shared_ptr<SceneNode> MeshRenderComponent::VCreateSceneNode(void)
{
    return shared_ptr<SceneNode>();
}

void MeshRenderComponent::VCreateInheritedXmlElements(tinyxml2::XMLElement *)
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

bool SphereRenderComponent::VDelegateInit(tinyxml2::XMLElement* pData)
{
    tinyxml2::XMLElement* pMesh = pData->FirstChildElement("Sphere");
	if (pMesh)
	{
		const char* radText = pMesh->Attribute("radius");
		if (radText)
			m_radius = std::stof(radText);

		const char* segText = pMesh->Attribute("segments");
		if (segText)
			m_segments = std::stoi(segText);
	}

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
	shared_ptr<SceneNode> sphere(QSE_NEW GLMeshNode(m_pOwner->GetId(), wbrcp, "art\\sphere.obj.model", RenderPass_Actor, &pTransformComponent->GetTransform()));

	// TODO: do this with all components
	shared_ptr<GLMaterialResourceExtraData> mat = sphere->GetMaterial();
	mat->SetDiffuse(GetColor());
	mat->SetAmbient(Color(100, 100, 100, 255));
	mat->SetSpecular(Color(255, 255, 255, 255), 15);
	//sphere->SetMaterial(mat);

	Resource shadRes("Shaders\\BasicLightingUntex.prog");
	shared_ptr<ResHandle> handle = QuicksandEngine::g_pApp->m_ResCache->GetHandle(&shadRes);
	sphere->SetShader(handle);
	return sphere;
}

void SphereRenderComponent::VCreateInheritedXmlElements(tinyxml2::XMLElement* pBaseElement)
{
    tinyxml2::XMLElement* pMesh = pBaseElement->ToDocument()->NewElement("Sphere");
	pMesh->SetAttribute("radius", ToStr(m_radius).c_str());
    pMesh->SetAttribute("segments", ToStr(m_segments).c_str());
    pBaseElement->LinkEndChild(pBaseElement);
}

/*
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
				return shared_ptr<SceneNode>(QSE_NEW GLTeapod(m_pOwner->GetId(), weakThis, "Effects\\GameCode4.fx", RenderPass_Actor, &pTransformComponent->GetTransform()));

			case GameCodeApp::Renderer_D3D11: 
			{
				glm::mat4 rot90;
				rot90.BuildRotationY(-AR_PI/2.0f);
				shared_ptr<SceneNode> parent(QSE_NEW SceneNode(m_pOwner->GetId(), weakThis, RenderPass_Actor, &pTransformComponent->GetTransform()));
				shared_ptr<SceneNode> teapot(QSE_NEW D3DTeapotMeshNode11(INVALID_ACTOR_ID, weakThis, RenderPass_Actor, &rot90));
				parent->VAddChild(teapot);
				return parent;
    }

    return shared_ptr<SceneNode>();
}

void TeapotRenderComponent::VCreateInheritedXmlElements(tinyxml2::XMLElement *)
{
}
*/

//---------------------------------------------------------------------------------------------------------------------
// GridRenderComponent
//---------------------------------------------------------------------------------------------------------------------
GridRenderComponent::GridRenderComponent(void)
{
    m_textureResource = "";
    m_squares = 0;
	m_fsquareLen = 0.5f;
}

bool GridRenderComponent::VDelegateInit(tinyxml2::XMLElement* pData)
{
    tinyxml2::XMLElement* pTexture = pData->FirstChildElement("Texture");
    if (pTexture)
	{
		m_textureResource = pTexture->FirstChild()->Value();
	}

    tinyxml2::XMLElement* pDivision = pData->FirstChildElement("Division");
    if (pDivision)
	{
		m_squares = atoi(pDivision->FirstChild()->Value());
	}

	tinyxml2::XMLElement* pSquareLen = pData->FirstChildElement("UnitLength");
	if (pSquareLen)
	{
		m_fsquareLen = atof(pSquareLen->FirstChild()->Value());
	}

    return true;
}

shared_ptr<SceneNode> GridRenderComponent::VCreateSceneNode(void)
{
    shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_Name));
    if (pTransformComponent)
    {
		WeakBaseRenderComponentPtr weakThis(this);

		return shared_ptr<SceneNode>(QSE_NEW GLGrid(m_pOwner->GetId(), weakThis, 10, 0.5f, g_Black, &(pTransformComponent->GetTransform())));  
    }

    return shared_ptr<SceneNode>();
}

void GridRenderComponent::VCreateInheritedXmlElements(tinyxml2::XMLElement *pBaseElement)
{
    tinyxml2::XMLElement* pTextureNode = pBaseElement->ToDocument()->NewElement("Texture");
	tinyxml2::XMLText* pTextureText = pTextureNode->ToDocument()->NewText(m_textureResource.c_str());
    pTextureNode->LinkEndChild(pTextureText);
    pBaseElement->LinkEndChild(pTextureNode);

	tinyxml2::XMLElement* pDivisionNode = pBaseElement->ToDocument()->NewElement("Division");
	tinyxml2::XMLText* pDivisionText = pDivisionNode->ToDocument()->NewText(ToStr(m_squares).c_str());
    pDivisionNode->LinkEndChild(pDivisionText);
    pBaseElement->LinkEndChild(pDivisionNode);
}


//---------------------------------------------------------------------------------------------------------------------
// LightRenderComponent
//---------------------------------------------------------------------------------------------------------------------
LightRenderComponent::LightRenderComponent(void)
{
}

bool LightRenderComponent::VDelegateInit(tinyxml2::XMLElement* pData)
{
    tinyxml2::XMLElement* pLight = pData->FirstChildElement("Light");

	double temp;
    tinyxml2::XMLElement* pAttenuationNode = NULL;
	pAttenuationNode = pLight->FirstChildElement("Attenuation");
    if (pAttenuationNode)
	{
		double temp;
		temp = std::stod(pAttenuationNode->Attribute("const"));
		m_Props.m_Attenuation[0] = (float) temp;

		temp = std::stod(pAttenuationNode->Attribute("linear"));
		m_Props.m_Attenuation[1] = (float) temp;

		temp = std::stod(pAttenuationNode->Attribute("exp"));
		m_Props.m_Attenuation[2] = (float) temp;
	}

    tinyxml2::XMLElement* pShapeNode = NULL;
	pShapeNode = pLight->FirstChildElement("Shape");
    if (pShapeNode)
	{
		temp = std::stod(pShapeNode->Attribute("range"));
		m_Props.m_Range = (float) temp;
		temp = std::stod(pShapeNode->Attribute("falloff"));
		m_Props.m_Falloff = (float) temp;
		temp = std::stod(pShapeNode->Attribute("theta"));		
		m_Props.m_Theta = (float) temp;
		temp = std::stod(pShapeNode->Attribute("phi"));
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

		return shared_ptr<SceneNode>(QSE_NEW GLLightNode(m_pOwner->GetId(), weakThis, m_Props, &(pTransformComponent->GetTransform())));  
	}
    return shared_ptr<SceneNode>();
}

void LightRenderComponent::VCreateInheritedXmlElements(tinyxml2::XMLElement *pBaseElement)
{
    tinyxml2::XMLElement* pSceneNode = pBaseElement->ToDocument()->NewElement("Light");

    // attenuation
    tinyxml2::XMLElement* pAttenuation = pSceneNode->ToDocument()->NewElement("Attenuation");
    pAttenuation->SetAttribute("const", ToStr(m_Props.m_Attenuation[0]).c_str());
    pAttenuation->SetAttribute("linear", ToStr(m_Props.m_Attenuation[1]).c_str());
    pAttenuation->SetAttribute("exp", ToStr(m_Props.m_Attenuation[2]).c_str());
    pSceneNode->LinkEndChild(pAttenuation);

    // shape
    tinyxml2::XMLElement* pShape = pSceneNode->ToDocument()->NewElement("Shape");
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

bool SkyRenderComponent::VDelegateInit(tinyxml2::XMLElement* pData)
{
    tinyxml2::XMLElement* pTexture = pData->FirstChildElement("Texture");
    if (pTexture)
	{
		m_textureResource = pTexture->FirstChild()->Value();
	}
	return true;
}

shared_ptr<SceneNode> SkyRenderComponent::VCreateSceneNode(void)
{
	return shared_ptr<SkyNode>(QSE_NEW GLSkyNode(m_textureResource.c_str() ));
}

void SkyRenderComponent::VCreateInheritedXmlElements(tinyxml2::XMLElement *pBaseElement)
{
    tinyxml2::XMLElement* pTextureNode = pBaseElement->ToDocument()->NewElement("Texture");
    tinyxml2::XMLText* pTextureText = pTextureNode->ToDocument()->NewText(m_textureResource.c_str());
    pTextureNode->LinkEndChild(pTextureText);
    pBaseElement->LinkEndChild(pTextureNode);
}



