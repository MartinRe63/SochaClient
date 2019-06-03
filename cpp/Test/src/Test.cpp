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

using namespace std;

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

	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
