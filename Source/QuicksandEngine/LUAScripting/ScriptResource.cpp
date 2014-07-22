#include "../Stdafx.hpp"
#include "ScriptResource.hpp"
#include "LuaStateManager.hpp"


bool ScriptResourceLoader::VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
    if (rawSize <= 0)
        return false;

	if (!QuicksandEngine::g_pApp->m_pGame || QuicksandEngine::g_pApp->m_pGame->CanRunLua())
		LuaStateManager::Get()->VExecuteString(rawBuffer);

    return true;
}


shared_ptr<IResourceLoader> CreateScriptResourceLoader()
{
    return shared_ptr<IResourceLoader>(QSE_NEW ScriptResourceLoader());
}

