#ifndef __MASC_H__
#define __MASC_H__

#include "int128.h"


int128 m_moveDist[100][4];
int128 m_field[100];
int128 m_fieldNeighbors[100][9];
int128 m_fieldDistanceSearch[100][7];
int128 *m_fieldSkipCheck[100][8];
int128 m_blocker[100][100];
int128 fishValueMasks[5];

unsigned char m_blockingFields[100][100];
unsigned char m_fieldNeighborIDs[100][9];
unsigned char m_maxMoveDistance[100][8];
unsigned char m_positionValue[100];
unsigned char m_distance[100][100];


void initMasks();

#endif