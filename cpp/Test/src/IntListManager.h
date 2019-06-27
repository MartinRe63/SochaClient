#pragma once

#include "FreeArrayManager.h"
#include "SuperPackedMove.h"
#include "NodeManager.h"

const int blockDataSize = 10;

class IntListManager
{
public:
	class ReadIterator
	{
	public:
		// https://stackoverflow.com/questions/30898705/usage-of-the-foreach-loop-in-c
		smallNode* GetNextItem();
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
		void AddItem( smallNode );
		WriteIterator(int ListIdx, IntListManager* Ilm);

	private:
		int relativBlockIdx;
		int blockIdx;
		int listIdx;
		IntListManager* ilm;
	};
	IntListManager( int );
	int ReserveList();

	inline int GetLength(int ListIdx)
	{
		return data[ListIdx].f.length;
	};

	void Release(int ListIdx);
	ReadIterator* GetReadIterator(int ListIdx);
	WriteIterator* GetWriteIterator(int ListIdx);

	friend class WriteIterator;
	friend class ReadIterator;

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
	inline int GetBlockIdx(int ListIdx, int Idx)
	{
		int blockCnt = (Idx + 1) / blockDataSize;
		int idx = ListIdx;
		for (int k = 0; k < blockCnt; k++)
		{
			idx = data[idx].s.h.nextIdx;
		}
		return idx;
	};
	void Add(int ListIdx, int BlockIdx, int relativeIdx, smallNode NewItem);

	listData* data;
	FreeArrayManager* fam;
};
