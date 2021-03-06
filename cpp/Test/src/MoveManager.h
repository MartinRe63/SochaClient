/*
 * MoveManager.h
 *
 *  Created on: 29.05.2019
 *      Author: mrenneke
 */

#ifndef MOVEMANAGER_H_
#define MOVEMANAGER_H_
#include "BoardManager.h"
#include "SuperPackedMove.h"


typedef unsigned packedMove;
typedef int coordinates;
typedef coordinates mov[2];


//
class MoveManager {
public:
	static packedMove getMove(int color, board SourceBoard, board DestinationBoard);
	static packedMove PackMove(mov M);
	static packedMove PackMove(coordinates CoordFrom, coordinates CoordTo);

	static void UnpackMove( packedMove PM, mov& M );
	static void UnpackMove( superPackedMove SPM, mov& M );

	static std::string CoordinatesToString(coordinates Coord);
    static inline void CoordinatesToXY(coordinates Coord, int* x, int* y);
	static std::string packMoveToString(packedMove move);

	static int moveValue(int fromX, int fromY, int toX, int toY);
	static int moveValueSPM(superPackedMove sPM);
	static int moveValuePM(packedMove PM);
	static int moveNeigborValue(const boardpane BP, mov m);

	static superPackedMove superPackMove(coordinates fromPos, coordinates toPos);

	static superPackedMove superPackMove(packedMove PM);
	static packedMove superPack2packMove( superPackedMove SPM );

	static coordinates movePossible( int x, int y, int dir, int lth, const board positionData, int color, boardpane superlong );
	static void addMoveToBoard(board positionData, int color, mov move);
	static void addMoveToBoard( board positionData, int color, packedMove move );
	
	static int FishCountInDirection( coordinates c, int dir, board positionData );
	static packedMove LastMove2packedMove( LastMove LM, board positionData );
	static LastMove PackedMove2LastMove( packedMove pM );
	static int getMoveList( const board positionData, int color, packedMove moves[], int& distance );
	static void MarkPotentialMoves( int fishPos, board pos, int color, boardpane reachedPositions, boardpane stopPositions, boardpane newMoves, bool& stopFound );

};

#endif /* MOVEMANAGER_H_ */
