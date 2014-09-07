#ifndef AR_STDAFX_HPP
#define AR_STDAFX_HPP

#pragma warning(disable : 4005)
#pragma warning(disable : 4099)

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

#ifdef _WIN32
#ifdef QUICKSAND_ENGINE_EXPORTS
#define  QSE_API __declspec(dllexport)
#else
#define  QSE_API __declspec(dllimport)
#endif
#else
#define QSE_API
#endif

//linking
class QSE_API AStar;
class QSE_API Actor;
class QSE_API ActorComponent;
class QSE_API ActorFactory;
class QSE_API AmmoPickup;
class QSE_API Audio;
class QSE_API AudioBuffer;
class QSE_API AudioComponent;
class QSE_API BaseEventData;
class QSE_API BaseGameLogic;
class QSE_API BaseRenderComponent;
class QSE_API BaseScriptComponent;
class QSE_API BaseSocketManager;
class QSE_API BaseUI;
class QSE_API BinaryPacket;
class QSE_API BulletDebugDrawer;
class QSE_API BulletPhysics;
class QSE_API CameraNode;
class QSE_API ClientSocketManager;
class QSE_API Configuration;
class QSE_API CriticalSection;
class QSE_API DefaultResourceLoader;
class QSE_API DevelopmentResourceZipFile;
class QSE_API EventManager;
class QSE_API EvtData_Decompress_Request;
class QSE_API EvtData_Decompression_Progress;
class QSE_API EvtData_Destroy_Actor;
class QSE_API EvtData_Environment_Loaded;
class QSE_API EvtData_Game_State;
class QSE_API EvtData_Modified_Render_Component;
class QSE_API EvtData_Move_Actor;
class QSE_API EvtData_Network_Player_Actor_Assignment;
class QSE_API EvtData_New_Actor;
class QSE_API EvtData_New_Render_Component;
class QSE_API EvtData_PhysCollision;
class QSE_API EvtData_PhysSeperation;
class QSE_API EvtData_PhysTrigger_Enter;
class QSE_API EvtData_PhysTrigger_Leave;
class QSE_API EvtData_Play_Sound;
class QSE_API EvtData_Remote_Environment_Loaded;
class QSE_API EvtData_Remote_Client;
class QSE_API EvtData_Request_New_Actor;
class QSE_API EvtData_Request_Start_Game;
class QSE_API EvtData_Update_Tick;
class QSE_API ExplosionProcess;
class QSE_API FadeProcess;
class QSE_API FontHandler;
class QSE_API Frustum;
class QSE_API GCCRandom;
class QSE_API GLCtrlShaderResourceLoader;
class QSE_API GLEvalShaderResourceLoader;
class QSE_API GLFragShaderResourceLoader;
class QSE_API GLGeomShaderResourceLoader;
class QSE_API GLGrid;
class QSE_API GLLightNode;
class QSE_API GLLineDrawer;
class QSE_API GLMaterial;
class QSE_API GLMeshNode;
class QSE_API GLMessageBox;
class QSE_API GLObjMeshResourceExtraData;
class QSE_API GLObjMeshResourceLoader;
class QSE_API GLProgramResourceExtraData;
class QSE_API GLProgramResourceLoader;
class QSE_API GLRenderPass;
class QSE_API GLRenderSkyBox;
class QSE_API GLRenderSkyBoxPass;
class QSE_API GLRenderer;
class QSE_API GLRendererAlphaPass;
class QSE_API GLRenderer_Base;
class QSE_API GLSkyNode;
class QSE_API GLTextureResourceExtraData;
class QSE_API GLVertShaderResourceLoader;
class QSE_API GridRenderComponent;
class QSE_API HashedString;
class QSE_API HealthPickup;
class QSE_API HumanView;
class QSE_API IAudio;
class QSE_API IAudioBuffer;
class QSE_API IEventData;
class QSE_API IEventManager;
class QSE_API IGameLogic;
class QSE_API IGamePhysics;
class QSE_API IGameView;
class QSE_API IGamepadHandler;
class QSE_API IJoystickHandler;
class QSE_API IKeyboardHandler;
class QSE_API IPacket;
class QSE_API IPointerHandler;
class QSE_API IRenderState;
class QSE_API IRenderer;
class QSE_API IResourceExtraData;
class QSE_API IResourceFile;
class QSE_API IResourceLoader;
class QSE_API ISceneNode;
class QSE_API IScreenElement;
class QSE_API IScriptManager;
class QSE_API Intersection;
class QSE_API LevelManager;
class QSE_API LightManager;
class QSE_API LightNode;
class QSE_API LightRenderComponent;
class QSE_API LuaStateManager;
class QSE_API Math;
class QSE_API MeshRenderComponent;
class QSE_API MovementController;
class QSE_API NetSocket;
class QSE_API NetworkEventForwarder;
class QSE_API NetworkGameView;
class QSE_API NullPhysics;
class QSE_API OggResourceLoader;
class QSE_API PathPlanNode;
class QSE_API PathingArc;
class QSE_API PathingGraph;
class QSE_API PathingNode;
class QSE_API PhysicsComponent;
class QSE_API PickupInterface;
class QSE_API Plane;
class QSE_API PrimeSearch;
class QSE_API Process;
class QSE_API ProcessManager;
class QSE_API QuicksandEngineApp;
class QSE_API RayCast;
class QSE_API RealtimeProcess;
class QSE_API RemoteEventSocket;
class QSE_API RenderComponent;
class QSE_API RenderComponentInterface;
class QSE_API ResCache;
class QSE_API ResHandle;
class QSE_API Resource;
class QSE_API ResourceZipFile;
class QSE_API RootNode;
class QSE_API SFMLSoundAudio;
class QSE_API SFMLSoundBuffer;
class QSE_API Scene;
class QSE_API SceneNode;
class QSE_API SceneNodeProperties;
class QSE_API ScopedCriticalSection;
class QSE_API ScreenElementScene;
class QSE_API ScriptComponentInterface;
class QSE_API ScriptEvent;
class QSE_API SkyNode;
class QSE_API SoundProcess;
class QSE_API SoundResourceExtraData;
class QSE_API SphereRenderComponent;
class QSE_API StringsManager;
class QSE_API TextPacket;
class QSE_API TextureResourceLoader;
class QSE_API TransformComponent;
class QSE_API WaveResourceLoader;
class QSE_API XmlResourceExtraData;
class QSE_API XmlResourceLoader;
class QSE_API ZipFile;

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

