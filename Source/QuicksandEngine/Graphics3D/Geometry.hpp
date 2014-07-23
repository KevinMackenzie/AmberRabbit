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
	vec3 normal;
	GLfloat distance;

	inline void Normalize();

	// normal faces away from you if you send in verts in counter clockwise order....
	inline void Init(const vec3 &p0, const vec3 &p1, const vec3 &p2);
	bool Inside(const vec3 &point, const float radius) const;
	bool Inside(const vec3 &point) const;
};

inline void Plane::Normalize()
{
	float mag;
	mag = glm::sqrt(dot(normal,normal));
	normal /= mag;
	distance /= mag;
}

inline void Plane::Init(const vec3 &p0, const vec3 &p1, const vec3 &p2)
{
	normal = cross(p1 - p0, p2 - p0);
	distance = dot(normal, p0);
	Normalize();
}

inline GLfloat PlaneDotCoord(const Plane &plane, const vec3 &vec)
{
	return dot(plane.normal, vec) + plane.distance;
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
	vec3 m_NearClip[4];			// verts of the near clip plane in camera space
	vec3 m_FarClip[4];			// verts of the far clip plane in camera space

	float m_Fov;				// field of view in radians
	float m_Aspect;				// aspect ratio - width divided by height
	float m_Near;				// near clipping distance
	float m_Far;				// far clipping distance

public:
	Frustum();

	bool Inside(const vec3 &point) const;
	bool Inside(const vec3 &point, const float radius) const;
	const Plane &Get(Side side) { return m_Planes[side]; }
	void SetFOV(float fov) { m_Fov = fov; Init(m_Fov, m_Aspect, m_Near, m_Far); }
	void SetAspect(float aspect) { m_Aspect = aspect; Init(m_Fov, m_Aspect, m_Near, m_Far); }
	void SetNear(float nearClip) { m_Near = nearClip; Init(m_Fov, m_Aspect, m_Near, m_Far); }
	void SetFar(float farClip) { m_Far = farClip; Init(m_Fov, m_Aspect, m_Near, m_Far); }
	void Init(const float fov, const float aspect, const float near, const float far);

	void Render();
};


inline vec3 CalcVelocity(vec3 const &pos0, vec3 const &pos1, float time)
{
	// CalcVelocity - Chapter 15, page 526
	return (pos1 - pos0) / time;
}

inline vec3 CalcAcceleration(vec3 const &vel0, vec3 const &vel1, float time)
{
	// CalcVelocity - Chapter 15, page 526
	return (vel1 - vel0) / time;
}

inline void HandleAccel(vec3 &pos, vec3 &vel, vec3 &accel, float time)
{
	// CalcVelocity - Chapter 15, page 526
	vel += accel * time;
	pos += vel * time;
}

// converts Barycentric coordinates to world coordinates
// inputs are the 3 verts of the triangle, and the u,v barycentric coordinates
extern vec3 BarycentricTovec3(vec3 v0, vec3 v1, vec3 v2, float u, float v);

extern bool IntersectTriangle(const vec3& orig, const vec3& dir,
	vec3& v0, vec3& v1, vec3& v2,
	FLOAT* t, FLOAT* u, FLOAT* v);


void BuildYawPitchRoll(mat4& m, const float yawRadians, const float pitchRadians, const float rollRadians)
{
	m = m * vec4(quat(vec3(yawRadians, pitchRadians, rollRadians)));
}


#endif