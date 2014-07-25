#ifndef QSE_TYPEDEFS_HPP
#define QSE_TYPEDEFS_HPP


typedef glm::vec4 Color;

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

typedef string ActorType;
typedef std::map<ComponentId, StrongActorComponentPtr> ActorComponents;
#endif