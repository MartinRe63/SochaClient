#include <crtdbg.h>
#include "FreeArrayManager.h"

const int maxFreeId = ~ ( 1 << 31 );

FreeArrayManager::FreeArrayManager(nextFreeId* firstArrayEntry, int ArrayLength, int ItemSize)
{
	toManageOn = firstArrayEntry;
	arrayLength = ArrayLength;
	itemSize = ItemSize;
	firstFreeId = 0;
	nextFreeId* item = toManageOn;
	for (int k; k < arrayLength-1; k++) {
		item->isFree = 1;
		item->nextFreeId = k + 1;
		item += itemSize;
	}
	item->isFree = 1;
	item->nextFreeId = maxFreeId;
}

nextFreeId* FreeArrayManager::GetItem(int idx)
{
	return toManageOn + idx * itemSize;
}
int FreeArrayManager::ReserveNextFree()
{
	int ret = firstFreeId;
	nextFreeId* item = toManageOn + firstFreeId * itemSize; // GetItem(firstFreeId);
	_ASSERT_EXPR(item->nextFreeId < maxFreeId, "No free Element available.");
	_ASSERT_EXPR ( item->isFree, "Unknows Software Issue - firstFreeId is not a free item.");
	if (item->nextFreeId >= maxFreeId)
		ret = -1;
	firstFreeId = item->nextFreeId;
	item->isFree = 0;
	return (ret);
}
void FreeArrayManager::DisposeAt(int ToFreeId)
{
	nextFreeId* item = toManageOn + ToFreeId * itemSize; // GetItem(ToFreeId);
	_ASSERT_EXPR( ! item->isFree , "The element is isready a free element. Maybe the free bit was overridden.");
	item->isFree = 1;
	item->nextFreeId = firstFreeId; 
	firstFreeId = ToFreeId;
}


FreeArrayManager::~FreeArrayManager()
{
}
