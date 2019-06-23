/*
 * MoveManager.cpp
 *
 *  Created on: 29.05.2019
 *      Author: mrenneke
 */

#include <string.h>
#include <assert.h>
#include "BitManager.h"
#include "MoveManager.h"
#include "MaskManager.h"

static mov dirVectors[] = {
	{1, 0},
	{1, 1},
	{0, 1},
	{-1, 1},
	{-1, 0},
	{-1, -1},
	{0, -1},
	{1, -1}
};

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

void MoveManager::UnpackMove( superPackedMove SPM, mov& M )
{
	M[0] = SPM.packedMove % 128;
	M[1] = SPM.packedMove / 128;
}


std::string MoveManager::CoordinatesToString(coordinates Coord)
{
	int vala = (int)'a';
	int val0 = (int)'0';
	return std::string(std::string((std::string(1, (const char)(vala + Coord % 10)) + std::string(1, (const char)(val0 + Coord / 10)))));
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
inline int MoveManager::moveValue(int fromX, int fromY, int toX, int toY)
{
	int toVal = (toX >= 5 ? 9 - toX : toX) * (toY >= 5 ? 9 - toY : toY);
	int fromVal = (fromX >= 5 ? 9 - fromX : fromX) * (fromY >= 5 ? 9 - fromY : fromY);
	return (toVal - fromVal);
}
int MoveManager::moveValueSPM(superPackedMove sPM)
{
	int from = sPM.packedMove % 128;
	int to = sPM.packedMove / 128;
	int fromX = from % 10;
	int fromY = from / 10;
	int toX = to % 10;
	int toY = to / 10;
	return moveValue(fromX, fromY, toX, toY);
}
int MoveManager::moveValuePM(packedMove PM)
{
	int mask = 15;
	int fromX = (PM & mask); mask <<= 4;
	int fromY = (PM & mask) >> 4; mask <<= 4;
	int toX = (PM & mask) >> 8; mask <<= 4;
	int toY = (PM & mask) >> 12;
	return moveValue(fromX, fromY, toX, toY);
}

inline superPackedMove MoveManager::superPackMove(coordinates fromPos, coordinates toPos)
{
	superPackedMove ret;
	ret.isSuperPackedMove = 1;
	ret.packedMove = toPos * 128 + fromPos;
	return ret;
}

superPackedMove MoveManager::superPackMove(packedMove PM)
{
	mov m;
	MoveManager::UnpackMove(PM, m);
	superPackedMove ret;
	ret.isSuperPackedMove = 1;
	ret.isGameEndNode = 0;
	ret.totValue = 1;
	ret.packedMove = m[1] * 128 + m[0];
	return ret;
}

packedMove MoveManager::superPack2packMove( superPackedMove SPM )
{
	mov m;
	MoveManager::UnpackMove( SPM, m );
	return MoveManager::PackMove( m );
}

coordinates MoveManager::movePossible(int x, int y, int dir, int lth, board positionData, int color, boardpane superlong)
{
	int newX = 0;
	int newY = 0;
	int p = y * 10 + x;
	// goal - use less calculations as possible
	int oppositeColor = (color + 1) % 2;
	// masks can't check regarding a board extension
	switch (dir)
	{
	case 0: if ((newX = x + lth) >= 10) return -1;
		newY = y;
		break;
	case 1: if ((newX = x + lth) >= 10 || (newY = y + lth) >= 10) return -1;
		break;
	case 2: if ((newY = y + lth) >= 10) return -1;
		newX = x;
		break;
	case 3: if ((newX = x - lth) < 0 || (newY = y + lth) >= 10) return -1;
		break;
	case 4: if ((newX = x - lth) < 0) return -1;
		newY = y;
		break;
	case 5: if ((newX = x - lth) < 0 || (newY = y - lth) < 0) return -1;
		break;
	case 6: if ((newY = y - lth) < 0) return -1;
		newX = x;
		break;
	case 7: if ((newX = x + lth) >= 10 || (newY = y - lth) < 0) return -1;
		break;
	}
	int newP = newY * 10 + newX;
	uint64_t* mask = MaskManager::moveMasks[dir][lth - 1][p];
	if (lth > 1)
	{
		// can't jump over opposite color fishes
		if (((mask[0] & positionData[oppositeColor][0]) != 0) ||
			((mask[1] & positionData[oppositeColor][1]) != 0))
		{
			return -1;
		}
	}
	// can't jump over or on crakes - wrong - can jump over crakes
	superlong[0] = MaskManager::moveMasks[dir][lth][p][0];
	superlong[1] = MaskManager::moveMasks[dir][lth][p][1];
	//if ( ( ( maskFull[0] & positionData[2][0] ) != 0 ) ||
	//      ( ( maskFull[1] & positionData[2][1] ) != 0 ) )
	//{
	//	return -1;
	//}
	superlong[0] ^= mask[0];
	superlong[1] ^= mask[1];
	// can't jump on my own fish or on crakes
	if (((superlong[0] & positionData[color][0]) != 0) ||
		((superlong[1] & positionData[color][1]) != 0) ||
		((superlong[0] & positionData[2][0]) != 0) ||
		((superlong[1] & positionData[2][1]) != 0))
	{
		return -1;
	}
	return newP;
}

static long long dbg_cnt = 0;

int MoveManager::FishCountInDirection(coordinates p, int dir, board positionData)
{
	int moveLth =
		BitManager::BitCount(MaskManager::directionMasks[dir][p][0] & positionData[0][0]) +
		BitManager::BitCount(MaskManager::directionMasks[dir][p][1] & positionData[0][1]) +
		BitManager::BitCount(MaskManager::directionMasks[dir][p][0] & positionData[1][0]) +
		BitManager::BitCount(MaskManager::directionMasks[dir][p][1] & positionData[1][1]);
	return moveLth;
}
packedMove MoveManager::LastMove2packedMove(LastMove LM, board positionData)
{
	int fishCnt = FishCountInDirection(LM.x + LM.y * 10, LM.dir, positionData);
	int x = LM.x + dirVectors[LM.dir][0] * fishCnt;
	int y = LM.y + dirVectors[LM.dir][1] * fishCnt;
	coordinates goal = x+y*10;
	return MoveManager::PackMove(LM.x + LM.y * 10, goal);
}
LastMove MoveManager::PackedMove2LastMove(packedMove pM)
{
	mov m;
	MoveManager::UnpackMove(pM, m);
	int x = m[0] % 10;
	int y = m[0] / 10;
	int dx = m[1] % 10 - x;
	int dy = m[1] / 10 - y;
	assert(abs(dx) == abs(dy) || dy == 0 || dx == 0);
	dx /= abs(dx) ? abs(dx) : 1;
	dy /= abs(dy) ? abs(dy) : 1;
	for (int d = 0; d < 8; d++) {
		if (dx == dirVectors[d][0] && dy == dirVectors[d][1]) {
			LastMove r;
			r.dir = d;
			r.x = x;
			r.y = y;
			// printf("Turn x: %d, y: %d, direction: %d, xt: %d, yt: %d\n", r.x, r.y, r.dir, t.target % 10, t.target / 10);
			return r;
		}
	}
	assert(0);
	LastMove l = { 0 };
	return l;
}
int MoveManager::getMoveList(board positionData, int color, packedMove moves[])
{
	int moveCnt = 0;
	boardpane long128; // used by movePossible
	board pos;
	BoardManager::Copy(positionData, pos);
	int p = BitManager::GetFirstRightBitPos(pos[color][0], pos[color][1]);
	while (p < 100)
	{
		int x = p % 10;
		int y = p / 10;
		for (int dir = 0; dir < 4; dir++)
		{
			// count blue and red fishes in this direction
			int moveLth =
				BitManager::BitCount(MaskManager::directionMasks[dir][p][0] & positionData[0][0]) +
				BitManager::BitCount(MaskManager::directionMasks[dir][p][1] & positionData[0][1]) +
				BitManager::BitCount(MaskManager::directionMasks[dir][p][0] & positionData[1][0]) +
				BitManager::BitCount(MaskManager::directionMasks[dir][p][1] & positionData[1][1]);
			coordinates newP;
			if ((newP = MoveManager::movePossible(x, y, dir, moveLth, positionData, color, long128)) > -1)
				moves[moveCnt++] = MoveManager::PackMove(p, newP);
			if ((newP = movePossible(x, y, dir + 4, moveLth, positionData, color, long128)) > -1)
				moves[moveCnt++] = MoveManager::PackMove(p, newP);;
		}
		int oldp = p;
		p = BitManager::GetNextRightBitPosIgnorePrevious(pos[color][0], pos[color][1], p);
		_ASSERT_EXPR(p > oldp, "don't run in endless cycle.");
	}
	return moveCnt;
}

long long i = 0;

void MoveManager::addMoveToBoard( board positionData, int color, packedMove PM )
{
	mov move;
	MoveManager::UnpackMove(PM, move);

	// try to enable register usage
	int moveFrom = move[0];
	int moveTo = move[1];

	i++;

	assert(BitManager::IsBit(positionData[color], moveFrom));
	assert(!BitManager::IsBit(positionData[color], moveTo));
	assert(!BitManager::IsBit(positionData[2], moveTo));
	_ASSERT_EXPR( BitManager::IsBit(positionData[color], moveFrom), "unknown software issue - fish to move is not at the position to move from.");
	_ASSERT_EXPR( ! BitManager::IsBit(positionData[color], moveTo ), "unknown software issue - at the moveto position is a fish of the same color.");
	_ASSERT_EXPR( ! BitManager::IsBit(positionData[2], moveTo ), "unknown software issue - at the moveto position is a crake.");

	BitManager::ClearBit(positionData[color], moveFrom );
	BitManager::SetBit(positionData[color], moveTo );
	if ( BitManager::IsBit(positionData[(color+1)%2], moveTo ) )
		// remove opposite color fish if available
		BitManager::ClearBit(positionData[(color+1)%2], moveTo );

}
