#include "../Stdafx.hpp"
#include "BaseScriptComponent.hpp"
#include "../LUAScripting/LuaStateManager.hpp"
#include "../Utilities/String.hpp"
#include "../Utilities/Math.hpp"

// component interfaces
#include "TransformComponent.hpp"
#include "PhysicsComponent.hpp"
#include "RenderComponentInterface.hpp"

// This is the name of the metatable where all the function definitions exported to Lua will live.
static const char* METATABLE_NAME = "BaseScriptComponentMetaTable";
const char* BaseScriptComponent::g_Name = "BaseScriptComponent";

BaseScriptComponent::BaseScriptComponent(void)
{
	m_scriptObject.AssignNil(LuaStateManager::Get()->GetLuaState());
    m_scriptDestructor.AssignNil(LuaStateManager::Get()->GetLuaState());
}

BaseScriptComponent::~BaseScriptComponent(void)
{
    // call the script destructor if there is one
    if (m_scriptDestructor.IsFunction())
    {
        LuaPlus::LuaFunction<void> func(m_scriptDestructor);
        func(m_scriptObject);
    }

    // clear out the script object
	m_scriptObject.AssignNil(LuaStateManager::Get()->GetLuaState());
	
    // if we were given a path for this script object, set it to nil
	if (!m_scriptObjectName.empty())
	{
		m_scriptObjectName += " = nil;";
		LuaStateManager::Get()->VExecuteString(m_scriptObjectName.c_str());
	}
}

bool BaseScriptComponent::VInit(tinyxml2::XMLElement* pData)
{
    LuaStateManager* pStateMgr = LuaStateManager::Get();
    LOG_ASSERT(pStateMgr);

    // load the <ScriptObject> tag and validate it
    tinyxml2::XMLElement* pScriptObjectElement = pData->FirstChildElement("ScriptObject");
    if (!pScriptObjectElement)
    {
        LOG_ERROR("No <ScriptObject> tag in XML.  This won't be a very useful script component.");
        return true;  // technically it succeeded even though it won't be accessible
    }

    // read all the attributes
    const char* temp = NULL;
    temp = pScriptObjectElement->Attribute("var");
    if (temp)
        m_scriptObjectName = temp;

    temp = pScriptObjectElement->Attribute("constructor");
    if (temp)
        m_constructorName = temp;

    temp = pScriptObjectElement->Attribute("destructor");
    if (temp)
        m_destructorName = temp;

    // Having a var attribute will export the instance of this object to that name.
    if (!m_scriptObjectName.empty())
    {
        m_scriptObject = pStateMgr->CreatePath(m_scriptObjectName.c_str());

        if (!m_scriptObject.IsNil())
        {
            CreateScriptObject();
        }
	}

    // The scriptConstructor attribute will also cause a Lua object to be created if one wasn't created in 
    // the previous step.  The scriptConstructor string is treated as a function of the form f(scriptObject) 
    // and is called.
    if (!m_constructorName.empty())
    {
        m_scriptConstructor = pStateMgr->GetGlobalVars().Lookup(m_constructorName.c_str());
        if (m_scriptConstructor.IsFunction())
        {
			// m_scriptObject could be nil if there was no scriptObject attribute.  If this is the case, 
            // the Lua object is created here.
			if (m_scriptObject.IsNil())
			{
				m_scriptObject.AssignNewTable(pStateMgr->GetLuaState());
				CreateScriptObject();
			}
		}
    }

    // The scriptDestructor attribute is treated as a function in the form of f(scriptObject) and is called
    // when the C++ ScriptObject instance is destroyed.
    if (!m_destructorName.empty())
    {
        m_scriptDestructor = pStateMgr->GetGlobalVars().Lookup(m_destructorName.c_str());
	}

    // read the <ScriptData> tag
    tinyxml2::XMLElement* pScriptDataElement = pData->FirstChildElement("ScriptData");
    if (pScriptDataElement)
    {
        if (m_scriptObject.IsNil())
        {
            LOG_ERROR("m_scriptObject cannot be nil when ScriptData has been defined");
            return false;
        }

        for (const tinyxml2::XMLAttribute* pAttribute = pScriptDataElement->FirstAttribute(); pAttribute != NULL; pAttribute = pAttribute->Next())
        {
            m_scriptObject.SetString(pAttribute->Name(), pAttribute->Value());
        }
    }

	return true;
}

