/*
 * BitManager.cpp
 *
 *  Created on: 28.05.2019
 *      Author: mrenneke
 */
#include <stdint.h>
#include "BitManager.h"


void BitManager::SetBit( int64_t pos[], int bitId )
{
	if ( bitId < 64 )
		pos[0] |= ( 1L << bitId );
	else
		pos[1] |= ( 1L << ( bitId-64 ) );
}

int BitManager::BitCnt(int64_t pos[])
{
	return __builtin_popcountll (pos[0]) + __builtin_popcountll(pos[1]);
}

void BitManager::ClearBit( int64_t pos[], int bitId )
{
	if ( bitId < 64 )
		pos[0] &= ~( 1L << bitId );
	else
		pos[1] &= ~( 1L << ( bitId-64 ) );
}
bool BitManager::IsBit( int64_t pos[], int bitId )
{
	if ( bitId < 64 )
		return ( ( pos[0] & ( 1L << bitId ) ) ) != 0;
	else
		return ( ( pos[1] & ( 1L << bitId ) ) ) != 0;
}




