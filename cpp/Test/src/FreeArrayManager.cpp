#include <crtdbg.h>
#include "FreeArrayManager.h"

const int maxFreeId = ~(1 << 31);
FreeArrayManager::FreeArrayManager(nextFreeId* firstArrayEntry, int ArrayLength, int ItemSize)
{
	toManageOn = firstArrayEntry;
	arrayLength = ArrayLength;
	itemSize = ItemSize;
	firstFreeId = 0;
	nextFreeId* item = toManageOn;
	for (int k = 0; k < arrayLength-1; k++) {
		item->isFree = 1;
		item->nextFreeId = k + 1;
		// !!! nextFreeId is pointing to 4 byte only !!! argh - dirty - better template usage
		item = (nextFreeId* )(((char *)item ) + itemSize );
	}
	item->isFree = 1;
	item->nextFreeId = maxFreeId; // once the last free item is used, firstFreeId = maxFreeId;
	itemsAvailable = ArrayLength;
}

int FreeArrayManager::ReserveNextFree()
{
	int ret = firstFreeId;
	_ASSERT_EXPR( ret < maxFreeId, "No free Element available.");
	if (ret >= maxFreeId || ! HasFreeItemsAvailable())
		return -1;  // this should force an out of bound exception, if the result -1 is not checked.
	nextFreeId* item = GetItem(firstFreeId);  // GetItem(firstFreeId);
	_ASSERT_EXPR ( item->isFree, "Unknown Software Issue - firstFreeId is not a free item.");
	firstFreeId = item->nextFreeId;
	itemsAvailable--;
	item->isFree = 0;
	return (ret);
}
void FreeArrayManager::DisposeAt(int ToFreeId)
{
	nextFreeId* item = GetItem( ToFreeId ); // GetItem(ToFreeId);
	_ASSERT_EXPR( ! item->isFree , "The element is already a free element. Maybe the free bit was overridden.");
	_ASSERT_EXPR( ToFreeId < arrayLength && ToFreeId >= 0, "The ID is out of range.");
	item->isFree = 1;
	item->nextFreeId = firstFreeId; 
	firstFreeId = ToFreeId;
	itemsAvailable++;
}

FreeArrayManager::~FreeArrayManager()
{
}
