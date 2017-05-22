#include "CServer.h"

CServer *Server;


__int64 LastBulletTick = 0;
__int64 ItemTick = 0;
__int64 BuildingTick = 0;
__int64 BulletTick = 0;
__int64 Top20Tick = 0;
__int64 CityTick = 0;


void GameUpdate()
{
		if (Server->Tick > ItemTick)
		{
			Server->Item->cycle();
			ItemTick = Server->Tick + 100;
		}

		if (Server->Tick > BuildingTick)
		{
			Server->Build->cycle();
			BuildingTick = BuildingTick + 50;
		}

		Server->Bullet->cycle();
		
		if (Server->Tick > Top20Tick)
		{
			Server->Account->GenerateTop20();
			Top20Tick = Server->Tick + 300000;
		}

		if (Server->Tick > CityTick)
		{
			for (int i = 0; i < 64; i++)
			{
				Server->City[i]->cycle();
			}
			CityTick = Server->Tick + 1000;
		}
}


int main(int argc, char *argv[])
{
	Server = new CServer();

	timeBeginPeriod(1);

	Server->Init();

	int DEFAULT_FPS = 75;
	long period = 15;
	static int NO_DELAYS_PER_YIELD = 16;
	static int MAX_FRAME_SKIPS = 8;

	__int64 beforeTime, afterTime, timeDiff = 0, sleepTime;
	__int64 overSleepTime = 0L;
	int noDelays = 0;
	__int64 excess = 0L;

	Server->Tick = Server->Timer->GetTime();
	beforeTime = Server->Tick;

	while (Server->running == 1)
	{
		Server->lastTick = Server->Tick;
		Server->Tick = Server->Timer->GetTime();

		Server->Winsock->Cycle();

		GameUpdate();

		Server->Winsock->Cycle();

		afterTime = Server->Timer->GetTime();
		timeDiff = afterTime - beforeTime;
		sleepTime = (period - timeDiff) - overSleepTime;

		if (sleepTime > 0)
		{
			Sleep((DWORD)sleepTime);
			Server->Winsock->Cycle();
			overSleepTime = (Server->Timer->GetTime() - afterTime) - sleepTime;
		}
		else
		{
			excess -= sleepTime;
			overSleepTime = 0L;
		}

		beforeTime = Server->Timer->GetTime();

		int skips = 0;
		while ((excess > period) && (skips < MAX_FRAME_SKIPS))
		{
			excess -= period;
			GameUpdate();
			skips++;
		}
	}

	cout << "Server::Close" << endl;

	for (int i = 0; i < MaxPlayers; i++)
	{
		if (Server->Player[i]->State > State_Disconnected)
		{
			Server->Player[i]->Clear();
		}
	}

	delete Server;
	
	cout << endl << endl << endl << "The server has been safely shutdown.  Press any key to exit" << endl;

#ifdef WIN32
	getch();
#endif

	return EXIT_SUCCESS;
}