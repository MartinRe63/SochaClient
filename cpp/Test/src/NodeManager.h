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

	void expandNode(int nodeId, int moveColor, board position, int depth);


	m* memory;
	int firstMoveColor;
	int firstMoveDepth;
	int firstNodeIdx;
	boardpane* firstBoard;
	FreeArrayManager* fam;
	IntListManager* ilm;
	packedMove moveList[16*8]; // all fishes might move into all directions
};
