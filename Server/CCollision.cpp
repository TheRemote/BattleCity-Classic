#include "CCollision.h"
Rect tr, td;

CCollision::CCollision(CServer *Server)
{
	p = Server;
}

CCollision::~CCollision()
{

}

bool CCollision::RectCollision(int X, int Y, int width, int height,
			int tolerance1, int X2, int Y2, int width2, int height2,
			int tolerance2)
	{
		Rect rect1;

		rect1.x = X + tolerance1;
		rect1.y = Y + tolerance1;
		rect1.w = width - tolerance1 * 2;
		rect1.h = height - tolerance1 * 2;

		Rect rect2;

		rect2.x = X2 + tolerance2;
		rect2.y = Y2 + tolerance2;
		rect2.w = width2 - tolerance2 * 2;
		rect2.h = height2 - tolerance2 * 2;

		rect1.w += rect1.x; 
		rect1.h += rect1.y;
		rect2.w += rect2.x;
		rect2.h += rect2.y;
		
		if (rect1.w < rect2.x)
			return false;
		if (rect2.w < rect1.x)
			return false;

		if (rect1.h < rect2.y)
			return false;
		if (rect2.h < rect1.y)
			return false;

		return true;
	}
