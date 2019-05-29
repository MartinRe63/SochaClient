/*
 * MaskManager.cpp
 *
 *  Created on: 28.05.2019
 *      Author: mrenneke
 */
#include <string>
#include "MaskManager.h"
#include "BitManager.h"

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
						lowMask |= (bitCnt < 64 ? (1L << bitCnt ) : 0);
						highMask |= (bitCnt >= 64 ? ( 1L << (bitCnt - 64)) : 0);

						if ( moveCnt > 0 )
						{
							lowMoveMask |= (bitCnt < 64 ? (1L << bitCnt ) : 0);
							highMoveMask |= (bitCnt >= 64 ? ( 1L << (bitCnt - 64)) : 0);
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
				directionMasks[dir][y*10+x][0] = (long)lowMask;
				directionMasks[dir][y*10+x][1] = (long)highMask;
				// System.out.println("dir=" + dir + " coord=" + (y*10+x) + ">" + int64_t.toBinaryString(highMask) + " " + int64_t.toBinaryString(lowMask));
			}
		}
	}
	for ( int y= 0; y < 10; y++)
	{
		for ( int x = 0; x < 10; x++)
		{
			int val = std::min(x >= 5 ? 9-x : x, y >= 5 ? 9-y : y);
			BitManager::SetBit(fishValueMasks[val], y*10+x);
		}
	}
}

std::string MaskManager::initGame() {
	string field =
	  (string)".11111111." +
	  "0........0" +
	  "0........0" +
	  "0........0" +
	  "0...X....0" +
	  "0....X...0" +
	  "0........0" +
	  "0........0" +
	  "0........0" +
	  ".11111111.";
	return field;
}



