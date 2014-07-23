#include "../Stdafx.hpp"
#include "BaseGameLogic.hpp"


#include <mmsystem.h>

#include "../AI/Pathing.hpp"
#include "../EventManager/Events.hpp"				// only for EvtData_Game_State
#include "../Mainloop/Initialization.hpp"			// only for GameOptions
#include "../MainLoop/Process.hpp"
#include "../Network/Network.hpp"
#include "../ResourceCache/XmlResource.hpp"
#include "../Physics/Physics.hpp"
#include "../Actor/Actor.hpp"
#include "../Actor/ActorFactory.hpp"
#include "../Utilities/String.hpp"
#include "../UserInterface/HumanView.hpp"  // [rez] not ideal, but the loading sequence needs to know if this is a human view

#include "BaseGameLogic.hpp"

//========================================================================
//
// LevelManager implementation
//
//========================================================================

bool LevelManager::Initialize(std::vector<std::string> &levels)
{
	std::vector<std::string>::iterator i = levels.begin();
	while (i != levels.end())
	{
		m_Levels.push_back(*i);
		++i;
	}
	return true;
}




//========================================================================
//
// BaseGameLogic implementation
//
//========================================================================


BaseGameLogic::BaseGameLogic()
{
	m_LastActorId = 0;
	m_Lifetime = 0;
	m_pProcessManager = QSE_NEW ProcessManager;
	m_random.Randomize();
	m_State = BGS_Initializing;
	m_bProxy = false;
	m_RenderDiagnostics = false;
	m_ExpectedPlayers = 0;
	m_ExpectedRemotePlayers = 0;
	m_ExpectedAI = 0;
	m_HumanPlayersAttached = 0;
	m_AIPlayersAttached = 0;
	m_HumanGamesLoaded = 0;
	m_pPathingGraph = NULL;
	m_pActorFactory = NULL;

	m_pLevelManager = QSE_NEW LevelManager;
	LOG_ASSERT(m_pProcessManager && m_pLevelManager);
	m_pLevelManager->Initialize(QuicksandEngine::g_pApp->m_ResCache->Match("world\\*.xml"));

	// register script events from the engine
	//   [mrmike] this was moved to the constructor post-press, since this function can be called when new levels are loaded by the game or editor
	RegisterEngineScriptEvents();
}

BaseGameLogic::~BaseGameLogic()
{
	// MrMike: 12-Apr-2009 
	//   Added this to explicitly remove views from the game logic list.
	while (!m_gameViews.empty())
		m_gameViews.pop_front();

	SAFE_DELETE(m_pLevelManager);
	SAFE_DELETE(m_pProcessManager);
	SAFE_DELETE(m_pActorFactory);

	// destroy all actors
	for (auto it = m_actors.begin(); it != m_actors.end(); ++it)
		it->second->Destroy();
	m_actors.clear();

	IEventManager::Get()->VRemoveListener(MakeDelegate(this, &BaseGameLogic::RequestDestroyActorDelegate), EvtData_Request_Destroy_Actor::sk_EventType);
}

bool BaseGameLogic::Init(void)
{
	m_pActorFactory = VCreateActorFactory();
	m_pPathingGraph.reset(CreatePathingGraph());

	IEventManager::Get()->VAddListener(MakeDelegate(this, &BaseGameLogic::RequestDestroyActorDelegate), EvtData_Request_Destroy_Actor::sk_EventType);

	return true;
}

std::string BaseGameLogic::GetActorXml(const ActorId id)
{
	StrongActorPtr pActor = MakeStrongPtr(VGetActor(id));
	if (pActor)
		return pActor->ToXML();
	else
		LOG_ERROR("Couldn't find actor: " + ToStr(id));

	return std::string();
}

