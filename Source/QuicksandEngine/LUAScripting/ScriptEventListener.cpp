#include "../Stdafx.hpp"

#include "ScriptEventListener.hpp"

using LuaPlus::LuaObject;


// ScriptEventListener::ScriptEventListener				- Chapter 11, page 335
ScriptEventListener::ScriptEventListener( LuaObject explicitHandlerFunction )
: m_HandlerFunction( explicitHandlerFunction )
{
	assert( explicitHandlerFunction.IsFunction() && "Script listener *MUST* be a valid function!" );
}

// ScriptEventListener::HandleEvent						- Chapter 11, page 335
bool ScriptEventListener::HandleEvent( ILuaable const & luaable )
{
	// If we don't already have Lua event data built, do so now.
	if ( false == luaable.VHasLuaEventData() )
	{
		// This goes against everything you are taught in C++ class.
		// We're going to make this const IEventData non-const because
		// we need to serialize the event for Lua to understand it.
		// We're doing this for three reasons:
		//  a) So we only build Lua data ONCE for any triggered event,
		//     and ONLY when the event needs to be sent to a Lua function
		//     (we don't want to ALWAYS build Lua data).
		//  b) We're not technically changing any of the "real" event
		//     *data*...
		//  c) If we make other listener's HandleEvent() calls take
		//     a non-const event, they could alter it.
		// Don't make a habit of doing this.
		// Note:  We could potentially avoid this by marking the Lua
		// object with the 'mutable' keyword.

		// Pray the const away.
		ILuaable & NCLuaable = const_cast< ILuaable & >( luaable );

		// Build it and never mention this again.
		NCLuaable.VBuildLuaEventData();
	}

	LuaObject & eventDataObj = luaable.VGetLuaEventData();

	//Call the handler function.
	const bool bResult = VCallLuaFunction( eventDataObj );

	return bResult;
}

// ScriptEventListener::VCallLuaFunction				- Chapter 11, page 335
bool ScriptEventListener::VCallLuaFunction( LuaObject & eventData )
{
	LuaFunction<bool> function( m_HandlerFunction );
	return function( eventData );
}

//-The ScriptActorEventListener.  Same as the ScriptEventListener, but
// passes in an actor's script context, too.
ScriptActorEventListener::ScriptActorEventListener( LuaObject explicitHandlerFunction, const ActorId actorID )
: ScriptEventListener( explicitHandlerFunction )
, m_SrcActorID( actorID )
{
}

// ScriptActorEventListener::VCallLuaFunction			- Chapter 11, page 343
bool ScriptActorEventListener::VCallLuaFunction( LuaObject & eventData )
{
	// Find our actor to pass in the actor script data context.
	
	// This is more sanity checking than anything, to ensure that the actor
	// still exists.
	shared_ptr< IActor > gameActor = g_pApp->m_pGame->VGetActor( m_SrcActorID );
	if ( !gameActor )
	{
		assert( 0 && "Attempted to call a script listener for an actor that couldn't be found!  Did you delete the actor without removing all listeners?" );
		return false;
	}

	// Get ahold of the actor's script data.
	LuaState * pState = g_pApp->m_pLuaStateManager->GetGlobalState().Get();
	LuaObject globalActorTable = g_pApp->m_pLuaStateManager->GetGlobalActorTable();
	assert( globalActorTable.IsTable() && "Global actor table is NOT a table!" );
	LuaObject actorData = globalActorTable[ m_SrcActorID ];

	// We pass in the event data IN ADDITION TO the actor's script data.
	LuaFunction<bool> function( m_HandlerFunction );
	return function( eventData, actorData );
}