#pragma once

#include <vector>
#include "FreeArrayManager.h"
#include "IntListManager.h"
#include "SuperPackedMove.h"
// #include "NodeManager.h"

#define  VectorBased 1

const int blockDataSize = 16;
using std::vector;

class IntListManager
{
public:
	class ReadIterator
	{
	public:
#ifndef VectorBased
		smallNode* GetNextItem();
#else
		inline smallNode* GetNextItem()
		{
			vector<smallNode>* nodeList = ilm->data[listIdx].nodeList;
			if (virtualIdx >= nodeList->size())
				return 0;
			else
				return &(*nodeList)[virtualIdx++];
		}
#endif
		ReadIterator(int ListIdx, IntListManager* Ilm);
		void Init(int ListIdx);

	private:
		int virtualIdx;
		int blockIdx;
		int listIdx;
		IntListManager* ilm;
	};
	class WriteIterator
	{
	public:
#ifndef VectorBased
		void AddItem( smallNode );
#else
		inline void WriteIterator::AddItem(smallNode n)
		{
			(*ilm->data[listIdx].nodeList)[relativBlockIdx++] = n;
		}
#endif
		WriteIterator(int ListIdx, IntListManager* Ilm);

	private:
		int relativBlockIdx;
		int blockIdx;
		int listIdx;
		IntListManager* ilm;
	};


	IntListManager( int );
	int ReserveList( int size );

	inline int GetLength(int ListIdx)
	{
#ifndef VectorBased
		return data[ListIdx].f.length;
#else
		return data[ListIdx].nodeList->size();
#endif
	};

	void Release(int ListIdx);
	ReadIterator* GetReadIterator(int ListIdx);
	ReadIterator* GetReadIterator();
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
#ifndef VectorBased
			first f;
			second s;
#else
			std::vector<smallNode>* nodeList;
#endif
			nextFreeId fId;
		};
	};

#ifndef VectorBased
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
#endif
	void Add(int ListIdx, int BlockIdx, int relativeIdx, smallNode NewItem);

	listData* data;
	FreeArrayManager* fam;
};
