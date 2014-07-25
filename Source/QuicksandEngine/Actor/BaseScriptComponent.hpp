#ifndef QSE_BASESCRIPTCOMPONENT_HPP
#define QSE_BASESCRIPTCOMPONENT_HPP

#include "ScriptComponentInterface.hpp"
#include "../LuaPlus/LuaPlus.h"


//---------------------------------------------------------------------------------------------------------------------
// BaseScriptComponent class
// Chapter 12, page 337
//---------------------------------------------------------------------------------------------------------------------
class BaseScriptComponent : public ScriptComponentInterface
{
	string m_scriptObjectName;
    string m_constructorName;
    string m_destructorName;

	LuaPlus::LuaObject m_scriptObject;
    LuaPlus::LuaObject m_scriptConstructor;
	LuaPlus::LuaObject m_scriptDestructor;
	
public:
	static const char* g_Name;
	virtual const char* VGetName() const { return g_Name; }

	BaseScriptComponent(void);
	virtual ~BaseScriptComponent(void);
	virtual bool VInit(XMLElement* pData);
    virtual void VPostInit(void);
    virtual XMLElement* VGenerateXml(XMLDocument* pDoc);
	
	static void RegisterScriptFunctions(void);
	static void UnregisterScriptFunctions(void);
	
private:
	void CreateScriptObject(void);
	
	// component script functions
	LuaPlus::LuaObject GetActorId(void);
	
	// physics component script functions
	LuaPlus::LuaObject GetPos(void);
    void SetPos(LuaPlus::LuaObject newPos);
    LuaPlus::LuaObject GetLookAt(void) const;
    float GetYOrientationRadians(void) const;
	void RotateY(float angleRadians);
	void SetPosition(float x, float y, float z);
	void LookAt(glm::vec3 target);
    void Stop(void);
};


#endif