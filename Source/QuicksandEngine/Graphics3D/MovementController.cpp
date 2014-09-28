#include "../Stdafx.hpp"
#include "Geometry.hpp"
#include "MovementController.hpp"
#include "SceneNode.hpp"

////////////////////////////////////////////////////
// MovementController Implementation			- Chapter 10, page 282
////////////////////////////////////////////////////

#define MAX(a, b) ((a) >= (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

MovementController::MovementController(shared_ptr<SceneNode> object, float initialYaw, float initialPitch, bool rotateWhenLButtonDown)
	: m_object(object)
{
	m_object->VGet()->Transform(&m_matToWorld, &m_matFromWorld);

	m_fTargetYaw = m_fYaw = RADIANS_TO_DEGREES(-initialYaw);
	m_fTargetPitch = m_fPitch = RADIANS_TO_DEGREES(initialPitch);

	m_maxSpeed = 30.0f;			// 30 meters per second
	m_currentSpeed = 0.0f;

	glm::vec3 pos = GetPosition(m_matToWorld);

	m_matPosition = glm::translate(m_matPosition, pos);

	POINT ptCursor;
	GetCursorPos(&ptCursor);
	m_lastMousePos = ptCursor;

	memset(m_bKey, 0x00, sizeof(m_bKey));

	m_mouseLButtonDown = false;
	m_bRotateWhenLButtonDown = rotateWhenLButtonDown;
}

//
// MovementController::VOnPointerButtonDown		- Chapter 10, page 282
//
bool MovementController::VOnPointerButtonDown(const Point &mousePos, const int radius, const std::string &buttonName)
{
	if (buttonName == "PointerLeft")
	{
		m_mouseLButtonDown = true;

		// We want mouse movement to be relative to the position
		// the cursor was at when the user first presses down on
		// the left button
		m_lastMousePos = mousePos;
		return true;
	}
	return false;
}

bool MovementController::VOnPointerButtonUp(const Point &mousePos, const int radius, const std::string &buttonName)
{
	if (buttonName == "PointerLeft")
	{
		m_mouseLButtonDown = false;
		return true;
	}
	return false;
}


//  class MovementController::VOnMouseMove		- Chapter 10, page 282

bool MovementController::VOnPointerMove(const Point &mousePos, const int radius)
{
	// There are two modes supported by this controller.

	if (m_bRotateWhenLButtonDown)
	{
		// Mode 1 - rotate the view only when the left mouse button is down
		// Only look around if the left button is down
		if (m_lastMousePos != mousePos && m_mouseLButtonDown)
		{
			m_fTargetYaw = m_fTargetYaw + (m_lastMousePos.x - mousePos.x);
			m_fTargetPitch = m_fTargetPitch + (mousePos.y - m_lastMousePos.y);
			m_lastMousePos = mousePos;
		}
	}
	else if (m_lastMousePos != mousePos)
	{
		// Mode 2 - rotate the controller when the mouse buttons are up
		m_fTargetYaw = m_fTargetYaw + (m_lastMousePos.x - mousePos.x);
		m_fTargetPitch = m_fTargetPitch + (mousePos.y - m_lastMousePos.y);
		m_lastMousePos = mousePos;
	}

	return true;
}

//  class MovementController::OnUpdate			- Chapter 10, page 283

