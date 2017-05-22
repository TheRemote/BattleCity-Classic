#include "CCollision.h"

CCollision::CCollision(CGame *game)
{
	p = game;
}

CCollision::~CCollision()
{

}

int CCollision::CheckPlayerCollision(unsigned short id)
{
	if (p->Player[id]->isAdmin == 2) return 0;

	//Map edges
	if (p->Player[id]->X < 0) return 200;
	if (p->Player[id]->X > (511 * 48)) return 201;
	if (p->Player[id]->Y < 0) return 202;
	if (p->Player[id]->Y > (511 * 48)) return 203;

	//Map Terrain (lava, rocks, CC corners)
	/*
	if (p->Map->map[(int)(p->Player[id]->X+gap)/48][(int)(p->Player[id]->Y+gap)/48] != 0) return 2; //top left corner
	if (p->Map->map[(int)(p->Player[id]->X+48-gap*2)/48][(int)(p->Player[id]->Y+gap)/48] != 0) return 2; //top right corner
	if (p->Map->map[(int)(p->Player[id]->X+48-gap*2)/48][(int)(p->Player[id]->Y+48-gap*2)/48] != 0) return 2; //bottom right corner
	if (p->Map->map[(int)(p->Player[id]->X+gap)/48][(int)(p->Player[id]->Y+48-gap*2)/48] != 0) return 2; //bottom left corner
	*/
		int mYX = (int) p->Player[id]->X / 48;
		int mYY = (int) p->Player[id]->Y / 48;
	for (int i = mYX - 3; i <= (mYX + 3); i++)
	{
		for (int j = mYY - 3; j <= (mYY + 3); j++)
		{
			if (i >= 0 && j >= 0 && i < 512 && j < 512)
			{
				if (p->Map->map[i][j] == 1 || p->Map->map[i][j] == 2)
				{
					if (RectCollision(
							(int) p->Player[id]->X, (int) p->Player[id]->Y, 48, 48, 4,
							i * 48, j * 48, 48, 48, 8) == true)
					{
						return 2;
					}
				}
			}
		}
	}

	CBuilding *bld = p->Build->buildings;
	if (bld)
	{
		while (bld->prev)
			bld = bld->prev;

		while (bld)
		{
			int BuildingX = (bld->X-2)*48;
			int BuildingY = (bld->Y-2)*48;
			int BuildingHeight = 144;
			if (bld->Type / 100 <= 2)
			{
				if (bld->Type == 200 && p->Tick > p->InGame->hospTick)
				{
					if (id == p->Winsock->MyIndex && RectCollision((int)p->Player[id]->X, (int)p->Player[id]->Y, 48, 48, 4, BuildingX, BuildingY, 144, BuildingHeight, 4))
					{
						if (RectCollision((int)p->Player[id]->X, (int)p->Player[id]->Y, 48, 48, 16, BuildingX, BuildingY, 144, BuildingHeight, 12))
						{
							p->Player[p->Winsock->MyIndex]->SetHP(p->Player[p->Winsock->MyIndex]->HP + 5);
							if (p->Player[p->Winsock->MyIndex]->HP > 40) p->Player[p->Winsock->MyIndex]->SetHP(40);
							p->InGame->hospTick = p->Tick + 500;
						}
						BuildingY = (bld->Y-1)*48;
						BuildingHeight = 96;
					}
					else
					{
						BuildingY = (bld->Y-1)*48;
						BuildingHeight = 96;
					}
				}
				else
				{
					BuildingY = (bld->Y-1)*48;
					BuildingHeight = 96;
				}
			}

			if (RectCollision((int)p->Player[id]->X, (int)p->Player[id]->Y, 48, 48, 4, BuildingX, BuildingY, 144, BuildingHeight, 8))
			{
				return 2;
			}
			
			bld = bld->next;
		}
	}

	CItem *itm = p->Item->items;
	if (itm)
	{
		while (itm->prev)
			itm = itm->prev;

		while (itm)
		{
			
			if (itm->active)
			{
				if (RectCollision((int)p->Player[id]->X, (int)p->Player[id]->Y, 48, 48, 4, itm->X*48, itm->Y*48, 48, 48, 8) && itm->Type >= 8 && itm->active)	return 2; //walls and turrets

				//Mine and DFG checking, use smaller player collision area for player
				if (RectCollision((int)p->Player[id]->X+24, (int)p->Player[id]->Y+24, 1, 1, 0, itm->X*48, itm->Y*48, 48, 48, 4) && itm->Type == 4 && p->Player[id]->City != itm->City) return 101;
				if (RectCollision((int)p->Player[id]->X+24, (int)p->Player[id]->Y+24, 1, 1, 0, itm->X*48, itm->Y*48, 48, 48, 4) && itm->Type == 7 && p->Player[id]->City != itm->City) return 103;

			}
			itm = itm->next; //isMoving to next item
		}
	}
	return 0;
}

bool CCollision::RectCollision(int X, int Y, int width, int height,
			int tolerance1, int X2, int Y2, int width2, int height2,
			int tolerance2)
	{
		Rect rect1;

		rect1.X = X + tolerance1;
		rect1.Y = Y + tolerance1;
		rect1.w = width - tolerance1 * 2;
		rect1.h = height - tolerance1 * 2;

		Rect rect2;

		rect2.X = X2 + tolerance2;
		rect2.Y = Y2 + tolerance2;
		rect2.w = width2 - tolerance2 * 2;
		rect2.h = height2 - tolerance2 * 2;

		rect1.w += rect1.X; 
		rect1.h += rect1.Y;
		rect2.w += rect2.X;
		rect2.h += rect2.Y;
		
		if (rect1.w < rect2.X)
			return false;
		if (rect2.w < rect1.X)
			return false;

		if (rect1.h < rect2.Y)
			return false;
		if (rect2.h < rect1.Y)
			return false;

		return true;
	}

int CCollision::CheckBuildingCollision(int X, int Y)
{
	int me = p->Winsock->MyIndex;

	if (X < 0 || Y < 0) return 1;

	//map objects
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			if (p->Map->map[X-j][Y-i] != 0)
				return 1;

	CItem *itm = p->Item->items;
	if (itm)
	{
		while (itm->prev)
			itm = itm->prev;

		while (itm)
		{
			for (int i = 0; i < 3; i++)
				for (int j = 0; j < 3; j++)
				if (X == itm->X+j && Y == itm->Y+i)
					return 1;
			itm = itm->next;
		}
	}

	CBuilding *bld = p->Build->buildings;
	if (bld)
	{
		while (bld->prev)
			bld = bld->prev;

		while (bld)
		{
			if (X >= bld->X-2 && X <= bld->X+2 && Y >= bld->Y-2 && Y <= bld->Y+2)
				return 1;
			bld = bld->next;
		}
	}

	//players
	for (int a = 0; a < MaxPlayers; a++)
	{
		if (p->Player[a]->X-48 <= X*48 && p->Player[a]->X+48 >= (X-2)*48 && p->Player[a]->Y-48 <= Y*48 && p->Player[a]->Y+48 >= (Y-2)*48) return 1;		
	}
	if (p->Player[me]->X-48 <= X*48 && p->Player[me]->X+48 >= (X-2)*48 && p->Player[me]->Y-48 <= Y*48 && p->Player[me]->Y+48 >= (Y-2)*48) return 1;

	return 0; //no colliding object
}