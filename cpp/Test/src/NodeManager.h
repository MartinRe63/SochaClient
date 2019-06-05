#pragma once

#include "FreeArrayManager.h"

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
		unsigned int isSuperPackedMove : 1, packedMove : 31;
		unsigned int isNoNodeIdx : 1, nodeIdx : 31;
	};
};


class NodeManager
{

public:
	NodeManager( int nodeCount );
	~NodeManager();

	static smallNode superPackMove( int fromPos, int toPos );
private: 
	m* memory;
};