void BaseScriptComponent::VPostInit(void)
{
    // call the script constructor if one exists
    if (m_scriptConstructor.IsFunction())
    {
        LuaPlus::LuaFunction<bool> func(m_scriptConstructor);
        func(m_scriptObject);
    }
}

tinyxml2::XMLElement* BaseScriptComponent::VGenerateXml(tinyxml2::XMLDocument* pDoc)
{
    tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(VGetName());

    // ScriptObject
	tinyxml2::XMLElement* pScriptObjectElement = pDoc->NewElement("ScriptObject");
    if (!m_scriptObjectName.empty())
        pScriptObjectElement->SetAttribute("var", m_scriptObjectName.c_str());
    if (!m_constructorName.empty())
        pScriptObjectElement->SetAttribute("constructor", m_constructorName.c_str());
    if (!m_destructorName.empty())
        pScriptObjectElement->SetAttribute("destructor", m_destructorName.c_str());
    pBaseElement->LinkEndChild(pScriptObjectElement);

    return pBaseElement;
}

void BaseScriptComponent::CreateScriptObject(void)
{
    LuaStateManager* pStateMgr = LuaStateManager::Get();
    LOG_ASSERT(pStateMgr);
	LOG_ASSERT(!m_scriptObject.IsNil());
	
	LuaPlus::LuaObject metaTableObj = pStateMgr->GetGlobalVars().Lookup(METATABLE_NAME);
	LOG_ASSERT(!metaTableObj.IsNil());
	
	LuaPlus::LuaObject boxedPtr = pStateMgr->GetLuaState()->BoxPointer(this);
	boxedPtr.SetMetaTable(metaTableObj);
	m_scriptObject.SetLightUserData("__object", this);
	m_scriptObject.SetMetaTable(metaTableObj);
}

void BaseScriptComponent::RegisterScriptFunctions(void)
{
	// create the metatable
	LuaPlus::LuaObject metaTableObj = LuaStateManager::Get()->GetGlobalVars().CreateTable(METATABLE_NAME);
	metaTableObj.SetObject("__index", metaTableObj);

	// transform component functions
	metaTableObj.RegisterObjectDirect("GetActorId",		        (BaseScriptComponent*)0, &BaseScriptComponent::GetActorId);
	metaTableObj.RegisterObjectDirect("GetPos",				    (BaseScriptComponent*)0, &BaseScriptComponent::GetPos);
    metaTableObj.RegisterObjectDirect("SetPos",				    (BaseScriptComponent*)0, &BaseScriptComponent::SetPos);
	metaTableObj.RegisterObjectDirect("GetLookAt",	            (BaseScriptComponent*)0, &BaseScriptComponent::GetLookAt);
	metaTableObj.RegisterObjectDirect("GetYOrientationRadians", (BaseScriptComponent*)0, &BaseScriptComponent::GetYOrientationRadians);
	metaTableObj.RegisterObjectDirect("RotateY",	            (BaseScriptComponent*)0, &BaseScriptComponent::RotateY);
    metaTableObj.RegisterObjectDirect("Stop",	                (BaseScriptComponent*)0, &BaseScriptComponent::Stop);

	metaTableObj.RegisterObjectDirect("SetPosition",	        (BaseScriptComponent*)0, &BaseScriptComponent::SetPosition);
}

