#ifndef QSE_TYPEDEFS_HPP
#define QSE_TYPEDEFS_HPP


//typedef glm::vec4 Color;

typedef std::vector<GLuint> GLIndexArray;
typedef std::vector<glm::vec2> Vec2Array;
typedef std::vector<glm::vec3> Vec3Array;
typedef std::vector<glm::vec4> Vec4Array;

typedef std::list<glm::vec3> Vec3List;
typedef std::list<glm::vec4> Vec4List;

struct GLVertexTextured
{
	glm::vec3 Pos;
	glm::vec2 Uv;
};

typedef std::vector<GLVertexTextured> GLVertexTexturedList;

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