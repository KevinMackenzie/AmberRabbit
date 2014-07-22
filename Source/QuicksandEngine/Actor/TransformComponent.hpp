#ifndef QSE_TRANSFORMCOMPONENT_HPP
#define QSE_TRANSFORMCOMPONENT_HPP

#include "ActorComponent.hpp"

//---------------------------------------------------------------------------------------------------------------------
// This component implementation is a very simple representation of the physical aspect of an actor.  It just defines 
// the transform and doesn't register with the physics system at all.
//---------------------------------------------------------------------------------------------------------------------
class TransformComponent : public ActorComponent
{
    mat4 m_transform;
public:
	static const char* g_Name;
	virtual const char* VGetName() const { return g_Name; }

    TransformComponent(void) : m_transform(mat4()) { }
    virtual bool VInit(XMLElement* pData) override;
    virtual XMLElement* VGenerateXml(XMLDocument* pDoc) override;

    // transform functions
    mat4 GetTransform(void) const { return m_transform; }
    void SetTransform(const mat4& newTransform) { m_transform = newTransform; }
    vec3 GetPosition(void) const { return ::GetPosition(m_transform); }
	void SetPosition(const vec3& pos) { ::SetPosition(m_transform, pos); }
    vec3 GetLookAt(void) const { return ::GetDirection(m_transform); }
};

#endif