bool BaseGameLogic::VLoadGame(const char* levelResource)
{
	// Grab the root XML node
	XMLElement* pRoot = XmlResourceLoader::LoadAndReturnRootXmlElement(levelResource);
	if (!pRoot)
	{
		LOG_ERROR("Failed to find level resource file: " + std::string(levelResource));
		return false;
	}

	// pre and post load scripts
	const char* preLoadScript = NULL;
	const char* postLoadScript = NULL;

	// parse the pre & post script attributes
	XMLElement* pScriptElement = pRoot->FirstChildElement("Script");
	if (pScriptElement)
	{
		preLoadScript = pScriptElement->Attribute("preLoad");
		postLoadScript = pScriptElement->Attribute("postLoad");
	}

	// load the pre-load script if there is one
	if (preLoadScript)
	{
		Resource resource(preLoadScript);
		shared_ptr<ResHandle> pResourceHandle = QuicksandEngine::g_pApp->m_ResCache->GetHandle(&resource);  // this actually loads the XML file from the zip file
	}

	// load all initial actors
	XMLElement* pActorsNode = pRoot->FirstChildElement("StaticActors");
	if (pActorsNode)
	{
		for (XMLElement* pNode = pActorsNode->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement())
		{
			const char* actorResource = pNode->Attribute("resource");

			StrongActorPtr pActor = VCreateActor(actorResource, pNode);
			if (pActor)
			{
				// fire an event letting everyone else know that we created a new actor
				shared_ptr<EvtData_New_Actor> pNewActorEvent(QSE_NEW EvtData_New_Actor(pActor->GetId()));
				IEventManager::Get()->VQueueEvent(pNewActorEvent);
			}
		}
	}

	// initialize all human views
	for (auto it = m_gameViews.begin(); it != m_gameViews.end(); ++it)
	{
		shared_ptr<IGameView> pView = *it;
		if (pView->VGetType() == GameView_Human)
		{
			shared_ptr<HumanView> pHumanView = static_pointer_cast<HumanView, IGameView>(pView);
			pHumanView->LoadGame(pRoot);
		}
	}

	// register script events from the engine
	//   [mrmike] this was moved to the constructor post-press, since this function can be called when new levels are loaded by the game or editor
	// RegisterEngineScriptEvents();

	// call the delegate load function
	if (!VLoadGameDelegate(pRoot))
		return false;  // no error message here because it's assumed VLoadGameDelegate() kicked out the error

	// load the post-load script if there is one
	if (postLoadScript)
	{
		Resource resource(postLoadScript);
		shared_ptr<ResHandle> pResourceHandle = QuicksandEngine::g_pApp->m_ResCache->GetHandle(&resource);  // this actually loads the XML file from the zip file
	}

	// trigger the Environment Loaded Game event - only then can player actors and AI be spawned!
	if (m_bProxy)
	{
		shared_ptr<EvtData_Remote_Environment_Loaded> pNewGameEvent(QSE_NEW EvtData_Remote_Environment_Loaded);
		IEventManager::Get()->VTriggerEvent(pNewGameEvent);
	}
	else
	{
		shared_ptr<EvtData_Environment_Loaded> pNewGameEvent(QSE_NEW EvtData_Environment_Loaded);
		IEventManager::Get()->VTriggerEvent(pNewGameEvent);
	}
	return true;
}



void BaseGameLogic::VSetProxy()
{
	m_bProxy = true;

	IEventManager::Get()->VAddListener(MakeDelegate(this, &BaseGameLogic::RequestNewActorDelegate), EvtData_Request_New_Actor::sk_EventType);

	m_pPhysics.reset(CreateNullPhysics());
}


StrongActorPtr BaseGameLogic::VCreateActor(const std::string &actorResource, XMLElement* overrides, const mat4* initialTransform, const ActorId serversActorId)
{
	LOG_ASSERT(m_pActorFactory);
	if (!m_bProxy && serversActorId != INVALID_ACTOR_ID)
		return StrongActorPtr();

	if (m_bProxy && serversActorId == INVALID_ACTOR_ID)
		return StrongActorPtr();

	StrongActorPtr pActor = m_pActorFactory->CreateActor(actorResource.c_str(), overrides, initialTransform, serversActorId);
	if (pActor)
	{
		m_actors.insert(std::make_pair(pActor->GetId(), pActor));
		if (!m_bProxy && (m_State == BGS_SpawningPlayersActors || m_State == BGS_Running))
		{
			shared_ptr<EvtData_Request_New_Actor> pNewActor(QSE_NEW EvtData_Request_New_Actor(actorResource, initialTransform, pActor->GetId()));
			IEventManager::Get()->VTriggerEvent(pNewActor);
		}
		return pActor;
	}
	else
	{
		// FUTURE WORK: Log error: couldn't create actor
		return StrongActorPtr();
	}
}

