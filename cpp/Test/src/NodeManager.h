#pragma once
#include "SuperPackedMove.h"
#include "FreeArrayManager.h"
#include "IntListManager.h"
#include "BoardManager.h"
#include "MoveManager.h"

struct moveAndVisits
{
	// unsigned int move : 8, visits : 24;
	packedMove move;
	unsigned int visits;
};

struct childId
{
	unsigned int reserved: 1, syncSignal: 1, id : 30;
};

struct node
{
    childId child;
	float totValue;
	moveAndVisits v;
};
struct m
{
	union {
		node node;
		nextFreeId free;
	};
};

class IntListManager;
class NodeManager
{

public:
	NodeManager( int NodeCount, int MyColor, board FirstBoard, int FirstMoveDepth );
	~NodeManager();

private: 
	bool hasChild(smallNode);
	void InitFirstNode();
	void InitNode(int nodeId, packedMove move, long visitCnt);

	void expandNode(smallNode* sN , int moveColor, board position, int depth);
	double rollOut( int nodeId, int color, board pos, int depth );

	smallNode * selectMove(smallNode * sN);

	void selectAction(bool oneCycle);

	m* memory;
	int firstMoveColor;
	int firstMoveDepth;
	smallNode firstNodeIdx;
	boardpane* firstBoard;
	FreeArrayManager* fam;
	IntListManager* ilm;
	packedMove moveList[16*8]; // all fishes might move into all directions
	boardpane blockList[2][16]; // all blocks of a board
	int blockCnt[2]; // blocklist for red and blue
	bool stopSelection;
	int visitedCnt;
	smallNode* visited[60];
	board firstPosition;
	board pos;
};
