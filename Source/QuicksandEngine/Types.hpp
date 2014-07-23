#ifndef QSE_TYPEDEFS_HPP
#define QSE_TYPEDEFS_HPP


typedef vec4 Color;

typedef std::vector<GLuint> GLIndexArray;
typedef std::vector<vec2> Vec2Array;
typedef std::vector<vec3> Vec3Array;
typedef std::vector<vec4> Vec4Array;

typedef std::list<vec3> Vec3List;
typedef std::list<vec4> Vec4List;

struct GLVertexTextured
{
	vec3 Pos;
	vec2 Uv;
};

typedef std::vector<GLVertexTextured> GLVertexTexturedList;

typedef string ActorType;
typedef std::map<ComponentId, StrongActorComponentPtr> ActorComponents;
#endif