#include <exception>
#include <assert.h>
#include <iostream>
#include <crtdbg.h>
#include <stdlib.h>

#include "FreeArrayManager.h"

using namespace std;

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