void MovementController::OnUpdate(double const fEllapsed)
{
	//if (m_bKey['Q'])
	//{
	//	// This code is a cheat to position the camera exactly in a given
	//	// spot so I can take screen shots!

	//	glm::mat4 camTranslate;
	//	D3DXMatrixTranslation(&m_matPosition, 8.847f, 7.055f, 11.618f);

	//	m_fTargetYaw = m_fYaw += -64.35f;
	//	m_fTargetPitch = m_fPitch = 28.57f;

	//	// Calculate the new rotation matrix from the camera
	//	// yaw and pitch.
	//	glm::mat4 matRot;
	//	D3DXMatrixRotationYawPitchRoll(&matRot, DEGREES_TO_RADIANS(m_fYaw), DEGREES_TO_RADIANS(m_fPitch), 0);

	//	// Create the new object-to-world matrix, and the
	//	// new world-to-object matrix. 
	//	D3DXMatrixMultiply(&m_matToWorld, &matRot, &m_matPosition);
	//	D3DXMatrixInverse(&m_matFromWorld, NULL, &m_matToWorld);

	//	m_object->VSetTransform(&m_matToWorld, &m_matFromWorld);
	//	return;
	//}

	bool bTranslating = false;
	glm::vec4 atWorld(0, 0, 0, 0);
	glm::vec4 rightWorld(0, 0, 0, 0);
	glm::vec4 upWorld(0, 0, 0, 0);

	if (m_bKey['W'] || m_bKey['S'])
	{
		// In D3D, the "look at" default is always
		// the positive Z axis.
		glm::vec4 at = g_Forward4;
		if (m_bKey['S'])
			at *= -1;

		// This will give us the "look at" vector 
		// in world space - we'll use that to move
		// the camera.
		atWorld = Xform(m_matToWorld,at);
		bTranslating = true;
	}

	if (m_bKey['A'] || m_bKey['D'])
	{
		// In D3D, the "right" default is always
		// the positive X axis.
		glm::vec4 right = g_Right4;
		if (m_bKey['A'])
			right *= -1;

		// This will give us the "right" vector 
		// in world space - we'll use that to move
		// the camera
		rightWorld = Xform(m_matToWorld,right);
		bTranslating = true;
	}

	if (m_bKey[' '] || m_bKey['C'] || m_bKey['X'])
	{
		// In D3D, the "up" default is always
		// the positive Y axis.
		glm::vec4 up = g_Right4;
		if (!m_bKey[' '])
			up *= -1;

		//Unlike strafing, Up is always up no matter
		//which way you are looking
		upWorld = up;
		bTranslating = true;
	}

	//Handling rotation as a result of mouse position
	{
		// The secret formula!!! Don't give it away!
		//If you are seeing this now, then you must be some kind of elite hacker!
		m_fYaw += (m_fTargetYaw - m_fYaw) * (.35f);
		m_fTargetPitch = std::clamp(m_fTargetPitch, -90.0f, 90.0f);
		m_fPitch += (m_fTargetPitch - m_fPitch) * (.35f);

		// Calculate the new rotation matrix from the camera
		// yaw and pitch.
		glm::mat4 matRot;
		BuildYawPitchRoll(matRot, DEGREES_TO_RADIANS(-m_fYaw), DEGREES_TO_RADIANS(m_fPitch), 0);

		// Create the new object-to-world matrix, and the
		// new world-to-object matrix. 

		m_matToWorld = matRot * m_matPosition;
		m_matFromWorld = glm::inverse(m_matToWorld);
		m_object->VSetTransform(&m_matToWorld, &m_matFromWorld);
	}

	if (bTranslating)
	{
		float elapsedTime = (float)fEllapsed / 1000.0f;

		glm::vec3 direction = glm::vec3(atWorld + rightWorld + upWorld);
		direction = glm::normalize(direction);

		// Ramp the acceleration by the elapsed time.
		float numberOfSeconds = 5.f;
		m_currentSpeed += m_maxSpeed * ((elapsedTime*elapsedTime) / numberOfSeconds);
		if (m_currentSpeed > m_maxSpeed)
			m_currentSpeed = m_maxSpeed;

		direction *= m_currentSpeed;

		glm::vec3 pos = GetPosition(m_matPosition) + direction;
		SetPosition(m_matPosition,pos);
		SetPosition(m_matToWorld, pos);

		m_matFromWorld = glm::inverse(m_matToWorld);
		m_object->VSetTransform(&m_matToWorld, &m_matFromWorld);
	}
	else
	{
		m_currentSpeed = 0.0f;
	}
}

