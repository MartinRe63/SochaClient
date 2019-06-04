#pragma once
#include "FreeArrayManager.h"

const int blockDataSize = 10;

class IntListManager
{
public:
	class IntListIterator
	{
	public:
		IntListIterator( int ListIdx, IntListManager*listIdx Ilm );
		// https://stackoverflow.com/questions/30898705/usage-of-the-foreach-loop-in-c
		int GetNextItem();
	private:
		int virtualIdx;
		int blockIdx;
		int listIdx;
		IntListManager* ilm;
	};
	IntListManager( int );

	int ReserveList();
	void Add(int ListIdx, int NewItem);
	int GetLength(int ListIdx);
	void Release(int ListIdx);

protected:
	typedef struct
	{
		unsigned int free: 1, nextIdx: 31;
	} listHeader;

	struct first {
		listHeader h;
		int length;
		int item[blockDataSize - 1];
	};

	struct second {
		listHeader h;
		int item[blockDataSize];
	};

	struct listData
	{
		union
		{
			first f;
			second s;
			nextFreeId fId;
		};
	};
	int GetBlockIdx(int ListIdx, int Idx);

	listData* data;
	FreeArrayManager* fam;
};
