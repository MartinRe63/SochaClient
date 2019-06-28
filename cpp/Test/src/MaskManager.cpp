/*
 * MaskManager.cpp
 *
 *  Created on: 28.05.2019
 *      Author: mrenneke
 */
#include <algorithm>
#include <string>
#include "MaskManager.h"
#include "BitManager.h"

uint64_t MaskManager::directionMasks[4][100][2];
uint64_t MaskManager::moveMasks[8][10][100][2];
uint64_t MaskManager::neighborMasks[100][2];
uint64_t MaskManager::fishValueMasks[5][2];

int128 MaskManager::m_moveDist[100][4];
int128 MaskManager::m_field[100];
int128 MaskManager::m_fieldNeighbors[100][9];
int128 MaskManager::m_fieldDistanceSearch[100][7];
int128 *MaskManager::m_fieldSkipCheck[100][8];
int128 MaskManager::m_blocker[100][100];
unsigned char MaskManager::MaskManager::m_blockingFields[100][100];
unsigned char MaskManager::m_fieldNeighborIDs[100][9];
unsigned char MaskManager::m_maxMoveDistance[100][8];
unsigned char MaskManager::m_positionValue[100];

unsigned char MaskManager::m_distance[100][100];

void MaskManager::initMasks(void) {
	int xMove = 0;
	int yMove = 0;
	for ( int dir = 0; dir < 4; dir++ )
	{
		if ( dir == 0 ) { xMove = 1; yMove = 0; }
		if ( dir == 1 ) { xMove = 1; yMove = 1; }
		if ( dir == 2 ) { xMove = 0; yMove = 1; }
		if ( dir == 3 ) { xMove = -1; yMove = 1; }
		for ( int y = 0; y < 10; y++)
		{
			for ( int x = 0; x < 10; x++)
			{
				uint64_t lowMask = 0;
				uint64_t highMask = 0;
				for ( int corr = 1; corr > -2; corr-= 2 )
				{
					int nextX = x;
					int nextY = y;
					int bitCnt = 0;
					int moveCnt = 0;
					uint64_t lowMoveMask = 0;
					uint64_t highMoveMask = 0;
					while ( nextX < 10 && nextX >= 0 && nextY < 10 && nextY >= 0 )
					{
						bitCnt = nextX + nextY * 10;
						lowMask |= (bitCnt < 64 ? (1ULL << bitCnt ) : 0);
						highMask |= (bitCnt >= 64 ? ( 1ULL << (bitCnt - 64)) : 0);

						if ( moveCnt > 0 )
						{
							lowMoveMask |= (bitCnt < 64 ? (1ULL << bitCnt ) : 0);
							highMoveMask |= (bitCnt >= 64 ? ( 1ULL << (bitCnt - 64)) : 0);
							moveMasks[corr == 1 ? dir : dir + 4][moveCnt][y*10+x][0] |= lowMoveMask;
							moveMasks[corr == 1 ? dir : dir + 4][moveCnt][y*10+x][1] |= highMoveMask;
							// System.out.println("dir=" + (corr == 1 ? dir : dir + 4) + " moveCnt=" + moveCnt + " coord=" + (y*10+x) + ">" + int64_t.toBinaryString(highMoveMask) + " " + int64_t.toBinaryString(lowMoveMask));
							if (moveCnt == 1)
							{
								neighborMasks[y*10+x][0] |= lowMoveMask;
								neighborMasks[y*10+x][1] |= highMoveMask;
								// System.out.println("dir=" + (corr == 1 ? dir : dir + 4) + " coord=" + (y*10+x) + ">" + int64_t.toBinaryString(neighborMasks[y*10+x][1]) + " " + int64_t.toBinaryString(neighborMasks[y*10+x][0]));
							}
						}

						nextX += xMove * corr;
						nextY += yMove * corr;
						moveCnt++;
					}
				}
				directionMasks[dir][y*10+x][0] = lowMask;
				directionMasks[dir][y*10+x][1] = highMask;
				// System.out.println("dir=" + dir + " coord=" + (y*10+x) + ">" + int64_t.toBinaryString(highMask) + " " + int64_t.toBinaryString(lowMask));
			}
		}
	}
	for ( int y= 0; y < 10; y++)
	{
		for ( int x = 0; x < 10; x++)
		{
			int val = __min(x >= 5 ? 9-x : x, y >= 5 ? 9-y : y);
			BitManager::SetBit(fishValueMasks[val], y*10+x);
		}
	}


	initMasksBlubber();
}

