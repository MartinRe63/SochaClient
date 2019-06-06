/*
 * BoardManager.h
 *
 *  Created on: 29.05.2019
 *      Author: mrenneke
 */

#ifndef BOARDMANAGER_H_
#define BOARDMANAGER_H_

#include <string>
#include <stdint.h>
#include <intrin.h>
#include <crtdbg.h>
#include "BitManager.h"


typedef uint64_t boardpane[2];
typedef boardpane board[3];


class GameEndException
{
	double res;
public:
	GameEndException(double Result);
	int GetResult();
};

class BoardManager {
public:

	static int GetNextRightBitPos(boardpane BP, int currentPos);

	static void Copy(board Source, board Destination);
	static void FromString(std::string Init, board &B);
	static std::string ToString( board B);

	static long blockValue(boardpane);
	static void ExtendBlock(boardpane posData, boardpane block, uint64_t newFishesLow, uint64_t newFishesHigh, int firstFishPos);
	static int GetBlockAndCnt(boardpane posData, boardpane blockList[]);

	static double Analysis(int moveCnt, boardpane blockListAll[][16], int blockCount[], board pos, int MaxDepth);

	static double GetValue(board pos, int color, boardpane blockList[][16], int blockCnt[], int depth, int firstMoveDepth);
	static std::string AnalysisToString(double val);
private:
	static long GetPosValue(board pos, int color, boardpane blockList[][16], int blockCnt[]);

};


#endif /* BOARDMANAGER_H_ */
