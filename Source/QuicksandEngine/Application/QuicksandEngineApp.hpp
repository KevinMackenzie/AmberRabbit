#ifndef QSE_QUICKSANDENGINEAPP_HPP
#define QSE_QUICKSANDENGINEAPP_HPP

#include "../Config/ConfigManager.hpp"
#include "Interfaces.hpp"
#include "../Utilities/Types.hpp"

#include "../Mainloop/Initialization.hpp"
#include "../GameLogic/BaseGameLogic.hpp"
#include "../Graphics3D/SceneNode.hpp"
#include "../UserInterface/UserInterface.hpp"

class FontHandler;
class BaseUI;
class EventManager;
class LuaStateManager;
class BaseSocketManager;
class NetworkEventForwarder;
class HumanView;



class  QuicksandEngineApp
{


protected:

	HINSTANCE m_hInstance;					// the module instance
	bool m_bWindowedMode;					// true if the app is windowed, false if fullscreen
	bool m_bIsRunning;						// true if everything is initialized and the game is in the main loop
	bool m_bQuitRequested;					// true if the app should run the exit sequence
	bool m_bQuitting;						// true if the app is running the exit sequence
	//GLUFRect m_rcDesktop;					// current desktop size - not necessarilly the client window size
	Point m_screenSize;						// game screen size
	int m_iColorDepth;						// current color depth (16 or 32)
	bool m_bIsEditorRunning;				// true if the game editor is running

	GLFWwindow *m_pWindow; //this is the window
	HCURSOR m_pCursor;

public:

	StringsManager mConfigManager;//this is used for configuration
	StringsManager mStringsManager;//this is used for strings (language)
	const Point &GetScreenSize()  { return m_screenSize; }

	GLFWwindow* GetWindow(){ return m_pWindow; }

protected:
	std::map<std::wstring, std::wstring> m_textResource;
	std::map<std::wstring, UINT> m_hotkeys;

	int m_HasModalDialog;					// determines if a modal dialog is up
	int PumpUntilMessage(UINT msgEnd, WPARAM* pWParam, LPARAM* pLParam);
	int	EatSpecificMessages(UINT msgType, optional<LPARAM> lParam, optional<WPARAM> wParam);
	void FlashWhileMinimized();

public:

	QuicksandEngineApp();

	// Game Application Data
	// You must define these in an inherited
	// class - see TeapotWarsApp for an example
	virtual char *VGetGameTitle() = 0;
	virtual char *VGetGameAppDirectory() = 0;
	virtual HICON VGetIcon() = 0;

	// Win32 Specific Stuff
	HWND GetHwnd();
	HINSTANCE GetInstance() { return m_hInstance; }
	virtual bool InitInstance(HINSTANCE hInstance, LPSTR lpCmdLine, HWND hWnd = NULL, int screenWidth = SCREEN_WIDTH, int screenHeight = SCREEN_HEIGHT);

	static bool MsgProc(GLUF_MESSAGE_TYPE msg, int param1, int param2, int param3, int param4);
	bool HasModalDialog() { return m_HasModalDialog != 0; }
	void ForceModalExit() { PostMessage(GetHwnd(), g_MsgEndModal, 0, g_QuitNoPrompt); }

	bool OnDisplayChange(int colorDepth, int width, int height);
	bool OnPowerBroadcast(int event);
	bool OnSysCommand(GLUF_MESSAGE_TYPE msg, int param1, int param2);
	bool OnClose();

	// Game Application actions
	bool OnAltEnter();
	bool OnNcCreate(LPCREATESTRUCT cs);

	bool LoadStrings(string language);
	std::wstring GetString(std::wstring sID);
	int GetHotKeyForString(std::wstring sID);
	UINT MapCharToKeycode(const char pHotkey);

	int Modal(shared_ptr<IScreenElement> pModalScreen, int defaultAnswer);

	// FUTURE WORK - Seems like this class could be refactored, especailly the renderer !


	shared_ptr<IRenderer> m_Renderer;

	//GLFW Specific Stuff
	GLFWwindow* GLFWWindow(){ return m_pWindow; }

	static void GLFWErrorFunc(int error, const char* description);

	void GLFrameRender(double fTime, double elapsedTime);

	//OpenGL Specific Stuff

	/*
	// DirectX9 Specific Stuff
	static bool CALLBACK IsD3D9DeviceAcceptable(D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext);
	static HRESULT CALLBACK OnD3D9CreateDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
	static HRESULT CALLBACK OnD3D9ResetDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
	static void CALLBACK OnD3D9FrameRender(IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext);
	static void CALLBACK OnD3D9LostDevice(void* pUserContext);
	static void CALLBACK OnD3D9DestroyDevice(void* pUserContext);

	// DirectX 11 Specific Stuff
	static bool CALLBACK IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo, DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext);
	static HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
	static HRESULT CALLBACK OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
	static void CALLBACK OnD3D11ReleasingSwapChain(void* pUserContext);
	static void CALLBACK OnD3D11DestroyDevice(void* pUserContext);
	static void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime, float fElapsedTime, void* pUserContext);

	static bool CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext);*/
	
	
	static void OnUpdateGame(double fTime, float fElapsedTime);


	// GameCode Specific Stuff
	BaseGameLogic *m_pGame;

	// You must define these functions to initialize your game.
	// Look for them in TeapotWars.h and TeapotWars.cpp
	virtual BaseGameLogic *VCreateGameAndView() = 0;
	virtual bool VLoadGame(void);
	HumanView* GetHumanView();				// Added post press - it was convenient to grab the HumanView attached to the game.

	// File and Resource System
	class ResCache *m_ResCache;
	char m_saveGameDirectory[MAX_PATH];

	bool IsEditorRunning() { return m_bIsEditorRunning; }

	// Event manager
	EventManager *m_pEventManager;

	// Socket manager - could be server or client
	BaseSocketManager *m_pBaseSocketManager;
	NetworkEventForwarder* m_pNetworkEventForwarder;
	bool AttachAsClient();

protected:
	virtual void VCreateNetworkEventForwarder(void);
	virtual void VDestroyNetworkEventForwarder(void);

public:

	// Main loop processing
	void AbortGame() { m_bQuitting = true; }
	int GetExitCode() { return 0;/*for now*/ }
	bool IsRunning() { return m_bIsRunning; }
	void SetQuitting(bool quitting) { m_bQuitting = quitting; }

	BaseGameLogic* GetGameLogic(void) const { return m_pGame; }

protected:
	virtual void VRegisterGameEvents(void) {}

private:
	void RegisterEngineEvents(void);

};


namespace QuicksandEngine
{
	extern  QSE_API QuicksandEngineApp *g_pApp;
}

#endif