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

int IntListManager::ReserveList(int size)
{
	int listIdx = fam->ReserveNextFree();
#ifndef VectorBased
	data[listIdx].f.h.nextIdx = endMarker;
	data[listIdx].f.length = 0;
#else 
	data[listIdx].nodeList = new vector<smallNode>( size );
#endif
	return listIdx;
};

void IntListManager::Add(int ListIdx, int BlockIdx, int RelativeIdx, smallNode NewItem)
{
#ifndef VectorBased
	data[ListIdx].f.length++;
	data[BlockIdx].s.item[RelativeIdx] = NewItem;
#else 
	throw "not implemendet."; // data[ListIdx].nodeList->push_back(NewItem);
#endif
};

void IntListManager::Release(int ListIdx)
{
#ifndef VectorBased
	while (ListIdx != endMarker)
	{
		int listIdx = ListIdx;
		ListIdx = data[listIdx].f.h.nextIdx;
		fam->DisposeAt(listIdx);
	}
#else 
	delete data[ListIdx].nodeList;
	fam->DisposeAt(ListIdx);
#endif
};

IntListManager::ReadIterator* IntListManager::GetReadIterator(int ListIdx)
{
	return new IntListManager::ReadIterator(ListIdx, this);
}

IntListManager::ReadIterator* IntListManager::GetReadIterator()
{
	return new IntListManager::ReadIterator(-1, this);
}

IntListManager::WriteIterator* IntListManager::GetWriteIterator(int ListIdx)
{
	return new IntListManager::WriteIterator(ListIdx, this);
}


IntListManager::ReadIterator::ReadIterator( int ListIdx, IntListManager* Ilm )
{
	ilm = Ilm;
	Init(ListIdx);
}
void IntListManager::ReadIterator::Init(int ListIdx)
{
	blockIdx = ListIdx;
	listIdx = ListIdx;
	virtualIdx = 0;
}

#ifndef VectorBased
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
#endif

IntListManager::WriteIterator::WriteIterator( int ListIdx, IntListManager* Ilm )
{
	blockIdx = ListIdx;
#ifndef VectorBased
	relativBlockIdx = 1;
#else
	relativBlockIdx = 0;
#endif
	listIdx = ListIdx;
	ilm = Ilm;
}

#ifndef VectorBased
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
#endif

