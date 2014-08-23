#ifndef AR_STDAFX_HPP
#define AR_STDAFX_HPP


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// Windows Header Files:
#ifdef _WIN32

#define NOMINMAX
#include <windows.h>
#include <windowsx.h>

#include <crtdbg.h>


#include <mmsystem.h>
#include <mmreg.h>

#define _VS2005_
#if _MSC_VER < 1400
#undef _VS2005_
#endif

#endif
// [rez] I am saddened that Visual Studio 2010 doesn't support the new C++0x final keyword.  It does support 
// the override keyword as well as a special Microsoft-specific sealed keyword.  The override keyword works 
// exactly as the C++0x spec says it does and the sealed keyword acts like the C++0x final keyword.  I toyed 
// around with whether or not I wanted to include this so I will leave the decision up to you.  If you want 
// to use the new C++0x style of override & final, just uncomment this line.  This works on Visual Studio 2008 
// and up for sure.  I'm not sure about earlier versions.
//#define final sealed


// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <mmsystem.h>

#include <algorithm>
#include <string>
#include <list>
#include <vector>
#include <memory>
#include <queue>
#include <map>
#include <sstream>

//clean some older windows macros
#undef near
#undef far

#undef FAR
#define FAR

//some boost stuff
#include <boost/lexical_cast.hpp>

using std::shared_ptr;
using std::weak_ptr;
using std::string;
using std::static_pointer_cast;
using std::dynamic_pointer_cast;

class GCC_noncopyable
{
private:
	GCC_noncopyable(const GCC_noncopyable& x);
	GCC_noncopyable& operator=(const GCC_noncopyable& x);
public:
	GCC_noncopyable() {}; // Default constructor  
};


// Game Code Complete - Chapter 12, page 446-447
#if defined(_DEBUG)
#	define QSE_NEW new(_NORMAL_BLOCK,__FILE__, __LINE__)
#else
#	define QSE_NEW new
#endif

#if defined(DEBUG) || defined(_DEBUG)
#ifndef V
#define V(x)           { hr = (x); if( FAILED(hr) ) { LOG_ERROR("V Error Thrown"); } }
#endif
#ifndef V_RETURN
#define V_RETURN(x)    { hr = (x); if( FAILED(hr) ) { LOG_ERROR("V_RETURN Error Thrown"); return E_FAIL;  } }
#endif
#else
#ifndef V
#define V(x)           { hr = (x); }
#endif
#ifndef V_RETURN
#define V_RETURN(x)    { hr = (x); if( FAILED(hr) ) { return hr; } }
#endif
#endif

//ObjGLUF Include
#define SUPPRESS_RADIAN_ERROR
#define SUPPRESS_UTF8_ERROR
#define USING_ASSIMP
#include "../ObjGLUF/ObjGLUF.h"
#include "../ObjGLUF/GLUFGui.h"

using GLUF::Color;
using GLUF::Color4f;

// OpenGL Includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <GL/glew.h>
#include <GL/GL.h>
#include <GLFW/glfw3.h>
#include <IL/il.h>


#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>

#include <tinyxml2.h>
//using namespace tinyxml2;

// fast delegate stuff
#include "FastDelegate/FastDelegate.h"
using fastdelegate::MakeDelegate;

#pragma warning( disable : 4996 ) // 'function' declared deprecated - gets rid of all those 2005 warnings....


// GameCode #includes
#include "Debugging/Logging.hpp"
#include "Utilities/Templates.hpp"
#include "Graphics3D/Geometry.hpp"

#include "Types.hpp"
#include "Utilities/Types.hpp"

//typedef glm::u8vec4 Color;

extern Color g_White;
extern Color g_Black;
extern Color g_Cyan;
extern Color g_Red;
extern Color g_Green;
extern Color g_Blue;
extern Color g_Yellow;
extern Color g_Gray40;
extern Color g_Gray25;
extern Color g_Gray65;
extern Color g_Transparent;

extern glm::vec3 g_Up;
extern glm::vec3 g_Right;
extern glm::vec3 g_Forward;

//this assums that while looking down the negative ZAxis, X is positive to the right, and Y is positive 
extern glm::vec3 g_XAxis;
extern glm::vec3 g_YAxis;
extern glm::vec3 g_ZAxis;

extern glm::vec4 g_Up4;
extern glm::vec4 g_Right4;
extern glm::vec4 g_Forward4;


//  AppMsg				- Chapter 9, page 248

struct AppMsg
{
	GLUF_MESSAGE_TYPE m_Event;
	int param1, param2, param3, param4;
};




// Useful #defines

extern const float fOPAQUE;
extern const int iOPAQUE;
extern const float fTRANSPARENT;
extern const int iTRANSPARENT;

extern const int MEGABYTE;
extern const float SIXTY_HERTZ;

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

#if !defined(SAFE_DELETE)
#define SAFE_DELETE(x) if(x) delete x; x=NULL;
#endif

#if !defined(SAFE_DELETE_ARRAY)
#define SAFE_DELETE_ARRAY(x) if (x) delete [] x; x=NULL; 
#endif

#if !defined(SAFE_RELEASE)
#define SAFE_RELEASE(x) if(x) x->Release(); x=NULL;
#endif

#ifdef UNICODE
#define _tcssprintf wsprintf
#define tcsplitpath _wsplitpath
#else
#define _tcssprintf sprintf
#define tcsplitpath _splitpath
#endif

#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __LOC__ __FILE__ "("__STR1__(__LINE__)") : Warning Msg: "

#include "Application/QuicksandEngineApp.hpp"
#include "Actor/Actor.hpp"
#include "Utilities/Math.hpp"
//#include "Graphics3D/Buffer.hpp"
#include "GameLogic/BaseGameLogic.hpp"
#include "Utilities/String.hpp"

extern INT WINAPI QuicksandEngineWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPWSTR    lpCmdLine,
	int       nCmdShow);


template <class M>
std::vector<M> ArrToVec(M* arr, size_t size);


#endif