#include "../Stdafx.hpp"

void glufErrorMethod(const char* message, const char* funcName, const char* sourceFile, unsigned int lineNum)
{
	AwLogging::WriteLog(AwLogging::ERROR_, message, funcName, sourceFile, lineNum);
}

INT WINAPI QuicksandEngineWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR    lpCmdLine,
	int       nCmdShow)
{
	// Set up checks for memory leaks.
	// Game Coding Complete reference - Chapter 21, page 834
	//
	int tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

	// set this flag to keep memory blocks around
	// tmpDbgFlag |= _CRTDBG_DELAY_FREE_MEM_DF;	       // this flag will cause intermittent pauses in your game and potientially cause it to run out of memory!

	// perform memory check for each alloc/dealloc
	//tmpDbgFlag |= _CRTDBG_CHECK_ALWAYS_DF;		   // remember this is VERY VERY SLOW!

	//_CRTDBG_LEAK_CHECK_DF is used at program initialization to force a 
	//   leak check just before program exit. This is important because
	//   some classes may dynamically allocate memory in globally constructed
	//   objects.
	tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;

	_CrtSetDbgFlag(tmpDbgFlag);

	//g_pApp->m_Options.Init("PlayerOptions.xml", lpCmdLine);
	//well, load the logging system first TO SET THE LOG LOCATION... LOL again
	QuicksandEngine::g_pApp->mConfigManager.AddConfigElements("PlayerOptions.xml");

	// [rez] Initialize the logging system as the very first thing you ever do!
	// [mrmike] LOL after the memory system flags are set, that is!
	if (!AwLogging::Init())
		return 1;//fail


	// Set the callback functions. These functions allow the sample framework to notify
	// the application about device changes, user input, and windows messages.  The 
	// callbacks are optional so you need only set callbacks for events you're interested 
	// in. However, if you don't handle the device reset/lost callbacks then the sample 
	// framework won't be able to reset your device since the application must first 
	// release all device resources before resetting.  Likewise, if you don't handle the 
	// device created/destroyed callbacks then the sample framework won't be able to 
	// recreate your device resources.

	/*
	GLUFRegisterErrorMethod(glufErrorMethod);
	if (!GLUFInit())
	{
		return false;
	}*/

	GLUFRegisterErrorMethod(glufErrorMethod);
	GLUFInit();

	//DXUTSetCallbackMsgProc(GameCodeApp::MsgProc);
	//DXUTSetCallbackFrameMove(GameCodeApp::OnUpdateGame);
	//DXUTSetCallbackDeviceChanging(GameCodeApp::ModifyDeviceSettings);

	/*
	if (g_pApp->m_Options.m_Renderer == "Direct3D 9")
	{
	DXUTSetCallbackD3D9DeviceAcceptable(GameCodeApp::IsD3D9DeviceAcceptable);
	DXUTSetCallbackD3D9DeviceCreated(GameCodeApp::OnD3D9CreateDevice);
	DXUTSetCallbackD3D9DeviceReset(GameCodeApp::OnD3D9ResetDevice);
	DXUTSetCallbackD3D9DeviceLost(GameCodeApp::OnD3D9LostDevice);
	DXUTSetCallbackD3D9DeviceDestroyed(GameCodeApp::OnD3D9DestroyDevice);
	DXUTSetCallbackD3D9FrameRender(GameCodeApp::OnD3D9FrameRender);
	}
	else if (g_pApp->m_Options.m_Renderer == "Direct3D 11")
	{
	DXUTSetCallbackD3D11DeviceAcceptable(GameCodeApp::IsD3D11DeviceAcceptable);
	DXUTSetCallbackD3D11DeviceCreated(GameCodeApp::OnD3D11CreateDevice);
	DXUTSetCallbackD3D11SwapChainResized(GameCodeApp::OnD3D11ResizedSwapChain);
	DXUTSetCallbackD3D11SwapChainReleasing(GameCodeApp::OnD3D11ReleasingSwapChain);
	DXUTSetCallbackD3D11DeviceDestroyed(GameCodeApp::OnD3D11DestroyDevice);
	DXUTSetCallbackD3D11FrameRender(GameCodeApp::OnD3D11FrameRender);
	}
	else
	{
	LOG_ASSERT(0 && "Unknown renderer specified in game options.");
	return false;
	}*/



	// Perform application initialization
	if (!QuicksandEngine::g_pApp->InitInstance(hInstance, lpCmdLine, 0, GET_CONFIG_ELEMENT_S("WIDTH"), GET_CONFIG_ELEMENT_S("HEIGHT")))
	{
		// [rez] Note: Fix memory leaks if we hit this branch.  Also, print an error.
		return FALSE;
	}

	// Pass control to the sample framework for handling the message pump and 
	// dispatching render calls. The sample framework will call your FrameMove 
	// and FrameRender callback when there is idle time between handling window messages.
	/*
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = QuicksandEngineApp::MsgProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;*/

	//return RegisterClassEx(&wcex);

	double currentTime = GLUFGetTime();
	while (QuicksandEngine::g_pApp->IsRunning())
	{
		GLUFStats();

		// Update the game views, but nothing else!
		// Remember this is a modal screen.
		double timeNow = GLUFGetTime();
		float deltatime = float(timeNow - currentTime);

		currentTime = timeNow;

		QuicksandEngine::g_pApp->OnUpdateGame(timeNow, deltatime);
		QuicksandEngine::g_pApp->GLFrameRender(timeNow, deltatime);

		glfwSwapBuffers(QuicksandEngine::g_pApp->GetWindow());
		glfwPollEvents();
	}

	QuicksandEngine::g_pApp->OnClose();

	//DXUTMainLoop();
	//DXUTShutdown();

	

	glfwDestroyWindow(QuicksandEngine::g_pApp->GLFWWindow());
	glfwTerminate();

	// [rez] Destroy the logging system at the last possible moment
	AwLogging::Destroy();

	return QuicksandEngine::g_pApp->GetExitCode();
}
