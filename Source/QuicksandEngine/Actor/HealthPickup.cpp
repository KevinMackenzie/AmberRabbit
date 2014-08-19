#include "../Stdafx.hpp"
#include "HealthPickup.hpp"
#include "../Utilities/String.hpp"

const char* HealthPickup::g_Name = "HealthPickup";

bool HealthPickup::VInit(tinyxml2::XMLElement* pData)
{
    return true;
}

tinyxml2::XMLElement* HealthPickup::VGenerateXml(tinyxml2::XMLDocument* pDoc)
{
    tinyxml2::XMLElement* pComponentElement = pDoc->NewElement(VGetName());
    return pComponentElement;
}

void HealthPickup::VApply(WeakActorPtr pActor)
{
    StrongActorPtr pStrongActor = MakeStrongPtr(pActor);
    if (pStrongActor)
    {
        LOG_WRITE("Actor", "Applying health pickup to actor id " + ToStr(pStrongActor->GetId()));
    }
}

