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
typedef coordinates mov[2];
struct superPackedMove
{
	unsigned int isSuperPackedMove : 1, packedMove : 31;
};

//
class MoveManager {
public:
	static packedMove getMove(int color, board SourceBoard, board DestinationBoard);
	static packedMove PackMove(mov M);
	static packedMove PackMove(coordinates CoordFrom, coordinates CoordTo);

	static void UnpackMove(packedMove PM, mov& M);

	static std::string CoordinatesToString(coordinates Coord);
    static void CoordinatesToXY(coordinates Coord, int* x, int* y);
	static std::string packMoveToString(packedMove move);

	inline static int moveValue(superPackedMove SPM );
	inline static int moveValue(int fromX, int fromY, int toX, int toY );
	inline static int moveValue(packedMove PM);
	static superPackedMove superPackMove( coordinates fromCoord, coordinates toCoord );
	
};

#endif /* MOVEMANAGER_H_ */
