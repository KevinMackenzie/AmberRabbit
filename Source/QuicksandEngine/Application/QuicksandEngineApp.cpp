#include "../Stdafx.hpp"
#include <shlobj.h>
#include <direct.h>


#include "QuicksandEngineApp.hpp"

#include "../Physics/PhysicsEventListener.hpp"
#include "../MainLoop/Initialization.hpp"
//#include "../Debugging/MiniDump.hpp"
#include "../GameLogic/BaseGameLogic.hpp"
#include "../Graphics3D/GLRenderer.hpp"
#include "../EventManager/EventManagerImpl.hpp"
#include "../EventManager/Events.hpp"
#include "../Network/Network.hpp"
#include "../LUAScripting/LuaStateManager.hpp"
#include "../LUAScripting/ScriptExports.hpp"
#include "../LUAScripting/ScriptProcess.hpp"
#include "../ResourceCache/ResCache.hpp"
#include "../ResourceCache/XmlResource.hpp"
#include "../UserInterface/UserInterface.hpp"
#include "../UserInterface/MessageBox.hpp"
#include "../UserInterface/HumanView.hpp"
#include "../Utilities/Math.hpp"
#include "../Utilities/String.hpp"
#include "../Actor/BaseScriptComponent.hpp"

const char* SCRIPT_PREINIT_FILE = "Scripts\\PreInit.lua";

namespace QuicksandEngine
{
	QuicksandEngineApp* g_pApp;
}

QuicksandEngineApp::QuicksandEngineApp()
{
	QuicksandEngine::g_pApp = this;
	m_pGame = NULL;

	//m_rcDesktop.bottom = m_rcDesktop.left = m_rcDesktop.right = m_rcDesktop.top = 0;
	m_screenSize.x = m_screenSize.y = 0;
	m_iColorDepth = 32;

	m_bIsRunning = false;
	m_bIsEditorRunning = false;

	m_pEventManager = NULL;
	m_ResCache = NULL;

	m_pNetworkEventForwarder = NULL;
	m_pBaseSocketManager = NULL;

	m_bQuitRequested = false;
	m_bQuitting = false;
	m_HasModalDialog = 0;
}


HWND QuicksandEngineApp::GetHwnd()
{
	return glfwGetWin32Window(m_pWindow);
}


//===================================================================
// Win32 Specific Stuff
//
// InitInstance - this checks system resources, creates your window, and launches the game
//
// preprocessor macro setting in the C++ options of the project provides the base macro
// C preprocessor string concatenation takes care of the rest.
//
//===================================================================

