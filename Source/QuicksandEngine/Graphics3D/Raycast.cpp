#include "../Stdafx.hpp"
#include "Raycast.hpp"

#include <glm/gtx/intersect.hpp>
#include "Mesh.hpp"
#include "Scene.hpp"
#include "Raycast.hpp"
#include "SceneNode.hpp"

template <class T>
void InitIntersection(Intersection &intersection, DWORD faceIndex, FLOAT dist, FLOAT u, FLOAT v, ActorId actorId, WORD* pIndices, T* pVertices, const glm::mat4 &matWorld)
{
	intersection.m_dwFace = faceIndex;
	intersection.m_fDist = dist;
	intersection.m_fBary1 = u;
	intersection.m_fBary2 = v;

	T v0 = pVertices[pIndices[3 * faceIndex + 0]];
	T v1 = pVertices[pIndices[3 * faceIndex + 1]];
	T v2 = pVertices[pIndices[3 * faceIndex + 2]];

	// If all you want is the vertices hit, then you are done.  In this sample, we
	// want to show how to infer texture coordinates as well, using the BaryCentric
	// coordinates supplied by D3DXIntersect
	/*FLOAT dtu1 = v1->tu - v0->tu;
	FLOAT dtu2 = v2->tu - v0->tu;
	FLOAT dtv1 = v1->tv - v0->tv;
	FLOAT dtv2 = v2->tv - v0->tv;
	intersection.m_tu = v0->tu + intersection.m_fBary1 * dtu1 + intersection.m_fBary2 * dtu2;
	intersection.m_tv = v0->tv + intersection.m_fBary1 * dtv1 + intersection.m_fBary2 * dtv2;*/

	glm::vec3 a = v0 - v1;
	glm::vec3 b = v2 - v1;

	glm::vec3 cross = glm::cross(a, b);
	cross /= glm::length(cross);

	glm::vec3 actorLoc = BarycentricTovec3(v0, v1, v2, intersection.m_fBary1, intersection.m_fBary2);
	intersection.m_actorLoc = actorLoc;
	intersection.m_worldLoc = Xform(matWorld, actorLoc);
	intersection.m_actorId = actorId;
	intersection.m_normal = cross;
}



RayCast::RayCast(Point point, DWORD maxIntersections)
{
	m_MaxIntersections = maxIntersections;
	m_IntersectionArray.reserve(m_MaxIntersections);
	m_bUseD3DXIntersect = true;
	m_bAllHits = true;
	m_NumIntersections = 0;
	m_Point = point;
}

