//
// Basic, simple, physics event listener construct
//

#include "../Stdafx.hpp"

#include "PhysicsEventListener.hpp"

#include "Physics.hpp"
#include "..\EventManager\Events.hpp"
#include "../LUAScripting/LuaStateManager.hpp"

const EventType EvtData_PhysTrigger_Enter::sk_EventType(0x99358c15);
const EventType EvtData_PhysTrigger_Leave::sk_EventType(0x3f49c41f);
const EventType EvtData_PhysCollision::sk_EventType(0x54c58d0d);
const EventType EvtData_PhysSeparation::sk_EventType(0x3dcea6e1);


void EvtData_PhysCollision::VBuildEventData(void)
{
    m_eventData.AssignNewTable(LuaStateManager::Get()->GetLuaState());
    m_eventData.SetInteger("actorA", m_ActorA);
    m_eventData.SetInteger("actorB", m_ActorB);
}


