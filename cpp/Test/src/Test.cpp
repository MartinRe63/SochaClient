//============================================================================
// Name        : Test.cpp
// Author      : M
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "MaskManager.h"
#include "BitManager.h"
#include "NodeManager.h"

using namespace std;

void testFreeArrayManager() 
{
	
	int size = 100;
	m* TestArr = new m[size];

	FreeArrayManager* fam = new FreeArrayManager( &TestArr[0].free, size, sizeof(node));
	int id; 
	int k = 0;
	while ((id = fam->ReserveNextFree()) >= 0)
	{
		TestArr[id].node.v.visits = k;

	}

}

int main() {
	MaskManager::initMasks();

	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
