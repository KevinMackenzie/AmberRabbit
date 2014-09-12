//========================================================================
// TeapotWars.cpp : source file for the sample game
//
// Part of the GameCode4 Application
//
// GameCode4 is the sample application that encapsulates much of the source code
// discussed in "Game Coding Complete - 4th Edition" by Mike McShaffry and David
// "Rez" Graham, published by Charles River Media. 
// ISBN-10: 1133776574 | ISBN-13: 978-1133776574
//
// If this source code has found it's way to you, and you think it has helped you
// in any way, do the authors a favor and buy a new copy of the book - there are 
// detailed explanations in it that compliment this code well. Buy a copy at Amazon.com
// by clicking here: 
//    http://www.amazon.com/gp/product/1133776574/ref=olp_product_details?ie=UTF8&me=&seller=
//
// There's a companion web site at http://www.mcshaffry.com/GameCode/
// 
// The source code is managed and maintained through Google Code: 
//    http://code.google.com/p/gamecode4/
//
// (c) Copyright 2012 Michael L. McShaffry and David Graham
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser GPL v3
// as published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See 
// http://www.gnu.org/licenses/lgpl-3.0.txt for more details.
//
// You should have received a copy of the GNU Lesser GPL v3
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
//========================================================================

#include "../QuicksandEngine/Stdafx.hpp"

#include "../QuicksandEngine/Application/QuicksandEngineApp.hpp"
#include "../QuicksandEngine/Audio/Audio.hpp"
#include "../QuicksandEngine/Audio/SoundProcess.hpp"
#include "../QuicksandEngine/Graphics3D/GLRenderer.hpp"
#include "../QuicksandEngine/Graphics3D/Geometry.hpp"
#include "../QuicksandEngine/Graphics3D/MovementController.hpp"
#include "../QuicksandEngine/Graphics3D/Raycast.hpp"
#include "../QuicksandEngine/Graphics3D/SceneNode.hpp"
#include "../QuicksandEngine/Graphics3D/Sky.hpp"
#include "../QuicksandEngine/MainLoop/Initialization.hpp"
#include "../QuicksandEngine/EventManager/Events.hpp"
#include "../QuicksandEngine/EventManager/EventManagerImpl.hpp"
#include "../QuicksandEngine/Physics/PhysicsEventListener.hpp"
#include "../QuicksandEngine/UserInterface/HumanView.hpp"
#include "../QuicksandEngine/UserInterface/MessageBox.hpp"
#include "../QuicksandEngine/Actor/Actor.hpp"
#include "../QuicksandEngine/Actor/RenderComponent.hpp"
#include "../QuicksandEngine/Utilities/String.hpp"
#include "../QuicksandEngine/LUAScripting/LuaStateManager.hpp"  // [rez] For debugging only; the view shouldn't use Lua

#include "TeapotWarsView.h"
#include "TeapotWarsNetwork.h"
#include "TeapotEvents.h"

#include "TeapotController.h"
#include "TeapotWars.h"

extern TeapotWarsApp g_TeapotWarsApp;

//========================================================================
//
// MainMenuUI & MainMenuView implementation
//
//
//========================================================================

#define CID_CREATE_GAME_RADIO			(2)
#define CID_JOIN_GAME_RADIO				(3)
#define CID_NUM_AI_SLIDER				(4)
#define CID_NUM_PLAYER_SLIDER			(5)
#define CID_HOST_LISTEN_PORT			(6)
#define CID_CLIENT_ATTACH_PORT			(7)
#define CID_START_BUTTON				(8)
#define CID_HOST_NAME					(9)
#define CID_NUM_AI_LABEL				(10)
#define CID_NUM_PLAYER_LABEL			(11)
#define CID_HOST_LISTEN_PORT_LABEL		(12)
#define CID_CLIENT_ATTACH_PORT_LABEL	(13)
#define CID_HOST_NAME_LABEL				(14)
#define CID_LEVEL_LABEL					(15)
#define CID_LEVEL_LISTBOX				(16)

#define ARIEL_FONT_DIALOG_LOCATION (1)

const int g_SampleUIWidth = 700;
const int g_SampleUIHeight = 700;

unsigned int g_ArielFontLocation;

