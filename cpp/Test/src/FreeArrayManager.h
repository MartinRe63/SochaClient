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
private:
	nextFreeId* GetItem(int idx);
	nextFreeId* toManageOn;
	int itemSize;
	int arrayLength;
	int firstFreeId;
};

