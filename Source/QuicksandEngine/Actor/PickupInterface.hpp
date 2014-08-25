#ifndef QSE_PICKUPINTERFACES_HPP
#define QSE_PICKUPINTERFACES_HPP

#include "ActorComponent.hpp"

class  PickupInterface : public ActorComponent
{
public:
//    static ComponentId COMPONENT_ID;  // unique ID for this component type
//    virtual ComponentId VGetComponentId(void) const override { return COMPONENT_ID; }

    // Pickup interface
    virtual void VApply(WeakActorPtr pActor) = 0;
};

#endif