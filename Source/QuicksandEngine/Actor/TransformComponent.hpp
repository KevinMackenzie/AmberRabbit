#ifndef QSE_TRANSFORMCOMPONENT_HPP
#define QSE_TRANSFORMCOMPONENT_HPP

#include "ActorComponent.hpp"

//---------------------------------------------------------------------------------------------------------------------
// This component implementation is a very simple representation of the physical aspect of an actor.  It just defines 
// the transform and doesn't register with the physics system at all.
//---------------------------------------------------------------------------------------------------------------------
class TransformComponent : public ActorComponent
{
    glm::mat4 m_transform;
public:
	static const char* g_Name;
	virtual const char* VGetName() const { return g_Name; }

    TransformComponent(void) : m_transform(glm::mat4()) { }
    virtual bool VInit(tinyxml2::XMLElement* pData) override;
    virtual tinyxml2::XMLElement* VGenerateXml(tinyxml2::XMLDocument* pDoc) override;

    // transform functions
    glm::mat4 GetTransform(void) const { return m_transform; }
    void SetTransform(const glm::mat4& newTransform) { m_transform = newTransform; }
    glm::vec3 GetPosition(void) const { return ::GetPosition(m_transform); }
	void SetPosition(const glm::vec3& pos) { ::SetPosition(m_transform, pos); }
    glm::vec3 GetLookAt(void) const { return ::GetDirection(m_transform); }
};

#endif