MainMenuUI::MainMenuUI()
{
	m_SampleUI.Init( GLRenderer_Base::g_pDialogResourceManager );
    m_SampleUI.SetCallback( OnGUIEvent, this ); 
	m_SampleUI.EnableKeyboardInput(true);

	int nY = 10;
	int nX = 35;
	int nX2 = g_SampleUIWidth / 2;
	int nWidth = (g_SampleUIWidth / 2) - 10;
	int nHeight = 25;
	int nLineHeight = nHeight + 2;

	// grab defaults from the game options.
	m_NumAIs = GET_CONFIG_ELEMENT_I("NUM_AI");
	m_NumPlayers = GET_CONFIG_ELEMENT_I("EXPECTED_PLAYERS");
	m_HostName = GET_CONFIG_ELEMENT_STR("GAME_HOST");
	m_HostListenPort = GET_CONFIG_ELEMENT_STR("LISTEN_PORT");
	m_ClientAttachPort = GET_CONFIG_ELEMENT_STR("LISTEN_PORT");

	m_bCreatingGame = true;

	GLUF::Color color = GLUF::Color(200, 0, 0, 255);
	m_SampleUI.SetBackgroundColor(color);

	/*Resource ariel("Fonts/Ariel.ttf");
	m_ArielFont = QuicksandEngine::g_pApp->m_ResCache->GetHandle(&ariel);
	g_ArielFontLocation = GLRenderer_Base::g_pDialogResourceManager->AddFont(static_pointer_cast<TTFResourceExtraData>(m_ArielFont->GetExtra())->GetFont(), FONT_WEIGHT_NORMAL);
	m_SampleUI.SetFont(ARIEL_FONT_DIALOG_LOCATION, g_ArielFontLocation);*/
	//m_SampleUI.SetFont(0, L"Ariel", height, 0);

	m_SampleUI.AddStatic(0, L"Teapot Wars Main Menu", nX - 20, nY, g_SampleUIWidth, nHeight * 2);
	nY += (nLineHeight * 3);

	m_SampleUI.AddRadioButton(CID_CREATE_GAME_RADIO, 1, L"Create Game", nX, nY, g_SampleUIWidth, nHeight);
	nY += nLineHeight;

	m_SampleUI.AddStatic(CID_LEVEL_LABEL, L"Level", nX, nY, nWidth, nHeight);
	m_SampleUI.AddListBox(CID_LEVEL_LISTBOX, nX2, nY, nWidth, nLineHeight * 5);
	std::vector<Level> levels = QuicksandEngine::g_pApp->GetGameLogic()->GetLevelManager()->GetLevels();
	m_Levels.reserve(levels.size());
	int count = 0;
	for (std::vector<Level>::iterator i = levels.begin(); i != levels.end(); ++i, ++count)
	{
		m_Levels.push_back(s2ws(*i));
		m_SampleUI.GetListBox ( CID_LEVEL_LISTBOX )->AddItem(m_Levels[count].c_str(), NULL);
	}
	nY += (nLineHeight * 5);
	//m_SampleUI.GetListBox(CID_LEVEL_LISTBOX)->GetElement(0)->SetFont(0, 0x0);

	m_SampleUI.AddStatic(CID_NUM_AI_LABEL, L"", nX, nY, nWidth, nHeight);
	m_SampleUI.AddSlider(CID_NUM_AI_SLIDER, nX2, nY, nWidth, nHeight);
	m_SampleUI.GetSlider( CID_NUM_AI_SLIDER )->SetRange(0, GET_CONFIG_ELEMENT_I("MAX_AI"));
	m_SampleUI.GetSlider( CID_NUM_AI_SLIDER )->SetValue(m_NumAIs); // should be ai options default
	nY += nLineHeight;

	m_SampleUI.AddStatic(CID_NUM_PLAYER_LABEL, L"", nX, nY, nWidth, nHeight);
	m_SampleUI.AddSlider(CID_NUM_PLAYER_SLIDER, nX2, nY, nWidth, nHeight);
	m_SampleUI.GetSlider( CID_NUM_PLAYER_SLIDER )->SetRange(0, GET_CONFIG_ELEMENT_UC("MAX_PLAYERS"));
	m_SampleUI.GetSlider( CID_NUM_PLAYER_SLIDER )->SetValue(m_NumPlayers);  // should be player options default
	nY += nLineHeight;

	m_SampleUI.AddStatic(CID_HOST_LISTEN_PORT_LABEL, L"Host Listen Port", nX, nY, nWidth, nHeight);
	m_SampleUI.AddEditBox(CID_HOST_LISTEN_PORT, L"57", nX2, nY, nWidth, nHeight * 2, Numeric);
	GLUFEditBox *eb = m_SampleUI.GetEditBox( CID_HOST_LISTEN_PORT );
	eb->SetVisible(false);
	nY += nLineHeight * 3;

	m_SampleUI.AddRadioButton(CID_JOIN_GAME_RADIO, 1, L"Join Game", nX, nY, nWidth, nHeight);
    m_SampleUI.GetRadioButton( CID_JOIN_GAME_RADIO )->SetChecked( true ); 
	nY += nLineHeight;

	m_SampleUI.AddStatic(CID_CLIENT_ATTACH_PORT_LABEL, L"Host Attach Port", nX, nY, nWidth, nHeight);
	m_SampleUI.AddEditBox(CID_CLIENT_ATTACH_PORT, L"57", nX2, nY, nWidth, nHeight * 2, Numeric);
	nY += nLineHeight * 3;


	m_SampleUI.AddStatic(CID_HOST_NAME_LABEL, L"Host Name", nX, nY, nWidth, nHeight);
	m_SampleUI.AddEditBox(CID_HOST_NAME, L"sunshine", nX2, nY, nWidth, nHeight * 2);
	nY += nLineHeight;

	m_SampleUI.AddButton(CID_START_BUTTON, L"Start Game", (g_SampleUIWidth - (nWidth / 2)) / 2, nY += nLineHeight, nWidth / 2, nHeight);

    m_SampleUI.GetRadioButton( CID_CREATE_GAME_RADIO )->SetChecked(true);

	Set();
}


