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
#include "SuperPackedMove.h"

class GameEndException
{
	double res;
public:
	GameEndException(double Result);
	int GetResult();
};

typedef struct
{
	int x; 
	int y;
} point;

typedef struct
{
	boardpane bp;
	int lowX;
	int lowY;

} swarmInfo;



class BoardManager {
public:

	static int GetNextRightBitPos(boardpane BP, int currentPos);

	static void Copy(board Source, board Destination);
	static void FromString(std::string Init, board &B);
	static std::string ToString( board B);

	static long blockValue(boardpane);
	static unsigned long long quickBoardPaneValue(boardpane BP);
	static void ExtendBlock(boardpane posData, boardpane block, uint64_t newFishesLow, uint64_t newFishesHigh, int firstFishPos);
	static int GetBlockAndCnt(boardpane posData, boardpane blockList[], int& fishCnt);

	static double Analysis(int moveCnt, boardpane blockListAll[][16], int blockCount[], board pos, int FirstDepth);

	static float GetValue(board pos, int color, boardpane blockList[][16], int blockCnt[], int depth, int firstMoveDepth, bool& gameEnd, int distance);
	static float quick_rateState(board b, int turn, int turningColor, bool & gameEnd);
	static double board_rateState(board b, int turn, int turningColor, bool & gameEnd);
	static std::string AnalysisToString(double val);
	static void SetBoardValue(board b, int x, int y, int color);

	static int GetDistance(board pos, int color, boardpane bestBlock);

	static void Outline(boardpane pos, boardpane res);
private:
	static long GetValueOfBlocks(board pos, int color, boardpane blockList[][16], int blockCnt[], int& fishCount);
};
#endif /* BOARDMANAGER_H_ */