HRESULT RayCast::Pick(Scene *pScene, ActorId actorId, GLUFMeshBarebones *pMesh)
{
	if (!m_bAllHits && m_NumIntersections > 0)
		return S_OK;

	HRESULT hr;


	// Get the inverse view matrix
	const glm::mat4 matView = pScene->GetCamera()->GetView();
	const glm::mat4 matWorld = pScene->GetTopMatrix();
	const glm::mat4 proj = pScene->GetCamera()->GetProjection();

	// Compute the vector of the Pick ray in screen space
	glm::vec3 v;
	v.x = (((2.0f * m_Point.x) / QuicksandEngine::g_pApp->GetScreenSize().x) - 1) / proj[1][1];
	v.y = -(((2.0f * m_Point.y) / QuicksandEngine::g_pApp->GetScreenSize().y) - 1) / proj[2][2];
	v.z = 1.0f;


	glm::mat4 mWorldView = matWorld * matView;
	glm::mat4 m = glm::inverse(mWorldView);
	

	// Transform the screen space Pick ray into 3D space
	m_vPickRayDir.x = v.x * m[1][1] + v.y * m[2][1] + v.z * m[3][1];
	m_vPickRayDir.y = v.x * m[1][2] + v.y * m[2][2] + v.z * m[3][2];
	m_vPickRayDir.z = v.x * m[1][3] + v.y * m[2][3] + v.z * m[3][3];
	m_vPickRayOrig.x = m[4][1];
	m_vPickRayOrig.y = m[4][2];
	m_vPickRayOrig.z = m[4][3];

	Vec3Array pVB = pMesh->mVertices;
	IndexArray pIB = pMesh->mIndices;
	//TODO: READ THIS NOW: i must calculate my own intersect method

	DWORD intersections = 0;

	// When calling D3DXIntersect, one can get just the closest intersection and not
	// need to work with a D3DXBUFFER.  Or, to get all intersections between the ray and 
	// the Mesh, one can use a D3DXBUFFER to receive all intersections.  We show both
	// methods.
	if (!m_bAllHits)
	{
		// Collect only the closest intersection
		BOOL bHit;
		DWORD dwFace;
		FLOAT fBary1, fBary2, fDist;
		//Why is this an error
		RayIntersect(*pMesh, m_vPickRayOrig, m_vPickRayDir, &bHit, &dwFace, &fBary1, &fBary2, &fDist);
		if (bHit)
		{
			m_NumIntersections = 1;
			m_IntersectionArray.resize(1);
			InitIntersection(m_IntersectionArray[0], dwFace, fDist, fBary1, fBary2, actorId, &pIB[0], &pVB[0], matWorld);
		}
		else
		{
			m_NumIntersections = 0;
		}
	}
	else
	{
		// Collect all intersections
		BOOL bHit;
		IntersectionArray pBuffer;
		if (!RayIntersect(*pMesh, m_vPickRayOrig, m_vPickRayDir, &bHit, nullptr, nullptr, nullptr, nullptr,
			&pBuffer, &intersections))
		{
			return hr;
		}
		if (intersections > 0)
		{
			m_IntersectionArray.resize(m_NumIntersections + intersections);
			

			if (m_NumIntersections > m_MaxIntersections)
				m_NumIntersections = m_MaxIntersections;

			for (DWORD i = 0; i < intersections; i++)
			{
				Intersection* pIntersection;
				pIntersection = &m_IntersectionArray[i + m_NumIntersections];

				InitIntersection(*pIntersection, pBuffer[i].m_dwFace,
					pBuffer[i].m_fDist,
					pBuffer[i].m_fBary1,
					pBuffer[i].m_fBary2,
					actorId, pIB.data(), pVB.data(), matWorld);
			}
		}
	}

	m_NumIntersections += intersections;

	return S_OK;
}




HRESULT RayCast::Pick(Scene *pScene, ActorId actorId, Vec3Array pVB, GLIndexArray pIB)
{
	if (!m_bAllHits && m_NumIntersections > 0)
		return S_OK;



	// Get the inverse view matrix
	const glm::mat4 matView = pScene->GetCamera()->GetView();
	const glm::mat4 matWorld = pScene->GetTopMatrix();
	const glm::mat4 proj = pScene->GetCamera()->GetProjection();

	// Compute the vector of the Pick ray in screen space
	glm::vec3 v;
	v.x = (((2.0f * m_Point.x) / QuicksandEngine::g_pApp->GetScreenSize().x) - 1) / proj[1][1];
	v.y = -(((2.0f * m_Point.y) / QuicksandEngine::g_pApp->GetScreenSize().y) - 1) / proj[2][2];
	v.z = 1.0f;


	glm::mat4 mWorldView = matWorld * matView;
	glm::mat4 m;
	m = glm::inverse(mWorldView);

	// Transform the screen space Pick ray into 3D space
	m_vPickRayDir.x = v.x * m[1][1] + v.y * m[2][1] + v.z * m[3][1];
	m_vPickRayDir.y = v.x * m[1][2] + v.y * m[2][2] + v.z * m[3][2];
	m_vPickRayDir.z = v.x * m[1][3] + v.y * m[2][3] + v.z * m[3][3];
	m_vPickRayOrig.x = m[4][1];
	m_vPickRayOrig.y = m[4][2];
	m_vPickRayOrig.z = m[4][3];

	FLOAT fBary1, fBary2;
	FLOAT fDist;
	for (DWORD i = 0; i < pIB.size(); i++)
	{
		glm::vec3 v0 = pVB[pIB[3 * i + 0]];
		glm::vec3 v1 = pVB[pIB[3 * i + 1]];
		glm::vec3 v2 = pVB[pIB[3 * i + 2]];

		// Check if the Pick ray passes through this point
		if (IntersectTriangle(m_vPickRayOrig, m_vPickRayDir, v0, v1, v2,
			&fDist, &fBary1, &fBary2))
		{
			if (m_bAllHits || m_NumIntersections == 0 || fDist < m_IntersectionArray[0].m_fDist)
			{

				if (!m_bAllHits)
					m_NumIntersections = 0;

				++m_NumIntersections;

				m_IntersectionArray.resize(m_NumIntersections);

				Intersection* pIntersection;
				pIntersection = &m_IntersectionArray[m_NumIntersections - 1];

				InitIntersection(*pIntersection, i, fDist, fBary1, fBary2, actorId, pIB.data(), pVB.data(), matWorld);

				if (m_NumIntersections == m_MaxIntersections)
					break;
			}
		}
	}

	return S_OK;
}




