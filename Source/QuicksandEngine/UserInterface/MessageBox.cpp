#include "../Stdafx.hpp"
#include "../Graphics3D/GLRenderer.hpp"

#include "HumanView.hpp"
#include "MessageBox.hpp"


//
// class MessageBox::OnGUIEvent					- Chapter 10, page 292 & 296
//
void GLMessageBox::OnGUIEvent(GLUF_EVENT nEvent, int nControlID, GLUFControl* pControl)
{
	PostMessage(QuicksandEngine::g_pApp->GetHwnd(), g_MsgEndModal, 0, nControlID);
}

//
// class MessageBox::MessageBox					- Chapter 10, page 287
//
GLMessageBox::GLMessageBox(std::wstring msg, std::wstring title, int buttonFlags)
{
	// Initialize dialogs
	m_UI.Init( &GLRenderer::g_DialogResourceManager );
    m_UI.SetCallback( OnGUIEvent ); 

	// Find the dimensions of the message
 	GLUFRect rc;
	GLUFSetRect( rc, 0.0f,0.0f,0.0f,0.0f);
	m_UI.CalcTextRect( msg, m_UI.GetDefaultElement(GLUF_CONTROL_STATIC,0), rc );
    float msgWidth = rc.right - rc.left;
	float msgHeight = rc.bottom - rc.top;

	int numButtons = 2;
	if ( (buttonFlags == MB_ABORTRETRYIGNORE) ||
		(buttonFlags == MB_CANCELTRYCONTINUE) ||
		(buttonFlags == MB_CANCELTRYCONTINUE) )
	{
		numButtons = 3;
	}
	else if (buttonFlags == MB_OK)
	{
		numButtons = 1;
	}

	float btnWidth = ((float)QuicksandEngine::g_pApp->GetScreenSize().x * 0.15f);
	float btnHeight = ((float)QuicksandEngine::g_pApp->GetScreenSize().y * 0.037f);
	float border = ((float)QuicksandEngine::g_pApp->GetScreenSize().x * 0.043f);

	m_Width = std::max(msgWidth + 2.0f * border, btnWidth + 2 * border);
	m_Height = msgHeight + (numButtons * (btnHeight+border) ) + (2 * border);

	m_PosX = (1.0f - m_Width) / 2;
	m_PosY = (1.0f - m_Height) / 2;
	m_UI.SetLocation(m_PosX, m_PosY);

	m_UI.SetSize( m_Width, m_Height );
	//m_UI.SetBackgroundColors(g_Gray40);

	Color red = Color(255,0,0,200);
	m_UI.SetBackgroundColor(red);

	float fY = border; 
	float fX = (m_Width - msgWidth) / 2; 

	m_UI.AddStatic( 0, msg.c_str(), fX, fY, msgWidth, msgHeight);

	fX = (m_Width - btnWidth) / 2;
	fY = m_Height - btnHeight - border;

	buttonFlags &= 0xF;
	if ( (buttonFlags == MB_ABORTRETRYIGNORE) ||
		 (buttonFlags == MB_CANCELTRYCONTINUE) )

	{
		// The message box contains three push buttons: Cancel, Try Again, Continue. 
		// This is the new standard over Abort,Retry,Ignore
		m_UI.AddButton(IDCONTINUE, QuicksandEngine::g_pApp->GetString(_T("IDS_CONTINUE")).c_str(), fX, fY - (2 * border), btnWidth, btnHeight);
		m_UI.AddButton(IDTRYAGAIN, QuicksandEngine::g_pApp->GetString(_T("IDS_TRYAGAIN")).c_str(), fX, fY - border, btnWidth, btnHeight);
		m_UI.AddButton(IDCANCEL, QuicksandEngine::g_pApp->GetString(_T("IDS_CANCEL")).c_str(), fX, fY, btnWidth, btnHeight);
	}
	else if (buttonFlags == MB_OKCANCEL)
	{
		//The message box contains two push buttons: OK and Cancel.
		m_UI.AddButton(IDOK, QuicksandEngine::g_pApp->GetString(_T("IDS_OK")).c_str(), fX, fY - border, btnWidth, btnHeight);
		m_UI.AddButton(IDCANCEL, QuicksandEngine::g_pApp->GetString(_T("IDS_CANCEL")).c_str(), fX, fY, btnWidth, btnHeight);
	}
	else if (buttonFlags == MB_RETRYCANCEL)
	{
		//The message box contains two push buttons: Retry and Cancel.
		m_UI.AddButton(IDRETRY, QuicksandEngine::g_pApp->GetString(_T("IDS_RETRY")).c_str(), fX, fY - border, btnWidth, btnHeight);
		m_UI.AddButton(IDCANCEL, QuicksandEngine::g_pApp->GetString(_T("IDS_CANCEL")).c_str(), fX, fY, btnWidth, btnHeight);
	}
	else if (buttonFlags == MB_YESNO)
	{
		//The message box contains two push buttons: Yes and No.
		m_UI.AddButton(IDYES, QuicksandEngine::g_pApp->GetString(_T("IDS_YES")).c_str(), fX, fY - border, btnWidth, btnHeight, 0x59);
		m_UI.AddButton(IDNO, QuicksandEngine::g_pApp->GetString(_T("IDS_NO")).c_str(), fX, fY, btnWidth, btnHeight, 0x4E);
	}
	else if (buttonFlags == MB_YESNOCANCEL)
	{
		//The message box contains three push buttons: Yes, No, and Cancel.
		m_UI.AddButton(IDYES, QuicksandEngine::g_pApp->GetString(_T("IDS_YES")).c_str(), fX, fY - (2 * border), btnWidth, btnHeight);
		m_UI.AddButton(IDNO, QuicksandEngine::g_pApp->GetString(_T("IDS_NO")).c_str(), fX, fY - border, btnWidth, btnHeight);
		m_UI.AddButton(IDCANCEL, QuicksandEngine::g_pApp->GetString(_T("IDS_CANCEL")).c_str(), fX, fY, btnWidth, btnHeight);
	}
	else //if (buttonFlags & MB_OK)
	{
        // The message box contains one push button: OK. This is the default.
		m_UI.AddButton(IDOK, QuicksandEngine::g_pApp->GetString(_T("IDS_OK")).c_str(), fX, fY, btnWidth, btnHeight);
	}
}