//some boost stuff
#include <boost/lexical_cast.hpp>

using std::shared_ptr;
using std::weak_ptr;
using std::string;
using std::static_pointer_cast;
using std::dynamic_pointer_cast;

class QSE_API GCC_noncopyable
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

using namespace GLUF;

// OpenGL Includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <GL/glew.h>
#include <GL/GL.h>
#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>

#include <tinyxml2.h>
#include "Types.hpp"
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

extern Color		QSE_API g_White;
extern Color		QSE_API g_Black;
extern Color		QSE_API g_Cyan;
extern Color		QSE_API g_Red;
extern Color		QSE_API g_Green;
extern Color		QSE_API g_Blue;
extern Color		QSE_API g_Yellow;
extern Color		QSE_API g_Gray40;
extern Color		QSE_API g_Gray25;
extern Color		QSE_API g_Gray65;
extern Color		QSE_API g_Transparent;

extern glm::vec3	QSE_API g_Up;
extern glm::vec3	QSE_API g_Right;
extern glm::vec3	QSE_API g_Forward;

//right handed coordinates
extern glm::vec3	QSE_API g_XAxis;
extern glm::vec3	QSE_API g_YAxis;
extern glm::vec3	QSE_API g_ZAxis;

extern glm::vec4	QSE_API g_Up4;
extern glm::vec4	QSE_API g_Right4;
extern glm::vec4	QSE_API g_Forward4;


//  AppMsg				- Chapter 9, page 248

struct QSE_API AppMsg
{
	GLUF_MESSAGE_TYPE m_Event;
	int param1, param2, param3, param4;
};




// Useful #defines

extern const float QSE_API fOPAQUE;
extern const int   QSE_API iOPAQUE;
extern const float QSE_API fTRANSPARENT;
extern const int   QSE_API iTRANSPARENT;

extern const int   QSE_API MEGABYTE;
extern const float QSE_API SIXTY_HERTZ;

extern const int   QSE_API SCREEN_WIDTH;
extern const int   QSE_API SCREEN_HEIGHT;

#if !defined(SAFE_DELETE)
#define SAFE_DELETE(x) if(x){delete x; x=nullptr;};
#endif

#if !defined(SAFE_DELETE_ARRAY)
#define SAFE_DELETE_ARRAY(x) if (x) delete [] x; x=nullptr; 
#endif

#if !defined(SAFE_RELEASE)
#define SAFE_RELEASE(x) if(x) x->Release(); x=nullptr;
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

extern QSE_API INT WINAPI QuicksandEngineWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR    lpCmdLine,
	int       nCmdShow);



#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#endif