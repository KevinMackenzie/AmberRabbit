#include "../Stdafx.hpp"
#include "PhysicsComponent.hpp"
#include "TransformComponent.hpp"
//#include "../Physics/Physics.hpp"
#include "../Utilities/String.hpp"

// all times are units-per-second
const float DEFAULT_MAX_VELOCITY = 7.5f;
const float DEFAULT_MAX_ANGULAR_VELOCITY = 1.2f;


//---------------------------------------------------------------------------------------------------------------------
// PhysicsComponent implementation
//---------------------------------------------------------------------------------------------------------------------

const char *PhysicsComponent::g_Name = "PhysicsComponent";

PhysicsComponent::PhysicsComponent(void)
{
    m_RigidBodyLocation = vec3(0.f,0.f,0.f);
	m_RigidBodyOrientation = vec3(0.f,0.f,0.f);
	m_RigidBodyScale = vec3(1.f,1.f,1.f);

    m_acceleration = 0;
    m_angularAcceleration = 0;
    m_maxVelocity = DEFAULT_MAX_VELOCITY;
    m_maxAngularVelocity = DEFAULT_MAX_ANGULAR_VELOCITY;
}

PhysicsComponent::~PhysicsComponent(void)
{
    m_pGamePhysics->VRemoveActor(m_pOwner->GetId());
}

bool PhysicsComponent::VInit(XMLElement* pData)
{
    // no point in having a physics component with no game physics
    m_pGamePhysics = QuicksandEngine::g_pApp->m_pGame->VGetGamePhysics();
    if (!m_pGamePhysics)
        return false;

    // shape
    XMLElement* pShape = pData->FirstChildElement("Shape");
    if (pShape)
    {
		m_shape = pShape->FirstChild()->Value();
    }

    // density
    XMLElement* pDensity = pData->FirstChildElement("Density");
    if (pDensity)
        m_density = pDensity->FirstChild()->Value();

    // material
    XMLElement* pMaterial = pData->FirstChildElement("PhysicsMaterial");
    if (pMaterial)
        m_material = pMaterial->FirstChild()->Value();

    // initial transform
    XMLElement* pRigidBodyTransform = pData->FirstChildElement("RigidBodyTransform");
    if (pRigidBodyTransform)
        BuildRigidBodyTransform(pRigidBodyTransform);

    return true;
}

XMLElement* PhysicsComponent::VGenerateXml(XMLDocument* pDoc)
{
	XMLElement* pBaseElement = pDoc->NewElement(VGetName());

    // shape
	XMLElement* pShape = pDoc->NewElement("Shape");
	XMLText* pShapeText = pDoc->NewText(m_shape.c_str());
    pShape->LinkEndChild(pShapeText);
	pBaseElement->LinkEndChild(pShape);

    // density
	XMLElement* pDensity = pDoc->NewElement("Density");
	XMLText* pDensityText = pDoc->NewText(m_density.c_str());
    pDensity->LinkEndChild(pDensityText);
    pBaseElement->LinkEndChild(pDensity);

    // material
	XMLElement* pMaterial = pDoc->NewElement("Material");
	XMLText* pMaterialText = pDoc->NewText(m_material.c_str());
    pMaterial->LinkEndChild(pMaterialText);
    pBaseElement->LinkEndChild(pMaterial);

    // rigid body transform
	XMLElement* pInitialTransform = pDoc->NewElement("RigidBodyTransform");

    // initial transform -> position
	XMLElement* pPosition = pDoc->NewElement("Position");
    pPosition->SetAttribute("x", ToStr(m_RigidBodyLocation.x).c_str());
    pPosition->SetAttribute("y", ToStr(m_RigidBodyLocation.y).c_str());
    pPosition->SetAttribute("z", ToStr(m_RigidBodyLocation.z).c_str());
    pInitialTransform->LinkEndChild(pPosition);

    // initial transform -> orientation
	XMLElement* pOrientation = pDoc->NewElement("Orientation");
    pOrientation->SetAttribute("yaw", ToStr(m_RigidBodyOrientation.x).c_str());
    pOrientation->SetAttribute("pitch", ToStr(m_RigidBodyOrientation.y).c_str());
    pOrientation->SetAttribute("roll", ToStr(m_RigidBodyOrientation.z).c_str());
    pInitialTransform->LinkEndChild(pOrientation);

	// initial transform -> scale 
	XMLElement* pScale = pDoc->NewElement("Scale");
    pScale->SetAttribute("x", ToStr(m_RigidBodyScale.x).c_str());
    pScale->SetAttribute("y", ToStr(m_RigidBodyScale.y).c_str());
    pScale->SetAttribute("z", ToStr(m_RigidBodyScale.z).c_str());
    pInitialTransform->LinkEndChild(pScale);

    pBaseElement->LinkEndChild(pInitialTransform);

    return pBaseElement;
}

