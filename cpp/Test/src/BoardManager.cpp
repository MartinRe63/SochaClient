/*
 * BoardManager.cpp
 *
 *  Created on: 28.05.2019
 *      Author: mrenneke
 */
// #include <string>
#include <assert.h>
#include <algorithm>
#include <iostream>
#include <crtdbg.h>
#include <intrin.h>
#include <algorithm>
#include "BoardManager.h"
#include "BitManager.h"
#include "MaskManager.h"
#include "MoveManager.h"


GameEndException::GameEndException(double Result) {
	res = Result;
};
int GameEndException::GetResult(void) {
	return res;
}

inline float texelWeight(float turnPart, float startWeight, float endWeight)
{
	return turnPart * endWeight + (1.0 - turnPart) * startWeight;
}

void BoardManager::Copy(const board Source, board Destination)
{
	for ( int c = 0; c < 3; c++ )
		for ( int b = 0; b < 2; b++ )
			Destination[c][b] = Source[c][b];
};

void BoardManager::FromString(std::string Init, board &B)
{
	B[0][0] = 0;
	B[0][1] = 0;
	B[1][0] = 0;
	B[1][1] = 0;
	B[2][0] = 0;
	B[2][1] = 0;
	for (int y = 0; y < 10; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			std::string s = Init.substr((9 - y) * 10 + x, 1);
			if (s == "0")
				BitManager::SetBit(B[0], y * 10 + x);
			else if (s == "1")
				BitManager::SetBit(B[1], y * 10 + x);
			else if (s == "C" )
				BitManager::SetBit(B[2], y * 10 + x);
		}
	}
}