MainMenuUI::~MainMenuUI()
{ 
	GLRenderer_Base::g_pDialogResourceManager->UnregisterDialog(&m_SampleUI);
}

void MainMenuUI::Set()
{
	WCHAR buffer[256];
	CHAR ansiBuffer[256];

	m_LevelIndex = m_SampleUI.GetListBox ( CID_LEVEL_LISTBOX)->GetSelectedIndex();
	m_SampleUI.GetListBox ( CID_LEVEL_LISTBOX)->SetVisible(m_bCreatingGame);

	m_NumAIs = (int)m_SampleUI.GetSlider( CID_NUM_AI_SLIDER )->GetValue(); 
	m_SampleUI.GetSlider(CID_NUM_AI_SLIDER)->SetVisible(m_bCreatingGame);

	wsprintf( buffer, _T("%s: %d\n"), L"Number of AIs", m_NumAIs );
	m_SampleUI.GetStatic(CID_NUM_AI_LABEL)->SetText(buffer);
	m_SampleUI.GetStatic(CID_NUM_AI_LABEL)->SetVisible(m_bCreatingGame);
	
	m_NumPlayers = (int)m_SampleUI.GetSlider(CID_NUM_PLAYER_SLIDER)->GetValue();
	m_SampleUI.GetSlider(CID_NUM_PLAYER_SLIDER)->SetVisible(m_bCreatingGame);
	wsprintf( buffer, _T("%s: %d\n"), L"Number of Players", m_NumPlayers );
	m_SampleUI.GetStatic(CID_NUM_PLAYER_LABEL)->SetText(buffer);
	m_SampleUI.GetStatic(CID_NUM_PLAYER_LABEL)->SetVisible(m_bCreatingGame);

	m_SampleUI.GetStatic( CID_HOST_LISTEN_PORT_LABEL )->SetVisible(m_NumPlayers>1 && m_bCreatingGame);
	m_SampleUI.GetEditBox( CID_HOST_LISTEN_PORT )->SetVisible(m_NumPlayers>1 && m_bCreatingGame);
	if (m_bCreatingGame)
	{
		WideToAnsiCch(ansiBuffer, m_SampleUI.GetEditBox( CID_HOST_LISTEN_PORT )->GetText().c_str(), 256);
		m_HostListenPort = ansiBuffer;
	}

	m_SampleUI.GetStatic( CID_HOST_NAME_LABEL )->SetVisible(!m_bCreatingGame);
	m_SampleUI.GetEditBox( CID_HOST_NAME )->SetVisible(!m_bCreatingGame);

	WideToAnsiCch(ansiBuffer, m_SampleUI.GetEditBox( CID_HOST_NAME )->GetText().c_str(), 256);
	m_HostName = ansiBuffer;

	m_SampleUI.GetStatic( CID_CLIENT_ATTACH_PORT_LABEL )->SetVisible(!m_bCreatingGame);
	m_SampleUI.GetEditBox( CID_CLIENT_ATTACH_PORT )->SetVisible(!m_bCreatingGame);
	if (!m_bCreatingGame)
	{
		WideToAnsiCch(ansiBuffer, m_SampleUI.GetEditBox( CID_CLIENT_ATTACH_PORT )->GetText().c_str(), 256);
		m_ClientAttachPort = ansiBuffer;
	}

}

