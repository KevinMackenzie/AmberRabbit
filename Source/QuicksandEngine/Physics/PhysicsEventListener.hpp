#ifndef QSE_PHYSICSEVENTLISTENER_HPP
#define QSE_PHYSICSEVENTLISTENER_HPP

#include "../EventManager/EventManager.hpp"
#include "../LUAScripting/ScriptEvent.hpp"

//
// Physics event implementation 
//

class EvtData_PhysTrigger_Enter : public BaseEventData
{
	int m_triggerID;
    ActorId m_other;

public:
	static const EventType sk_EventType;

	virtual const EventType & VGetEventType( void ) const
	{
		return sk_EventType;
	}

	EvtData_PhysTrigger_Enter()
	{
		m_triggerID = -1;
		m_other = INVALID_ACTOR_ID;
	}

	explicit EvtData_PhysTrigger_Enter( int triggerID, ActorId other )
		: m_triggerID(triggerID),
		  m_other(other)
	{}

	IEventDataPtr VCopy() const 
	{
		return IEventDataPtr(QSE_NEW EvtData_PhysTrigger_Enter(m_triggerID, m_other));
	}

    virtual const char* GetName(void) const
    {
        return "EvtData_PhysTrigger_Enter";
    }

    int GetTriggerId(void) const
    {
        return m_triggerID;
    }

    ActorId GetOtherActor(void) const
    {
        return m_other;
    }
};

class EvtData_PhysTrigger_Leave : public BaseEventData
{
	int m_triggerID;
    ActorId m_other;

public:
	static const EventType sk_EventType;

	virtual const EventType & VGetEventType( void ) const
	{
		return sk_EventType;
	}

	EvtData_PhysTrigger_Leave()
	{
		m_triggerID = -1;
		m_other = INVALID_ACTOR_ID;
	}

	explicit EvtData_PhysTrigger_Leave( int triggerID, ActorId other )
		: m_triggerID(triggerID),
		  m_other(other)
	{}

	virtual IEventDataPtr VCopy() const
	{
		return IEventDataPtr ( QSE_NEW EvtData_PhysTrigger_Leave(m_triggerID, m_other) );
	}

    virtual const char* GetName(void) const
    {
        return "EvtData_PhysTrigger_Leave";
    }

    int GetTriggerId(void) const
    {
        return m_triggerID;
    }
    
    ActorId GetOtherActor(void) const
    {
        return m_other;
    }
};

class EvtData_PhysCollision : public ScriptEvent
{
	ActorId m_ActorA;
    ActorId m_ActorB;
	glm::vec3 m_SumNormalForce;
    glm::vec3 m_SumFrictionForce;
    Vec3Array m_CollisionPoints;

public:
	static const EventType sk_EventType;

	virtual const EventType & VGetEventType( void ) const
	{
		return sk_EventType;
	}

	EvtData_PhysCollision()
	{
		m_ActorA = INVALID_ACTOR_ID;
		m_ActorB = INVALID_ACTOR_ID;
		m_SumNormalForce = glm::vec3(0.0f, 0.0f, 0.0f);
		m_SumFrictionForce = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	explicit EvtData_PhysCollision(ActorId actorA,
								ActorId actorB,
								glm::vec3 sumNormalForce,
								glm::vec3 sumFrictionForce,
								Vec3Array collisionPoints)
		: m_ActorA(actorA),
		m_ActorB(actorB),
		m_SumNormalForce(sumNormalForce),
		m_SumFrictionForce(sumFrictionForce),
		m_CollisionPoints(collisionPoints)
	{}

	virtual IEventDataPtr VCopy() const
	{
		return IEventDataPtr ( QSE_NEW EvtData_PhysCollision(m_ActorA, m_ActorB, m_SumNormalForce, m_SumFrictionForce, m_CollisionPoints));
	}

    virtual const char* GetName(void) const
    {
        return "EvtData_PhysCollision";
    }

    ActorId GetActorA(void) const
    {
        return m_ActorA;
    }

    ActorId GetActorB(void) const
    {
        return m_ActorB;
    }

    const glm::vec3& GetSumNormalForce(void) const
    {
        return m_SumNormalForce;
    }

    const glm::vec3& GetSumFrictionForce(void) const
    {
        return m_SumFrictionForce;
    }

	const Vec3Array& GetCollisionPoints(void) const
    {
        return m_CollisionPoints;
    }

    virtual void VBuildEventData(void);

    EXPORT_FOR_SCRIPT_EVENT(EvtData_PhysCollision);
};


class EvtData_PhysSeparation : public BaseEventData
{
	ActorId m_ActorA;
    ActorId m_ActorB;

public:
	static const EventType sk_EventType;

	virtual const EventType & VGetEventType( void ) const
	{
		return sk_EventType;
	}

	EvtData_PhysSeparation()
	{

		m_ActorA = INVALID_ACTOR_ID;
		m_ActorB = INVALID_ACTOR_ID;
	}

	explicit EvtData_PhysSeparation(ActorId actorA, ActorId actorB)
		: m_ActorA(actorA)
		, m_ActorB(actorB)
	{}

	virtual IEventDataPtr VCopy() const
	{
		return IEventDataPtr ( QSE_NEW EvtData_PhysSeparation(m_ActorA, m_ActorB) );
	}

    virtual const char* GetName(void) const
    {
        return "EvtData_PhysSeparation";
    }

    ActorId GetActorA(void) const
    {
        return m_ActorA;
    }

    ActorId GetActorB(void) const
    {
        return m_ActorB;
    }
};


#endif