GLMessageBox::~GLMessageBox()
{
	LOG_ASSERT(1);
}


//
// class MessageBox::VOnRestore					- Chapter 10, page 291
//
HRESULT GLMessageBox::VOnRestore()
{
    m_UI.SetLocation( m_PosX, m_PosY );
    m_UI.SetSize( m_Width, m_Height );
	return S_OK;
}


//
// class MessageBox::VOnRender					- Chapter 10, page 291
//
HRESULT GLMessageBox::VOnRender(double fTime, double fElapsedTime)
{
	HRESULT hr;
	V( m_UI.OnRender( (float)fElapsedTime ) );
	return S_OK;
};


//
// class MessageBox::VOnMsgProc					- Chapter 10, page 292
//
LRESULT GLMessageBox::VOnMsgProc(AppMsg msg)
{
    return m_UI.MsgProc(msg.m_Event, msg.param1, msg.param2, msg.param3, msg.param4);
}

int GLMessageBox::Ask(MessageBox_Questions question)
{
	std::wstring msg;
	std::wstring title;
	int buttonFlags;
	int defaultAnswer = IDOK;

	switch(question)
	{
		case QUESTION_WHERES_THE_CD:
		{
			msg = QuicksandEngine::g_pApp->GetString(_T("IDS_QUESTION_WHERES_THE_CD"));
			title = QuicksandEngine::g_pApp->GetString(_T("IDS_ALERT"));
			buttonFlags = MB_RETRYCANCEL|MB_ICONEXCLAMATION;
			defaultAnswer = IDCANCEL;
			break;
		}
		case QUESTION_QUIT_GAME:
		{
			msg = QuicksandEngine::g_pApp->GetString(_T("IDS_QUESTION_QUIT_GAME"));
			title = QuicksandEngine::g_pApp->GetString(_T("IDS_QUESTION"));
			buttonFlags = MB_YESNO|MB_ICONQUESTION;
			defaultAnswer = IDNO;
			break;
		}
		default:
			LOG_ASSERT(0 && _T("Undefined question in Game::Ask"));
			return IDCANCEL;
	}

	if (QuicksandEngine::g_pApp && QuicksandEngine::g_pApp->IsRunning())
	{
		ShowCursor(true);
		shared_ptr<GLMessageBox> pMessageBox(new GLMessageBox(msg, title, buttonFlags));
		int result = QuicksandEngine::g_pApp->Modal(pMessageBox, defaultAnswer);
		ShowCursor(false);
		return result;
	}

	//todo: cross platform?
	return ::MessageBox(QuicksandEngine::g_pApp ? QuicksandEngine::g_pApp->GetHwnd() : NULL, msg.c_str(), title.c_str(), buttonFlags);
}