void PhysicsComponent::VPostInit(void)
{
    if (m_pOwner)
    {
		if (m_shape == "Sphere")
		{
			m_pGamePhysics->VAddSphere((float)m_RigidBodyScale.x, m_pOwner, m_density, m_material);
		}
		else if (m_shape == "Box")
		{
			m_pGamePhysics->VAddBox(m_RigidBodyScale, m_pOwner, m_density, m_material);
		}
		else if (m_shape == "PointCloud")
		{
			LOG_ERROR("Not supported yet!");
		}
	}
}

void PhysicsComponent::VUpdate(int deltaMs)
{
    // get the transform component
    shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_Name));
    if (!pTransformComponent)
    {
		LOG_ERROR("No transform component!");
        return;
    }

    // get the direction the object is facing
    mat4 transform = pTransformComponent->GetTransform();

	if (m_acceleration != 0)
    {
        // calculate the acceleration this frame

        float accelerationToApplyThisFrame = m_acceleration / 1000.f * (float)deltaMs;

        // Get the current velocity vector and convert to a scalar.  The velocity vector is a combination of 
        // the direction this actor is going in and the speed of the actor.  The scalar is just the speed 
        // component.
        vec3 velocity(m_pGamePhysics->VGetVelocity(m_pOwner->GetId()));
		float velocityScalar = length(velocity);

		vec3 direction(GetDirection(transform));
		m_pGamePhysics->VApplyForce(direction, accelerationToApplyThisFrame, m_pOwner->GetId());

        // logging
        // [rez] Comment this back in if you want to debug the physics thrust & rotation stuff.  It spams quite 
        // a bit of info the output window so I'm commenting it out for now.
/*
        GCC_LOG("Actor", "Acceleration: " + ToStr(accelerationToApplyThisFrame) + "; velocityScalar: " + ToStr(velocityScalar) + \
                "; direction: " + ToStr(direction) + "; direction.Length(): " + ToStr(direction.Length()) + \
                "; velocity: " + ToStr(velocity) + "; velocity.Length(): " + ToStr(velocity.Length()));
*/
    }

    if (m_angularAcceleration != 0)
    {
        // calculate the acceleration this frame
        float angularAccelerationToApplyThisFrame = m_angularAcceleration / 1000.f * (float)deltaMs;
		m_pGamePhysics->VApplyTorque(GetUp(transform), angularAccelerationToApplyThisFrame, m_pOwner->GetId());

        // logging
        // [rez] Comment this back in if you want to debug the physics thrust & rotation stuff.  It spams quite 
        // a bit of info the output window so I'm commenting it out for now.
        //GCC_LOG("Actor", "Angular Acceleration: " + ToStr(angularAccelerationToApplyThisFrame) );
    }
}

