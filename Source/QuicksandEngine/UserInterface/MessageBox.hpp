#ifndef QSE_MESSAGEBOX_HPP
#define QSE_MESSAGEBOX_HPP

//#include <DXUTGui.h>
#include "UserInterface.hpp"

//
// class MessageBox - Chapter 10, page 286
//
class GLMessageBox : public BaseUI
{
protected:
	GLUFDialog m_UI;             // ObjGLUF dialog
	int m_ButtonId;

public:
	GLMessageBox(std::wstring msg, std::wstring title, int buttonFlags = MB_OK);
	~GLMessageBox();

	// IScreenElement Implementation
	virtual HRESULT VOnRestore();
	virtual HRESULT VOnRender(double fTime, float fElapsedTime);
	virtual int VGetZOrder() const { return 99; }
	virtual void VSetZOrder(int const zOrder) { }

	virtual LRESULT  VOnMsgProc( AppMsg msg );
	static void  OnGUIEvent(GLUF_EVENT nEvent, int nControlID, GLUFControl* pControl);
	static int Ask(MessageBox_Questions question);

};

#endif