HRESULT MainMenuUI::VOnRestore()
{
	m_SampleUI.SetCaptionHeight(0);
	m_SampleUI.SetLocation(50, 50);
    m_SampleUI.SetSize( g_SampleUIWidth, g_SampleUIHeight );
	return S_OK;
}

HRESULT MainMenuUI::VOnRender(double fTime, double fElapsedTime)
{
	/*HRESULT hr;
	DXUT_BeginPerfEvent( DXUT_PERFEVENTCOLOR, L"TeapotWarsHUD" ); // These events are to help PIX identify what the code is doing
	V( m_SampleUI.OnRender( fElapsedTime ) );
	DXUT_EndPerfEvent();*/
	m_SampleUI.OnRender((float)fElapsedTime);
	return S_OK;
};

LRESULT MainMenuUI::VOnMsgProc( AppMsg msg )
{
	return m_SampleUI.MsgProc( msg.m_Event, msg.param1, msg.param2, msg.param3, msg.param4 );
}



//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
//    Note: pUserContext added to comply with DirectX 9c - June 2005 Update
//
void MainMenuUI::_OnGUIEvent(GLUF_EVENT nEvent, int nControlID, GLUFControl* pControl)
{
    switch (nControlID)
    {
		case CID_CREATE_GAME_RADIO:
        {
			m_bCreatingGame = true;
			break;
        }

		case CID_JOIN_GAME_RADIO:
        {
			m_bCreatingGame = false;
			break;
        }

		case CID_LEVEL_LISTBOX:
		case CID_NUM_AI_SLIDER:
		case CID_NUM_PLAYER_SLIDER:
		case CID_HOST_LISTEN_PORT:
		case CID_CLIENT_ATTACH_PORT:
		case CID_HOST_NAME:
        {
			break;
        }

		case CID_START_BUTTON:
        {
			MODIFY_CONFIG_ELEMENT("NUM_AI",m_NumAIs);
			MODIFY_CONFIG_ELEMENT("EXPECTED_PLAYERS", m_NumPlayers);
			if (m_bCreatingGame)
			{
				if (m_LevelIndex == -1)
				{
					// FUTURE WORK - AN ERROR DIALOG WOULD BE GOOD HERE, OR JUST DEFALT THE SELECTION TO SOMETHING VALID
					return;
				}
		        MODIFY_CONFIG_ELEMENT("LEVEL", ws2s(m_Levels[m_LevelIndex]));
				MODIFY_CONFIG_ELEMENT("GAME_HOST", "");
				MODIFY_CONFIG_ELEMENT("LISTEN_PORT", atoi(m_HostListenPort.c_str()));
			}
			else
			{
				MODIFY_CONFIG_ELEMENT("GAME_HOST", m_HostName);
				MODIFY_CONFIG_ELEMENT("LISTEN_PORT", atoi(m_ClientAttachPort.c_str()));
			}

			VSetVisible(false);

            shared_ptr<EvtData_Request_Start_Game> pRequestStartGameEvent(QSE_NEW EvtData_Request_Start_Game());
            IEventManager::Get()->VQueueEvent(pRequestStartGameEvent);

			break;
        }

		default:
        {
			LOG_ERROR("Unknown control.");
        }
    }

	Set();
}

void MainMenuUI::OnGUIEvent(GLUF_EVENT nEvent, int nControlID, GLUFControl* pControl, void* pContext)
{
	MainMenuUI *ui = static_cast<MainMenuUI *>(pContext);
	ui->_OnGUIEvent(nEvent, nControlID, pControl);
}


MainMenuView::MainMenuView() : HumanView(shared_ptr<IRenderer>())
{
	m_MainMenuUI.reset(QSE_NEW MainMenuUI); 
	VPushElement(m_MainMenuUI);
}


