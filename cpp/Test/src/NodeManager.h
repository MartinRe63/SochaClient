#pragma once
#include "SuperPackedMove.h"
#include "FreeArrayManager.h"
#include "IntListManager.h"
#include "BoardManager.h"
#include "MoveManager.h"

struct moveAndVisits
{
	// unsigned int move : 8, visits : 24;
	mov move;
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


class NodeManager
{

public:
	NodeManager( int NodeCount, int MyColor, board FirstBoard, int FirstMoveDepth );
	~NodeManager();

private: 
	bool hasChild(smallNode);
	void InitFirstNode();
	void InitNode(int nodeId, mov move, long visitCnt);


	m* memory;
	int firstMoveColor;
	int firstMoveDepth;
	int firstNodeIdx;
	boardpane* firstBoard;
	FreeArrayManager* fam;
	void* ilm;
};

