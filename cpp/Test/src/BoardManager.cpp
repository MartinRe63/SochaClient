/*
 * BoardManager.cpp
 *
 *  Created on: 28.05.2019
 *      Author: mrenneke
 */
// #include <string>
#include <crtdbg.h>
#include <intrin.h>
#include "BoardManager.h"
#include "BitManager.h"
#include "MaskManager.h"


GameEndException::GameEndException(double Result) {
	res = Result;
};
int GameEndException::GetResult(void) {
	return res;
}


void BoardManager::Copy(board Source, board Destination)
{
	for ( int c = 0; c < 3; c++ )
		for ( int b = 0; b < 2; b++ )
			Destination[c][b] = Source[c][b];
};

void BoardManager::FromString(std::string Init, board &B)
{
	for (int y = 0; y < 10; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			std::string s = Init.substr((9 - y) * 10 + x, (9 - y) * 10 + x + 1);
			if (s == "0")
				BitManager::SetBit(B[0], y * 10 + x);
			else if (s == "1")
				BitManager::SetBit(B[1], y * 10 + x);
			else if (s == "C" )
				BitManager::SetBit(B[2], y * 10 + x);
		}
	}
}

std::string ToString(board B)
{
	std::string res = std::string(
		". . . . . . . . . . \r\n" ) +
		". . . . . . . . . . \r\n" +
		". . . . . . . . . . \r\n" +
		". . . . . . . . . . \r\n" +
		". . . . . . . . . . \r\n" +
		". . . . . . . . . . \r\n" +
		". . . . . . . . . . \r\n" +
		". . . . . . . . . . \r\n" +
		". . . . . . . . . . \r\n" +
		". . . . . . . . . . \r\n";
	;
	// char res1[240] = c_str(res);
	for (int y = 0; y < 10; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			if (BitManager::IsBit(B[0], y * 10 + x))
				res[(9 - y) * 22 + x * 2] = '0';
			else if (BitManager::IsBit(B[1], y * 10 + x))
				res[(9 - y) * 22 + x * 2] = '1';
			else if (BitManager::IsBit(B[2], y * 10 + x))
				res[(9 - y) * 22 + x * 2] = 'C';
		}
	}
	// res = new std::string(res1);
	return res;
}

long BoardManager::blockValue(boardpane BP)
{
	long ret = __popcnt64(BP[0]) + __popcnt64(BP[1]);
	int val = 4;
	while ((BP[0] & MaskManager::fishValueMasks[val][0]) == 0 && (BP[1] & MaskManager::fishValueMasks[val][1]) == 0)
		val--;
	long multiplier = val > 1 ? 8 : val;
	for (int i = 3; i <= val; i++) multiplier <<= 3;
	ret *= multiplier;
	return ret;
}


/**
 * Extend a block of fishes for a number of new fishes
 * @param posData      - posData fishes not in the block and not new for the current block
 * @param blockList    - fishes already considered for the block / 0 = lowFishes - 1 = highFishes - 2 = maxValue
 * @param newFishes    - new fishes for the current block
 * @return
 */
void BoardManager::ExtendBlock(boardpane posData, boardpane block, uint64_t newFishesLow, uint64_t newFishesHigh, int firstFishPos)
{
	_ASSERT_EXPR((newFishesLow != 0) || (newFishesHigh != 0), "Software Issue - no new fishes found." );
	uint64_t bitCnt = __popcnt64(newFishesLow) + __popcnt64(newFishesHigh);
	int bitPos = firstFishPos;
	for (uint64_t k = 0; k < bitCnt; k++)
	{
		BitManager::SetBit(block, bitPos);
		long foundNewFishesLow = MaskManager::neighborMasks[bitPos][0] & (posData[0] ^ block[0]);
		long foundNewFishesHigh = MaskManager::neighborMasks[bitPos][1] & (posData[1] ^ block[1]);
		if (__popcnt64(foundNewFishesLow) > 0 || __popcnt64(foundNewFishesHigh) > 0)
		{
			int firstPos = BitManager::GetFirstRightBitPos(foundNewFishesLow, foundNewFishesHigh);
			ExtendBlock(posData, block, foundNewFishesLow, foundNewFishesHigh, firstPos);
		}
		if (k + 1 < bitCnt)
			bitPos = BitManager::GetNextRightBitPosIgnorePrevious(newFishesLow, newFishesHigh, bitPos);
	}
}
int BoardManager::GetBlockAndCnt( boardpane posData, boardpane blockList[] )
{
	// _ASSERT_EXPR ( blockList.length >= 16,  "Software Issue. A blocklist can maximum contain 16 blocks." );
	// _ASSERT_EXPR ( blockList[15].length >= 2, "Software Issue. A blocklist hold the low and high long of the fishes." );
	int cnt = 0;
	int bitToFind = BitManager::BitCnt(posData);
	long restFishesLow = posData[0];
	long restFishesHigh = posData[1];
	while (bitToFind > 0)
	{
		for (int k = 0; k < 2; k++) blockList[cnt][k] = 0;
		int bitId = BitManager::GetFirstRightBitPos(restFishesLow, restFishesHigh);
		BitManager::SetBit(blockList[cnt], bitId); // use empty blocklist Storage to set the first fish of a block
		ExtendBlock(posData, blockList[cnt], blockList[cnt][0], blockList[cnt][1], bitId);
		int bitFound = BitManager::BitCnt(blockList[cnt]);
		_ASSERT_EXPR(  bitFound > 0 ,  "Software Issue. A block must contain in minimum 1 fish, otherwise this is an endless loop." );
		bitToFind -= bitFound;
		// switch off fishes found
		restFishesLow ^= blockList[cnt][0];
		restFishesHigh ^= blockList[cnt][1];
		cnt++;
	}
	return cnt;
}


