#pragma once
typedef struct
{
	unsigned int isFree : 1, nextFreeId : 31;
} nextFreeId; 

class FreeArrayManager
{
public:
	FreeArrayManager( nextFreeId* firstArrayEntry, int ArrayLength, int ItemSize );
	~FreeArrayManager();
	int ReserveNextFree();
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

	int itemsAvailable;

private:
	nextFreeId* GetItem(int idx);
	nextFreeId* toManageOn;
	int itemSize;
	int arrayLength;
	int firstFreeId;
};