bool QuicksandEngineApp::InitInstance(HINSTANCE hInstance, LPSTR lpCmdLine, HWND hWnd, int screenWidth, int screenHeight)
{
	// Check for existing instance of the same window
	// 
#ifndef _DEBUG
	// Note - it can be really useful to debug network code to have
	// more than one instance of the game up at one time - so
	// feel free to comment these lines in or out as you wish!

	char* title = VGetGameTitle();
	wchar_t *wTitle = new wchar_t[strlen(title)];
	mbstowcs(wTitle, title, strlen(title));
	if (!IsOnlyInstance(wTitle));
		return false;
#endif



	// Show the cursor and clip it when in full screen
	m_pCursor = GetCursor();

	// We don't need a mouse cursor by default, let the game turn it on
	SetCursor(NULL);

	// Check for adequate machine resources.
	bool resourceCheck = false;
	while (!resourceCheck)
	{
		const DWORDLONG physicalRAM = 512 * MEGABYTE;
		const DWORDLONG virtualRAM = 1024 * MEGABYTE;
		const DWORDLONG diskSpace = 10 * MEGABYTE;
		if (!CheckStorage(diskSpace))
			return false;

		const DWORD minCpuSpeed = 1300;			// 1.3Ghz
		DWORD thisCPU = ReadCPUSpeed();
		if (thisCPU < minCpuSpeed)
		{
			LOG_ERROR("GetCPUSpeed reports CPU is too slow for this game.");
			return false;
		}

		resourceCheck = true;
	}

	m_hInstance = hInstance;

	// register all events
	RegisterEngineEvents();
	VRegisterGameEvents();

	//
	// Initialize the ResCache - Chapter 5, page 141
	//
	//    Note - this is a little different from the book. Here we have a speccial resource ZIP file class, DevelopmentResourceZipFile,
	//    that actually reads directly from the source asset files, rather than the ZIP file. This is MUCH better during development, since
	//    you don't want to have to rebuild the ZIP file every time you make a minor change to an asset.
	//
	IResourceFile *zipFile = (m_bIsEditorRunning || GET_CONFIG_ELEMENT_STR("USE_DEVELOPMENT_DIRECTORIES") == "TRUE") ?
		QSE_NEW DevelopmentResourceZipFile(L"Assets.zip", DevelopmentResourceZipFile::Editor) :
		QSE_NEW ResourceZipFile(L"Assets.zip");

	m_ResCache = QSE_NEW ResCache(50, zipFile);

	if (!m_ResCache->Init())
	{
		LOG_ERROR("Failed to initialize resource cache!  Are your paths set up correctly?");
		return false;
	}

	extern shared_ptr<IResourceLoader> CreateWAVResourceLoader();
	extern shared_ptr<IResourceLoader> CreateOGGResourceLoader();
	extern shared_ptr<IResourceLoader> CreateDDSResourceLoader();
	extern shared_ptr<IResourceLoader> CreatePNGResourceLoader();
	extern shared_ptr<IResourceLoader> CreateJPGResourceLoader();
	extern shared_ptr<IResourceLoader> CreateTTFResourceLoader();
	extern shared_ptr<IResourceLoader> CreatePROGResourceLoader();
	extern shared_ptr<IResourceLoader> CreateVERTResourceLoader();
	extern shared_ptr<IResourceLoader> CreateCTRLResourceLoader();
	extern shared_ptr<IResourceLoader> CreateEVALResourceLoader();
	extern shared_ptr<IResourceLoader> CreateGEOMResourceLoader();
	extern shared_ptr<IResourceLoader> CreateFRAGResourceLoader();
	//extern shared_ptr<IResourceLoader> CreateJPGResourceLoader();
	extern shared_ptr<IResourceLoader> CreateXmlResourceLoader();
	extern shared_ptr<IResourceLoader> CreateObjMeshResourceLoader();
	extern shared_ptr<IResourceLoader> CreateScriptResourceLoader();

	// Note - register these in order from least specific to most specific! They get pushed onto a list.
	// RegisterLoader is discussed in Chapter 5, page 142
	m_ResCache->RegisterLoader(CreateWAVResourceLoader());
	m_ResCache->RegisterLoader(CreateOGGResourceLoader());
	m_ResCache->RegisterLoader(CreateDDSResourceLoader());
	m_ResCache->RegisterLoader(CreatePNGResourceLoader());
	m_ResCache->RegisterLoader(CreateJPGResourceLoader());
	m_ResCache->RegisterLoader(CreateTTFResourceLoader());
	m_ResCache->RegisterLoader(CreatePROGResourceLoader());
	m_ResCache->RegisterLoader(CreateVERTResourceLoader());
	m_ResCache->RegisterLoader(CreateCTRLResourceLoader());
	m_ResCache->RegisterLoader(CreateEVALResourceLoader());
	m_ResCache->RegisterLoader(CreateGEOMResourceLoader());
	m_ResCache->RegisterLoader(CreateFRAGResourceLoader());
	m_ResCache->RegisterLoader(CreateXmlResourceLoader());
	m_ResCache->RegisterLoader(CreateObjMeshResourceLoader());
	m_ResCache->RegisterLoader(CreateScriptResourceLoader());

	if (!LoadStrings("en_US"))
	{
		LOG_ERROR("Failed to load strings");
		return false;
	}

	// Rez up the Lua State manager now, and run the initial script - discussed in Chapter 5, page 144.
	if (!LuaStateManager::Create())
	{
		LOG_ERROR("Failed to initialize Lua");
		return false;
	}

	// Load the preinit file.  This is within braces to create a scope and destroy the resource once it's loaded.  We
	// don't need to do anything with it, we just need to load it.
	{
		Resource resource(SCRIPT_PREINIT_FILE);
		shared_ptr<ResHandle> pResourceHandle = m_ResCache->GetHandle(&resource);  // this actually loads the XML file from the zip file
	}

	// Register function exported from C++
	ScriptExports::Register();
	ScriptProcess::RegisterScriptClass();
	BaseScriptComponent::RegisterScriptFunctions();

	// The event manager should be created next so that subsystems can hook in as desired.
	// Discussed in Chapter 5, page 144
	m_pEventManager = QSE_NEW EventManager("QuicksandEngineApp Event Mgr", true);
	if (!m_pEventManager)
	{
		LOG_ERROR("Failed to create EventManager.");
		return false;
	}

	// DXUTInit, DXUTCreateWindow - Chapter 5, page 145	
	//DXUTInit(true, true, lpCmdLine, true); // Parse the command line, handle the default hotkeys, and show msgboxes

	m_pWindow = glfwCreateWindow(screenWidth, screenHeight, VGetGameTitle(), (GET_CONFIG_ELEMENT_STR("FULLSCREEN") == "TRUE") ? glfwGetPrimaryMonitor() : nullptr, nullptr);

	if (!m_pWindow)
	{
		LOG_ERROR("GLFW window creation failed!");
		return false;
	}

	glfwMakeContextCurrent(m_pWindow);
	
	if (!GLUFInitOpenGLExtentions())
		return false;

	Resource ctrlRes("Art\\" + GET_CONFIG_ELEMENT_STR("CONTROL_TEXTURE"));
	shared_ptr<ResHandle> pCtrlHandle = m_ResCache->GetHandle(&ctrlRes);

	if (!GLUFInitGui(m_pWindow, QuicksandEngineApp::MsgProc, static_pointer_cast<GLTextureResourceExtraData>(pCtrlHandle->GetExtra())->GetTexture()))
		return false;

	//since GLUFInitGui initializes the freetype library, this must be called afterwards
	Resource defFontRes("Art\\Font\\" + GET_CONFIG_ELEMENT_STR("DEFAULT_FONT"));
	shared_ptr<ResHandle> pDefFontHandle = m_ResCache->GetHandle(&defFontRes);
	GLUFSetDefaultFont(static_pointer_cast<TTFResourceExtraData>(pDefFontHandle->GetExtra())->GetFont());


	GLRenderer_Base::g_pDialogResourceManager = QSE_NEW GLUFDialogResourceManager();
	GLRenderer_Base::g_pTextHelper = QSE_NEW GLUFTextHelper(GLRenderer_Base::g_pDialogResourceManager, 20);

	/*if (hWnd == NULL)
	{
		DXUTCreateWindow(VGetGameTitle(), hInstance, VGetIcon());
	}
	else
	{
		DXUTSetWindow(hWnd, hWnd, hWnd);
	}*/

	if (!GetHwnd())
	{
		return FALSE;
	}
	//SetWindowText(GetHwnd(), VGetGameTitle());

	// initialize the directory location you can store save game files
	strcpy(m_saveGameDirectory, GetSaveGameDirectory(GetHwnd(), VGetGameAppDirectory()));

	// DXUTCreateDevice - Chapter 5 - page 139
	m_screenSize = Point(screenWidth, screenHeight);

	//set the screen position
	glfwSetWindowPos(m_pWindow, GET_CONFIG_ELEMENT_I("WINDOW_XPOS"), GET_CONFIG_ELEMENT_I("WINDOW_YPOS"));

	//If you have an older video card that only supports D3D9, comment in the next line, and make sure 
	//the renderer setting in Game\PlayerOptions.xml is set to "Direct3D 9"
	//DXUTCreateDevice( D3D_FEATURE_LEVEL_9_3, true, screenWidth, screenHeight);
	//DXUTCreateDevice(D3D_FEATURE_LEVEL_10_1, true, screenWidth, screenHeight);

	/*if (GetRendererImpl() == Renderer_D3D9)
	{
		m_Renderer = shared_ptr<IRenderer>(QSE_NEW D3DRenderer9());
	}
	else if (GetRendererImpl() == Renderer_D3D11)
	{
		m_Renderer = shared_ptr<IRenderer>(QSE_NEW D3DRenderer11());
	}*/
	m_Renderer = shared_ptr<IRenderer>(QSE_NEW GLRenderer());
	m_Renderer->VSetBackgroundColor(255, 20, 20, 200);
	m_Renderer->VOnRestore();


	// You usually must have an HWND to initialize your game views...
	//    VCreateGameAndView			- Chapter 5, page 145
	m_pGame = VCreateGameAndView();
	if (!m_pGame)
		return false;

	// now that all the major systems are initialized, preload resources 
	//    Preload calls are discussed in Chapter 5, page 148
	m_ResCache->Preload("*.ogg", NULL);
	m_ResCache->Preload("*.dds", NULL);
	m_ResCache->Preload("*.png", NULL);
	m_ResCache->Preload("*.jpg", NULL);
	m_ResCache->Preload("*.prog", NULL);
	m_ResCache->Preload("*.obj.model", NULL);
	m_ResCache->Preload("*.font", NULL);

	CheckForJoystick(GetHwnd());

	m_bIsRunning = true;

	return TRUE;
}