void BaseGameLogic::VDestroyActor(const ActorId actorId)
{
	// We need to trigger a synchronous event to ensure that any systems responding to this event can still access a 
	// valid actor if need be.  The actor will be destroyed after this.
	shared_ptr<EvtData_Destroy_Actor> pEvent(QSE_NEW EvtData_Destroy_Actor(actorId));
	IEventManager::Get()->VTriggerEvent(pEvent);

	auto findIt = m_actors.find(actorId);
	if (findIt != m_actors.end())
	{
		findIt->second->Destroy();
		m_actors.erase(findIt);
	}
}

WeakActorPtr BaseGameLogic::VGetActor(const ActorId actorId)
{
	ActorMap::iterator findIt = m_actors.find(actorId);
	if (findIt != m_actors.end())
		return findIt->second;
	return WeakActorPtr();
}

void BaseGameLogic::VModifyActor(const ActorId actorId, XMLElement* overrides)
{
	LOG_ASSERT(m_pActorFactory);
	if (!m_pActorFactory)
		return;

	auto findIt = m_actors.find(actorId);
	if (findIt != m_actors.end())
	{
		m_pActorFactory->ModifyActor(findIt->second, overrides);
	}
}

void BaseGameLogic::VOnUpdate(float time, float elapsedTime)
{
	int deltaMilliseconds = int(elapsedTime * 1000.0f);
	m_Lifetime += elapsedTime;

	switch (m_State)
	{
	case BGS_Initializing:
		// If we get to here we're ready to attach players
		VChangeState(BGS_MainMenu);
		break;

	case BGS_MainMenu:
		break;

	case BGS_LoadingGameEnvironment:
		/***
		// [mrmike] This was modified a little from what you see in the book - VLoadGame() is now called from
		//          BaseGameLogic::VChangeState()
		//
		if (!g_pApp->VLoadGame())
		{
		LOG_ERROR("The game failed to load.");
		g_pApp->AbortGame();
		}
		***/
		break;

	case BGS_WaitingForPlayersToLoadEnvironment:
		if (m_ExpectedPlayers + m_ExpectedRemotePlayers <= m_HumanGamesLoaded)
		{
			VChangeState(BGS_SpawningPlayersActors);
		}
		break;

	case BGS_SpawningPlayersActors:
		VChangeState(BGS_Running);
		break;

	case BGS_WaitingForPlayers:
		if (m_ExpectedPlayers + m_ExpectedRemotePlayers == m_HumanPlayersAttached)
		{
			// The server sends us the level name as a part of the login message. 
			// We have to wait until it arrives before loading the level
			if (!(GET_CONFIG_ELEMENT_STR("LEVEL_ISEMPTY") == "TRUE"))
			{
				VChangeState(BGS_LoadingGameEnvironment);
			}
		}
		break;

	case BGS_Running:
		m_pProcessManager->UpdateProcesses(deltaMilliseconds);

		if (m_pPhysics && !m_bProxy)
		{
			m_pPhysics->VOnUpdate(elapsedTime);
			m_pPhysics->VSyncVisibleScene();
		}

		break;

	default:
		LOG_ERROR("Unrecognized state.");
	}

	// update all game views
	for (GameViewList::iterator it = m_gameViews.begin(); it != m_gameViews.end(); ++it)
	{
		(*it)->VOnUpdate(deltaMilliseconds);
	}

	// update game actors
	for (ActorMap::const_iterator it = m_actors.begin(); it != m_actors.end(); ++it)
	{
		it->second->Update(deltaMilliseconds);
	}

}