std::string MaskManager::initGame() {
	std::string field =
	  (string)".11111111." +
	  "0........0" +
	  "0........0" +
	  "0........0" +
	  "0...C....0" +
	  "0....C...0" +
	  "0........0" +
	  "0........0" +
	  "0........0" +
	  ".11111111.";
	return field;
}


enum {
	dir_RIGHT,
	dir_UP_RIGHT,
	dir_UP,
	dir_UP_LEFT,
	dir_LEFT,
	dir_DOWN_LEFT,
	dir_DOWN,
	dir_DOWN_RIGHT
};

#define Max std::max
#define Min std::min

int128 int128_or(int128 v1, int128 v2) {
	int128 r;
	r.v0 = v1.v0 | v2.v0;
	r.v1 = v1.v1 | v2.v1;
	return r;
}
int128 int128_xor(int128 v1, int128 v2) {
	int128 r;
	r.v0 = v1.v0 ^ v2.v0;
	r.v1 = v1.v1 ^ v2.v1;
	return r;
}
int128 int128_and(int128 v1, int128 v2) {
	int128 r;
	r.v0 = v1.v0 & v2.v0;
	r.v1 = v1.v1 & v2.v1;
	return r;
}
int128 int128_not(int128 v1) {
	int128 r;
	r.v0 = ~v1.v0;
	r.v1 = ~v1.v1;
	return r;
}
int int128_popcnt(int128 v) {
#ifndef _WIN
	return __builtin_popcountll(v.v0) + __builtin_popcountll(v.v1);
#else
	return __popcnt64(v.v0) + __popcnt64(v.v1);
#endif
}
int128 int128_setBit(int index) {
	int128 r;
	r.v0 = 0;
	r.v1 = 0;
	if (index < 64) {
		r.v0 = (unsigned long long)1 << index;
	}
	else {
		r.v1 = (unsigned long long)1 << (index - 64);
	}
	return r;
}
int int128_ffs(int128 value) {
	int id;
#ifndef _WIN
	if ((id = __builtin_ffsll(value.v0)))
		return id;
	else if ((id = __builtin_ffsll(value.v1)))
		return id + 64;
#else
	unsigned long ret = 0;
	unsigned char res = _BitScanForward64(&ret, value.v0); // Long.numberOfTrailingZeros(low);
	if (res == 0)
	{
		res = _BitScanForward64(&ret, value.v1);
		if (res == 0)
			return 0;
		else
			ret += 64;
		return ret + 1;
	}
#endif
	return ret + 1;
}
int int128_getBit(int128 value, int id) {
	if (id < 64) {
		return (value.v0 >> id) & 1;
	}
	else {
		return (value.v1 >> (id - 64)) & 1;
	}
}
int int128_notNull(int128 data) {
	return (data.v0 != 0) || (data.v1 != 0);
}

void initMoveDist() {
	memset(MaskManager::m_moveDist, 0, sizeof(MaskManager::m_moveDist));
	for (int y = 0; y < 10; y++) {
		for (int x = 0; x < 10; x++) {
			for (int i = 0; i < 10; i++) {
				MaskManager::m_moveDist[y * 10 + x][dir_RIGHT / 2] = int128_or(MaskManager::m_moveDist[y * 10 + x][dir_RIGHT / 2], int128_setBit(y * 10 + i));
			}
			for (int i = 0; i < 10; i++) {
				MaskManager::m_moveDist[y * 10 + x][dir_UP / 2] = int128_or(MaskManager::m_moveDist[y * 10 + x][dir_UP / 2], int128_setBit(i * 10 + x));
			}
			int k = y - x;
			for (int i = Max(-k, 0); i <= Min(9, 9 - k); i++) {
				MaskManager::m_moveDist[y * 10 + x][dir_UP_RIGHT / 2] = int128_or(MaskManager::m_moveDist[y * 10 + x][dir_UP_RIGHT / 2], int128_setBit((y - x + i) * 10 + i));
			}
			k = y + x;
			//0 = -x + k;
			//9 = -x + k;
			for (int i = Max(k - 9, 0); i <= Min(9, k); i++) {
				MaskManager::m_moveDist[y * 10 + x][dir_UP_LEFT / 2] = int128_or(MaskManager::m_moveDist[y * 10 + x][dir_UP_LEFT / 2], int128_setBit((y + x - i) * 10 + i));
			}
		}
	}
}