bool QuicksandEngineApp::VLoadGame(void)
{
	// Read the game options and see what the current game
	// needs to be - all of the game graphics are initialized by now, too...
	return m_pGame->VLoadGame(GET_CONFIG_ELEMENT_STR("LEVEL").c_str());
}

void QuicksandEngineApp::RegisterEngineEvents(void)
{
	REGISTER_EVENT(EvtData_Environment_Loaded);
	REGISTER_EVENT(EvtData_New_Actor);
	REGISTER_EVENT(EvtData_Move_Actor);
	REGISTER_EVENT(EvtData_Destroy_Actor);
	REGISTER_EVENT(EvtData_Request_New_Actor);
	REGISTER_EVENT(EvtData_Network_Player_Actor_Assignment);
}

//
// QuicksandEngineApp::LoadStrings										- Chapter 5, page 143
//
bool QuicksandEngineApp::LoadStrings(std::string language)
{
	std::string languageFile = "Lang\\";
	languageFile += language;
	languageFile += ".xml";

	tinyxml2::XMLElement* pRoot = XmlResourceLoader::LoadAndReturnRootXmlElement(languageFile.c_str());
	if (!pRoot)
	{
		LOG_ERROR("Strings are missing.");
		return false;
	}

	// Loop through each child element and load the component
	for (tinyxml2::XMLElement* pElem = pRoot->FirstChildElement(); pElem; pElem = pElem->NextSiblingElement())
	{
		const char *pKey = pElem->Attribute("id");
		const char *pText = pElem->Attribute("value");
		const char *pHotkey = pElem->Attribute("hotkey");
		if (pKey && pText)
		{
			wchar_t wideKey[64];
			wchar_t wideText[1024];
			AnsiToWideCch(wideKey, pKey, 64);
			AnsiToWideCch(wideText, pText, 1024);
			m_textResource[std::wstring(wideKey)] = std::wstring(wideText);

			if (pHotkey)
			{
				m_hotkeys[std::wstring(wideKey)] = MapCharToKeycode(*pHotkey);
			}
		}
	}
	return true;
}