void BaseScriptComponent::UnregisterScriptFunctions(void)
{
	LuaPlus::LuaObject metaTableObj = LuaStateManager::Get()->GetGlobalVars().Lookup(METATABLE_NAME);
	if (!metaTableObj.IsNil())
		metaTableObj.AssignNil(LuaStateManager::Get()->GetLuaState());
}

LuaPlus::LuaObject BaseScriptComponent::GetActorId(void)
{
	//GCC_LOG("ObjectSystem", "BaseScriptComponent::GetEntityId() return 0x" + ToStr(m_pOwner->GetId(), 16) + " on C++ side");

	// [rez] Wtf??  Returning an unsigned int here will produce really odd results in Release mode (debug is 
	// fine).  The number will sometimes get truncated, so 0xb1437643 might become 0xb1437600.  Even though the 
	// C++ side is getting the right number, it gets mangled on the way out to Lua.  The somewhat hacky fix here 
	// is to return a LuaPlus::LuaObject, which seems to work just fine.  I'm not sure if this will cause any 
    // efficiency issues, but I'm guessing the LuaPlus internals do something like this anyway to marshal the 
    // data back to Lua land, so it's probably okay.
	LuaPlus::LuaObject ret;
	ret.AssignInteger(LuaStateManager::Get()->GetLuaState(), m_pOwner->GetId());
	return ret;

	//return m_pOwner->GetId();
}

LuaPlus::LuaObject BaseScriptComponent::GetPos(void)
{
    LuaPlus::LuaObject ret;

    shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_Name));
    if (pTransformComponent)
        LuaStateManager::Get()->Convertvec3ToTable(pTransformComponent->GetPosition(), ret);
    else
        ret.AssignNil(LuaStateManager::Get()->GetLuaState());

    return ret;
}

void BaseScriptComponent::SetPos(LuaPlus::LuaObject newPos)
{
    shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_Name));
    if (pTransformComponent)
    {
        glm::vec3 pos;
        LuaStateManager::Get()->ConvertTableTovec3(newPos, pos);
        pTransformComponent->SetPosition(pos);
    }
    else
    {
        LOG_ERROR("Attempting to call SetPos() on an actor with no physcial component; ActorId: " + ToStr(m_pOwner->GetId()));
    }
}


LuaPlus::LuaObject BaseScriptComponent::GetLookAt(void) const
{
    LuaPlus::LuaObject ret;

    shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_Name));
    if (pTransformComponent)
        LuaStateManager::Get()->Convertvec3ToTable(pTransformComponent->GetLookAt(), ret);
    else
        ret.AssignNil(LuaStateManager::Get()->GetLuaState());

    return ret;
}

float BaseScriptComponent::GetYOrientationRadians(void) const
{
    shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_Name));
    if (pTransformComponent)
    {
        return (GetYRotationFromVector(pTransformComponent->GetLookAt()));
    }
    else
    {
		LOG_ERROR("Attempting to call GetYOrientationRadians() on actor with no physical component");
        return 0;
    }
}

void BaseScriptComponent::RotateY(float angleRadians)
{
    shared_ptr<PhysicsComponent> pPhysicalComponent = MakeStrongPtr(m_pOwner->GetComponent<PhysicsComponent>(PhysicsComponent::g_Name));
    if (pPhysicalComponent)
        pPhysicalComponent->RotateY(angleRadians);
}


void BaseScriptComponent::SetPosition(float x, float y, float z)
{
    shared_ptr<PhysicsComponent> pPhysicalComponent = MakeStrongPtr(m_pOwner->GetComponent<PhysicsComponent>(PhysicsComponent::g_Name));
    if (pPhysicalComponent)
        pPhysicalComponent->SetPosition(x, y, z);
}

void BaseScriptComponent::Stop(void)
{
    shared_ptr<PhysicsComponent> pPhysicalComponent = MakeStrongPtr(m_pOwner->GetComponent<PhysicsComponent>(PhysicsComponent::g_Name));
    if (pPhysicalComponent)
        pPhysicalComponent->Stop();
}

