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

using namespace std;

int main() {
	MaskManager::initMasks();

	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