UINT QuicksandEngineApp::MapCharToKeycode(const char pHotKey)
{
	if (pHotKey >= '0' && pHotKey <= '9')
		return 0x30 + pHotKey - '0';

	if (pHotKey >= 'A' && pHotKey <= 'Z')
		return 0x41 + pHotKey - 'A';

	LOG_ASSERT(0 && "Platform specific hotkey is not defined");
	return 0;
}


//----------------------------------------------------------
// QuicksandEngineApp::GetString								- Chapter 5, page 144
//
// creates a string from a string resource ID in the string table
// stored in a special DLL, LANG.DLL, so game text strings
// can be language independant
//
std::wstring QuicksandEngineApp::GetString(std::wstring sID)
{
	auto localizedString = m_textResource.find(sID);
	if (localizedString == m_textResource.end())
	{
		LOG_ASSERT(0 && "String not found!");
		return L"";
	}
	return localizedString->second;
}


//----------------------------------------------------------
// Win32 Specific Message Handlers
//
// WndProc - the main message handler for the window class
//
// OnNcCreate - this is where you can set window data before it is created
// OnMove - called whenever the window moves; used to update members of g_App
// OnDeviceChange - called whenever you eject the CD-ROM.
// OnDisplayChange - called whenever the user changes the desktop settings
// OnPowerBroadcast - called whenever a power message forces a shutdown
// OnActivate - called whenever windows on the desktop change focus.
//
// Note: pUserContext added to comply with DirectX 9c - June 2005 Update
//
bool QuicksandEngineApp::MsgProc(GLUF_MESSAGE_TYPE msg, int param1, int param2, int param3, int param4)
{
	// Always allow dialog resource manager calls to handle global messages
	// so GUI state is updated correctly
	//TODO:
	
	bool *pbNoFurtherProcessing = new bool;
	*pbNoFurtherProcessing = GLRenderer::g_pDialogResourceManager->MsgProc(GLUF_PASS_CALLBACK_PARAM);
	if (*pbNoFurtherProcessing)
		return 0;
	
	bool result = false;

	switch (msg)
	{
	/*case WM_POWERBROADCAST:
	{
		int event = (int)wParam;
		result = QuicksandEngine::g_pApp->OnPowerBroadcast(event);
		break;
	}*/

	case GM_RESIZE:
	{
		//int colorDepth = (int)wParam;
		//int width = (int)LOWORD(lParam);
		//int height = (int)HIWORD(lParam);

		result = QuicksandEngine::g_pApp->OnDisplayChange(16/*TODO*/, param1, param2);
	}

	case GM_CLOSE:
	case GM_FOCUS:
	{
		result = QuicksandEngine::g_pApp->OnSysCommand(msg, param1, param2);
		if (result)
		{
			*pbNoFurtherProcessing = true;
		}
	}

	/*case WM_SYSKEYDOWN:
	{
		if (wParam == VK_RETURN)
		{
			*pbNoFurtherProcessing = true;
			return QuicksandEngine::g_pApp->OnAltEnter();
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}*/


	case GM_KEY:
	{
		if (param1 == GLFW_KEY_ESCAPE)//DEBUG MODE ONLY
		{
			// DXUT apps choose ESC key as a default exit command.
			// GameCode4 doesn't like this so we disable it by checking 
			// the m_bQuitting bool, and if we're not really quitting
			// set the "no further processing" parameter to true.
			if (QuicksandEngine::g_pApp->m_bQuitting)
			{
				result = QuicksandEngine::g_pApp->OnClose();
			}
			else
			{
				*pbNoFurtherProcessing = true;
			}
		}
	}
	case GM_UNICODE_CHAR:
	case GM_CURSOR_POS:
	case GM_MB:
	case GM_POS:
	//case MM_JOY1BUTTONDOWN:
	//case MM_JOY1BUTTONUP:
	//case MM_JOY1MOVE:
	//case MM_JOY1ZMOVE:
	//case MM_JOY2BUTTONDOWN:
	//case MM_JOY2BUTTONUP:
	//case MM_JOY2MOVE:
	//case MM_JOY2ZMOVE:
	{
		//
		// See Chapter 10, page 278 for more explanation of this code.
		//
		if (QuicksandEngine::g_pApp->m_pGame)
		{
			BaseGameLogic *pGame = QuicksandEngine::g_pApp->m_pGame;
			// Note the reverse order! User input is grabbed first from the view that is on top, 
			// which is the last one in the list.
			AppMsg appMsg;
			//msg.m_hWnd = hWnd;
			appMsg.m_Event = msg;
			appMsg.param1 = param1;
			appMsg.param2 = param2;
			appMsg.param3 = param3;
			appMsg.param4 = param4;
			for (GameViewList::reverse_iterator i = pGame->m_gameViews.rbegin(); i != pGame->m_gameViews.rend(); ++i)
			{
				if ((*i)->VOnMsgProc(appMsg))
				{
					result = true;
					break;				// WARNING! This breaks out of the for loop.
				}
			}
		}
		break;
	}

		/**********************
		WARNING!!!!! You MIGHT think you need this, but if you use the DirectX
		Framework the DefWindowProc is called for you....

		default:*/
		//return DefWindowProc(hWnd, message, wParam, lParam);
		/*
		***********************/
	}

	return result;
}


