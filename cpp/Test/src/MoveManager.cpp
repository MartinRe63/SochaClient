/*
 * MoveManager.cpp
 *
 *  Created on: 29.05.2019
 *      Author: mrenneke
 */

#include <string.h>
#include "MoveManager.h"

unsigned MoveManager::getMove(int color, board SourceBoard, board DestinationBoard)
{
	return 0;
}

packedMove MoveManager::PackMove( mov M )
{
	int fromX = M[0] % 10;
	int fromY = M[0] / 10;
	int toX = M[1] % 10;
	int toY = M[1] / 10;
	return fromX | fromY << 4 | toX << 8 | toY << 12;
}

packedMove MoveManager::PackMove(coordinates CoordFrom, coordinates CoordTo)
{
	mov M = { CoordFrom, CoordTo };
	return PackMove(M);
}

void MoveManager::UnpackMove( packedMove PM, mov& M )
{
	int mask = 15;
	M[0] = PM & mask; mask <<= 4;
	M[0] += ((PM & mask) >> 4) * 10; mask <<= 4;
	M[1] = (PM & mask) >> 8; mask <<= 4;
	M[1] += ((PM & mask) >> 12) * 10;
}

std::string MoveManager::CoordinatesToString(coordinates Coord)
{
	int val = (int)'a';
	return std::string(std::string((std::string(1, (const char)(val + Coord % 10)) + std::string(1, (const char)(Coord / 10)))));
}

void MoveManager::CoordinatesToXY(coordinates Coord, int * x, int * y)
{
	*x = Coord % 10;
	*y = Coord / 10; 
}

std::string MoveManager::packMoveToString(packedMove PM)
{
	mov M;

	UnpackMove(PM, M);
	std::string res = CoordinatesToString(M[0]) + "->" + CoordinatesToString(M[1]);
	return res;
}

int MoveManager::moveValue(packedMove PM)
{
	int mask = 15;
	int fromX = (PM & mask); mask <<= 4;
	int fromY = (PM & mask) >> 4; mask <<= 4;
	int toX = (PM & mask) >> 8; mask <<= 4;
	int toY = (PM & mask) >> 12;

	int toVal = (toX >= 5 ? 9 - toX : toX) * (toY >= 5 ? 9 - toY : toY);
	int fromVal = (fromX >= 5 ? 9 - fromX : fromX) * (fromY >= 5 ? 9 - fromY : fromY);
	return (toVal - fromVal);
}



