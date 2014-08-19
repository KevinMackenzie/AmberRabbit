#include "../Stdafx.hpp"
#include "TransformComponent.hpp"
#include "../Utilities/String.hpp"

const char* TransformComponent::g_Name = "TransformComponent";

bool TransformComponent::VInit(tinyxml2::XMLElement* pData)
{
    LOG_ASSERT(pData);

	// [mrmike] - this was changed post-press - because changes to the TransformComponents can come in partial definitions,
	//            such as from the editor, its better to grab the current values rather than clear them out.
    
	glm::vec3 yawPitchRoll = GetYawPitchRoll(m_transform);
	yawPitchRoll.x = RADIANS_TO_DEGREES(yawPitchRoll.x);
	yawPitchRoll.y = RADIANS_TO_DEGREES(yawPitchRoll.y);
	yawPitchRoll.z = RADIANS_TO_DEGREES(yawPitchRoll.z);

	glm::vec3 position = ::GetPosition(m_transform);

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
		yaw = std::stod(pOrientationElement->Attribute("yaw"));
		pitch = std::stod(pOrientationElement->Attribute("pitch"));
		roll = std::stod(pOrientationElement->Attribute("roll"));
		yawPitchRoll = glm::vec3(yaw, pitch, roll);
	}

	glm::mat4 translation = translate(translation, position);

	glm::mat4 rotation;
	quat tmpQuat = quat(glm::vec3((float)DEGREES_TO_RADIANS(yawPitchRoll.x), (float)DEGREES_TO_RADIANS(yawPitchRoll.y), (float)DEGREES_TO_RADIANS(yawPitchRoll.z)));
	rotation = rotate(rotation, tmpQuat.w, glm::vec3(tmpQuat.x, tmpQuat.y, tmpQuat.z));

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

    m_transform = rotation * translation;
    
    return true;
}

tinyxml2::XMLElement* TransformComponent::VGenerateXml(tinyxml2::XMLDocument* pDoc)
{
	tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(VGetName());

    // initial transform -> position
	tinyxml2::XMLElement* pPosition = pDoc->NewElement("Position");
    glm::vec3 pos(::GetPosition(m_transform));
    pPosition->SetAttribute("x", ToStr(pos.x).c_str());
    pPosition->SetAttribute("y", ToStr(pos.y).c_str());
    pPosition->SetAttribute("z", ToStr(pos.z).c_str());
    pBaseElement->LinkEndChild(pPosition);

    // initial transform -> LookAt
	tinyxml2::XMLElement* pDirection = pDoc->NewElement("YawPitchRoll");
	glm::vec3 orient(GetYawPitchRoll(m_transform));
	orient.x = RADIANS_TO_DEGREES(orient.x);
	orient.y = RADIANS_TO_DEGREES(orient.y);
	orient.z = RADIANS_TO_DEGREES(orient.z);
    pDirection->SetAttribute("x", ToStr(orient.x).c_str());
    pDirection->SetAttribute("y", ToStr(orient.y).c_str());
    pDirection->SetAttribute("z", ToStr(orient.z).c_str());
    pBaseElement->LinkEndChild(pDirection);

	/***
	// This is not supported yet
    // initial transform -> position
    tinyxml2::XMLElement* pScale = QSE_NEW tinyxml2::XMLElement("Scale");
    pPosition->SetAttribute("x", ToStr(m_scale.x).c_str());
    pPosition->SetAttribute("y", ToStr(m_scale.y).c_str());
    pPosition->SetAttribute("z", ToStr(m_scale.z).c_str());
    pBaseElement->LinkEndChild(pScale);
	**/

    return pBaseElement;
}