//=========================================================
// QuicksandEngineApp::OnNcCreate
//
// Handles the WM_NCCREATE message
//
// Not described in the book
// It sets a few members of the CREATESTRUCT, so the 
// window looks like we want it to look.
//=========================================================

bool QuicksandEngineApp::OnNcCreate(LPCREATESTRUCT cs)
{
	// If you want to override something in the CREATESTRUCT, do it here!
	// You'll usually do something like change window borders, etc.
	return true;
}

//=========================================================
// QuicksandEngineApp::OnDisplayChange - Chapter X, page Y
//
// Handles the WM_DISPLAYCHANGE message
//
//=========================================================

bool QuicksandEngineApp::OnDisplayChange(int colorDepth, int width, int height)
{
	m_screenSize.x = width;
	m_screenSize.y = height;
	m_iColorDepth = colorDepth;

	return 0;
}

//=========================================================
// QuicksandEngineApp::OnPowerBroadcast - Chapter X, page Y
//
// Handles the WM_POWERBROADCAST message
//
//=========================================================
//TODO: never called
bool QuicksandEngineApp::OnPowerBroadcast(int event)
{
	// Don't allow the game to go into sleep mode
	if (event == PBT_APMQUERYSUSPEND)
		return false;
	else if (event == PBT_APMBATTERYLOW)
	{
		AbortGame();
	}

	return true;
}

//=========================================================
// QuicksandEngineApp::OnSysCommand - Chapter X, page Y
//
// Handles the WM_SYSCOMMAND message
//
//=========================================================