MainMenuView::~MainMenuView()
{
}


void MainMenuView::VRenderText()
{
	HumanView::VRenderText();
}


void MainMenuView::VOnUpdate(unsigned long deltaMs)
{
	HumanView::VOnUpdate( deltaMs );
}

LRESULT MainMenuView::VOnMsgProc( AppMsg msg )
{
	if (m_MainMenuUI->VIsVisible() )
	{
		if (HumanView::VOnMsgProc(msg))
			return 1;
	}
	return 0;
}


//========================================================================
//
// StandardHUD implementation
//
//
//========================================================================

//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           2
#define IDC_CHANGEDEVICE        3

//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
//    Note: pUserContext added to comply with DirectX 9c - June 2005 Update
//
void StandardHUD::OnGUIEvent(GLUF_EVENT nEvent, int nControlID, GLUFControl* pControl, void* pContext)
{
    /*switch( nControlID )
    {
        case IDC_TOGGLEFULLSCREEN: DXUTToggleFullScreen(); break;
        case IDC_TOGGLEREF:        DXUTToggleREF(); break;
//        case IDC_CHANGEDEVICE:     DXUTSetShowSettingsDialog( !DXUTGetShowSettingsDialog() ); break;
    }*/
}


StandardHUD::StandardHUD()
{
    // Initialize dialogs
	m_HUD.Init( GLRenderer_Base::g_pDialogResourceManager );
	m_HUD.SetCallback(OnGUIEvent); int nY = 10;
	m_HUD.AddButton(IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 35, nY, 125, 22);
	m_HUD.AddButton(IDC_TOGGLEREF, L"Toggle REF (F3)", 35, nY += 24, 125, 22);
    //m_HUD.AddButton( IDC_CHANGEDEVICE, L"Change device (F2)", 35, iY += 24, 125, 22 );
}


StandardHUD::~StandardHUD() 
{ 
//  [mrmike] - this causes a "memory written after freed error" so I commented it out.
//	D3DRenderer::g_pDialogResourceManager.UnregisterDialog(&m_HUD); 
}


HRESULT StandardHUD::VOnRestore()
{
	m_HUD.SetLocation(QuicksandEngine::g_pApp->GetScreenSize().x - 170, 0);
	m_HUD.SetSize(170, 170);
	return S_OK;
}



HRESULT StandardHUD::VOnRender(double fTime, double fElapsedTime)
{
	HRESULT hr;
	//DXUT_BeginPerfEvent( DXUT_PERFEVENTCOLOR, L"StandardUI" ); // These events are to help PIX identify what the code is doing
	V( m_HUD.OnRender( (float)fElapsedTime ) );
	//DXUT_EndPerfEvent();
	return S_OK;
};


LRESULT StandardHUD::VOnMsgProc( AppMsg msg )
{
    return m_HUD.MsgProc( msg.m_Event, msg.param1, msg.param2, msg.param3, msg.param4 );
}



//========================================================================
//
// TeapotWarsHumanView Implementation
//
//========================================================================

//
// TeapotWarsHumanView::TeapotWarsHumanView	- Chapter 19, page 724
//
TeapotWarsHumanView::TeapotWarsHumanView(shared_ptr<IRenderer> renderer) : HumanView(renderer)
{
	GLRenderer_Base::g_pTextHelper->Init(20);//make sure to set this up initially, because the engine does not do this by default
	m_bShowUI = true; 
    RegisterAllDelegates();
}


TeapotWarsHumanView::~TeapotWarsHumanView() 
{
    RemoveAllDelegates();
}