void initField() {
	for (int y = 0; y < 10; y++) {
		for (int x = 0; x < 10; x++) {
			MaskManager::m_field[y * 10 + x] = int128_setBit(y * 10 + x);
		}
	}
}
//Das sollte man nicht so machen!!!

void initSkipCheck() {
	for (int y = 0; y < 10; y++) {
		for (int x = 0; x < 10; x++) {
			MaskManager::m_maxMoveDistance[y * 10 + x][dir_DOWN] = y;
			MaskManager::m_maxMoveDistance[y * 10 + x][dir_LEFT] = x;
			MaskManager::m_maxMoveDistance[y * 10 + x][dir_RIGHT] = 9 - x;
			MaskManager::m_maxMoveDistance[y * 10 + x][dir_UP] = 9 - y;

			MaskManager::m_maxMoveDistance[y * 10 + x][dir_UP_LEFT] = Min(MaskManager::m_maxMoveDistance[y * 10 + x][dir_UP], MaskManager::m_maxMoveDistance[y * 10 + x][dir_LEFT]);
			MaskManager::m_maxMoveDistance[y * 10 + x][dir_DOWN_LEFT] = Min(MaskManager::m_maxMoveDistance[y * 10 + x][dir_DOWN], MaskManager::m_maxMoveDistance[y * 10 + x][dir_LEFT]);
			MaskManager::m_maxMoveDistance[y * 10 + x][dir_DOWN_RIGHT] = Min(MaskManager::m_maxMoveDistance[y * 10 + x][dir_DOWN], MaskManager::m_maxMoveDistance[y * 10 + x][dir_RIGHT]);
			MaskManager::m_maxMoveDistance[y * 10 + x][dir_UP_RIGHT] = Min(MaskManager::m_maxMoveDistance[y * 10 + x][dir_UP], MaskManager::m_maxMoveDistance[y * 10 + x][dir_RIGHT]);
		}
	}
	for (int y = 0; y < 10; y++) {
		for (int x = 0; x < 10; x++) {
			for (int i = 0; i < 8; i++) {
				int mmd = MaskManager::m_maxMoveDistance[y * 10 + x][i];
				if (mmd >= 1) {
					MaskManager::m_fieldSkipCheck[y * 10 + x][i] = (int128*) malloc(sizeof(int128) * (mmd));
					if (MaskManager::m_fieldSkipCheck[y * 10 + x][i] == 0) {
						printf("Out of Memory!\n");
						exit(-1);
					}
					memset(MaskManager::m_fieldSkipCheck[y * 10 + x][i], 0, sizeof(int128) * (mmd));
					MaskManager::m_fieldSkipCheck[y * 10 + x][i][0] = int128_setBit((y + dirVectors[i][1]) * 10 + x + dirVectors[i][0]);
					for (int j = 2; j < mmd + 1; j++) {
						MaskManager::m_fieldSkipCheck[y * 10 + x][i][j - 1] = int128_or(MaskManager::m_fieldSkipCheck[y * 10 + x][i][j - 2], int128_setBit((y + dirVectors[i][1] * j) * 10 + x + dirVectors[i][0] * j));
					}
				}
			}
		}
	}
}

void initFielNeighborIDs() {
	for (int y = 0; y < 10; y++) {
		for (int x = 0; x < 10; x++) {
			unsigned char count = 0;
			for (int i = 0; i < 8; i++) {
				int tx = x + dirVectors[i][0];
				int ty = y + dirVectors[i][1];
				if (tx >= 0 && tx < 10 && ty >= 0 && ty < 10) {
					MaskManager::m_fieldNeighborIDs[y * 10 + x][count + 1] = (unsigned char)(ty * 10 + tx);
					count++;
				}
			}
			MaskManager::m_fieldNeighborIDs[y * 10 + x][0] = count;
		}
	}
}

int inbound(int x, int y) {
	return x >= 0 && x < 10 && y >= 0 && y < 10;
}

