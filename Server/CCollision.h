#ifndef __COL__
#define __COL__

#include "CServer.h"

class CServer;

struct Rect
{
	long x;
	long y;
	long w;
	long h;
};

class CCollision
{
public:
	CCollision(CServer *Server);
	~CCollision();

	bool RectCollision(int X, int Y, int width, int height,
			int tolerance1, int X2, int Y2, int width2, int height2,
			int tolerance2);
private:
	CServer *p;
protected:
};

#endif