bool QuicksandEngineApp::OnSysCommand(GLUF_MESSAGE_TYPE msg, int param1, int param2)
{
	switch (msg)
	{
	case GM_FOCUS:
	{
		// If windowed and ready...
		if (m_bWindowedMode && IsRunning())
		{
			// Make maximize into FULLSCREEN toggle
			OnAltEnter();
		}
	}
		return 0;

	case GM_CLOSE:
	{
		// The quit dialog confirmation would appear once for
		// every SC_CLOSE we get - which happens multiple times
		// if modal dialogs are up.  This now uses the g_QuitNoPrompt
		// flag to only prompt when receiving a SC_CLOSE that isn't
		// generated by us (identified by g_QuitNoPrompt).

		// If closing, prompt to close if this isn't a forced quit
		//if (lParam != g_QuitNoPrompt)
		//{
			// ET - 05/21/01 - Bug #1916 - Begin
			// We were receiving multiple close dialogs
			// when closing again ALT-F4 while the close
			// confirmation dialog was up.
			// Eat if already servicing a close
			if (m_bQuitRequested)
				return true;

			// Wait for the application to be restored
			// before going any further with the new 
			// screen.  Flash until the person selects
			// that they want to restore the game, then
			// reinit the display if fullscreen.  
			// The reinit is necessary otherwise the game
			// will switch to windowed mode.

			// Quit requested
			m_bQuitRequested = true;
			// Prompt
			if (GLMessageBox::Ask(QUESTION_QUIT_GAME) == IDNO)
			{
				// Bail - quit aborted

				// Reset quit requested flag
				m_bQuitRequested = false;

				return true;
			}
		//}

		m_bQuitting = true;
		m_bIsRunning = false;

		// Is there a game modal dialog up?
		if (HasModalDialog())
		{
			// Close the modal
			// and keep posting close to the app
			ForceModalExit();

			// Reissue the close to the app

			// Issue the new close after handling the current one,
			// but send in g_QuitNoPrompt to differentiate it from a 
			// regular CLOSE issued by the system.
			//PostMessage(GetHwnd(), WM_SYSCOMMAND, SC_CLOSE, g_QuitNoPrompt);

			m_bQuitRequested = false;

			// Eat the close
			return true;
		}

		// Reset the quit after any other dialogs have popped up from this close
		m_bQuitRequested = false;
	}
		return 0;

	default:
		break;
		// return non-zero of we didn't process the SYSCOMMAND message
		//return DefWindowProc(GetHwnd(), WM_SYSCOMMAND, wParam, lParam);
	}

	return true;
}

void QuicksandEngineApp::GLFrameRender(double dTime, double dEllapsed)
{
	BaseGameLogic *pGame = m_pGame;

	for (GameViewList::iterator i = pGame->m_gameViews.begin(),
		end = pGame->m_gameViews.end(); i != end; ++i)
	{
		(*i)->VOnRender(dTime, dEllapsed);
	}

	m_pGame->VRenderDiagnostics();
}

//=========================================================
// QuicksandEngineApp::OnClose - Chapter 5, page 152
//
// Handles the WM_CLOSE message
//
//=========================================================

bool QuicksandEngineApp::OnClose()
{
	// release all the game systems in reverse order from which they were created

	SAFE_DELETE(m_pGame);

	DestroyWindow(GetHwnd());

	VDestroyNetworkEventForwarder();

	SAFE_DELETE(m_pBaseSocketManager);

	SAFE_DELETE(m_pEventManager);

	BaseScriptComponent::UnregisterScriptFunctions();
	ScriptExports::Unregister();
	LuaStateManager::Destroy();

	SAFE_DELETE(m_ResCache);

	return false;
}



//=========================================================
// QuicksandEngineApp::FlashWhileMinimized - Chapter 5, page 149
//
// Wait for the application to be restored
// before going any further with the new 
// screen.  Flash until the person selects
// that they want to restore the game.
//
//=========================================================

void QuicksandEngineApp::FlashWhileMinimized()
{
	// Flash the application on the taskbar
	// until it's restored.
	if (!GetHwnd())
		return;

	// Blink the application if we are minimized,
	// waiting until we are no longer minimized
	if (IsIconic(GetHwnd()))
	{
		// Make sure the app is up when creating a new screen
		// this should be the case most of the time, but when
		// we close the app down, minimized, and a confirmation
		// dialog appears, we need to restore
		double now = glfwGetTime();
		double then = now;
		MSG msg;

		FlashWindow(GetHwnd(), true);

		while (true)
		{
			if (PeekMessage(&msg, NULL, 0, 0, 0))
			{
				if (msg.message != WM_SYSCOMMAND || msg.wParam != SC_CLOSE)
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

				// Are we done?
				if (!IsIconic(GetHwnd()))
				{
					FlashWindow(GetHwnd(), false);
					break;
				}
			}
			else
			{
				now = glfwGetTime();
				double timeSpan = now > then ? (now - then) : (then - now);
				if (timeSpan > 1.0)
				{
					then = now;
					FlashWindow(GetHwnd(), true);
				}
			}
		}
	}
}



//=========================================================
// QuicksandEngineApp::OnAltEnter
//
// Called when the player hits Alt-Enter to flip the 
// display mode.
//
// Not discussed in the book.
//=========================================================

bool QuicksandEngineApp::OnAltEnter()
{
	//TODO:
	//DXUTToggleFullScreen();
	return 0;
}

//
// QuicksandEngineApp::GetHumanView()					- not described in the book
//
//    FUTURE WORK - This function should accept a player number for split screen games
//
HumanView* QuicksandEngineApp::GetHumanView()
{
	HumanView *pView = NULL;
	for (GameViewList::iterator i = m_pGame->m_gameViews.begin(); i != m_pGame->m_gameViews.end(); ++i)
	{
		if ((*i)->VGetType() == GameView_Human)
		{
			shared_ptr<IGameView> pIGameView(*i);
			pView = static_cast<HumanView *>(&*pIGameView);
			break;
		}
	}
	return pView;
}