void RayCast::Sort()
{
	std::sort(m_IntersectionArray.begin(), m_IntersectionArray.end());
}




bool RayIntersect(GLUFMeshBarebones mesh, glm::vec3 rayPos, glm::vec3 rayDir, BOOL *hit, DWORD *pFaceIndex, GLfloat *baryU, GLfloat *baryV, GLfloat *pDist, IntersectionArray* pAllHits, DWORD *pCountOfHits)
{
	bool hasSucceeded = false;
	bool hasSucceeded1 = false;
	for (unsigned int i = 0; i < mesh.mVertices.size() && hasSucceeded == false; i += 3)
	{
		glm::vec3 baryTmp;
		if (glm::intersectRayTriangle(rayPos, rayDir, mesh.mVertices[mesh.mIndices[i]], mesh.mVertices[mesh.mIndices[i + 1]], mesh.mVertices[mesh.mIndices[i + 2]], baryTmp))
		{
			//only collect the other data from the first hit
			if (!hasSucceeded1)
			{
				hasSucceeded1 = true;
			}


			(hit) ? *hit = TRUE : true;
			(pCountOfHits) ? *pCountOfHits++ : true;
			(baryU) ? *baryU = baryTmp.x : false;
			(baryV) ? *baryV = baryTmp.y : false;
			(pFaceIndex) ? *pFaceIndex = i / 3 : true;

			//for the distance get the distance from the furthest vertex on the face
			float *dist0 = new float(0.0f);
			float dist1 = glm::length(mesh.mVertices[mesh.mIndices[i]] - rayPos);
			float dist2 = glm::length(mesh.mVertices[mesh.mIndices[i + 1]] - rayPos);
			float dist3 = glm::length(mesh.mVertices[mesh.mIndices[i + 2]] - rayPos);

			if (dist1 >= dist2)
			{
				if (dist1 >= dist3)
				{
					*dist0 = dist1;
				}
				else
				{
					*dist0 = dist3;
				}
			}
			else
			{
				if (dist2 >= dist3)
				{
					*dist0 = dist2;
				}
				else
				{
					*dist0 = dist3;
				}
			}

			(pDist) ? *pDist = *dist0 : true;

			if (!pAllHits)
			{
				hasSucceeded = true;
			}
			else
			{
				Intersection intInfo;
				intInfo.m_fDist = *dist0;
				intInfo.m_dwFace = i / 3;
				intInfo.m_fBary1 = baryTmp.x;
				intInfo.m_fBary2 = baryTmp.y;
				pAllHits->push_back(intInfo);
			}
		}
	}

	//always return true
	return true;
}