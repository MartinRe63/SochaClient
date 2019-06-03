#include <crtdbg.h>
#include "IntListManager.h"

static const int endMarker = -1;

IntListManager::IntListManager( int BlockAmount )
{
	data = new listData[BlockAmount];
	fam = new FreeArrayManager((nextFreeId*)data, BlockAmount, sizeof(listData));

};

int IntListManager::ReserveList()
{
	int listIdx = fam->ReserveNextFree();
	data[listIdx].f.h.nextIdx = endMarker;
	data[listIdx].f.length = 0;
};

void IntListManager::Add(int ListIdx, int NewItem)
{
	int lth = data[ListIdx].f.length;
	int oldBlockCnt = ( lth + 1 ) / dataSize;
	int oldBlockIdx = GetBlockIdx( lth + 1 );
	lth++;
	int newBlockCnt = ( lth + 1 ) / dataSize;
	if ( oldBlockCnt != newBlockCnt )
	{
		// add a block to the chain of blocks
		int listIdx = fam->ReserveNextFree();
		data[listIdx].f.h.nextIdx = endMarker;
		data[oldBlockIdx].f.h.nextIdx = listIdx;
	}
	data[ListIdx].f.length++;
};

int IntListManager::GetLength(int ListIdx)
{
	return data[ListIdx].f.length;
};

void IntListManager::Release(int ListIdx)
{
	while (ListIdx != endMarker)
	{
		int listIdx = ListIdx;
		ListIdx = data[listIdx].f.h.nextIdx;
		fam->DisposeAt(listIdx);
	}
};

int IntListManager::GetBlockIdx(int ListIdx, int Idx)
{
	int lth = data[ListIdx].f.length;
	int blockCnt = (Idx + 1) / dataSize;
	int idx = ListIdx; 
	for (int k = 0; k < blockCnt; k++)
	{
		idx = data[idx].s.h.nextIdx;
	}
	return idx;
};