//
// BaseGameLogic::VChangeState				- Chapter 19, page 710
//
void BaseGameLogic::VChangeState(BaseGameState newState)
{
	if (newState == BGS_WaitingForPlayers)
	{
		// Get rid of the Main Menu...
		m_gameViews.pop_front();

		// Note: Split screen support would require this to change!
		m_ExpectedPlayers = 1;
		m_ExpectedRemotePlayers = GET_CONFIG_ELEMENT_S("EXPECTED_PLAYERS") - 1;
		m_ExpectedAI = GET_CONFIG_ELEMENT_S("EXPECTED_AIS");

		if (!(GET_CONFIG_ELEMENT_STR("GAME_HOST_EMPTY") == "TRUE"))
		{
			VSetProxy();
			m_ExpectedAI = 0;				// the server will create these
			m_ExpectedRemotePlayers = 0;	// the server will create these

			if (!QuicksandEngine::g_pApp->AttachAsClient())
			{
				// Throw up a main menu
				VChangeState(BGS_MainMenu);
				return;
			}
		}
		else if (m_ExpectedRemotePlayers > 0)
		{
			BaseSocketManager *pServer = QSE_NEW BaseSocketManager();
			if (!pServer->Init())
			{
				// Throw up a main menu
				VChangeState(BGS_MainMenu);
				return;
			}

			pServer->AddSocket(new GameServerListenSocket(GET_CONFIG_ELEMENT_S("LISTENER_PORT")));
			QuicksandEngine::g_pApp->m_pBaseSocketManager = pServer;
		}
	}
	else if (newState == BGS_LoadingGameEnvironment)
	{
		m_State = newState;
		if (!QuicksandEngine::g_pApp->VLoadGame())
		{
			LOG_ERROR("The game failed to load.");
			QuicksandEngine::g_pApp->AbortGame();
		}
		else
		{
			VChangeState(BGS_WaitingForPlayersToLoadEnvironment);			// we must wait for all human player to report their environments are loaded.
			return;
		}
	}

	m_State = newState;
}


// Chapter 19/20 refactor work
//   - move physics pointer into BaseGameLogic
//   - but make the inherited logic choose the implementation
void BaseGameLogic::VRenderDiagnostics()
{
	if (m_RenderDiagnostics)
	{
		m_pPhysics->VRenderDiagnostics();
	}
}


void BaseGameLogic::VAddView(shared_ptr<IGameView> pView, ActorId actorId)
{
	// This makes sure that all views have a non-zero view id.
	int viewId = static_cast<int>(m_gameViews.size());
	m_gameViews.push_back(pView);
	pView->VOnAttach(viewId, actorId);
	pView->VOnRestore();
}


void BaseGameLogic::VRemoveView(shared_ptr<IGameView> pView)
{
	m_gameViews.remove(pView);
}


ActorFactory* BaseGameLogic::VCreateActorFactory(void)
{
	return QSE_NEW ActorFactory;
}



void BaseGameLogic::RequestDestroyActorDelegate(IEventDataPtr pEventData)
{
	shared_ptr<EvtData_Request_Destroy_Actor> pCastEventData = static_pointer_cast<EvtData_Request_Destroy_Actor>(pEventData);
	VDestroyActor(pCastEventData->GetActorId());
}



// [mrmike] - These were moved here after the chapter for BaseGameLogic was written. 
//            These were originally in TeapotWarsLogic class, but should really be in the BaseLogic class.

void BaseGameLogic::MoveActorDelegate(IEventDataPtr pEventData)
{
	shared_ptr<EvtData_Move_Actor> pCastEventData = static_pointer_cast<EvtData_Move_Actor>(pEventData);
	VMoveActor(pCastEventData->GetId(), pCastEventData->GetMatrix());
}

void BaseGameLogic::RequestNewActorDelegate(IEventDataPtr pEventData)
{
	// This should only happen if the game logic is a proxy, and there's a server asking us to create an actor.
	LOG_ASSERT(m_bProxy);
	if (!m_bProxy)
		return;

	shared_ptr<EvtData_Request_New_Actor> pCastEventData = static_pointer_cast<EvtData_Request_New_Actor>(pEventData);

	// create the actor
	StrongActorPtr pActor = VCreateActor(pCastEventData->GetActorResource(), NULL, pCastEventData->GetInitialTransform(), pCastEventData->GetServerActorId());
	if (pActor)
	{
		shared_ptr<EvtData_New_Actor> pNewActorEvent(QSE_NEW EvtData_New_Actor(pActor->GetId(), pCastEventData->GetViewId()));
		IEventManager::Get()->VQueueEvent(pNewActorEvent);
	}
}



