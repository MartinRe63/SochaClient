/*
 * BitManager.h
 *
 *  Created on: 28.05.2019
 *      Author: mrenneke
 */

#ifndef MASKMANAGER_H_
#define MASKMANAGER_H_


#include <string>


using namespace std;
class MaskManager
{
	//
	// Die Move Masken haben eine doppelte Nutzung
	// 1. Zur Prüfung ob in die jeweilige Raumrichtung gegnerische Steine oder Kraken sind (Use MoveLenght-1)
	// 2. Zur Prüfung ob am Zielpunkt ein gegnerischer Stein ist, der entfernt werden muss (Einfach ausschalten - Ob dort ein Stein ist, ist unwichtig)
public:
	static int64_t directionMasks[4][100][2];
	static int64_t moveMasks[8][10][100][2]; // = new int64_t [8][10][100][2]; // DIMENSIONS - Direction, MoveLength, Positionen, High/Low Bits

	static int64_t neighborMasks[100][2]; //  = new int64_t [100][2];

	// A fish at 0,x or x,0 has a value 0
	// A fish at 1,x or x,1 has a value 1
	static int64_t fishValueMasks[5][2]; // = new int64_t[5][2];  // Dimension - value, bitmask

	static void initMasks();

	static std::string initGame();
};

#endif /* MASKMANAGER_H_ */
