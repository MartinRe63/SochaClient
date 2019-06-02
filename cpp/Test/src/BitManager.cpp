/*
 * BitManager.cpp
 *
 *  Created on: 28.05.2019
 *      Author: mrenneke
 */
#include <stdint.h>
#include <Intrin.h>
#include <crtdbg.h>
#include "BitManager.h"


void BitManager::SetBit( uint64_t pos[], int bitId )
{
	if ( bitId < 64 )
		pos[0] |= ( 1L << bitId );
	else
		pos[1] |= ( 1L << ( bitId-64 ) );
}


void BitManager::ClearBit( uint64_t pos[], int bitId )
{
	if ( bitId < 64 )
		pos[0] &= ~( 1L << bitId );
	else
		pos[1] &= ~( 1L << ( bitId-64 ) );
}
bool BitManager::IsBit( uint64_t pos[], int bitId )
{
	if ( bitId < 64 )
		return ( ( pos[0] & ( 1L << bitId ) ) ) != 0;
	else
		return ( ( pos[1] & ( 1L << bitId ) ) ) != 0;
}

int BitManager::NumberOfTrailingZeros(uint64_t low, uint64_t high)
{
	unsigned long ret = 0;
	int res = _BitScanForward64( &ret, low); // Long.numberOfTrailingZeros(low);
	if (res == 0)
	{
		res = _BitScanForward64( &ret, high);
		ret += 64;
	}
	return ret;
}

int BitManager::GetFirstRightBitPos(long low, long high)
{
	return NumberOfTrailingZeros(low, high);
}
int BitManager::GetNextRightBitPos(long low, long high, int currentPos)
{
	_ASSERT_EXPR(GetFirstRightBitPos(low, high) >= currentPos, "bit found before " + currentPos);
	if (currentPos >= 64)
	{
		high &= ~(1L << (currentPos - 64));  // switch of current pos in copied data
	}
	else
	{
		low &= ~(1L << currentPos);
	}
	return (GetFirstRightBitPos(low, high));
}
int BitManager::GetNextRightBitPosIgnorePrevious(long low, long high, int currentPos)
{
	int p = GetFirstRightBitPos(low, high);
	// switch of current pos in copied data
	while (p < currentPos)
	{
		if (p >= 64) {
			high &= ~(1L << p - 64);
		}
		else {
			low &= ~(1L << p);
		}
		p = GetFirstRightBitPos(low, high);
	}
	if (p > currentPos)
		return p;
	else
		return GetNextRightBitPos(low, high, p);
}



