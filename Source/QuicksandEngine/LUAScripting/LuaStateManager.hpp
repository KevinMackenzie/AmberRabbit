#ifndef QSE_LUASTATEMANAGER_HPP
#define QSE_LUASTATEMANAGER_HPP

//#include "LuaPlus.h"
#include "../LuaPlus/LuaPlus.h"

//---------------------------------------------------------------------------------------------------------------------
// LuaStateManager                              - Chapter 12, page 367
//---------------------------------------------------------------------------------------------------------------------
class LuaStateManager : public IScriptManager
{
    static LuaStateManager* s_pSingleton;
    LuaPlus::LuaState* m_pLuaState;
    string m_lastError;

public:
    // Singleton functions
    static bool Create(void);
    static void Destroy(void);
    static LuaStateManager* Get(void) { LOG_ASSERT(s_pSingleton); return s_pSingleton; }

    // IScriptManager interface
	virtual bool VInit(void) override;
    virtual void VExecuteFile(const char* resource) override;
    virtual void VExecuteString(const char* str) override;

    LuaPlus::LuaObject GetGlobalVars(void);
    LuaPlus::LuaState* GetLuaState(void) const;

    // public helpers
    LuaPlus::LuaObject CreatePath(const char* pathString, bool toIgnoreLastElement = false);
    void Convertvec3ToTable(const vec3& vec, LuaPlus::LuaObject& outLuaTable) const;
    void ConvertTableTovec3(const LuaPlus::LuaObject& luaTable, vec3& outvec3) const;

private:
    void SetError(int errorNum);
    void ClearStack(void);

    // private constructor & destructor; call the static Create() and Destroy() functions instead
    explicit LuaStateManager(void);
    virtual ~LuaStateManager(void);
};

#endif