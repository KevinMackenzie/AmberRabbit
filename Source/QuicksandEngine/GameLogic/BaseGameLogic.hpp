#ifndef QSE_BASEGAMELOGIC_HPP
#define QSE_BASEGAMELOGIC_HPP


#include "../Mainloop/ProcessManager.hpp"
#include "../Utilities/Math.hpp"
#include "../EventManager/EventManager.hpp"
#include "../Actor/Actor.hpp"

class PathingGraph;
class ActorFactory;
class LevelManager;

enum BaseGameState
{
	BGS_Invalid,
	BGS_Initializing,
	BGS_MainMenu,
	BGS_WaitingForPlayers,
	BGS_LoadingGameEnvironment,
	BGS_WaitingForPlayersToLoadEnvironment,
	BGS_SpawningPlayersActors,
	BGS_Running
};

typedef std::map<ActorId, StrongActorPtr> ActorMap;
typedef std::string Level;

class  LevelManager
{
public:
	const std::vector<Level> &GetLevels() const { return m_Levels; }
	const int GetCurrentLevel() const { return m_CurrentLevel; }
	bool Initialize(std::vector<std::string> &levels);

protected:
	std::vector<Level> m_Levels;
	int m_CurrentLevel;
};



class  BaseGameLogic : public IGameLogic
{
	friend class QuicksandEngineApp;						// This is only to gain access to the view list

protected:
	float m_Lifetime;								//indicates how long this game has been in session
	ProcessManager* m_pProcessManager;				// a game logic entity
	GCCRandom m_random;								// our RNG
	ActorMap m_actors;
	ActorId m_LastActorId;
	BaseGameState m_State;							// game state: loading, running, etc.
	int m_ExpectedPlayers;							// how many local human players
	int m_ExpectedRemotePlayers;					// expected remote human players
	int m_ExpectedAI;								// how many AI players
	int m_HumanPlayersAttached;
	int m_AIPlayersAttached;
	int m_HumanGamesLoaded;
	GameViewList m_gameViews;						// views that are attached to our game
	shared_ptr<PathingGraph> m_pPathingGraph;		// the pathing graph
	ActorFactory* m_pActorFactory;

	bool m_bProxy;									// set if this is a proxy game logic, not a real one
	int m_remotePlayerId;							// if we are a remote player - what is out socket number on the server

	bool m_RenderDiagnostics;						// Are we rendering diagnostics?
	shared_ptr<IGamePhysics> m_pPhysics;

	LevelManager* m_pLevelManager;					// Manages loading and chaining levels

public:

	BaseGameLogic();
	virtual ~BaseGameLogic();
	bool Init(void);

	void SetProxy(bool isProxy)
	{
		m_bProxy = isProxy;
	}
	const bool IsProxy() const { return m_bProxy; }

	// [mrmike] CanRunLua() is a bit of a hack - but I can't have Lua scripts running on the clients. They should belong to the logic.
	// FUTURE WORK - Perhaps the scripts can have a marker or even a special place in the resource file for any scripts that can run on remote clients
	const bool CanRunLua() const { return !IsProxy() || GetState() != BGS_Running; }

	ActorId GetNewActorID(void)
	{
		return ++m_LastActorId;
	}

	shared_ptr<PathingGraph> GetPathingGraph(void) { return m_pPathingGraph; }
	GCCRandom& GetRNG(void) { return m_random; }

	virtual void VAddView(shared_ptr<IGameView> pView, ActorId actorId = INVALID_ACTOR_ID);
	virtual void VRemoveView(shared_ptr<IGameView> pView);

	virtual StrongActorPtr VCreateActor(const std::string &actorResource, tinyxml2::XMLElement *overrides, const glm::mat4* initialTransform = NULL, const ActorId serversActorId = INVALID_ACTOR_ID);  // [rez] note: don't store this strong pointer outside of this class
	virtual void VDestroyActor(const ActorId actorId);
	virtual WeakActorPtr VGetActor(const ActorId actorId);
	virtual void VModifyActor(const ActorId actorId, tinyxml2::XMLElement *overrides);

	virtual void VMoveActor(const ActorId id, glm::mat4 const &mat) {}

	// editor functions
	std::string GetActorXml(const ActorId id);

	// Level management
	const LevelManager* GetLevelManager() { return m_pLevelManager; }
	virtual bool VLoadGame(const char* levelResource) override;  // [rez] Subclasses shouldn't override this function; use VLoadGameDelegate() instead
	virtual void VSetProxy();

	// Logic Update
	virtual void VOnUpdate(float time, float elapsedTime);

	// Changing Game Logic State
	virtual void VChangeState(BaseGameState newState);
	const BaseGameState GetState() const { return m_State; }

	// Render Diagnostics
	void ToggleRenderDiagnostics() { m_RenderDiagnostics = !m_RenderDiagnostics; }
	virtual void VRenderDiagnostics();
	virtual shared_ptr<IGamePhysics> VGetGamePhysics(void) { return m_pPhysics; }

	void AttachProcess(StrongProcessPtr pProcess) { if (m_pProcessManager) { m_pProcessManager->AttachProcess(pProcess); } }

	// event delegates
	void RequestDestroyActorDelegate(IEventDataPtr pEventData);

protected:
	virtual ActorFactory* VCreateActorFactory(void);

	// [rez] Override this function to do any game-specific loading.
	virtual bool VLoadGameDelegate(tinyxml2::XMLElement* pLevelData) { return true; }

	void MoveActorDelegate(IEventDataPtr pEventData);
	void RequestNewActorDelegate(IEventDataPtr pEventData);
};



#endif