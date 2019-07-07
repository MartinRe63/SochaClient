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

	static inline uint64_t BitCount(uint64_t u)
	{
		return __popcnt64(u);
	}
	static inline uint64_t BitCnt(uint64_t pos[])
	{
		return __popcnt64(pos[0]) + __popcnt64(pos[1]);
	}

	static inline bool AndIsNotNull(uint64_t v1[2], uint64_t v2[2], uint64_t res[2])
	{
		return ( ( res[0] = (v1[0] & v2[0]) ) || ( res[1] = (v1[1] & v1[1]) ) );
	}

	static void ClearBit( uint64_t pos[], int bitId );
	static bool IsBit( uint64_t pos[], int bitId );

	static int NumberOfTrailingZeros(uint64_t low, uint64_t high);
	static unsigned long GetFirstRightBitPos(uint64_t low, uint64_t high);
	static unsigned long GetNextRightBitPos(uint64_t low, uint64_t high, int currentPos);
	static unsigned long GetNextRightBitPosIgnorePrevious(uint64_t low, uint64_t high, int currentPos);
};

#endif /* BITMANAGER_H_ */
