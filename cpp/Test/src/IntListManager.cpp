#pragma once

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

void IntListManager::Add(int ListIdx, int BlockIdx, int RelativeIdx, smallNode NewItem)
{
	data[ListIdx].f.length++;
	data[BlockIdx].s.item[RelativeIdx] = NewItem;

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

IntListManager::ReadIterator* IntListManager::GetReadIterator(int ListIdx)
{
	return new IntListManager::ReadIterator(ListIdx, this);
}

IntListManager::WriteIterator* IntListManager::GetWriteIterator(int ListIdx)
{
	return new IntListManager::WriteIterator(ListIdx, this);
}


IntListManager::ReadIterator::ReadIterator( int ListIdx, IntListManager* Ilm )
{
	blockIdx = ListIdx;
	virtualIdx = 0;
	listIdx = ListIdx;
	ilm = Ilm;
}
smallNode* IntListManager::ReadIterator::GetNextItem()
{
	if ( virtualIdx >= ilm->GetLength( listIdx ) )
	{
		// throw std::out_of_range("End of the list is reached.");
		return 0;
	}
	int nextId = virtualIdx+1;
    if ( nextId % blockDataSize == 0 )
    {
    	blockIdx = ilm->GetBlockIdx(listIdx, virtualIdx);
    }
    smallNode* item = &ilm->data[blockIdx].s.item[nextId % blockDataSize];
    virtualIdx = nextId;
    return item;
}


IntListManager::WriteIterator::WriteIterator( int ListIdx, IntListManager* Ilm )
{
	blockIdx = ListIdx;
	relativBlockIdx = 1;
	listIdx = ListIdx;
	ilm = Ilm;
}

void IntListManager::WriteIterator::AddItem( smallNode n )
{
	ilm->Add(listIdx, blockIdx, relativBlockIdx, n);
	relativBlockIdx++;
	if ( relativBlockIdx == blockDataSize )
	{
		relativBlockIdx = 0;
		int newBlockIdx = ilm->fam->ReserveNextFree();
		ilm->data[blockIdx].s.h.nextIdx = newBlockIdx; 
		blockIdx = newBlockIdx;
		ilm->data[blockIdx].s.h.nextIdx = endMarker;
	}
}