void PhysicsComponent::BuildRigidBodyTransform(XMLElement* pTransformElement)
{
	// FUTURE WORK Mrmike - this should be exactly the same as the TransformComponent - maybe factor into a helper method?
    LOG_ASSERT(pTransformElement);

    XMLElement* pPositionElement = pTransformElement->FirstChildElement("Position");
    if (pPositionElement)
    {
        double x = 0;
        double y = 0;
        double z = 0;
		x = std::stod(pPositionElement->Attribute("x"));
		y = std::stod(pPositionElement->Attribute("y"));
		z = std::stod(pPositionElement->Attribute("z"));
		m_RigidBodyLocation = vec3(x, y, z);
    }

    XMLElement* pOrientationElement = pTransformElement->FirstChildElement("Orientation");
    if (pOrientationElement)
    {
        double yaw = 0;
        double pitch = 0;
        double roll = 0;
		yaw   = std::stod(pOrientationElement->Attribute("yaw"));
		pitch = std::stod(pOrientationElement->Attribute("pitch"));
		roll  = std::stod(pOrientationElement->Attribute("roll"));
        m_RigidBodyOrientation = vec3((float)DEGREES_TO_RADIANS(yaw), (float)DEGREES_TO_RADIANS(pitch), (float)DEGREES_TO_RADIANS(roll));
    }

    XMLElement* pScaleElement = pTransformElement->FirstChildElement("Scale");
    if (pScaleElement)
    {
        double x = 0;
        double y = 0;
		double z = 0;
		x = std::stod(pScaleElement->Attribute("x"));
		y = std::stod(pScaleElement->Attribute("y"));
		z = std::stod(pScaleElement->Attribute("z"));
        m_RigidBodyScale = vec3((float)x, (float)y, (float)z);
    }
}

void PhysicsComponent::ApplyForce(const vec3& direction, float forceNewtons)
{
    m_pGamePhysics->VApplyForce(direction, forceNewtons, m_pOwner->GetId());
}

void PhysicsComponent::ApplyTorque(const vec3& direction, float forceNewtons)
{
    m_pGamePhysics->VApplyTorque(direction, forceNewtons, m_pOwner->GetId());
}

bool PhysicsComponent::KinematicMove(const mat4 &transform)
{
	return m_pGamePhysics->VKinematicMove(transform, m_pOwner->GetId());
}

void PhysicsComponent::ApplyAcceleration(float acceleration)
{
    m_acceleration = acceleration;
}

void PhysicsComponent::RemoveAcceleration(void)
{
    m_acceleration = 0;
}

void PhysicsComponent::ApplyAngularAcceleration(float acceleration)
{
    m_angularAcceleration = acceleration;
}

void PhysicsComponent::RemoveAngularAcceleration(void)
{
    m_angularAcceleration = 0;
}

vec3 PhysicsComponent::GetVelocity(void)
{
    return m_pGamePhysics->VGetVelocity(m_pOwner->GetId());
}

void PhysicsComponent::SetVelocity(const vec3& velocity)
{
    m_pGamePhysics->VSetVelocity(m_pOwner->GetId(), velocity);
}

void PhysicsComponent::RotateY(float angleRadians)
{
    shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_Name));
    if (pTransformComponent)
    {
        mat4 transform = pTransformComponent->GetTransform();
		vec3 position = GetPosition(transform);

        mat4 rotateY;
        rotateY = rotate(rotateY, angleRadians, g_YAxis);
		::SetPosition(rotateY, position);

        KinematicMove(rotateY);
    }
    else
        LOG_ERROR("Attempting to call RotateY() on actor with no transform component");
}

void PhysicsComponent::SetPosition(float x, float y, float z)
{
    shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_Name));
    if (pTransformComponent)
    {
        mat4 transform = pTransformComponent->GetTransform();
        vec3 position = vec3(x, y, z);
        ::SetPosition(transform, position);

        KinematicMove(transform);
    }
    else
        LOG_ERROR("Attempting to call RotateY() on actor with no trnasform component");
}

void PhysicsComponent::Stop(void)
{
    return m_pGamePhysics->VStopActor(m_pOwner->GetId());
}

