#include "../Stdafx.hpp"
#include "Geometry.hpp"

//========================================================================
// 
// Note - FVFs are part of the fixed function pipeline, and were described in
//        Game Coding Complete, 3rd Edition. 
//
// Our custom FVF, which describes our custom vertex structure
// These were #define'd in the book - now they are static constants.
//

/*const DWORD D3D9Vertex_UnlitColored::FVF =
(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_SPECULAR);
const DWORD D3D9Vertex_ColoredTextured::FVF = (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
const DWORD D3D9Vertex_Colored::FVF = (D3DFVF_XYZ | D3DFVF_DIFFUSE);
const DWORD D3D9Vertex_UnlitTextured::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

const mat4 mat4::g_Identity(D3DXMATRIX(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1));
const Quaternion Quaternion::g_Identity(D3DXQUATERNION(0, 0, 0, 1));*/

bool Plane::Inside(const vec3 &point) const
{
	// Inside the plane is defined as the direction the normal is facing
	float result = PlaneDotCoord(*this, point);
	return (result >= 0.0f);
}

bool Plane::Inside(const vec3 &point, const float radius) const
{
	float fDistance;	// calculate our distances to each of the planes

	// find the distance to this plane
	fDistance = PlaneDotCoord(*this, point);

	// if this distance is < -radius, we are outside
	return (fDistance >= -radius);
}

//
// Frustum::Frustum					- Chapter 14, page 477
//
Frustum::Frustum()
{
	m_Fov = AR_PI / 4.0f;		// default field of view is 90 degrees
	m_Aspect = 1.0f;			// default aspect ratio is 1:1
	m_Near = 1.0f;				// default near clip plane is 1m away from the camera
	m_Far = 1000.0f;				// default near clip plane is 100m away from the camera
}


//
// Frustum::Inside					- Chapter 14, page 477
//
bool Frustum::Inside(const vec3 &point) const
{
	for (int i = 0; i<NumPlanes; ++i)
	{
		if (!m_Planes[i].Inside(point))
			return false;
	}

	return true;
}


//
// Frustum::Inside					- Chapter 14, page 477
//
bool Frustum::Inside(const vec3 &point, const float radius) const
{
	for (int i = 0; i < NumPlanes; ++i)
	{
		if (!m_Planes[i].Inside(point, radius))
			return false;
	}

	// otherwise we are fully in view
	return(true);
}

//
// Frustum::Init					- Chapter 14, page 477
//
void Frustum::Init(const float fov, const float aspect, const float nearClip, const float farClip)
{
	m_Fov = fov;
	m_Aspect = aspect;
	m_Near = nearClip;
	m_Far = farClip;

	float tanFovOver2 = (float)tan(m_Fov / 2.0f);
	vec3 nearRight = (m_Near * tanFovOver2) * m_Aspect * g_Right;
	vec3 farRight = (m_Far * tanFovOver2) * m_Aspect * g_Right;
	vec3 nearUp = (m_Near * tanFovOver2) * g_Up;
	vec3 farUp = (m_Far * tanFovOver2)  * g_Up;

	// points start in the upper right and go around clockwise
	m_NearClip[0] = (m_Near * g_Forward) - nearRight + nearUp;
	m_NearClip[1] = (m_Near * g_Forward) + nearRight + nearUp;
	m_NearClip[2] = (m_Near * g_Forward) + nearRight - nearUp;
	m_NearClip[3] = (m_Near * g_Forward) - nearRight - nearUp;

	m_FarClip[0] = (m_Far * g_Forward) - farRight + farUp;
	m_FarClip[1] = (m_Far * g_Forward) + farRight + farUp;
	m_FarClip[2] = (m_Far * g_Forward) + farRight - farUp;
	m_FarClip[3] = (m_Far * g_Forward) - farRight - farUp;

	// now we have all eight points. Time to construct 6 planes.
	// the normals point away from you if you use counter clockwise verts.

	vec3 origin(0.0f, 0.0f, 0.0f);
	m_Planes[Near].Init(m_NearClip[2], m_NearClip[1], m_NearClip[0]);
	m_Planes[Far].Init(m_FarClip[0], m_FarClip[1], m_FarClip[2]);
	m_Planes[Right].Init(m_FarClip[2], m_FarClip[1], origin);
	m_Planes[Top].Init(m_FarClip[1], m_FarClip[0], origin);
	m_Planes[Left].Init(m_FarClip[0], m_FarClip[3], origin);
	m_Planes[Bottom].Init(m_FarClip[3], m_FarClip[2], origin);
}



