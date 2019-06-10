//============================================================================
// Name        : Test.cpp
// Author      : M
// Version     :
// Copyright   : Your copyright notice
// Description : Socha Player in C++, Ansi-style
//============================================================================

#include <iostream>
#include <stdlib.h>
#include "MaskManager.h"
#include "BitManager.h"
#include "NodeManager.h"
#include "FreeArrayManager.h"
#include "IntListManager.h"

using namespace std;

void testListManager()
{
	int size = 100;
	int* childArr = new int[size];
	for (int k = 0; k < size; k++)
	{
		childArr[k] = -1;
	}

	IntListManager* ilm = new IntListManager( 10000 );


	for (int k = 0; k < 20000; k++)
	{
		int id = rand() % 100;
		int childListId;
		switch (rand() % 3)
		{
		case 0:
			// Reserve Items
			if ( childArr[id] == -1 )
			{
				childListId = childArr[id] = ilm->ReserveList();
				int lth = rand() % 60;
				smallNode j; // = smallNode(0, 0);
				j.isSuperPackedMove = 0;
				j.nodeIdx = -1;
				IntListManager::WriteIterator* it = ilm->GetWriteIterator(childListId);
				for ( j.nodeIdx = 0; j.nodeIdx < lth; j.nodeIdx++ )
				{
					it->AddItem( j );
				}
			}
			break;

		case 1:
			// check Item content
			if ( ( childListId = childArr[id] ) != -1)
			{
				int lth = ilm->GetLength(childListId);
				IntListManager::ReadIterator* it = ilm->GetReadIterator(childListId);
				for (int j = 0; j < lth; j++)
					_ASSERT_EXPR(it->GetNextItem()->nodeIdx == j, "Software issue - wrong Data found in the List.");
			}
			break;
		case 2:
			// dispose Items
			if ((childListId = childArr[id]) != -1)
			{
				ilm->Release(childListId);
				childArr[id] = -1;
			}
			break;
		}
	}
}

void testFreeArrayManager() 
{
	
	int size = 100;
	m* TestArr = new m[size];

	FreeArrayManager* fam = new FreeArrayManager( &TestArr[0].free, size, sizeof(node));
	int id; 
	for ( int k = 0; k < 10000; k++ )
	{
		switch ( rand() % 3 )
		{
			case 0:
				// Reserve Items
				if ( fam->HasFreeItemsAvailable() )
				{
					id = fam->ReserveNextFree();
					TestArr[id].node.v.visits = id;
				}
				break;

			case 1:
				// check Item content
				id = rand() % size;
				_ASSERT_EXPR ( ! fam->IsUsed(id) || TestArr[id].node.v.visits == id, "Test Data are wrong." );
				break;
			case 2:
				// dispose Items
				id = rand() % size;
				if ( fam->IsUsed(id) )
				{
					TestArr[id].node.v.visits = 0;
					fam->DisposeAt( id );
				}
				break;
		}
	}
}

int main() {
	srand( 123456789 );
	MaskManager::initMasks();
	// testFreeArrayManager();
	testListManager();

	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