//
// TeapotWarsHumanView::VOnMsgProc				- Chapter 19, page 727
//
LRESULT TeapotWarsHumanView::VOnMsgProc( AppMsg msg )
{
	if (HumanView::VOnMsgProc(msg))
		return 1;

	if (msg.m_Event==GM_KEY && msg.param3 == GLFW_PRESS)
	{
		if (msg.param1==GLFW_KEY_F1)
		{
			m_bShowUI = !m_bShowUI;
			//m_StandardHUD->VSetVisible(m_bShowUI);
			return 1;
		}
		else if (msg.param1 == GLFW_KEY_F2)
		{
			// test the picking API

			POINT ptCursor;
			GetCursorPos( &ptCursor );
			ScreenToClient(QuicksandEngine::g_pApp->GetHwnd(), &ptCursor);

			RayCast rayCast(ptCursor);
			m_pScene->Pick(&rayCast);
			rayCast.Sort();

			if (rayCast.m_NumIntersections)
			{
				// You can iterate through the intersections on the raycast.
				int a = 0;
			}
		}
		else if (msg.param1 == GLFW_KEY_F3)
		{
			//extern void CreateThreads();
			//CreateThreads();

			extern void testThreading(ProcessManager *procMgr, bool runProtected);
			testThreading(m_pProcessManager, true);

			return 1;

			Sleep(5000);
			testThreading(m_pProcessManager, false);
			Sleep(5000);

			extern void testRealtimeEvents(ProcessManager *procMgr);
			testRealtimeEvents(m_pProcessManager);

			extern void testRealtimeDecompression(ProcessManager *procMgr);
			testRealtimeDecompression(m_pProcessManager);
		}
		else if (msg.param1 == GLFW_KEY_F4)
        {
            Resource resource("scripts\\test.lua");
			shared_ptr<ResHandle> pResourceHandle = QuicksandEngine::g_pApp->m_ResCache->GetHandle(&resource);  // this actually loads the Lua file from the zip file
        }
		else if (msg.param1 == GLFW_KEY_F5)
        {
            shared_ptr<EvtData_ScriptEventTest_ToLua> pEvent(QSE_NEW EvtData_ScriptEventTest_ToLua);
            IEventManager::Get()->VQueueEvent(pEvent);
        }
		else if (msg.param1 == GLFW_KEY_F6)
        {
            LuaStateManager::Get()->VExecuteString("TestProcess()");
        }
		else if (msg.param1 == GLFW_KEY_F8)
		{
			TeapotWarsLogic *twg = static_cast<TeapotWarsLogic *>(QuicksandEngine::g_pApp->m_pGame);
			twg->ToggleRenderDiagnostics();

		}
		else if (msg.param1 == GLFW_KEY_F9)
		{
			m_KeyboardHandler = m_pTeapotController;
			m_PointerHandler = m_pTeapotController;
			m_pCamera->SetTarget(m_pTeapot);
			m_pTeapot->SetAlpha(0.8f);
			ReleaseCapture();
			return 1;
		}
		else if (msg.param1 == GLFW_KEY_F11)
		{
			m_KeyboardHandler = m_pFreeCameraController;
			m_PointerHandler = m_pFreeCameraController;
			m_pCamera->ClearTarget();
			//m_pTeapot->SetAlpha(fOPAQUE);
			SetCapture(QuicksandEngine::g_pApp->GetHwnd());
			return 1;
		}
		else if (msg.param1=='Q' || msg.param1==GLFW_KEY_ESCAPE)				// Jan 2010 - mlm - added VK_ESCAPE since it is on the onscreen help!
		{
			if (::GLMessageBox::Ask(QUESTION_QUIT_GAME)==IDYES)
			{
				QuicksandEngine::g_pApp->SetQuitting(true);
			}
			return 1;
		}
	}

	return 0;
}