std::string BoardManager::ToString(board B)
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
	ret *= ret;
	//int val = 4;
	//while ((BP[0] & MaskManager::fishValueMasks[val][0]) == 0 && (BP[1] & MaskManager::fishValueMasks[val][1]) == 0)
	//	val--;
	//long multiplier = val > 1 ? 8 : val;
	//for (int i = 3; i <= val; i++) multiplier <<= 3;
	//ret *= multiplier;
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
		uint64_t foundNewFishesLow = MaskManager::neighborMasks[bitPos][0] & (posData[0] ^ block[0]);
		uint64_t foundNewFishesHigh = MaskManager::neighborMasks[bitPos][1] & (posData[1] ^ block[1]);
		if (__popcnt64(foundNewFishesLow) > 0 || __popcnt64(foundNewFishesHigh) > 0)
		{
			unsigned long firstPos = BitManager::GetFirstRightBitPos(foundNewFishesLow, foundNewFishesHigh);
			ExtendBlock(posData, block, foundNewFishesLow, foundNewFishesHigh, firstPos);
		}
		if (k + 1 < bitCnt)
			bitPos = BitManager::GetNextRightBitPosIgnorePrevious(newFishesLow, newFishesHigh, bitPos);
	}
}
int BoardManager::GetBlockAndCnt( boardpane posData, boardpane blockList[], int &fishCnt )
{
	// _ASSERT_EXPR ( blockList.length >= 16,  "Software Issue. A blocklist can maximum contain 16 blocks." );
	// _ASSERT_EXPR ( blockList[15].length >= 2, "Software Issue. A blocklist hold the low and high long of the fishes." );
	int cnt = 0;
	int bitToFind = fishCnt = BitManager::BitCnt(posData);
	uint64_t restFishesLow = posData[0];
	uint64_t restFishesHigh = posData[1];
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
		BP[1] &= ~(1ULL << (currentPos - 64));  // switch of current pos in the current position
	}
	else
	{
		BP[0] &= ~(1ULL << currentPos);
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
double BoardManager::Analysis(int moveCnt, boardpane blockListAll[][16], int blockCount[], board pos, int FirstDepth)
{
	int MaxDepth = 60 - FirstDepth;
	bool roundEnd = (moveCnt + FirstDepth ) % 2 == 0 && moveCnt > 0;
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

static const double factorBlocks = 0.2 / (512 * 16);
static const double factorBlockCount = 0.1 / 512;
static const double factorMoveCount = 0.3 / 512;
static long long dbg_cnt = 0;

float BoardManager::GetValue(board pos, int color, boardpane blockList[][16], int blockCnt[], int depth, int firstMoveDepth, bool& gameEnd, int distance)
{
	// calculate the value of this position
	// here to count number of blocks and calculate the block value
	// check if this is the secondMoveColor to check if moveColor will win = 1 or loss = 0
	// dbg_cnt++;
	//if (dbg_cnt == 12715)
	//{
	//	dbg_cnt--;
	//}
	
	gameEnd = false;
	int valCount, valOppositeCount;
	long valColor = GetValueOfBlocks(pos, color, blockList, blockCnt, valCount);
	int oppositeColor = !color;
	long valOppositeColor = GetValueOfBlocks(pos, oppositeColor, blockList, blockCnt, valOppositeCount);
	float res;
	float ret;
	// foundGameEnd = false;
	if ((res = BoardManager::Analysis(depth, blockList, blockCnt, pos, firstMoveDepth)) >= 0)
	{
		// foundGameEnd = true;
		if ( firstMoveDepth < 20 )
		{
			//cout << "Ups. Spielende gefunden. Erstaunlich. Fehler?" << endl;
			//cout << BoardManager::ToString(pos) << endl;
		}
		// !!! The result of the analysis is from the red view point !!!

		if (color == 1)
		{
			res = 1 - res;
		}
		// if (std::abs(res - ret) >= 0.6)
		// 	 dbg_cnt++;
		ret = res;

		gameEnd = true;
	}
	else
	{
		int blockCntVal = 256 / blockCnt[color];
		int blockCntOpVal = 256 / blockCnt[oppositeColor];
		float turnPart = 1.0 / (depth + firstMoveDepth);
		float k_bs = texelWeight(turnPart, 2, 6); // blocksize
		float k_d = texelWeight(turnPart, 4, 12);
		float k_c = texelWeight(turnPart, 14, 6);  // center part
		float k_cnt = texelWeight(turnPart, 2, 2);  
		float k_bc = texelWeight(turnPart, 2, 16); // blockcount

		float cnt = ( valCount * 1000000 / (valCount + valOppositeCount)) / 1e6f;
		float bs = ( valColor * 1000000 / (valColor + valOppositeColor)) / 1e6f;
		float bc = (blockCntVal * 1000000 / (blockCntVal + blockCntOpVal)) / 1e6f;

		bool emptyEnd;
		float c = BoardManager::quick_rateState(pos, 0, color, emptyEnd);
		//int oppositeDistance;
		//packedMove moveList[16 * 8]; // not used, because it's only a distance check
		//MoveManager::getMoveList(pos, !color, moveList, oppositeDistance);
		//float d = (distance*1000000 / (distance + oppositeDistance)) / 1e6f;
		boardpane bestBlock;
		float d = 0.5;
		if (firstMoveDepth + depth > 0)
		{
			int maxCnt = INT_MIN;
			int bestIdx = 0;
			int cntFishesInMaxBlock;
			for (int i = 0; i < blockCnt[color]; i++)
				if ((cntFishesInMaxBlock = BitManager::BitCnt(blockList[color][i])) > maxCnt)
				{
					maxCnt = cntFishesInMaxBlock;
					bestIdx = i;
				}
			int dist = 8192 / ( GetDistance(pos, color, blockList[color][bestIdx]) + 1 );
			maxCnt = INT_MIN;
			bestIdx = 0;
			for (int i = 0; i < blockCnt[oppositeColor]; i++)
				if ((cntFishesInMaxBlock = BitManager::BitCnt(blockList[oppositeColor][i])) > maxCnt)
				{
					maxCnt = cntFishesInMaxBlock;
					bestIdx = i;
				}
			int oDist = 8192 / ( GetDistance(pos, oppositeColor, blockList[oppositeColor][bestIdx]) + 1 );

			d = (dist * 1000000 / (dist + oDist)) / 1e6f;
		}
		ret = ( bs * k_bs + c * k_c + cnt * k_cnt + bc * k_bc + d * k_d ) / ( k_bs + k_c + k_cnt + k_bc + k_d );
		if ( ( firstMoveDepth + depth ) > 40 )
		{
			std::string bS;
			bS = BoardManager::ToString(pos);
		}
	}
	return ret;
}


int int128_ffs(boardpane value) {
	int id;
#ifndef _WIN
	if ((id = __builtin_ffsll(value.v0)))
		return id;
	else if ((id = __builtin_ffsll(value.v1)))
		return id + 64;
#else
	unsigned long ret = 0;
	unsigned char res = _BitScanForward64(&ret, value[0]); // Long.numberOfTrailingZeros(low);
	if (res == 0)
	{
		res = _BitScanForward64(&ret, value[1]);
		if (res == 0)
			return 0;
		else
			ret += 64;
		return ret + 1;
	}
#endif
	return ret + 1;
}
void int128_and(boardpane v1, int128 v2, boardpane& r) {
	r[0] = v1[0] & v2.v0;
	r[1] = v1[1] & v2.v1;
}
signed char getConnectedCount(int id, boardpane data, signed char fish[], signed char *fishCount) {
	//Pr�fe ob das feld belegt ist
	// assert(int128_notNull(int128_and(*data, m_field[id])));
	fish[*fishCount] = id;
	*fishCount = *fishCount + 1;
	//Bit auf diesem Feld auf Null
	data[0] ^= MaskManager::m_field[id].v0;
	data[1] ^= MaskManager::m_field[id].v1;
	signed char connections = 1;
	int id2;
	//int128_debugWrite(neighbors);
	boardpane res;
	int128_and(data, MaskManager::m_fieldNeighbors[id][0], res);
	while ((id2 = int128_ffs(res))) {
		connections += getConnectedCount(id2 - 1, data, fish, fishCount);
		int128_and(data, MaskManager::m_fieldNeighbors[id][0], res);
	}
	return connections;
}

unsigned long long BoardManager::quickBoardPaneValue(boardpane BP)
{
	unsigned long long res = std::min(((__popcnt64(BP[0] & MaskManager::fishValueMasks[4][0]) + __popcnt64(BP[1] & MaskManager::fishValueMasks[4][1]))), 2ULL) * 8L;
	res += std::min(((__popcnt64(BP[0] & MaskManager::fishValueMasks[3][0]) + __popcnt64(BP[1] & MaskManager::fishValueMasks[3][1]))), 6ULL) * 4L;
	res += std::min(((__popcnt64(BP[0] & MaskManager::fishValueMasks[2][0]) + __popcnt64(BP[1] & MaskManager::fishValueMasks[2][1]))), 10ULL) * 2L;
	return res;
}

float BoardManager::quick_rateState(board b, int turn, int turningColor, bool& gameEnd)
{
	gameEnd = false;
	uint64_t redval = BoardManager::quickBoardPaneValue(b[0]);
	uint64_t blueval = BoardManager::quickBoardPaneValue(b[1]);
	if (turningColor)
		return blueval  / (blueval + redval + 1.0);
	else
		return redval / (blueval + redval + 1.0);
}

const int color_red = 0;
const int color_blue = 1;
double BoardManager::board_rateState(board b, int turn, int turningColor, bool& gameEnd) {
	board cpy; 
	BoardManager::Copy( b, cpy );
	signed char swarmSizes[2][16] = { 0 };
	signed char fish[2][16];
	signed char swarmCount[2] = { 0, 0 };
	signed char fishCount[2] = { 0, 0 };
	signed char maxSwarmSize[2] = { 0, 0 };
	for (int c = 0; c < 2; c++) {
		int id;
		while ((id = int128_ffs(cpy[c]))) {
			swarmSizes[c][swarmCount[c]] = getConnectedCount(id - 1, cpy[c], fish[c], &fishCount[c]);
			if (swarmSizes[c][maxSwarmSize[c]] < swarmSizes[c][swarmCount[c]]) {
				maxSwarmSize[c] = swarmCount[c];
			}
			swarmCount[c]++;
		}
	}
	if (turn % 2 == 0) {
		if ((fishCount[color_red] == swarmSizes[color_red][maxSwarmSize[color_red]] && fishCount[color_blue] == swarmSizes[color_blue][maxSwarmSize[color_blue]]) || turn == 60) {
			if (swarmSizes[turningColor][maxSwarmSize[turningColor]] > swarmSizes[!turningColor][maxSwarmSize[!turningColor]]) {
				gameEnd = 1;
				return 1;
			}
			if (swarmSizes[!turningColor][maxSwarmSize[!turningColor]] > swarmSizes[turningColor][maxSwarmSize[turningColor]]) {
				gameEnd = 1;
				return 0;
			}
			gameEnd = 1;
			return 0.5f;
		}
		if (fishCount[turningColor] == swarmSizes[turningColor][maxSwarmSize[turningColor]]) {
			gameEnd = 1;
			return 1;
		}
		if (fishCount[!turningColor] == swarmSizes[!turningColor][maxSwarmSize[!turningColor]]) {
			gameEnd = 1;
			return 0;
		}
	}
	long totalFishDist[2] = { 0, 0 };
	long blocker[2] = { 0, 0 };
	//float connector[2] = {0};

	//int position[2] = {0};
	for (int c = 0, c2 = 1; c < 2; c++, c2--) {
		for (int s1 = 0; s1 < swarmCount[c]; s1++) {
			int start1 = 0;
			for (int f1 = start1; f1 < start1 + swarmSizes[c][s1]; f1++) {
				for (int f2 = start1 + swarmSizes[c][s1]; f2 < fishCount[c]; f2++) {
					boardpane res;
					int128_and(b[c2], MaskManager::m_blocker[fish[c][f1]][fish[c][f2]], res);
					blocker[c] += BitManager::BitCnt(res) * 8192 /
						MaskManager::m_blockingFields[fish[c][f1]][fish[c][f2]];
						totalFishDist[c] += MaskManager::m_distance[fish[c][f1]][fish[c][f2]] * 8192;
				}
			}
			start1 += swarmSizes[c][s1];
		}
		totalFishDist[c] += 8192 + blocker[c] * 16;
	}
	float bs = (float)swarmSizes[turningColor][maxSwarmSize[turningColor]] / (swarmSizes[0][maxSwarmSize[0]] + swarmSizes[1][maxSwarmSize[1]]);
	float d = (float)totalFishDist[!turningColor] / (totalFishDist[1] + totalFishDist[0] + 1);
	float blocking = (blocker[!turningColor]) / (blocker[1] + blocker[0] + 1);

	float k_bs = (1.0 / 60.0 * turn);
	float k_d = 1;

	float rating = ( bs * k_bs + d * k_d ) / ( k_bs + k_d );
	if ( false || (rating < 0 || rating > 1)) {
		printf("TC: %d\n", turningColor);
		printf("BL: %f, %f\n", blocker[turningColor], blocker[!turningColor]);
		printf("BS: %d, %d\n", swarmSizes[turningColor][maxSwarmSize[turningColor]], swarmSizes[!turningColor][maxSwarmSize[!turningColor]]);
		printf("SC: %d, %d\n", swarmCount[turningColor], swarmCount[!turningColor]);
		printf("DI: %f, %f\n", totalFishDist[turningColor], totalFishDist[!turningColor]);
		//printf("CN: %f, %f\n", connector[turningColor], connector[!turningColor]);
		printf("Blocker   : %f\n", blocking);
		//printf("Connector : %f\n", connecting);
		printf("Biggest S : %f\n", bs);
		printf("Fish Dist : %f\n", d);
	}
	assert(rating >= 0);
	assert(rating <= 1);
	return rating;
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

long BoardManager::GetValueOfBlocks(board pos, int color, boardpane blockList[][16], int blockCnt[], int& fishCnt)
{
	// System.out.println(PosManager.ToString( pos1 ));
	blockCnt[color] = BoardManager::GetBlockAndCnt(pos[color], blockList[color], fishCnt);
	long posValue = 0;
	for (int i = 0; i < blockCnt[color]; i++)
	{
		posValue += BoardManager::blockValue(blockList[color][i]);
	}
	return posValue;
}

void BoardManager::SetBoardValue(board b, int x, int y, int color)
{
	BitManager::SetBit(b[color], x + y * 10);
}


int BoardManager::GetDistance(board pos, int color, boardpane bestBlock)
{
	boardpane outlier;
	Outline(bestBlock, outlier);
	boardpane toCheck = { pos[color][0], pos[color][1] };
	boardpane startPane;
	boardpane reachedPositions;
	toCheck[0] ^= bestBlock[0]; toCheck[1] ^= bestBlock[1]; // switch off best block of fishes
	int totalDistance = 0;
	int distance;
	int p=BitManager::GetFirstRightBitPos(toCheck[0], toCheck[1]);
	while ( p < 100 )
	{
		// per fish not in the selected block (biggest block or center block - here named bestBlock
		startPane[0] = 0; startPane[1] = 0;
		BitManager::SetBit(startPane, p);
		reachedPositions[0] = startPane[0]; reachedPositions[1] = startPane[1];
		bool outlierFound = false;
		bool blocked = false;
		int distance = 0;
		while (!outlierFound && !blocked)
		{
			boardpane newMovePane = nullPane;
			int p1 = BitManager::GetFirstRightBitPos( startPane[0], startPane[1] );
			while (!outlierFound && p1 < 100 )
			{
				MoveManager::MarkPotentialMoves( p1, pos, color, reachedPositions, outlier, newMovePane, outlierFound );
				if ( ! outlierFound )
					p1 = BitManager::GetNextRightBitPosIgnorePrevious( startPane[0], startPane[1], p1 );
			}
			if ( ! outlierFound )
			{
				blocked = newMovePane[0] == 0 && newMovePane[1] == 0;
				if ( ! blocked )
				{
					reachedPositions[0] |= newMovePane[0]; reachedPositions[1] |= newMovePane[1];
				}
				else
					distance=15;
			}
			distance++;
			startPane[0] = newMovePane[0];  startPane[1] = newMovePane[1];
		}
		totalDistance += distance*distance;
		p = BitManager::GetNextRightBitPosIgnorePrevious(toCheck[0], toCheck[1], p);
	}
	return totalDistance;
}

void BoardManager::Outline(boardpane pos, boardpane res)
{
	res[0] = pos[0];
	res[1] = pos[1];

	boardpane r;
	bool right = BitManager::AndIsNotNull(pos, MaskManager::borderMask[0], r);
	boardpane l;
	bool left = BitManager::AndIsNotNull(pos, MaskManager::borderMask[2], l);
	if (right)
	{
		res[0] |= (pos[0] ^ r[0]) << 1;
		res[1] |= (pos[1] ^ r[1]) << 1;
	}
	else
	{
		res[0] |= pos[0] << 1;
		res[1] |= pos[1] << 1;
	}
	if (left)
	{
		res[0] |= (pos[0] ^ l[0]) >> 1;
		res[1] |= (pos[1] ^ l[1]) >> 1;
	}
	else
	{
		res[0] |= pos[0] >> 1;
		res[1] |= pos[1] >> 1;
	}
	boardpane t;
	boardpane u;
	if (BitManager::AndIsNotNull(pos, MaskManager::borderMask[1], u))
	{
		res[0] |= (t[0] = ((pos[0] ^ u[0]) << 10));
		res[1] |= (t[1] = ((pos[1] ^ u[1]) << 10));
	}
	else
	{
		res[0] |= (t[0] = (pos[0] << 10));
		res[1] |= (t[1] = (pos[1] << 10));
	}
    if ( t[0] || t[1] )
    {
		if (right)
		{
			res[0] |= ((t[0] & MaskManager::borderMask[0][0]) ^ t[0]) << 1;
			res[1] |= ((t[1] & MaskManager::borderMask[0][1]) ^ t[1]) << 1;
		}
		else
		{
			res[0] |= t[0] << 1;
			res[1] |= t[1] << 1;
		}
		if (left)
		{
			// cutoff fishes at the border first
			res[0] |= ((t[0] & MaskManager::borderMask[2][0]) ^ t[0]) >> 1;
			res[1] |= ((t[1] & MaskManager::borderMask[2][1]) ^ t[1]) >> 1;
		}
		else
		{
			res[0] |= t[0] >> 1;
			res[1] |= t[1] >> 1;
		}
    }
	boardpane d;
	if (BitManager::AndIsNotNull(pos, MaskManager::borderMask[1], d))
	{
		res[0] |= (t[0] = ((pos[0] ^ d[0]) >> 10));
		res[1] |= (t[1] = ((pos[1] ^ d[1]) >> 10));
	}
	else
	{
		res[0] |= (t[0] = (pos[0] >> 10));
		res[1] |= (t[1] = (pos[1] >> 10));
	}

    if ( t[0] || t[1] )
    {
		if (right)
		{
			res[0] |= ((t[0] & MaskManager::borderMask[0][0]) ^ t[0]) << 1;
			res[1] |= ((t[1] & MaskManager::borderMask[0][1]) ^ t[1]) << 1;
		}
		else
		{
			res[0] |= t[0] << 1;
			res[1] |= t[1] << 1;
		}
		if (left)
		{
			res[0] |= ((t[0] & MaskManager::borderMask[2][0]) ^ t[0]) >> 1;
			res[1] |= ((t[1] & MaskManager::borderMask[2][1]) ^ t[1]) >> 1;
		}
		else
		{
			res[0] |= t[0] >> 1;
			res[1] |= t[1] >> 1;
		}
    }
	res[0] ^= pos[0];
	res[1] ^= pos[1];
}