shared_ptr<GLMessageBox> g_Modal = nullptr;
bool ModalSysCallback(GLUF_GUI_CALLBACK_PARAM)
{
	//call the callback of the modal ONLY (NOTE: this only returns true or false anyway, so the conversion is OK)
	return (bool)g_Modal->VOnMsgProc({ msg, param1, param2, param3, param4 });
}

//
// class QuicksandEngineApp::Modal						- Chapter 10, page 293
//
int QuicksandEngineApp::Modal(shared_ptr<IScreenElement> pModalScreen, int defaultAnswer)
{
	// If we're going to display a dialog box, we need a human view 
	// to interact with.

	// [mrmike] This bit of code was refactored post press into QuicksandEngineApp::GetHumanView()
	/***
	HumanView *pView;
	for(GameViewList::iterator i=m_pGame->m_gameViews.begin(); i!=m_pGame->m_gameViews.end(); ++i)
	{
	if ((*i)->VGetType()==GameView_Human)
	{
	shared_ptr<IGameView> pIGameView(*i);
	pView = static_cast<HumanView *>(&*pIGameView);
	break;
	}
	}
	***/


	HumanView* pView = GetHumanView();

	if (!pView)
	{
		// Whoops! There's no human view attached.
		return defaultAnswer;
	}

	if (m_HasModalDialog & 0x10000000)
	{
		LOG_ASSERT(0 && "Too Many nested dialogs!");
		return defaultAnswer;
	}

	LOG_ASSERT(GetHwnd() != NULL && _T("Main Window is NULL!"));
	if ((GetHwnd() != NULL) && IsIconic(GetHwnd()))
	{
		FlashWhileMinimized();
	}


	PGLUFCALLBACK old = GLUFChangeCallbackFunc(ModalSysCallback);

	m_HasModalDialog <<= 1;
	m_HasModalDialog |= 1;

	pView->VPushElement(pModalScreen);

	g_Modal = static_pointer_cast<GLMessageBox>(pModalScreen);
	//LPARAM lParam = 0;
	//int result = PumpUntilMessage(g_MsgEndModal, nullptr, &lParam);

	BaseGameLogic *pGame = m_pGame;
	int buttonResult = 0;
	double currentTime = GLUFGetTime();
	for (;;)
	{
		buttonResult = g_Modal->GetButtonPressed();
		if (buttonResult)
		{
			break;
		}

		// Update the game views, but nothing else!
		// Remember this is a modal screen.
		// [Kevin] although this is a modal dialog, we still want to update everything!
		GLUFStats();


		double timeNow = GLUFGetTime();
		float deltatime = float(timeNow - currentTime);

		currentTime = timeNow;

		OnUpdateGame(timeNow, deltatime);
		GLFrameRender(timeNow, deltatime);

		glfwSwapBuffers(QuicksandEngine::g_pApp->GetWindow());
		glfwPollEvents();
	}

	
	/*if (lParam != 0)
	{
		if (lParam == g_QuitNoPrompt)
			result = defaultAnswer;
		else
			result = (int)lParam;
	}*/

	pView->VRemoveElement(pModalScreen);
	m_HasModalDialog >>= 1;

	GLUFChangeCallbackFunc(old);

	return buttonResult;
}


//
// class QuicksandEngineApp::PumpUntilMessage			- Chapter 10, page 295
//
int QuicksandEngineApp::PumpUntilMessage(UINT msgEnd, WPARAM* pWParam, LPARAM* pLParam)
{
	int currentTime = GLUFGetTimeMs();
	MSG msg;
	PGLUFCALLBACK old = GLUFChangeCallbackFunc(::ModalSysCallback);
	for (;;)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (msg.message == WM_CLOSE)
			{
				m_bQuitting = true;
				GetMessage(&msg, NULL, 0, 0);
				break;
			}
			else
			{
				// Default processing
				if (GetMessage(&msg, NULL, NULL, NULL))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

				// Are we done?
				if (msg.message == msgEnd)
					break;
			}
		}
		else
		{
			// Update the game views, but nothing else!
			// Remember this is a modal screen.
			if (m_pGame)
			{
				int timeNow = GLUFGetTimeMs();
				int deltaMilliseconds = timeNow - currentTime;
				for (GameViewList::iterator i = m_pGame->m_gameViews.begin(); i != m_pGame->m_gameViews.end(); ++i)
				{
					(*i)->VOnUpdate(deltaMilliseconds);
				}
				currentTime = timeNow;
				//TODO:
				//DXUTRender3DEnvironment();
			}
			glfwSwapBuffers(m_pWindow);
		}
	}
	GLUFChangeCallbackFunc(old);

	if (pLParam)
		*pLParam = msg.lParam;
	if (pWParam)
		*pWParam = msg.wParam;

	return 0;
}


