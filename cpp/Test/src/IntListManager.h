#pragma once
#include "FreeArrayManager.h"

const int dataSize = 10;
typedef struct
{
	unsigned int free: 1, nextIdx: 31;
} listHeader;

typedef struct {
	listHeader h;
	int length;
	int item[dataSize - 1];
} first;

typedef struct {
	listHeader h;
	int item[dataSize];
} second;

typedef struct
{
	union
	{
		first f;
		second s;
		nextFreeId fId;
	};
} listData;


class IntListManager
{
public:
	class IntListIterator
	{
	public:
		// IntListIterator( int ListIdx );
		// https://stackoverflow.com/questions/30898705/usage-of-the-foreach-loop-in-c
		// here ...

	};
	IntListManager( int );

	int ReserveList();
	void Add(int ListIdx, int NewItem);
	int GetLength(int ListIdx);
	void Release(int ListIdx);

private: 
	int GetBlockIdx(int ListIdx, int Idx);

	listData* data;
	FreeArrayManager* fam;
};