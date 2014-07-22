#ifndef QSE_ACTORS_HPP
#define QSE_ACTORS_HPP
//========================================================================
//  Content References in Game Coding Complete 3rd Edition
// 
//  struct ActorParams				- Chapter 19, page 683
//  struct SphereParams				- Chapter 19, page 738
//  struct TeapotParams				- Chapter 19, page 690




/****************************************

IMPORTANT GROUND RULES FOR DEFINING ACTOR PARAMETERS!!!!

1. No destructors.
2. No pointers.
3. Never break rules one, two, or three.

Why? These parameters are marshalled over god knows what. 
Could be a function call - or it could be the internet.

****************************************/

#include <strstream>

// TODO Rez - Need to bring LUA back
//#include "LuaPlus/LuaObject.h"

class BaseActor : public IActor
{
	friend class BaseGameLogic;
protected:
	ActorId m_id;
	mat4 m_Mat;
	int m_Type;
	shared_ptr<ActorParams> m_Params;


	virtual void VSetID(ActorId id) { m_id = id; } 
	virtual void VSetMat(const mat4 &newMat) { m_Mat = newMat; }

public:
	BaseActor(mat4 mat, int type, shared_ptr<ActorParams> params)
		{ m_Mat=mat; m_Type=type; m_Params=params; }

	virtual mat4 const &VGetMat() { return m_Mat; }			// virtual dumb here?
	virtual const int VGetType() { return m_Type; }				// virtual dumb here?
	virtual ActorId VGetID() { return m_id; }			// virtual dumb here?
	shared_ptr<ActorParams> VGetParams() { return m_Params; }
	virtual bool VIsPhysical() { return true; }
	virtual bool VIsGeometrical() { return true; }
	virtual void VOnUpdate(int deltaMilliseconds) { }
	virtual void VRotateY(float angleRadians);
};



#define VIEWID_NO_VIEW_ATTACHED	(0)

//TODO Boy I hope we can get rid of ActorTypes!
enum ActorType
{
	AT_Unknown,
	AT_Sphere,
	AT_Teapot,
	AT_TestObject,
	AT_Grid,
	AT_GenericMeshObject
};


class BaseGameLogic;
class SceneNode;


struct ActorParams
{
	int m_Size;
	optional<ActorId> m_Id;
	vec3 m_Pos;
	ActorType m_Type;
	Color m_Color;
	static const int sk_MaxFuncName = 64;
	char m_OnCreateLuaFunctionName[ sk_MaxFuncName ];	//Lua function name to call for this actor, upon creation.
	char m_OnDestroyLuaFunctionName[ sk_MaxFuncName ];	//As above, but on destruction.

	ActorParams(); 

	virtual ~ActorParams() { }

	virtual bool VInit(std::istrstream &in);
	virtual void VSerialize(std::ostrstream &out) const;

	typedef std::deque< std::string > TErrorMessageList;

	//TODO LUA
	//virtual bool VInit( LuaObject srcData, TErrorMessageList & errorMessages );

	static ActorParams *CreateFromStream(std::istrstream &in);
	//static ActorParams *CreateFromLuaObj( LuaObject srcData );

	virtual shared_ptr<IActor> VCreate(BaseGameLogic *logic) { shared_ptr<IActor> p; return p; }
	virtual shared_ptr<SceneNode> VCreateSceneNode(shared_ptr<Scene> pScene) { shared_ptr<SceneNode> p; return p; }
};


struct SphereParams : public ActorParams
{
	float m_Radius;
	int m_Segments;
	vec3 m_NormalDir;
	float m_Force;

	SphereParams();

	virtual bool VInit(std::istrstream &in);
	//TODO LUA
	//virtual bool VInit( LuaObject srcData, TErrorMessageList & errorMessages );
	virtual void VSerialize(std::ostrstream &out) const;
	virtual shared_ptr<IActor> VCreate(BaseGameLogic *logic);
	virtual shared_ptr<SceneNode> VCreateSceneNode(shared_ptr<Scene> pScene);
};

struct TeapotParams : public ActorParams
{
	float m_Length;
	GameViewId m_ViewId;
	mat4 m_Mat;

	TeapotParams();

	virtual bool VInit(std::istrstream &in);
	//TODO LUA
	//virtual bool VInit( LuaObject srcData, TErrorMessageList & errorMessages );
	virtual void VSerialize(std::ostrstream &out) const;
	virtual shared_ptr<IActor> VCreate(BaseGameLogic *logic);
	virtual shared_ptr<SceneNode> VCreateSceneNode(shared_ptr<Scene> pScene);
};

struct AiTeapotParams : public ActorParams
{
	float m_Length;
	GameViewId m_ViewId;
	mat4 m_Mat;

	AiTeapotParams(); 

	virtual bool VInit(std::istrstream &in);
	//TODO LUA
	//virtual bool VInit( LuaObject srcData, TErrorMessageList & errorMessages );
	virtual void VSerialize(std::ostrstream &out) const;
	virtual shared_ptr<IActor> VCreate(BaseGameLogic *logic);
	virtual shared_ptr<SceneNode> VCreateSceneNode(shared_ptr<Scene> pScene);
};

struct TestObjectParams : public ActorParams
{
	bool m_Squashed;
	mat4 m_Mat;

	TestObjectParams();

	virtual bool VInit(std::istrstream &in);
	//TODO LUA
	//virtual bool VInit( LuaObject srcData, TErrorMessageList & errorMessages );
	virtual void VSerialize(std::ostrstream &out) const;
	virtual shared_ptr<IActor> VCreate(BaseGameLogic *logic);
	virtual shared_ptr<SceneNode> VCreateSceneNode(shared_ptr<Scene> pScene);
};


struct GridParams : public ActorParams
{
	static const int sk_MaxTextureNameLen = _MAX_PATH;
	char m_Texture[sk_MaxTextureNameLen];
	unsigned int m_Squares;
	mat4 m_Mat;

	GridParams();

	virtual bool VInit(std::istrstream &in);
	//TODO LUA
	//virtual bool VInit( LuaObject srcData, TErrorMessageList & errorMessages );
	virtual void VSerialize(std::ostrstream &out) const;
	virtual shared_ptr<IActor> VCreate(BaseGameLogic *logic);
	virtual shared_ptr<SceneNode> VCreateSceneNode(shared_ptr<Scene> pScene);
};

struct GenericMeshObjectParams : public ActorParams
{
	static const int sk_MaxFileNameLen = _MAX_PATH;
	char m_XFileName[sk_MaxFileNameLen];
	char m_FXFileName[sk_MaxFileNameLen];
	mat4 m_Mat;

	GenericMeshObjectParams();

	virtual bool VInit(std::istrstream &in);
	//TODO LUA
	//virtual bool VInit( LuaObject srcData, TErrorMessageList & errorMessages );
	virtual void VSerialize(std::ostrstream &out) const;
	virtual shared_ptr<IActor> VCreate(BaseGameLogic *logic);
	virtual shared_ptr<SceneNode> VCreateSceneNode(shared_ptr<Scene> pScene);
};

#endif