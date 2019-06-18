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

#pragma intrinsic(_BitScanForward64)


void BitManager::SetBit( uint64_t pos[], int bitId )
{
	if ( bitId < 64 )
		pos[0] |= ( 1ULL << bitId );
	else
		pos[1] |= ( 1ULL << ( bitId-64 ) );
}


void BitManager::ClearBit( uint64_t pos[], int bitId )
{
	if ( bitId < 64 )
		pos[0] &= ~( 1ULL << bitId );
	else
		pos[1] &= ~( 1ULL << ( bitId-64 ) );
}
bool BitManager::IsBit( uint64_t pos[], int bitId )
{
	if ( bitId < 64 )
		return ( ( pos[0] & ( 1ULL << bitId ) ) ) != 0;
	else
		return ( ( pos[1] & ( 1ULL << bitId ) ) ) != 0;
}


int BitManager::NumberOfTrailingZeros(uint64_t low, uint64_t high)
{
	unsigned long ret = 0;
	unsigned char res = _BitScanForward64( &ret, low); // Long.numberOfTrailingZeros(low);
	if (res == 0)
	{
		res = _BitScanForward64( &ret, high);
		if (res == 0)
			ret += 64;
		else
			ret = 128;
	}
	return ret;
}

unsigned long BitManager::GetFirstRightBitPos(uint64_t low, uint64_t high)
{
	unsigned long ret = 0;
	unsigned char res = _BitScanForward64( &ret, low); // Long.numberOfTrailingZeros(low);
	if (res == 0)
	{
		res = _BitScanForward64(&ret, high);
		if (res == 0)
			ret = 128;
		else
			ret += 64;
	}
	return ret;
}
unsigned long BitManager::GetNextRightBitPos(uint64_t low, uint64_t high, int currentPos)
{
	if (currentPos >= 64)
	{
		high &= ~(1ULL << (currentPos - 64));  // switch of current pos in copied data
	}
	else
	{
		low &= ~(1ULL << currentPos);
	}
	unsigned long ret = GetFirstRightBitPos(low, high);
	_ASSERT_EXPR(ret >= currentPos, "bit found before " + currentPos);
	return (ret);
}
unsigned long BitManager::GetNextRightBitPosIgnorePrevious(uint64_t low, uint64_t high, int currentPos)
{
	if (currentPos >= 63) {
		high &= ~((1ULL << (currentPos - 63))-1);
		low = 0;
	}
	else {
		low &= ~((1ULL << (currentPos + 1))-1);
	}
	return GetFirstRightBitPos(low, high);
}
