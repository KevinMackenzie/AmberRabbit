#ifndef QSE_TYPEDEFS_HPP
#define QSE_TYPEDEFS_HPP



typedef unsigned int ActorId;
typedef unsigned int ComponentId;

const ActorId INVALID_ACTOR_ID = 0;
const ComponentId INVALID_COMPONENT_ID = 0;

class Actor;
class ActorComponent;

typedef shared_ptr<Actor> StrongActorPtr;
typedef weak_ptr<Actor> WeakActorPtr;
typedef shared_ptr<ActorComponent> StrongActorComponentPtr;
typedef weak_ptr<ActorComponent> WeakActorComponentPtr;

typedef string ActorType;
typedef std::map<ComponentId, StrongActorComponentPtr> ActorComponents;
#endif