//This function removes all of a *SPECIFIC* type of message from the queue...
int	QuicksandEngineApp::EatSpecificMessages(UINT msgType, optional<LPARAM> lParam, optional<WPARAM> wParam)
{
	bool done = false;

	while (!done)
	{
		MSG msg;

		if (PeekMessage(&msg, NULL, msgType, msgType, PM_NOREMOVE))
		{
			bool valid = true;

			if (lParam.valid())
			{
				valid &= (*lParam == msg.lParam);
			}

			if (wParam.valid())
			{
				valid &= (*wParam == msg.wParam);
			}

			if (valid)
			{
				//Soak!
				GetMessage(&msg, NULL, msgType, msgType);
			}
			else
			{
				done = true;
			}
		}
		else
		{
			done = true;	//No more messages!
		}
	}

	return 0;
}


//--------------------------------------------------------------------------------------
// This callback function will be called once at the beginning of every frame. This is the
// best location for your application to handle updates to the scene, but is not 
// intended to contain actual rendering calls, which should instead be placed in the 
// OnFrameRender callback.  
//
// See Game Coding Complete - 4th Edition - Chapter X, page Y
//--------------------------------------------------------------------------------------
void QuicksandEngineApp::OnUpdateGame(double fTime, float fElapsedTime)
{
	if (QuicksandEngine::g_pApp->HasModalDialog())
	{
		// don't update the game if a modal dialog is up.
		return;
	}

	if (QuicksandEngine::g_pApp->m_bQuitting)
	{
		PostMessage(QuicksandEngine::g_pApp->GetHwnd(), WM_CLOSE, 0, 0);
	}

	if (QuicksandEngine::g_pApp->m_pGame)
	{
		IEventManager::Get()->VUpdate(20); // allow event queue to process for up to 20 ms

		if (QuicksandEngine::g_pApp->m_pBaseSocketManager)
			QuicksandEngine::g_pApp->m_pBaseSocketManager->DoSelect(0);	// pause 0 microseconds

		QuicksandEngine::g_pApp->m_pGame->VOnUpdate(float(fTime), (float)fElapsedTime);
	}
}



bool QuicksandEngineApp::AttachAsClient()
{
	ClientSocketManager *pClient = QSE_NEW ClientSocketManager(GET_CONFIG_ELEMENT_STR("GAME_HOST"), GET_CONFIG_ELEMENT_US("LISTEN_PORT"));
	if (!pClient->Connect())
	{
		return false;
	}
	QuicksandEngine::g_pApp->m_pBaseSocketManager = pClient;
	VCreateNetworkEventForwarder();

	return true;
}


// Any events that will be received from the server logic should be here!
void QuicksandEngineApp::VCreateNetworkEventForwarder(void)
{
	if (m_pNetworkEventForwarder != NULL)
	{
		LOG_ERROR("Overwriting network event forwarder in TeapotWarsApp!");
		SAFE_DELETE(m_pNetworkEventForwarder);
	}

	m_pNetworkEventForwarder = QSE_NEW NetworkEventForwarder(0);

	IEventManager* pGlobalEventManager = IEventManager::Get();
	pGlobalEventManager->VAddListener(MakeDelegate(m_pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_Request_New_Actor::sk_EventType);
	pGlobalEventManager->VAddListener(MakeDelegate(m_pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_Environment_Loaded::sk_EventType);
	pGlobalEventManager->VAddListener(MakeDelegate(m_pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_PhysCollision::sk_EventType);

}

void QuicksandEngineApp::VDestroyNetworkEventForwarder(void)
{
	if (m_pNetworkEventForwarder)
	{
		IEventManager* pGlobalEventManager = IEventManager::Get();
		pGlobalEventManager->VRemoveListener(MakeDelegate(m_pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_Request_New_Actor::sk_EventType);
		pGlobalEventManager->VRemoveListener(MakeDelegate(m_pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_Environment_Loaded::sk_EventType);
		pGlobalEventManager->VRemoveListener(MakeDelegate(m_pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_PhysCollision::sk_EventType);
		SAFE_DELETE(m_pNetworkEventForwarder);
	}
}





////////////////////////////////////
//
//GLFW Message handling function
//
//
//
//

void QuicksandEngineApp::GLFWErrorFunc(int error, const char* description)
{
	std::stringstream ss;
	ss << "GLFW ERROR: error-code=" << error << ";  Message = \"" << description << "\"";
	LOG_ERROR(ss.str());
}
