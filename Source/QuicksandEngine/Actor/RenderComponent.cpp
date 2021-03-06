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
const char* MaterialRenderComponent::g_Name = "MaterialComponent";

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
	g = (glm::u8)std::stoi(pData->Attribute("g"));
	b = (glm::u8)std::stoi(pData->Attribute("b"));
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
	
	//get the material component
	shared_ptr<MaterialRenderComponent> pMaterialComponent = MakeStrongPtr(m_pOwner->GetComponent<MaterialRenderComponent>(MaterialRenderComponent::g_Name));
	shared_ptr<ResHandle> material;
	if (!pMaterialComponent)
	{
		material = QuicksandEngine::g_pApp->m_ResCache->CreateDummy<GLMaterialResourceExtraData>(); 
	}
	else
	{
		material = pMaterialComponent->GetMaterial();
	}

	WeakBaseRenderComponentPtr wbrcp(this);
	shared_ptr<SceneNode> sphere(QSE_NEW GLMeshNode(m_pOwner->GetId(), wbrcp, "art\\sphere.obj.model", "Shaders\\BasicLightingUntex.prog", RenderPass_Actor, &pTransformComponent->GetTransform()));
		
	sphere->SetMaterial(material);

	/*Resource shadRes();
	shared_ptr<ResHandle> handle = QuicksandEngine::g_pApp->m_ResCache->GetHandle(&shadRes);
	sphere->SetShader(handle);*/
	return sphere;
}

void SphereRenderComponent::VCreateInheritedXmlElements(tinyxml2::XMLElement* pBaseElement)
{
    tinyxml2::XMLElement* pMesh = pBaseElement->ToDocument()->NewElement("Sphere");
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
	if (!pTransformComponent)
	{
		return shared_ptr<SceneNode>();
	}

	//get the material component
	shared_ptr<MaterialRenderComponent> pMaterialComponent = MakeStrongPtr(m_pOwner->GetComponent<MaterialRenderComponent>(MaterialRenderComponent::g_Name));
	shared_ptr<ResHandle> material;
	if (!pMaterialComponent)
	{
		material = QuicksandEngine::g_pApp->m_ResCache->CreateDummy<GLMaterialResourceExtraData>();
	}
	else
	{
		material = pMaterialComponent->GetMaterial();
	}

	WeakBaseRenderComponentPtr weakThis(this);

	shared_ptr<SceneNode> teapot =  shared_ptr<SceneNode>(QSE_NEW GLMeshNode(m_pOwner->GetId(), weakThis, "Art\\teapot.obj.model", "Shaders\\BasicLightingUntex.prog", RenderPass_Actor, &pTransformComponent->GetTransform()));
	teapot->SetMaterial(material);
	//shared_ptr<ResHandle> teapotRes = teapot->GetMaterialResource();
	//shared_ptr<GLMaterialResourceExtraData> mat = static_pointer_cast<GLMaterialResourceExtraData>(teapotRes->GetExtra());// = sphere->GetMaterial();
	//mat->SetDiffuse(GetColor());

	return teapot;
}

void TeapotRenderComponent::VCreateInheritedXmlElements(tinyxml2::XMLElement *)
{
}


//---------------------------------------------------------------------------------------------------------------------
// GridRenderComponent
//---------------------------------------------------------------------------------------------------------------------
GridRenderComponent::GridRenderComponent(void)
{
    m_squares = 0;
	m_fsquareLen = 0.5f;
}

bool GridRenderComponent::VDelegateInit(tinyxml2::XMLElement* pData)
{
    /*tinyxml2::XMLElement* pTexture = pData->FirstChildElement("Texture");
    if (pTexture)
	{
		m_textureResource = pTexture->FirstChild()->Value();
	}*/

    tinyxml2::XMLElement* pDivision = pData->FirstChildElement("Division");
    if (pDivision)
	{
		m_squares = atoi(pDivision->FirstChild()->Value());
	}

	tinyxml2::XMLElement* pSquareLen = pData->FirstChildElement("UnitLength");
	if (pSquareLen)
	{
		m_fsquareLen = (float)atof(pSquareLen->FirstChild()->Value());
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
    tinyxml2::XMLElement* pTextureNode = pBaseElement->ToDocument()->NewElement("UnitLength");
	tinyxml2::XMLText* pTextureText = pTextureNode->ToDocument()->NewText(ToStr(m_fsquareLen).c_str());
    pTextureNode->LinkEndChild(pTextureText);
    pBaseElement->LinkEndChild(pTextureNode);

	tinyxml2::XMLElement* pDivisionNode = pBaseElement->ToDocument()->NewElement("Division");
	tinyxml2::XMLText* pDivisionText = pDivisionNode->ToDocument()->NewText(ToStr(m_squares).c_str());
    pDivisionNode->LinkEndChild(pDivisionText);
    pBaseElement->LinkEndChild(pDivisionNode);
}


//---------------------------------------------------------------------------------------------------------------------
// MaterialRenderComponent
//---------------------------------------------------------------------------------------------------------------------
MaterialRenderComponent::MaterialRenderComponent(void)
{
	m_Material = nullptr;
}

const shared_ptr<ResHandle> MaterialRenderComponent::GetMaterial()
{
	return m_Material;
}

bool MaterialRenderComponent::VDelegateInit(tinyxml2::XMLElement* pData)
{
	string mtlLocation = pData->Attribute("path");
	if (mtlLocation == "")
		mtlLocation = "art\\defaultmaterial.qmtl";

	Resource res(mtlLocation);
	m_Material = QuicksandEngine::g_pApp->m_ResCache->GetHandle(&res);

	if (!m_Material)
		return false;

	return true;
}

void MaterialRenderComponent::VCreateInheritedXmlElements(tinyxml2::XMLElement* pBaseElement)
{
	string path = "art\\defaultmaterial.qmtl";
	if (m_Material)
		path = m_Material->GetName();

	pBaseElement->SetAttribute("path", path.c_str());
}

shared_ptr<SceneNode> MaterialRenderComponent::VCreateSceneNode(void)
{
	return nullptr;
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
	string resName = "art\\afternoon_sky.cubemap.dds";
    if (pTexture)
	{
		resName = pTexture->GetText();
	}

	Resource res(resName);
	m_Cubemap = QuicksandEngine::g_pApp->m_ResCache->GetHandle(&res);
	return true;
}

shared_ptr<SceneNode> SkyRenderComponent::VCreateSceneNode(void)
{
	return shared_ptr<SkyNode>(QSE_NEW GLSkyNode(m_Cubemap));
}

void SkyRenderComponent::VCreateInheritedXmlElements(tinyxml2::XMLElement *pBaseElement)
{
    tinyxml2::XMLElement* pTextureNode = pBaseElement->ToDocument()->NewElement("Texture");
	tinyxml2::XMLText* pTextureText = pTextureNode->ToDocument()->NewText(m_Cubemap->GetName().c_str());
    pTextureNode->LinkEndChild(pTextureText);
    pBaseElement->LinkEndChild(pTextureNode);
}



