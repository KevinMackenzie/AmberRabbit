#ifndef QSE_RANDOM_HPP
#define QSE_RANDOM_HPP


//========================================================================
//
//  Original Code written at Compulsive Development
//
//========================================================================


//--------------------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------------------

#include <functional>
#include <vector>
#include <math.h>
//--------------------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------

/* Period parameters */  
#define CMATH_N 624
#define CMATH_M 397
#define CMATH_MATRIX_A 0x9908b0df   /* constant vector a */
#define CMATH_UPPER_MASK 0x80000000 /* most significant w-r bits */
#define CMATH_LOWER_MASK 0x7fffffff /* least significant r bits */

/* Tempering parameters */   
#define CMATH_TEMPERING_MASK_B 0x9d2c5680
#define CMATH_TEMPERING_MASK_C 0xefc60000
#define CMATH_TEMPERING_SHIFT_U(y)  (y >> 11)
#define CMATH_TEMPERING_SHIFT_S(y)  (y << 7)
#define CMATH_TEMPERING_SHIFT_T(y)  (y << 15)
#define CMATH_TEMPERING_SHIFT_L(y)  (y >> 18)

#define AS_PI (3.14159265358979323846)

#define RADIANS_TO_DEGREES(x) ((x) * 180.0f / AS_PI)
#define DEGREES_TO_RADIANS(x) ((x) * AS_PI / 180.0f)



class GCCRandom
{
private:
	// DATA
	unsigned int		rseed;
	unsigned int		rseed_sp;
	unsigned long mt[CMATH_N]; /* the array for the state vector  */
	int mti; /* mti==N+1 means mt[N] is not initialized */

	// FUNCTIONS
public:
	GCCRandom(void);	

	unsigned int	Random( unsigned int n );
	float			Random( );
	void			SetRandomSeed(unsigned int n);
	unsigned int	GetRandomSeed(void);
	void			Randomize(void);
};


/*
struct Point
{
	int x, y;
	Point() { x = y = 0; }
	Point(int _x, int _y) { x = _x; y = _y; }
};
*/

typedef std::vector<Point> Poly;


class Math
{
	// DATA
private:
	static const unsigned short angle_to_sin[90];

public:
	static GCCRandom		random;

	// FUNCTIONS
public:
	static int				Cos(short angle, int length);
	static int				Sin(short angle, int length);
	static unsigned int		Sqrt( unsigned int n );
	static void				InterpolateLine(int *x, int *y, int end_x, int end_y, int step_size);
	static unsigned short	GetAngle(int x, int y);
	static bool PointInPoly( Point const &test, const Poly & polygon);
	static bool				PointInPoly
							(
								int const			x,
								int const			y,
								int const * const	vertex,
								int const			nvertex
							);
	static RECT				BoundingBox
							( 
								const POINT &pt1,
								const POINT &pt2,
								const POINT &pt3,
								const POINT &pt4
							);
	static RECT				BoundingBox
							(
								const POINT *verts,
								const unsigned int nverts
							);
	static float const		GetDistanceBetween(POINT const & pt1, POINT const & pt2);

	// Used to determine the bounding box for a range of point-like objects.
	// This includes POINTS, CPoints, and VertexUV to name a few.
	// This works on any range which includes all STL containers as well as C style arrays.
	// See BoundingBox(const POINT*, const unsigned int) in cpp for example usage.
	template <typename PointType>
	class BoundingBoxFinder : std::unary_function<PointType, void>
	{
	public:
		void operator()(PointType const & item)
		{
			if (mBoundingBox.invalid())
			{
				RECT initialValue = { item.x, item.y, item.x, item.y };
				mBoundingBox = initialValue;
			}
			else
			{
				mBoundingBox->left = std::min(mBoundingBox->left, item.x);
				mBoundingBox->top = std::min(mBoundingBox->top, item.y);
				mBoundingBox->right = std::max(mBoundingBox->right, item.x);
				mBoundingBox->bottom = std::max(mBoundingBox->bottom, item.y);
			}
		}

		RECT const & BoundingBox() { return *mBoundingBox; }

	private:
		optional<RECT> mBoundingBox;
	};


};

#define	DONT_INTERSECT    0
#define	DO_INTERSECT      1
#define COLLINEAR         2

struct LineSegment
{
	Point m_begin, m_end;
	LineSegment(const Point &begin, const Point &end) { m_begin=begin; m_end=end; }
	LineSegment() { m_begin = m_end = Point(0,0); }
};


int lines_intersect( Point one,   /* First line segment */
					 Point two,

					Point three,   /* Second line segment */
					Point four,

					Point &result
               );

bool Intersect(const Rect &rect, const Point &center, double const radius);

float WrapPi(float wrapMe);  // wraps angle so it's between -PI and PI
float Wrap2Pi(float wrapMe);  // wraps angle so it's between 0 and 2PI
float AngleDiff( float lhs, float rhs );
glm::vec3 GetVectorFromYRotation(float angleRadians);
float GetYRotationFromVector(const glm::vec3& lookAt);

//This is a set of methods to accompany GLM that I cannot find an sutible presets
inline void SetPosition(glm::mat4& param, glm::vec3 const &pos)
{
	param[3][0] = pos.x;
	param[3][1] = pos.y;
	param[3][2] = pos.z;
	param[3][3] = 1.0f;
}

inline glm::vec3 Xform(const glm::mat4& param, glm::vec3 &v)
{
	return glm::vec3(glm::vec4(v, 1.0f) * param);
}

inline glm::vec4 Xform(const glm::mat4& param, glm::vec4 &v)
{
	return glm::vec4(glm::vec4(v) * param);
}

inline glm::vec3 GetPosition(const glm::mat4& param)
{
	return glm::vec3(param[3][0], param[3][1], param[3][2]);
}

inline glm::vec3 GetDirection(const glm::mat4& param)
{
	// Note - the following code can be used to double check the vector construction above.
	glm::mat4 justRot = param;
	SetPosition(justRot, glm::vec3());
	glm::vec3 forward = Xform(justRot, g_Forward);
	return forward;
}

inline glm::vec3 GetRight(const glm::mat4& param)
{
	// Note - the following code can be used to double check the vector construction above.
	glm::mat4 justRot = param;
	SetPosition(justRot, glm::vec3());
	glm::vec3 right = Xform(justRot, g_Right);
	return right;
}

inline glm::vec3 GetUp(const glm::mat4& param)
{
	// Note - the following code can be used to double check the vector construction above.
	glm::mat4 justRot = param;
	SetPosition(justRot, glm::vec3());
	glm::vec3 up = Xform(justRot, g_Up);
	return up;
}

inline glm::vec3 GetYawPitchRoll(const glm::mat4& param)
{
	float yaw, pitch, roll;

	pitch = asin(-param[3][2]);

	double threshold = 0.001; // Hardcoded constant - burn him, he's a witch
	double test = cos(pitch);

	if (test > threshold)
	{
		roll = atan2(param[1][2], param[2][2]);
		yaw = atan2(param[3][1], param[3][3]);
	}
	else
	{
		roll = atan2(-param[2][1], param[1][1]);
		yaw = 0.0;
	}

	return (glm::vec3(yaw, pitch, roll));
}


#endif