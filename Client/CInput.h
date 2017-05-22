#ifndef _CINPUT
#define _CINPUT

#include "CGame.h"

class CGame;

class CInput
{
public:
	CInput(CGame *game);
	~CInput();

	void ProcessKeys(char buffer[256]);
	void MouseMove(DIMOUSESTATE mouse_state, int X, int Y, char buffer[256]);
	void MouseUp(DIMOUSESTATE mouse_state, int X, int Y, char buffer[256]);
	void MouseDown(DIMOUSESTATE mouse_state, int X, int Y, char buffer[256]);
	void StartDInput();
	void Cycle();

	void InfoButton();
	void PointsButton();

	int LastMouseX;
	int LastMouseY;

	__int64 LastShot;

	char Tab;
	char MouseOverChat;
	
	POINT Mouse;
	POINT oMouse;
	DIMOUSESTATE oMouseState;
	int NeedRelease;
	int HasFocus;

	__int64 lastRefresh;
	__int64 DemolishTimer;
private:
	CGame *p;

	LPDIRECTINPUT8         lpDI;
	LPDIRECTINPUTDEVICE8   m_Keyboard;
	LPDIRECTINPUTDEVICE8   m_Mouse;
protected:

};

#endif