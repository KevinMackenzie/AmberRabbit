#ifndef QSE_HEALTHPICKUP_HPP
#define QSE_HEALTHPICKUP_HPP

#include "PickupInterface.hpp"


//---------------------------------------------------------------------------------------------------------------------
// HealthPickup implementation class.
// Chapter 6, page 170
// [rez] This component isn't actually used in Teapot wars.  It was used as an example for describing components in 
// the book.  I decided to include it here to give you a starting point to add health drops in your game.  :)
//---------------------------------------------------------------------------------------------------------------------
class HealthPickup : public PickupInterface
{
public:
	static const char* g_Name;
	virtual const char* VGetName() const { return g_Name; }

    virtual bool VInit(XMLElement* pData) override;
    virtual XMLElement* VGenerateXml(XMLDocument* pDoc) override;
    virtual void VApply(WeakActorPtr pActor) override;
};

#endif