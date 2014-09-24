#include "../Stdafx.hpp"
#include "TransformComponent.hpp"
#include "../Utilities/String.hpp"

const char* TransformComponent::g_Name = "TransformComponent";

bool TransformComponent::VInit(tinyxml2::XMLElement* pData)
{
    LOG_ASSERT(pData);

	// [mrmike] - this was changed post-press - because changes to the TransformComponents can come in partial definitions,
	//            such as from the editor, its better to grab the current values rather than clear them out.
    
	/*glm::vec3 yawPitchRoll = GetYawPitchRoll(m_transform);
	yawPitchRoll.x = RADIANS_TO_DEGREES(yawPitchRoll.x);
	yawPitchRoll.y = RADIANS_TO_DEGREES(yawPitchRoll.y);
	yawPitchRoll.z = RADIANS_TO_DEGREES(yawPitchRoll.z);*/

	glm::vec3 position = ::GetPosition(m_transform);
	glm::mat4 rotation = ::GetRotMat(m_transform);

    tinyxml2::XMLElement* pPositionElement = pData->FirstChildElement("Position");
    if (pPositionElement)
    {
        double x = 0;
        double y = 0;
		double z = 0;
		x = std::stod(pPositionElement->Attribute("x"));
		y = std::stod(pPositionElement->Attribute("y"));
		z = std::stod(pPositionElement->Attribute("z"));
		position = glm::vec3(x, y, z);

    }

    tinyxml2::XMLElement* pOrientationElement = pData->FirstChildElement("YawPitchRoll");
    if (pOrientationElement)
    {
        double yaw = 0;
        double pitch = 0;
		double roll = 0;
		yaw = std::stod(pOrientationElement->Attribute("x"));
		pitch = std::stod(pOrientationElement->Attribute("y"));
		roll = std::stod(pOrientationElement->Attribute("z"));
		glm::vec3 eulers = glm::vec3(yaw, pitch, roll);

		glm::quat tmpQuat = glm::quat(glm::vec3((float)DEGREES_TO_RADIANS(eulers.x), (float)DEGREES_TO_RADIANS(eulers.y), (float)DEGREES_TO_RADIANS(eulers.z)));
		rotation = glm::toMat4(tmpQuat);
	}

	/**
	// This is not supported yet.
    tinyxml2::XMLElement* pLookAtElement = pData->FirstChildElement("LookAt");
    if (pLookAtElement)
    {
        double x = 0;
        double y = 0;
        double z = 0;
        pLookAtElement->Attribute("x", &x);
        pLookAtElement->Attribute("y", &y);
        pLookAtElement->Attribute("z", &z);

		glm::vec3 lookAt((float)x, (float)y, (float)z);
		rotation.BuildRotationLookAt(translation.GetPosition(), lookAt, g_Up);
    }

    tinyxml2::XMLElement* pScaleElement = pData->FirstChildElement("Scale");
    if (pScaleElement)
    {
        double x = 0;
        double y = 0;
        double z = 0;
        pScaleElement->Attribute("x", &x);
        pScaleElement->Attribute("y", &y);
        pScaleElement->Attribute("z", &z);
        m_scale = glm::vec3((float)x, (float)y, (float)z);
    }
	**/

	m_transform = glm::translate(glm::mat4(), position) * rotation;

    
    return true;
}

tinyxml2::XMLElement* TransformComponent::VGenerateXml(tinyxml2::XMLDocument* pDoc)
{
	tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(VGetName());

    // initial transform -> position
	tinyxml2::XMLElement* pPosition = pDoc->NewElement("Position");
	glm::vec3 pos = ::GetPosition(m_transform);
    pPosition->SetAttribute("x", ToStr(pos.x).c_str());
	pPosition->SetAttribute("y", ToStr(pos.y).c_str());
	pPosition->SetAttribute("z", ToStr(pos.z).c_str());
    pBaseElement->LinkEndChild(pPosition);

    // initial transform -> LookAt
	tinyxml2::XMLElement* pDirection = pDoc->NewElement("YawPitchRoll");
	glm::vec3 eulerDegs = ::GetYawPitchRoll(m_transform);
	eulerDegs.x = RADIANS_TO_DEGREES(eulerDegs.x);
	eulerDegs.y = RADIANS_TO_DEGREES(eulerDegs.y);
	eulerDegs.z = RADIANS_TO_DEGREES(eulerDegs.z);
	pDirection->SetAttribute("x", ToStr(eulerDegs.x).c_str());
	pDirection->SetAttribute("y", ToStr(eulerDegs.y).c_str());
	pDirection->SetAttribute("z", ToStr(eulerDegs.z).c_str());
    pBaseElement->LinkEndChild(pDirection);

    return pBaseElement;
}



