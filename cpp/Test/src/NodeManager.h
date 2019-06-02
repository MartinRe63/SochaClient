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
	moveAndVisits visits;
} node;
typedef struct
{
	union {
		node node;
		nextFreeId free;
	};
} m;

class NodeManager
{

public:
	NodeManager( int nodeCount );
	~NodeManager();
private: 
	m* memory;
};

