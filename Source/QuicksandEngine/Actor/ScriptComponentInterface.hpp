#ifndef QSE_SCRIPTCOMPONENTINTERFACE_HPP
#define QSE_SCRIPTCOMPONENTINTERFACE_HPP


#include "ActorComponent.hpp"

//---------------------------------------------------------------------------------------------------------------------
// This component is essentially a placeholder for the script representation of the entity.  The engine doesn't do 
// much (if anything) with it, hence the lack of interface.
//---------------------------------------------------------------------------------------------------------------------
class ScriptComponentInterface : public ActorComponent
{
public:
//    static ComponentId COMPONENT_ID;
//    virtual ComponentId VGetComponentId(void) const { return COMPONENT_ID; }
};


#endif