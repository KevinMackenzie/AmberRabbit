#ifndef QSE_ACTORFACTOR_HPP
#define QSE_ACTORFACTOR_HPP

//---------------------------------------------------------------------------------------------------------------------
// ActorFactory class
// Chapter 6, page 161
//---------------------------------------------------------------------------------------------------------------------
class ActorFactory
{
    ActorId m_lastActorId;

protected:
    GenericObjectFactory<ActorComponent, ComponentId> m_componentFactory;

public:
    ActorFactory(void);

    StrongActorPtr CreateActor(const char* actorResource, XMLElement* overrides, const glm::mat4* initialTransform, const ActorId serversActorId);
	void ModifyActor(StrongActorPtr pActor, XMLElement* overrides);

//protected:
    // [rez] This function can be overridden by a subclass so you can create game-specific C++ components.  If you do
    // this, make sure you call the base-class version first.  If it returns NULL, you know it's not an engine component.
    virtual StrongActorComponentPtr VCreateComponent(XMLElement* pData);

private:
    ActorId GetNextActorId(void) { ++m_lastActorId; return m_lastActorId; }
};

#endif