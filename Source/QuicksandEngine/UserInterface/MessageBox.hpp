#ifndef QSE_MESSAGEBOX_HPP
#define QSE_MESSAGEBOX_HPP

//#include <DXUTGui.h>
#include "UserInterface.hpp"

//
// class MessageBox - Chapter 10, page 286
//
class MessageBox : public BaseUI
{
protected:
	CDXUTDialog m_UI;             // DirectX dialog
	int m_ButtonId;

public:
	MessageBox(std::wstring msg, std::wstring title, int buttonFlags=MB_OK);
	~MessageBox();

	// IScreenElement Implementation
	virtual HRESULT VOnRestore();
	virtual HRESULT VOnRender(double fTime, float fElapsedTime);
	virtual int VGetZOrder() const { return 99; }
	virtual void VSetZOrder(int const zOrder) { }

	virtual LRESULT CALLBACK VOnMsgProc( AppMsg msg );
	static void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void *pUserContext );
	static int Ask(MessageBox_Questions question);

};

#endif