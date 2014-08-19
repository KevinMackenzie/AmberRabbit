#include "../Stdafx.hpp"
#include "AmmoPickup.hpp"
#include "../Utilities/String.hpp"

const char* AmmoPickup::g_Name = "AmmoPickup";

bool AmmoPickup::VInit(tinyxml2::XMLElement* pData)
{
    return true;
}

tinyxml2::XMLElement* AmmoPickup::VGenerateXml(tinyxml2::XMLDocument* pDoc)
{
    tinyxml2::XMLElement* pComponentElement = QSE_NEW tinyxml2::XMLElement(VGetName());
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

