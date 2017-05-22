#ifndef _CEngine
#define _CEngine

#include "CGame.h"

class CGame;

class CEngine
{
public:
	CEngine(CGame *game);
	~CEngine();

	void Init();

	void logerror(std::string error);

	int loops;
	int FPS;
	__int64 FPSTick;
	char MsgUp;
	char MsgQueue;

	void ThreadMessage(std::string MsgText);
	void ThreadMessageQuit();
	std::string MsgText;

	void GameUpdate();
	void GameRender();
private:
	CGame *p;

	char FPSString[20];
	int lastFPS;
	int lastlastFPS;

	DWORD       gdwScanLine;
	DWORD       gdwScanLineLastTime;
	BOOL        gbUsed;
protected:

};

#endif