int BoardManager::GetNextRightBitPos(boardpane BP, int currentPos)
{
	_ASSERT_EXPR(BitManager::GetFirstRightBitPos(BP[0], BP[1]) == currentPos, currentPos + "bit found before " + currentPos);
	if (currentPos >= 64)
	{
		BP[1] &= ~(1L << (currentPos - 64));  // switch of current pos in the current position
	}
	else
	{
		BP[0] &= ~(1L << currentPos);
	}
	return (BitManager::GetFirstRightBitPos(BP[0], BP[1]));
}

/**
 * check if the end is reached (available moves checked afterwards) and if yes the result
 * @param blockListAll
 * @param pos
 * @return
 * -1 : End not reached
 * 0 : End reached -> Winner is blue
 * 1 : End reached -> Winner is red
 * 0.5 : End reached -> no Winner
 */
double BoardManager::Analysis(int moveCnt, boardpane blockListAll[][16], int blockCount[], board pos, int MaxDepth)
{
	bool roundEnd = moveCnt % 2 == 0 && moveCnt > 0;
	bool gameEnd = moveCnt >= MaxDepth || (roundEnd && (blockCount[0] == 1 || blockCount[1] == 1));

	if (gameEnd || roundEnd)
	{
		if (blockCount[0] == 1 && blockCount[1] == 1)
			return 0.5;
		else if (blockCount[0] == 1)
			return 1;
		else if (blockCount[1] == 1)
			return 0;

	}
	if (gameEnd)
	{
		int maxRed = 0; int maxBlue = 0; int cnt;
		for (int k = 0; k <= blockCount[0]; k++)
			if ((cnt = (BitManager::BitCnt(blockListAll[0][k]) )) > maxRed)
			{
				maxRed = cnt;
			}
		for (int k = 0; k <= blockCount[1]; k++)
			if ((cnt = (BitManager::BitCnt(blockListAll[1][k]))) > maxBlue)
			{
				maxBlue = cnt;
			}
		if (maxRed == maxBlue)
			return 0.5;
		else if (maxRed > maxBlue)
			return 1;
		else
			return 0;
	}
	return -1;
}

static const double factor = 0.3 / (512 * 16);

double BoardManager::GetValue(board pos, int color, boardpane blockList[][16], int blockCnt[], int depth, int firstMoveDepth)
{
	// calculate the value of this position
	// here to count number of blocks and calculate the block value
	// check if this is the secondMoveColor to check if moveColor will win = 1 or loss = 0
	long valColor = GetPosValue(pos, color, blockList, blockCnt);
	long valOppositeColor = GetPosValue(pos, (color + 1) % 2, blockList, blockCnt);
	double ret;
	// foundGameEnd = false;
	if ((ret = BoardManager::Analysis(depth, blockList, blockCnt, pos, 60 - firstMoveDepth)) < 0)
	{
		ret = (valColor - valOppositeColor) * factor + 0.5;
	}
	else
	{
		// foundGameEnd = true;
		if (color == 1)
		{
			ret = 1 - ret;
		}
		throw new GameEndException(ret);
	}
	return ret;
}

string BoardManager::AnalysisToString(double val)
{
	string ret = "The winner is ";
	if (val == 1.0)
		return ret + "red";
	else if (val == 0.0)
		return ret + "blue";
	else
		return "The game ends unentschieden.";
}

long BoardManager::GetPosValue(board pos, int color, boardpane blockList[][16], int blockCnt[])
{
	// System.out.println(PosManager.ToString( pos1 ));
	blockCnt[color] = BoardManager::GetBlockAndCnt(pos[color], blockList[color]);
	long posValue = 0;
	for (int i = 0; i < blockCnt[color]; i++)
	{
		posValue += BoardManager::blockValue(blockList[color][i]);
	}
	return posValue;
}


