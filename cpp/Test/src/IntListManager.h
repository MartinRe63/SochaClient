#pragma once
#include "FreeArrayManager.h"
#include "NodeManager.h"

const int blockDataSize = 10;

class IntListManager
{
public:
	class ReadIterator
	{
	public:
		// https://stackoverflow.com/questions/30898705/usage-of-the-foreach-loop-in-c
		smallNode GetNextItem();
		ReadIterator(int ListIdx, IntListManager* Ilm);

	private:
		int virtualIdx;
		int blockIdx;
		int listIdx;
		IntListManager* ilm;
	};
	class WriteIterator
	{
	public:
		// https://stackoverflow.com/questions/30898705/usage-of-the-foreach-loop-in-c
		void SetNextItem( smallNode );
		WriteIterator(int ListIdx, IntListManager* Ilm);

	private:
		int virtualIdx;
		int blockIdx;
		int listIdx;
		IntListManager* ilm;
	};
	IntListManager( int );

	int ReserveList();
	void Add(int ListIdx, smallNode NewItem);
	int GetLength(int ListIdx);
	void Release(int ListIdx);
	ReadIterator* GetIterator(int ListIdx);

private:
	typedef struct
	{
		unsigned int free: 1, nextIdx: 31;
	} listHeader;

	struct first {
		listHeader h;
		int length;
		smallNode item[blockDataSize - 1];
	};

	struct second {
		listHeader h;
		smallNode item[blockDataSize];
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
