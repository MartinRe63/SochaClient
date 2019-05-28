/*
 * BitManager.h
 *
 *  Created on: 28.05.2019
 *      Author: mrenneke
 */

#ifndef BITMANAGER_H_
#define BITMANAGER_H_

#include<stdint.h>

class BitManager {

public:
	static void SetBit( int64_t pos[], int bitId );
	static int BitCnt( int64_t pos[] );
	static void ClearBit( int64_t pos[], int bitId );
	static bool IsBit( int64_t pos[], int bitId );

};

#endif /* BITMANAGER_H_ */
