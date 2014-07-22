#ifndef QSE_PHYSICSCOMPONENTS_HPP
#define QSE_PHYSICSCOMPONENTS_HPP

#include "ActorComponent.hpp"


class PhysicsComponent : public ActorComponent
{
public:
	const static char *g_Name;
	virtual const char *VGetName() const override { return PhysicsComponent::g_Name; }

public:
    PhysicsComponent(void);
    virtual ~PhysicsComponent(void);
    virtual XMLElement* VGenerateXml(XMLDocument* pDoc) override;

    // ActorComponent interface
    virtual bool VInit(XMLElement* pData) override;
    virtual void VPostInit(void) override;
    virtual void VUpdate(int deltaMs) override;

    // Physics functions
    void ApplyForce(const vec3& direction, float forceNewtons);
	void ApplyTorque(const vec3& direction, float forceNewtons);
	bool KinematicMove(const mat4& transform);

    // acceleration
    void ApplyAcceleration(float acceleration);
    void RemoveAcceleration(void);
    void ApplyAngularAcceleration(float acceleration);
    void RemoveAngularAcceleration(void);

	//void RotateY(float angleRadians);
	vec3 GetVelocity(void);
	void SetVelocity(const vec3& velocity);
    void RotateY(float angleRadians);
    void SetPosition(float x, float y, float z);
    void Stop(void);


protected:
    void CreateShape();
    void BuildRigidBodyTransform(XMLElement* pTransformElement);

    float m_acceleration, m_angularAcceleration;
    float m_maxVelocity, m_maxAngularVelocity;

	string m_shape;
    string m_density;
    string m_material;
	
	vec3 m_RigidBodyLocation;		// this isn't world position! This is how the rigid body is offset from the position of the actor.
	vec3 m_RigidBodyOrientation;	// ditto, orientation
	vec3 m_RigidBodyScale;			// ditto, scale
	
    shared_ptr<IGamePhysics> m_pGamePhysics;  // might be better as a weak ptr...
};


#endif