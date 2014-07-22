#include "../Stdafx.hpp"
#include "AmmoPickup.hpp"
#include "../Utilities/String.hpp"

const char* AmmoPickup::g_Name = "AmmoPickup";

bool AmmoPickup::VInit(XMLElement* pData)
{
    return true;
}

XMLElement* AmmoPickup::VGenerateXml(XMLDocument* pDoc)
{
    XMLElement* pComponentElement = QSE_NEW XMLElement(VGetName());
    return pComponentElement;
}

void AmmoPickup::VApply(WeakActorPtr pActor)
{
    StrongActorPtr pStrongActor = MakeStrongPtr(pActor);
    if (pStrongActor)
    {
        LOG_WRITE("Actor", "Applying ammo pickup to actor id " + ToStr(pStrongActor->GetId()));
    }
}

