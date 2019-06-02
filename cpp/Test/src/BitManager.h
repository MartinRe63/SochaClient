/*
 * BitManager.h
 *
 *  Created on: 28.05.2019
 *      Author: mrenneke
 */

#ifndef BITMANAGER_H_
#define BITMANAGER_H_

#include <stdint.h>
#include "BoardManager.h"
class BitManager {

public:
	static void SetBit( uint64_t pos[], int bitId );

	static int BitCnt(uint64_t pos[])
	{
		return __popcnt64(pos[0]) + __popcnt64(pos[1]);
	}

	static void ClearBit( uint64_t pos[], int bitId );
	static bool IsBit( uint64_t pos[], int bitId );

	static int NumberOfTrailingZeros(uint64_t low, uint64_t high);
	static int GetFirstRightBitPos(long low, long high);
	static int GetNextRightBitPos(long low, long high, int currentPos);
	static int GetNextRightBitPosIgnorePrevious(long low, long high, int currentPos);
};

#endif /* BITMANAGER_H_ */
