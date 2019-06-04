#include <crtdbg.h>
#include <stdexcept>
#include "IntListManager.h"

static const int endMarker = ~(1 << 31);


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
	return listIdx;
};

void IntListManager::Add(int ListIdx, int NewItem)
{
	int lth = data[ListIdx].f.length;
	int oldBlockCnt = ( lth + 1 ) / blockDataSize;
	int oldBlockIdx = GetBlockIdx( ListIdx, lth + 1 );
	lth++;
	int newBlockCnt = ( lth + 1 ) / blockDataSize;
	if ( oldBlockCnt != newBlockCnt )
	{
		// add a block to the chain of blocks
		int listIdx = fam->ReserveNextFree();
		data[listIdx].f.h.nextIdx = endMarker;
		data[oldBlockIdx].f.h.nextIdx = listIdx;
		oldBlockIdx = listIdx;
	}
	data[ListIdx].f.length++;
	data[oldBlockIdx].s.item[lth % blockDataSize] = NewItem;

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
	int blockCnt = (Idx + 1) / blockDataSize;
	int idx = ListIdx; 
	for (int k = 0; k < blockCnt; k++)
	{
		idx = data[idx].s.h.nextIdx;
	}
	return idx;
};

IntListManager::IntListIterator* IntListManager::GetIterator(int ListIdx)
{
	return new IntListManager::IntListIterator(ListIdx, this);
}


IntListManager::IntListIterator::IntListIterator( int ListIdx, IntListManager* Ilm )
{
	blockIdx = ListIdx;
	virtualIdx = 0;
	listIdx = ListIdx;
	ilm = Ilm;
}
int IntListManager::IntListIterator::GetNextItem()
{
	if ( virtualIdx >= ilm->GetLength( listIdx ) )
	{
		throw std::out_of_range("End of the list is reached.");
		return -1;
	}
    if ( (virtualIdx+1) % blockDataSize == 0 )
    {
    	blockIdx = ilm->GetBlockIdx(listIdx, virtualIdx);
    }
    int item = ilm->data[blockIdx].s.item[(virtualIdx+1) % blockDataSize];
    virtualIdx++;
    return item;
}