void initFieldNeighbors() {
	for (int y = 0; y < 10; y++) {
		for (int x = 0; x < 10; x++) {
			for (int i = 0; i < 9; i++) {
				MaskManager::m_fieldNeighbors[y * 10 + x][i].v0 = 0;
				MaskManager::m_fieldNeighbors[y * 10 + x][i].v1 = 0;
				for (int ox = -i; ox <= i; ox++) {
					int tmpX = x + ox;
					int tmpY = y + i + 1;
					if (inbound(tmpX, tmpY)) {
						MaskManager::m_fieldNeighbors[y * 10 + x][i] = int128_or(MaskManager::m_fieldNeighbors[y * 10 + x][i], MaskManager::m_field[tmpY * 10 + tmpX]);
					}
					tmpY = y - i - 1;
					if (inbound(tmpX, tmpY)) {
						MaskManager::m_fieldNeighbors[y * 10 + x][i] = int128_or(MaskManager::m_fieldNeighbors[y * 10 + x][i], MaskManager::m_field[tmpY * 10 + tmpX]);
					}
				}
				for (int oy = -i - 1; oy <= i + 1; oy++) {
					int tmpX = x + i + 1;
					int tmpY = y + oy;
					if (inbound(tmpX, tmpY)) {
						MaskManager::m_fieldNeighbors[y * 10 + x][i] = int128_or(MaskManager::m_fieldNeighbors[y * 10 + x][i], MaskManager::m_field[tmpY * 10 + tmpX]);
					}
					tmpX = x - i - 1;
					if (inbound(tmpX, tmpY)) {
						MaskManager::m_fieldNeighbors[y * 10 + x][i] = int128_or(MaskManager::m_fieldNeighbors[y * 10 + x][i], MaskManager::m_field[tmpY * 10 + tmpX]);
					}
				}
				/*if ((x == 0 && y == 5) || (x == 9 && y == 0) || (x == 0 && y == 0) || (x == 3 && y == 4)) {
					printf("%d, %d, %d\n", x, y, i);
					int128_debugWrite(MaskManager::m_fieldNeighbors[y * 10 + x][i]);
				}*/
			}
		}
	}
}

void initFieldDistanceSearch() {
	for (int y = 0; y < 10; y++) {
		for (int x = 0; x < 10; x++) {
			memset(MaskManager::m_fieldDistanceSearch, 0, sizeof(int128) * 7);
			for (int i = 1; i < 5; i++) {
				MaskManager::m_fieldDistanceSearch[y * 10 + x][0] = int128_or(MaskManager::m_fieldDistanceSearch[y * 10 + x][0], MaskManager::m_fieldNeighbors[y * 10 + x][i]);
			}
			for (int i = 1; i < 3; i++) {
				MaskManager::m_fieldDistanceSearch[y * 10 + x][1] = int128_or(MaskManager::m_fieldDistanceSearch[y * 10 + x][1], MaskManager::m_fieldNeighbors[y * 10 + x][i]);
			}
			for (int i = 5; i < 7; i++) {
				MaskManager::m_fieldDistanceSearch[y * 10 + x][2] = int128_or(MaskManager::m_fieldDistanceSearch[y * 10 + x][2], MaskManager::m_fieldNeighbors[y * 10 + x][i]);
			}
			MaskManager::m_fieldDistanceSearch[y * 10 + x][3] = MaskManager::m_fieldNeighbors[y * 10 + x][1];
			MaskManager::m_fieldDistanceSearch[y * 10 + x][4] = MaskManager::m_fieldNeighbors[y * 10 + x][3];
			MaskManager::m_fieldDistanceSearch[y * 10 + x][5] = MaskManager::m_fieldNeighbors[y * 10 + x][5];
			MaskManager::m_fieldDistanceSearch[y * 10 + x][6] = MaskManager::m_fieldNeighbors[y * 10 + x][7];

		}
	}
}

