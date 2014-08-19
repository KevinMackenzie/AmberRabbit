#ifndef QSE_RAYCAST_HPP
#define QSE_RAYCAST_HPP

#include "../Stdafx.hpp"
#include "MovementController.hpp"

#include "Geometry.hpp"
#include "Mesh.hpp"

class Intersection
{
public:
	FLOAT m_fDist;                  // distance from ray origin to intersection
	DWORD m_dwFace;					// the face index of the intersection
	FLOAT m_fBary1, m_fBary2;		// Barycentric coordinates of the intersection
	//FLOAT m_tu, m_tv;               // texture coords of intersection
	ActorId m_actorId;				// Which actor was intersected if there was one
	glm::vec3 m_worldLoc;				// world location of intersection
	glm::vec3 m_actorLoc;				// actor local coordinates of intersection
	glm::vec3 m_normal;					// normal of intersection

	bool const operator <(Intersection const &other) { return m_fDist < other.m_fDist; }
};

template <class T>
void InitIntersection(Intersection &intersection, DWORD faceIndex, FLOAT dist, FLOAT u, FLOAT v, ActorId actorId, char32_t* pIndices, T* pVertices, const glm::mat4 &matWorld);


typedef std::vector<Intersection> IntersectionArray;

//the distance between rayPos and rayDir MUST be of unit length and the mesh MUST have indexes, AND must be divisible by three
bool RayIntersect(GLUFMeshBarebones mesh, glm::vec3 rayPos, glm::vec3 rayDir, BOOL *hit, DWORD *pFaceIndex, GLfloat *baryU, GLfloat *baryV, GLfloat *pDist, IntersectionArray* pAllHits = nullptr, DWORD *pCountOfHits = nullptr);




class RayCast
{
protected:
	GLUFMeshBarebones m_pVB;

public:
	RayCast(Point point, DWORD maxIntersections = 16);

	DWORD m_MaxIntersections;
	DWORD m_NumIntersections;
	bool m_bUseD3DXIntersect;       // Whether to use D3DXIntersect
	bool m_bAllHits;				// Whether to just get the first "hit" or all "hits"
	Point m_Point;

	glm::vec3 m_vPickRayDir;
	glm::vec3 m_vPickRayOrig;

	IntersectionArray m_IntersectionArray;

	HRESULT Pick(Scene *pScene, ActorId actorId, GLUFMeshBarebones *pMesh);

	HRESULT Pick(Scene *pScene, ActorId actorId, Vec3Array pVerts, GLIndexArray pIndices);

	void Sort();
};

#endif