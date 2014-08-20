#include "../Stdafx.hpp"
#include "Actor.hpp"
#include "ActorComponent.hpp"
#include "../Utilities/String.hpp"


//---------------------------------------------------------------------------------------------------------------------
// Actor
// Chapter 6, page 165
//---------------------------------------------------------------------------------------------------------------------
Actor::Actor(ActorId id)
{
    m_id = id;
    m_type = "Unknown";

	// [mrmike] added post press - this is an editor helper
	m_resource = "Unknown";
}

Actor::~Actor(void)
{
    LOG_WRITE(ConcatString("Actor", string("Destroying Actor ") + ToStr(m_id)));
    LOG_ASSERT(m_components.empty());  // [rez] if this assert fires, the actor was destroyed without calling Actor::Destroy()
}

bool Actor::Init(tinyxml2::XMLElement* pData)
{
	LOG_WRITE(ConcatString("Actor", string("Initializing Actor ") + ToStr(m_id)));

	m_type = pData->Attribute("type");
	m_resource = pData->Attribute("resource");
    return true;
}

void Actor::PostInit(void)
{
    for (ActorComponents::iterator it = m_components.begin(); it != m_components.end(); ++it)
    {
        it->second->VPostInit();
    }
}

void Actor::Destroy(void)
{
    m_components.clear();
}

void Actor::Update(int deltaMs)
{
    for (ActorComponents::iterator it = m_components.begin(); it != m_components.end(); ++it)
    {
        it->second->VUpdate(deltaMs);
    }
}

string Actor::ToXML()
{
    tinyxml2::XMLDocument outDoc;
	
    // Actor element
	tinyxml2::XMLElement* pActorElement = outDoc.NewElement("Actor");
    pActorElement->SetAttribute("type", m_type.c_str());
	pActorElement->SetAttribute("resource", m_resource.c_str());

    // components
    for (auto it = m_components.begin(); it != m_components.end(); ++it)
    {
        StrongActorComponentPtr pComponent = it->second;
		tinyxml2::XMLElement* pComponentElement = pComponent->VGenerateXml(&outDoc);
        pActorElement->LinkEndChild(pComponentElement);
    }

    outDoc.LinkEndChild(pActorElement);
	tinyxml2::XMLPrinter printer;
	outDoc.Accept(&printer);

	return printer.CStr();
}


void Actor::AddComponent(StrongActorComponentPtr pComponent)
{
    std::pair<ActorComponents::iterator, bool> success = m_components.insert(std::make_pair(pComponent->VGetId(), pComponent));
    LOG_ASSERT(success.second);
}

