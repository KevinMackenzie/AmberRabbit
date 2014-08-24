#ifndef QSE_GEOMETRY_HPP
#define QSE_GEOMETRY_HPP


const float  AR_PI = 3.14159265358979f;
const float  AR_2PI = 2 * AR_PI;

////////////////////////////////////////////////////
//
// Plane Definition
//
////////////////////////////////////////////////////



class Plane
{
public:
	glm::vec3 normal;
	GLfloat distance;

	inline void normalize();

	// normal faces away from you if you send in verts in counter clockwise order....
	inline void Init(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2);
	bool Inside(const glm::vec3 &point, const float radius) const;
	bool Inside(const glm::vec3 &point) const;
};

inline void Plane::normalize()
{
	float mag;
	mag = glm::sqrt(glm::dot(normal,normal));
	normal /= mag;
	distance /= mag;
}

inline void Plane::Init(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2)
{
	normal = glm::cross(p1 - p0, p2 - p0);
	distance = glm::dot(normal, p0);
	normalize();
}

inline GLfloat PlaneDotCoord(const Plane &plane, const glm::vec3 &vec)
{
	return glm::dot(plane.normal, vec) + plane.distance;
}


////////////////////////////////////////////////////
//
// Frustum Definition
//
////////////////////////////////////////////////////

class Frustum
{
public:
	enum Side { Near, Far, Top, Right, Bottom, Left, NumPlanes };

	Plane m_Planes[NumPlanes];	// planes of the frusum in camera space
	glm::vec3 m_NearClip[4];			// verts of the near clip plane in camera space
	glm::vec3 m_FarClip[4];			// verts of the far clip plane in camera space

	float m_Fov;				// field of view in radians
	float m_Aspect;				// aspect ratio - width divided by height
	float m_Near;				// near clipping distance
	float m_Far;				// far clipping distance

public:
	Frustum();

	bool Inside(const glm::vec3 &point) const;
	bool Inside(const glm::vec3 &point, const float radius) const;
	const Plane &Get(Side side) { return m_Planes[side]; }
	void SetFOV(float fov) { m_Fov = fov; Init(m_Fov, m_Aspect, m_Near, m_Far); }
	void SetAspect(float aspect) { m_Aspect = aspect; Init(m_Fov, m_Aspect, m_Near, m_Far); }
	void SetNear(float nearClip) { m_Near = nearClip; Init(m_Fov, m_Aspect, m_Near, m_Far); }
	void SetFar(float farClip) { m_Far = farClip; Init(m_Fov, m_Aspect, m_Near, m_Far); }
	void Init(const float fov, const float aspect, const float fNear, const float fFar);

	void Render();
};


inline glm::vec3 CalcVelocity(glm::vec3 const &pos0, glm::vec3 const &pos1, float time)
{
	// CalcVelocity - Chapter 15, page 526
	return (pos1 - pos0) / time;
}

inline glm::vec3 CalcAcceleration(glm::vec3 const &vel0, glm::vec3 const &vel1, float time)
{
	// CalcVelocity - Chapter 15, page 526
	return (vel1 - vel0) / time;
}

inline void HandleAccel(glm::vec3 &pos, glm::vec3 &vel, glm::vec3 &accel, float time)
{
	// CalcVelocity - Chapter 15, page 526
	vel += accel * time;
	pos += vel * time;
}

// converts Barycentric coordinates to world coordinates
// inputs are the 3 verts of the triangle, and the u,v barycentric coordinates
extern glm::vec3 BarycentricTovec3(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, float u, float v);

extern bool IntersectTriangle(const glm::vec3& orig, const glm::vec3& dir,
	glm::vec3& v0, glm::vec3& v1, glm::vec3& v2,
	FLOAT* t, FLOAT* u, FLOAT* v);


inline void BuildYawPitchRoll(glm::mat4& m, const float yawRadians, const float pitchRadians, const float rollRadians)
{
	m = m * glm::toMat4(glm::quat(glm::vec3(yawRadians, pitchRadians, rollRadians)));
}

//kinda expensive, but avoid gimbal lock
inline glm::mat4 BuildRotationX(float rot)
{
	return glm::toMat4(glm::quat(glm::vec3(rot, 0.0f, 0.0f)));
}

inline glm::mat4 BuildRotationY(float rot)
{
	return glm::toMat4(glm::quat(glm::vec3(0.0f, rot, 0.0f)));
}

inline glm::mat4 BuildRotationZ(float rot)
{
	return glm::toMat4(glm::quat(glm::vec3(0.0f, 0.0f, rot)));
}

#endif