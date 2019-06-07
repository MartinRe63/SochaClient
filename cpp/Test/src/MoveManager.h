/*
 * MoveManager.h
 *
 *  Created on: 29.05.2019
 *      Author: mrenneke
 */

#ifndef MOVEMANAGER_H_
#define MOVEMANAGER_H_
#include "BoardManager.h"

typedef unsigned packedMove;
typedef int coordinates;
typedef coordinates move[2];

//
class MoveManager {
public:
	static packedMove getMove(int color, board SourceBoard, board DestinationBoard);
	static packedMove PackMove(move M);
	static packedMove PackMove(coordinates CoordFrom, coordinates CoordTo);

	static void UnpackMove(packedMove PM, move& M);

	static std::string CoordinatesToString(coordinates Coord);
    static void CoordinatesToXY(coordinates Coord, int* x, int* y);
	static std::string packMoveToString(packedMove move);

	static int moveValue(packedMove PM);
	
};

#endif /* MOVEMANAGER_H_ */