//
// Frustum::Render					- not described in the 4th edition
//
void Frustum::Render()
{
	//TODO: create openGL way to rendering the frustum
	/*D3D9Vertex_Colored verts[24];
	for (int i = 0; i<8; ++i)
	{
		verts[i].color = g_White;
	}

	for (int i = 0; i<8; ++i)
	{
		verts[i + 8].color = g_Red;
	}

	for (int i = 0; i<8; ++i)
	{
		verts[i + 16].color = g_Blue;
	}


	// Draw the near clip plane
	verts[0].position = m_NearClip[0];	verts[1].position = m_NearClip[1];
	verts[2].position = m_NearClip[1];	verts[3].position = m_NearClip[2];
	verts[4].position = m_NearClip[2];	verts[5].position = m_NearClip[3];
	verts[6].position = m_NearClip[3];	verts[7].position = m_NearClip[0];

	// Draw the far clip plane
	verts[8].position = m_FarClip[0];	verts[9].position = m_FarClip[1];
	verts[10].position = m_FarClip[1];	verts[11].position = m_FarClip[2];
	verts[12].position = m_FarClip[2];	verts[13].position = m_FarClip[3];
	verts[14].position = m_FarClip[3];	verts[15].position = m_FarClip[0];

	// Draw the edges between the near and far clip plane
	verts[16].position = m_NearClip[0];	verts[17].position = m_FarClip[0];
	verts[18].position = m_NearClip[1];	verts[19].position = m_FarClip[1];
	verts[20].position = m_NearClip[2];	verts[21].position = m_FarClip[2];
	verts[22].position = m_NearClip[3];	verts[23].position = m_FarClip[3];

	DWORD oldLightMode;
	DXUTGetD3D9Device()->GetRenderState(D3DRS_LIGHTING, &oldLightMode);
	DXUTGetD3D9Device()->SetRenderState(D3DRS_LIGHTING, FALSE);

	DXUTGetD3D9Device()->SetFVF(D3D9Vertex_Colored::FVF);
	DXUTGetD3D9Device()->DrawPrimitiveUP(D3DPT_LINELIST, 12, verts, sizeof(D3D9Vertex_Colored));

	DXUTGetD3D9Device()->SetRenderState(D3DRS_LIGHTING, oldLightMode);*/
}


vec3 BarycentricTovec3(vec3 v0, vec3 v1, vec3 v2, float u, float v)
{
	//V1 + U(V2 - V1) + V(V3 - V1).
	vec3 result = v0 + u * (v1 - v0) + v * (v2 - v0);
	return result;
}


//--------------------------------------------------------------------------------------
// Given a ray origin (orig) and direction (dir), and three vertices of a triangle, this
// function returns TRUE and the interpolated texture coordinates if the ray intersects 
// the triangle
//--------------------------------------------------------------------------------------
bool IntersectTriangle(const vec3& orig, const vec3& dir,
	vec3& v0, vec3& v1, vec3& v2,
	GLfloat* t, GLfloat* u, GLfloat* v)
{
	// Find vectors for two edges sharing vert0
	vec3 edge1 = v1 - v0;
	vec3 edge2 = v2 - v0;

	// Begin calculating determinant - also used to calculate U parameter
	vec3 pvec = cross(dir, edge2);

	// If determinant is near zero, ray lies in plane of triangle
	GLfloat det = dot(edge1, pvec);

	vec3 tvec;
	if (det > 0)
	{
		tvec = orig - v0;
	}
	else
	{
		tvec = v0 - orig;
		det = -det;
	}

	if (det < 0.0001f)
		return FALSE;

	// Calculate U parameter and test bounds
	*u = dot(tvec, pvec);
	if (*u < 0.0f || *u > det)
		return FALSE;

	// Prepare to test V parameter
	vec3 qvec = cross(tvec, edge1);

	// Calculate V parameter and test bounds
	*v = dot(dir, qvec);
	if (*v < 0.0f || *u + *v > det)
		return FALSE;

	// Calculate t, scale parameters, ray intersects triangle
	*t = dot(edge2, qvec);
	GLfloat fInvDet = 1.0f / det;
	*t *= fInvDet;
	*u *= fInvDet;
	*v *= fInvDet;

	return TRUE;
}

void BuildYawPitchRoll(mat4& m, const float yawRadians, const float pitchRadians, const float rollRadians)
{

}