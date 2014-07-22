#include "../Stdafx.hpp"
#include "HealthPickup.hpp"
#include "../Utilities/String.hpp"

const char* HealthPickup::g_Name = "HealthPickup";

bool HealthPickup::VInit(XMLElement* pData)
{
    return true;
}

XMLElement* HealthPickup::VGenerateXml(XMLDocument* pDoc)
{
    XMLElement* pComponentElement = pDoc->NewElement(VGetName());
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