//
// TeapotWarsHumanView::VRenderText				- Chapter 19, page 727
//
void TeapotWarsHumanView::VRenderText()
{
	if (!GLRenderer_Base::g_pTextHelper)
		return;

	HumanView::VRenderText();

	GLRenderer_Base::g_pTextHelper->Begin(g_ArielFontLocation);

    // Gameplay UI (with shadow)....
    if (!m_gameplayText.empty())
    {
		GLRenderer_Base::g_pTextHelper->SetInsertionPos(GLUFPoint(QuicksandEngine::g_pApp->GetScreenSize().x / 2, 5));
		GLRenderer_Base::g_pTextHelper->SetForegroundColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
		GLRenderer_Base::g_pTextHelper->DrawTextLine(m_gameplayText.c_str());
		GLRenderer_Base::g_pTextHelper->SetInsertionPos(GLUFPoint(QuicksandEngine::g_pApp->GetScreenSize().x / 2 - 1, 5 - 1));
		GLRenderer_Base::g_pTextHelper->SetForegroundColor(Color(0.25f, 1.0f, 0.25f, 1.0f));
		GLRenderer_Base::g_pTextHelper->DrawTextLine(m_gameplayText.c_str());
    }
	// ...Gameplay UI

	if( m_bShowUI )
	{
		// Output statistics... TODO:
		GLRenderer_Base::g_pTextHelper->SetInsertionPos(GLUFPoint(5, 5));
		GLRenderer_Base::g_pTextHelper->SetForegroundColor(Color(1.0f, 1.0f, 0.0f, 1.0f));
		GLRenderer_Base::g_pTextHelper->DrawTextLine(GLUF::GLUFGetFrameStats());
		GLRenderer_Base::g_pTextHelper->DrawTextLine(GLUF::GLUFGetDeviceStats());
		//...output statistics


		GLRenderer_Base::g_pTextHelper->SetInsertionPos(GLUF::GLUFPoint(5, 5));
		GLRenderer_Base::g_pTextHelper->SetForegroundColor(Color(0, 0, 0, 128));

		//Game State...
		switch (m_BaseGameState)
		{
			case BGS_Initializing:
				GLRenderer_Base::g_pTextHelper->DrawTextLine(QuicksandEngine::g_pApp->GetString(_T("IDS_INITIALIZING")).c_str());
				break;

			case BGS_MainMenu:
				GLRenderer_Base::g_pTextHelper->DrawTextLine(L"Main Menu");
				break;

//			case BGS_SpawnAI:
//				D3DRenderer::g_pTextHelper->DrawTextLine(L"Spawn AI");
//				break;

			case BGS_WaitingForPlayers:
				GLRenderer_Base::g_pTextHelper->DrawTextLine(QuicksandEngine::g_pApp->GetString(_T("IDS_WAITING")).c_str());
				break;

			case BGS_LoadingGameEnvironment:
				GLRenderer_Base::g_pTextHelper->DrawTextLine(QuicksandEngine::g_pApp->GetString(_T("IDS_LOADING")).c_str());
				break;

			case BGS_Running:
#ifndef DISABLE_PHYSICS
				GLRenderer_Base::g_pTextHelper->DrawTextLine(QuicksandEngine::g_pApp->GetString(_T("IDS_RUNNING")).c_str());
#else
				D3DRenderer::g_pTextHelper->DrawTextLine(QuicksandEngine::g_pApp->GetString(_T("IDS_NOPHYSICS")).c_str());
#endif //!DISABLE_PHYSICS
				break;
		}
		//...Game State

		//Camera...
		TCHAR buffer[256];
		const TCHAR *s = NULL;
		glm::mat4 toWorld;
		glm::mat4 fromWorld;
		if (m_pCamera)
		{	
			m_pCamera->VGet()->Transform(&toWorld, &fromWorld);
		}
		swprintf(buffer, QuicksandEngine::g_pApp->GetString(_T("IDS_CAMERA_LOCATION")).c_str(), toWorld[3][0], toWorld[3][1], toWorld[3][2]);
		GLRenderer_Base::g_pTextHelper->DrawTextLine( buffer );
		//...Camera

		//Help text.  Right justified, lower right of screen.
		GLUFRect helpRect;
		helpRect.left = 0;
		helpRect.right = QuicksandEngine::g_pApp->GetScreenSize().x - 10;
		helpRect.top = QuicksandEngine::g_pApp->GetScreenSize().y + 15 * 8;
		helpRect.bottom = QuicksandEngine::g_pApp->GetScreenSize().y;
		GLRenderer_Base::g_pTextHelper->SetInsertionPos(GLUFPoint(helpRect.right, helpRect.top));
		GLRenderer_Base::g_pTextHelper->SetForegroundColor(Color(255, 192, 0, 255));
		GLRenderer_Base::g_pTextHelper->DrawTextLine(helpRect, DT_RIGHT, QuicksandEngine::g_pApp->GetString(_T("IDS_CONTROLS_HEADER")).c_str());
		helpRect.top = QuicksandEngine::g_pApp->GetScreenSize().y + 15 * 7;
		GLRenderer_Base::g_pTextHelper->DrawTextLine(helpRect, DT_RIGHT, QuicksandEngine::g_pApp->GetString(_T("IDS_CONTROLS")).c_str());
		//...Help
	}//end if (m_bShowUI)

	GLRenderer_Base::g_pTextHelper->End();
}



