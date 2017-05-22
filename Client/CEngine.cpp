#include "CEngine.h"
void _cdecl thrEngine(void * pParam);
void _cdecl thrMessageBox(void * pParam);
void _cdecl thrMessageQuit(void * pParam);

CEngine::CEngine(CGame *game)
{
	p = game;
	this->FPS = 0;
	this->FPSTick = 0;
	this->loops = 0;
	this->MsgUp = 0;
	this->MsgQueue = 0;

	this->lastFPS = 0;
	this->lastlastFPS = 0;

	gdwScanLine = 0;
	gdwScanLineLastTime = 0;
	gbUsed = false;
}

CEngine::~CEngine()
{

}

void CEngine::Init()
{
	_beginthread(thrEngine,0,p);
}

void CEngine::GameRender()
{
	//p->DDraw->DirectDraw->WaitForVerticalBlank( DDWAITVB_BLOCKBEGIN, NULL );

	if (p->Tick > p->Engine->FPSTick)
	{			
		if (p->State == STATE_GAME) p->InGame->RefreshArea();
		lastlastFPS = lastFPS;
		lastFPS = p->Engine->FPS;
		p->Engine->FPS = p->Engine->loops;
		p->Engine->FPSTick = p->Tick + 1000;
		p->Engine->loops = 0;
	}
	else
	{
		p->Engine->loops++;
	}


	switch (p->State)
	{
		case 0:
		case STATE_LOGIN:
		case STATE_RECOVER:
		case STATE_NEWACCOUNT:
		case STATE_PERSONALITY:
		case STATE_INTERVIEW:
		case STATE_MEETING:
		case STATE_VERIFY:
			p->DDraw->Clear();
				p->DDraw->Draw(p->DDraw->imgCompany, 400, 60, 320, 240);
				p->DDraw->Draw(p->DDraw->imgCompany, 80, 300, 320, 240);
				p->DDraw->Draw(p->DDraw->imgBCLogo, 80, 60, 320, 240);
				p->DDraw->Draw(p->DDraw->imgBCLogo, 400, 300, 320, 240);
			p->DDraw->Flip();
			Sleep(10);
			break;
		case STATE_JOINING:
			p->DDraw->Clear();
				p->DDraw->Draw(p->DDraw->imgCompany, 400, 60, 320, 240);
				p->DDraw->Draw(p->DDraw->imgCompany, 80, 300, 320, 240);
				p->DDraw->Draw(p->DDraw->imgBCLogo, 80, 60, 320, 240);
				p->DDraw->Draw(p->DDraw->imgBCLogo, 400, 300, 320, 240);
				p->DDraw->DTextOut(305, 280, "Joining Game - Please Wait", RGB(255, 215, 0));
			p->DDraw->Flip();
			Sleep(10);
			break;
		case STATE_GAME:
			p->DDraw->Clear();
				p->InGame->Render();
				if (p->Options->debug)
				{
					sprintf(FPSString, "FPS: %i", p->Engine->FPS);
					p->DDraw->DTextOut(p->Draw->MaxMapX + 28, 433, FPSString, RGB(255, 255, 255));
					sprintf(FPSString, "Ping: %i", p->InGame->TCPPing);
					p->DDraw->DTextOut(p->Draw->MaxMapX + 105, 433, FPSString, RGB(255, 255, 255));
				}
			p->DDraw->Flip();
	}
}

void CEngine::GameUpdate()
{
	p->Input->Cycle();
	p->Winsock->Cycle();

		switch (p->State)
	{
		case 0:
		case STATE_LOGIN:
		case STATE_RECOVER:
		case STATE_NEWACCOUNT:
		case STATE_PERSONALITY:
		case STATE_INTERVIEW:
		case STATE_MEETING:
		case STATE_VERIFY:
			break;
		case STATE_JOINING:
			break;
		case STATE_GAME:
			p->InGame->Cycle();
			break;
	}

	FSOUND_Update();

	if (p->Engine->FPS < 20 && lastFPS < 20 && lastlastFPS < 20 && p->State == STATE_GAME)
	{
		p->running = false;
		MessageBox(p->hWnd, "Your computer is running too slow to play BattleCity.  Try closing some other programs or updating your video card drivers.", "BattleCity", 0);
	}
}

void _cdecl thrEngine(void * pParam)
{
	CGame * p;
	p = (CGame *)pParam;

	int DEFAULT_FPS = 75;
	long period = 15;
	static int NO_DELAYS_PER_YIELD = 16;
	static int MAX_FRAME_SKIPS = 8;

	__int64 beforeTime, afterTime, timeDiff = 0, sleepTime;
	__int64 overSleepTime = 0L;
	int noDelays = 0;
	__int64 excess = 0L;

	p->Tick = p->Timer->GetTime();
	beforeTime = p->Tick;

	while (p->running)
	{
		p->Winsock->Cycle();
		if (p->running == false) break;
		p->lastTick = p->Tick;
		p->Tick = p->Timer->GetTime();

		p->Engine->GameRender();
		p->Engine->GameUpdate();

		afterTime = p->Timer->GetTime();
		timeDiff = afterTime - beforeTime;
		sleepTime = (period - timeDiff) - overSleepTime;

		if (sleepTime > 0)
		{
			Sleep((DWORD)sleepTime);
			overSleepTime = (p->Timer->GetTime() - afterTime) - sleepTime;
		}
		else
		{
			excess -= sleepTime;
			overSleepTime = 0L;
		}

		beforeTime = p->Timer->GetTime();

		int skips = 0;
		while ((excess > period) && (skips < MAX_FRAME_SKIPS))
		{
			excess -= period;
			p->Engine->GameUpdate();
			skips++;
		}
	}

	Sleep(100);

	_endthread();
}

void CEngine::logerror(std::string error)
{
	FILE *file;
	file = fopen("debug.log","a");
	char linebreak[3];
	linebreak[0] = 13;
	linebreak[1] = 10;
	linebreak[2] = 0;

	char ErrorString[255];
	strcpy(ErrorString, error.c_str());
	p->Winsock->SendData(cmCrash, ErrorString);

	if (p->State == STATE_GAME)
	{
		error = "WARNING:  Game Crash.  Code:  " + error;
		p->InGame->AppendChat(error.c_str(), RGB(255, 215, 0));
		p->Options->sound = 0;
	}

	error = linebreak + error;

	fwrite(error.c_str(), error.size(), 1, file); 

	fclose(file);

	error.clear();
}

void _cdecl thrMessageBox(void * pParam)
{
	CGame * p;
	p = (CGame *)pParam;

	p->Engine->MsgUp = 1;

	MessageBox(p->hWnd, p->Engine->MsgText.c_str(), "BattleCity", 0);

	p->Engine->MsgUp = 0;
	p->Engine->MsgQueue = 0;
}

void CEngine::ThreadMessage(std::string MsgText)
{
	if (this->MsgUp == 1) return;
	this->MsgText = MsgText;
	_beginthread(thrMessageBox, 0, p);
}

void CEngine::ThreadMessageQuit()
{
	if (p->Player[p->Winsock->MyIndex]->isFrozen == 0)
	{
		_beginthread(thrMessageQuit, 0, p);
	}
}

void _cdecl thrMessageQuit(void * pParam)
{
	CGame * p;
	p = (CGame *)pParam;

	p->Engine->MsgUp = 1;

	int response = MessageBox(p->hWnd, "Leave BattleCity?", "BattleCity", MB_YESNO || MB_ICONQUESTION);
	if (response == 1)
	{
		p->State = STATE_MEETING;
		p->Meeting->ShowMeetingDlg();
	}

	p->Engine->MsgUp = 0;
}
