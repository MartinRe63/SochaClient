/*
 * BitManager.h
 *
 *  Created on: 28.05.2019
 *      Author: mrenneke
 */

#ifndef MASKMANAGER_H_
#define MASKMANAGER_H_

#include <string>
#include <stdint.h>

using namespace std;

typedef	struct {
	unsigned long long v0;
	unsigned long long v1;
} int128;

static int dirVectors[][2] = {
	{1, 0},
	{1, 1},
	{0, 1},
	{-1, 1},
	{-1, 0},
	{-1, -1},
	{0, -1},
	{1, -1}
};

class MaskManager
{
	//
	// Die Move Masken haben eine doppelte Nutzung
	// 1. Zur Prüfung ob in die jeweilige Raumrichtung gegnerische Steine oder Kraken sind (Use MoveLenght-1)
	// 2. Zur Prüfung ob am Zielpunkt ein gegnerischer Stein ist, der entfernt werden muss (Einfach ausschalten - Ob dort ein Stein ist, ist unwichtig)
public:
	static uint64_t directionMasks[4][100][2];
	static uint64_t moveMasks[8][10][100][2]; // = new int64_t [8][10][100][2]; // DIMENSIONS - Direction, MoveLength, Positionen, High/Low Bits

	static uint64_t neighborMasks[100][2]; //  = new int64_t [100][2];

	// A fish at 0,x or x,0 has a value 0
	// A fish at 1,x or x,1 has a value 1
	static uint64_t fishValueMasks[5][2]; // = new int64_t[5][2];  // Dimension - value, bitmask

	static int128 m_moveDist[100][4];
	static int128 m_field[100];
	static int128 m_fieldNeighbors[100][9];
	static int128 m_fieldDistanceSearch[100][7];
	static int128 *m_fieldSkipCheck[100][8];
	static int128 m_blocker[100][100];
	static unsigned char m_blockingFields[100][100];
	static unsigned char m_fieldNeighborIDs[100][9];
	static unsigned char m_maxMoveDistance[100][8];
	static unsigned char m_positionValue[100];
	static unsigned char m_distance[100][100];

	static void initMasks();

	static std::string initGame();
	static void initMasksBlubber();
};

#endif /* MASKMANAGER_H_ */
