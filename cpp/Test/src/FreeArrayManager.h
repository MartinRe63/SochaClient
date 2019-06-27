#pragma once
#include "assert.h"
typedef struct
{
	unsigned int isFree : 1, nextFreeId : 31;
} nextFreeId; 

class FreeArrayManager
{
public:
	FreeArrayManager( nextFreeId* firstArrayEntry, int ArrayLength, int ItemSize );
	~FreeArrayManager();

	inline int ReserveNextFree()
	{
		int ret = firstFreeId;
		// _ASSERT_EXPR( ret < maxFreeId, "No free Element available.");
		assert( ret < itemsAvailable );
		if (ret >= itemsAvailable || ! HasFreeItemsAvailable())
		{
			// cout << "test" << endl; // (std::to_string("Itemsize:") + std::to_string(itemSize) + " no more element available.") << endl;
			throw std::exception(); // this should force an out of bound exception, if the result -1 is not checked.
		}
		nextFreeId* item = GetItem(firstFreeId);  // GetItem(firstFreeId);
		// _ASSERT_EXPR ( item->isFree, "Unknown Software Issue - firstFreeId is not a free item.");
		assert( item->isFree );
		firstFreeId = item->nextFreeId;
		itemsAvailable--;
		item->isFree = 0;
		return (ret);
	}

	void DisposeAt(int);

	inline bool IsUsed(int ToCheckId )
	{
		nextFreeId* item = GetItem( ToCheckId );
		return ! item->isFree;
	}

	inline int FreeItemsAvailableCount(void)
	{
		return itemsAvailable;
	}

	inline bool HasFreeItemsAvailable(void)
	{
		return itemsAvailable > 0;
	}

protected:
	inline nextFreeId* GetItem(int idx)
	{
		return (nextFreeId*)(((char *)toManageOn) + idx * itemSize);
	}
	int itemsAvailable;
	int arrayLength;

private:
	nextFreeId* toManageOn;
	int itemSize;
	int firstFreeId;
};