//
// TeapotWarsHumanView::VOnUpdate				- Chapter 19, page 730
//
void TeapotWarsHumanView::VOnUpdate(unsigned long deltaMs)
{
	HumanView::VOnUpdate( deltaMs );

	if (m_pFreeCameraController)
	{
		m_pFreeCameraController->OnUpdate(deltaMs);
	}

	if (m_pTeapotController)
	{
		m_pTeapotController->OnUpdate(deltaMs);
	}

	//Send out a tick to listeners.
	shared_ptr<EvtData_Update_Tick> pTickEvent(QSE_NEW EvtData_Update_Tick(deltaMs));
    IEventManager::Get()->VTriggerEvent(pTickEvent);
}

//
// TeapotWarsHumanView::VOnAttach				- Chapter 19, page 731
//
void TeapotWarsHumanView::VOnAttach(GameViewId vid, ActorId aid)
{
	HumanView::VOnAttach(vid, aid);
}

bool TeapotWarsHumanView::VLoadGameDelegate(tinyxml2::XMLElement* pLevelData)
{
	if (!HumanView::VLoadGameDelegate(pLevelData))
		return false;

    m_StandardHUD.reset(QSE_NEW StandardHUD); 
    VPushElement(m_StandardHUD);

    // A movement controller is going to control the camera, 
    // but it could be constructed with any of the objects you see in this
    // function. You can have your very own remote controlled sphere. What fun...
    m_pFreeCameraController.reset(QSE_NEW MovementController(m_pCamera, 0, 0, false));

    m_pScene->VOnRestore();
    return true;
}

void TeapotWarsHumanView::VSetControlledActor(ActorId actorId) 
{ 
	m_pTeapot = static_pointer_cast<SceneNode>(m_pScene->FindActor(actorId));
    if (!m_pTeapot)
    {
        LOG_ERROR("Invalid teapot");
        return;
    }

	HumanView::VSetControlledActor(actorId);

    m_pTeapotController.reset(QSE_NEW TeapotController(m_pTeapot));
    m_KeyboardHandler = m_pTeapotController;
    m_PointerHandler = m_pTeapotController;
    m_pCamera->SetTarget(m_pTeapot);
    m_pTeapot->SetAlpha(0.8f);
}

void TeapotWarsHumanView::GameplayUiUpdateDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EvtData_Gameplay_UI_Update> pCastEventData = static_pointer_cast<EvtData_Gameplay_UI_Update>(pEventData);
    if (!pCastEventData->GetUiString().empty())
        m_gameplayText = s2ws(pCastEventData->GetUiString());
    else
        m_gameplayText.clear();
}

void TeapotWarsHumanView::SetControlledActorDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EvtData_Set_Controlled_Actor> pCastEventData = static_pointer_cast<EvtData_Set_Controlled_Actor>(pEventData);
    VSetControlledActor(pCastEventData->GetActorId());
}


void TeapotWarsHumanView::RegisterAllDelegates(void)
{
	// [mrmike] Move, New, and Destroy actor events are now handled by the HumanView, as are the PlaySound and GameState events

    IEventManager* pGlobalEventManager = IEventManager::Get();
    pGlobalEventManager->VAddListener(MakeDelegate(this, &TeapotWarsHumanView::GameplayUiUpdateDelegate), EvtData_Gameplay_UI_Update::sk_EventType);
    pGlobalEventManager->VAddListener(MakeDelegate(this, &TeapotWarsHumanView::SetControlledActorDelegate), EvtData_Set_Controlled_Actor::sk_EventType);
}

void TeapotWarsHumanView::RemoveAllDelegates(void)
{
    IEventManager* pGlobalEventManager = IEventManager::Get();
    pGlobalEventManager->VRemoveListener(MakeDelegate(this, &TeapotWarsHumanView::GameplayUiUpdateDelegate), EvtData_Gameplay_UI_Update::sk_EventType);
    pGlobalEventManager->VRemoveListener(MakeDelegate(this, &TeapotWarsHumanView::SetControlledActorDelegate), EvtData_Set_Controlled_Actor::sk_EventType);
}



///////////////////////////////////////////////////////////////////////////////
//
// AITeapotView::AITeapotView					- Chapter 19, page 737
//
AITeapotView::AITeapotView(shared_ptr<PathingGraph> pPathingGraph) : IGameView(), m_pPathingGraph(pPathingGraph)
{
    //
}

//
// AITeapotView::~AITeapotView					- Chapter 19, page 737
//
AITeapotView::~AITeapotView(void)
{
    LOG_WRITE(ConcatString("AI", "Destroying AITeapotView"));
}
