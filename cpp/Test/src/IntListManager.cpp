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

void IntListManager::Add(int ListIdx, smallNode NewItem)
{
	int lth = data[ListIdx].f.length;
	int oldBlockIdx = GetBlockIdx( ListIdx, lth + 1 );
	lth++;
	if ( oldBlockIdx == endMarker )
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

IntListManager::ReadIterator* IntListManager::GetIterator(int ListIdx)
{
	return new IntListManager::ReadIterator(ListIdx, this);
}


IntListManager::ReadIterator::ReadIterator( int ListIdx, IntListManager* Ilm )
{
	blockIdx = ListIdx;
	virtualIdx = 0;
	listIdx = ListIdx;
	ilm = Ilm;
}
smallNode IntListManager::ReadIterator::GetNextItem()
{
	if ( virtualIdx >= ilm->GetLength( listIdx ) )
	{
		throw std::out_of_range("End of the list is reached.");
		smallNode sN;
		sN.isSuperPackedMove = 0;
		sN.nodeIdx = -1;
		return sN;
	}
	int nextId = virtualIdx+1;
    if ( nextId % blockDataSize == 0 )
    {
    	blockIdx = ilm->GetBlockIdx(listIdx, virtualIdx);
    }
    smallNode item = ilm->data[blockIdx].s.item[nextId % blockDataSize];
    virtualIdx = nextId;
    return item;
}


IntListManager::WriteIterator::WriteIterator( int ListIdx, IntListManager* Ilm )
{
	blockIdx = ListIdx;
	virtualIdx = 0;
	listIdx = ListIdx;
	ilm = Ilm;
}

void IntListManager::WriteIterator::SetNextItem( smallNode n )
{
}

