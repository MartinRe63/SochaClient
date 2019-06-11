#pragma once
#include "FreeArrayManager.h"
#include "BoardManager.h"
#include "MoveManager.h"

typedef struct 
{
	unsigned int move : 8, visits : 24;
} moveAndVisits;
typedef struct
{
	unsigned int reserved: 1, syncSignal: 1, childId : 30;
} childId;

typedef struct
{
    childId childId;
	float totValue;
	moveAndVisits v;
} node;
typedef struct
{
	union {
		node node;
		nextFreeId free;
	};
} m;

struct smallNode {
	union {
		superPackedMove smallMove;
		unsigned int isNoNodeIdx : 1, nodeIdx : 31;
	};
};

class NodeManager
{

public:
	NodeManager( int NodeCount, int MyColor, board FirstBoard, int FirstMoveDepth );
	~NodeManager();

private: 
	bool hasChild(smallNode);
	void initNode ( int nodeId, mov move, long visitCnt );

	m* memory;
	int firstMoveColor;
	int firstMoveDepth;
	boardpane* firstBoard;
};

