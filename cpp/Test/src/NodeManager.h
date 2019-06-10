#pragma once
#include "FreeArrayManager.h"
#include "BoardManager.h"
#include "MoveManager.h"

struct moveAndVisits
{
	unsigned int move : 8, visits : 24;
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

struct smallNode {
	union {
		unsigned int isSuperPackedMove : 1, packedMove : 31;
		unsigned int isNoNodeIdx : 1, nodeIdx : 31;
	};
};

class NodeManager
{

public:
	NodeManager( int NodeCount, int MyColor, board FirstBoard, int FirstMoveDepth );
	~NodeManager();

	static smallNode superPackMove(coordinates fromPos, coordinates toPos );
private: 
	bool hasChild(smallNode);

	m* memory;
	int firstMoveColor;
	int firstMoveDepth;
	boardpane* firstBoard;
};