void initBlocker() {
	for (int y1 = 0; y1 < 10; y1++) {
		for (int x1 = 0; x1 < 10; x1++) {
			for (int y2 = 0; y2 < 10; y2++) {
				for (int x2 = 0; x2 < 10; x2++) {
					int dirDiagonal = 0;
					int dirStraight = 0;
					//If Horizontal
					if (abs(x1 - x2) > abs(y1 - y2)) {
						//Right
						if (x2 > x1) {
							dirStraight = dir_RIGHT;
						}
						else {
							dirStraight = dir_LEFT;
						}
					}//Vertical
					else {
						//UP
						if (y2 > y1) {
							dirStraight = dir_UP;
						}
						else {
							dirStraight = dir_DOWN;
						}
					}
					//UP_Right
					if (x2 > x1 && y2 > y1) {
						dirDiagonal = dir_UP_RIGHT;
					}//BOT_Right
					else if (x2 > x1 && y2 <= y1) {
						dirDiagonal = dir_DOWN_RIGHT;
					}//BOT_LEFT
					else if (x2 <= x1 && y2 <= y1) {
						dirDiagonal = dir_DOWN_LEFT;
					}//TOP_LEFT
					else if (x2 <= x1 && y2 > y1) {
						dirDiagonal = dir_UP_LEFT;
					}
					int cx1 = x1;
					int cy1 = y1;
					int cx2 = cx1;
					int cy2 = cy1;
					MaskManager::m_blocker[y1 * 10 + x1][y2 * 10 + x2].v0 = 0;
					MaskManager::m_blocker[y1 * 10 + x1][y2 * 10 + x2].v1 = 0;
					int d = 0;
					int s = 0;
					while (cx2 != x2 || cy2 != y2) {
						cx2 = cx1;
						cy2 = cy1;
						d = 0;
						while (cx2 != x2 && cy2 != y2) {
							MaskManager::m_blocker[y1 * 10 + x1][y2 * 10 + x2] = int128_or(MaskManager::m_blocker[y1 * 10 + x1][y2 * 10 + x2], MaskManager::m_field[cy2 * 10 + cx2]);
							d++;
							cx2 += dirVectors[dirDiagonal][0];
							cy2 += dirVectors[dirDiagonal][1];
						}
						MaskManager::m_blocker[y1 * 10 + x1][y2 * 10 + x2] = int128_or(MaskManager::m_blocker[y1 * 10 + x1][y2 * 10 + x2], MaskManager::m_field[cy2 * 10 + cx2]);
						d++;
						s++;
						cx1 += dirVectors[dirStraight][0];
						cy1 += dirVectors[dirStraight][1];
					}
					//s++;
					MaskManager::m_blockingFields[y1 * 10 + x1][y2 * 10 + x2] = (unsigned char)Min(s, d);
					//assert(MaskManager::m_blockingFields[y1 * 10 + x1][y2 * 10 + x2]);
					/*if((y1 * 10 + x1 == 34 && y2 * 10 + x2 == 59) ||
							(y1 * 10 + x1 == 34 && y2 * 10 + x2 == 74) ||
							(y1 * 10 + x1 == 34 && y2 * 10 + x2 == 30) ||
							(y1 * 10 + x1 == 34 && y2 * 10 + x2 == 67)){
						printf("X1: %d, Y1: %d, X2: %d, Y2: %d\nFieldCount: %d\n", x1, y1, x2, y2, MaskManager::m_blockingFields[y1 * 10 + x1][y2 * 10 + x2]);
						int128_debugWrite(MaskManager::m_blocker[y1 * 10 + x1][y2 * 10 + x2]);
						printf("\n");
					}*/
				}
			}
		}
	}
}

void initPositionValue() {
	for (int y = 0; y < 10; y++) {
		for (int x = 0; x < 10; x++) {
			MaskManager::m_positionValue[y * 10 + x] = (unsigned char)((y <= 4 ? y + 1 : 10 - y) * (x <= 4 ? x + 1 : 10 - x)) + 1;
		}
	}
}

void initDistance() {
	for (int f1 = 0; f1 < 100; f1++) {
		for (int f2 = 0; f2 < 100; f2++) {
			MaskManager::m_distance[f1][f2] = Max(abs(f1 / 10 - f2 / 10), abs(f1 % 10 - f2 % 10));
			MaskManager::m_distance[f1][f2] = MaskManager::m_distance[f1][f2] * MaskManager::m_distance[f1][f2];
		}
	}
}

void MaskManager::initMasksBlubber() {
	initMoveDist();
	initSkipCheck();
	initField();
	initFielNeighborIDs();
	initFieldNeighbors();
	initPositionValue();
	initFieldDistanceSearch();
	initBlocker();
	initDistance();
}


