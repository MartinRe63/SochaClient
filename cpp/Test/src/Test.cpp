//============================================================================
// Name        : Test.cpp
// Author      : M
// Version     :
// Copyright   : Your copyright notice
// Description : Socha Player in C++, Ansi-style
//============================================================================

#include <iostream>
#include <ctime>
#include <ratio>
#include <chrono>

#include <stdlib.h>


#include "MaskManager.h"
#include "BitManager.h"
#include "NodeManager.h"
#include "FreeArrayManager.h"
#include "IntListManager.h"

using namespace std;
using namespace std::chrono;
class ElapsedTimer {
	// allows for easy reporting of elapsed time
	high_resolution_clock::time_point t1;

public:
	ElapsedTimer() {
		t1 = high_resolution_clock::now();
	}

	long elapsed() {
		duration<double, std::milli> time_span = high_resolution_clock::now() - t1;
		return time_span.count();
	}

	void reset() {
		t1 = high_resolution_clock::now();
	}

	std::string toString() {
		// now resets the timer...
		std::string ret = elapsed() + " ms elapsed";
		reset();
		return ret;
	}

	static void testElapsed(std::string args[]) 
	{
		ElapsedTimer* t = new ElapsedTimer();
		cout<<("ms elasped: " + t->elapsed());
	}
};


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
				j.sPM.isSuperPackedMove = 0;
				j.node.idx = -1;
				IntListManager::WriteIterator* it = ilm->GetWriteIterator(childListId);
				for ( j.node.idx = 0; j.node.idx < lth; j.node.idx++ )
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
					_ASSERT_EXPR(it->GetNextItem()->node.idx == j, "Software issue - wrong Data found in the List.");
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

void testGaming(board Pos)
{
	ElapsedTimer* t = new ElapsedTimer();

	NodeManager* redNM = NULL;
	NodeManager* blueNM = NULL;
	packedMove redMove = 0;
	packedMove blueMove = 0;
	int moveCnt = 0;
	long long ms = 0;

	long expands = 0;
	try
	{
		while (true)
		{
			t->reset();
			if (redNM == NULL)
				redNM = new NodeManager(10000000, 0, Pos, 0);
			else
				redNM->DisposeTree(redMove, blueMove);

			for (int k = 0; k < 10000; k++)
			{
				redNM->SelectAction( true );
				// System.out.println( nm.LastPositionToString() );
				expands++;
			}
			ms += t->elapsed();

			moveCnt++;
			MoveManager::addMoveToBoard(Pos, 0, (redMove = redNM->BestMove()));
			cout << (std::to_string(moveCnt) + "." + MoveManager::packMoveToString(redMove) + " Depth:" + std::to_string(redNM->GetMaxDepth())) << endl;
			cout << redNM->ValuesToString() << endl;
			cout << BoardManager::ToString(Pos) << endl;
			redNM->ResetMaxDepth();
			t->reset();

			if (blueNM == NULL)
				blueNM = new NodeManager(10000000, 1, Pos, 1);
			else
				blueNM->DisposeTree(blueMove, redMove);
			for (int k = 0; k < 5000; k++)
			{
				blueNM->SelectAction(true);
				// System.out.println( nm.LastPositionToString() );
				expands++;
			}
			ms += t->elapsed();

			moveCnt++;
			MoveManager::addMoveToBoard(Pos, 1, (blueMove = blueNM->BestMove()));
			cout << (std::to_string(moveCnt) + "." + MoveManager::packMoveToString(blueMove) + " Depth:" + std::to_string(blueNM->GetMaxDepth())) << endl;
			cout << blueNM->ValuesToString() << endl;
			cout << BoardManager::ToString(Pos) << endl;
			blueNM->ResetMaxDepth();
			t->reset();

			boardpane blockList[2][16]; // all blocks of a board
			int blockCnt[2]; // blocklist for red and blue
			BoardManager::GetValue(Pos, 1, blockList, blockCnt, moveCnt, 0);

			/* Total memory currently in use by the JVM */
			// System.out.println("Total memory (bytes): " +  Runtime.getRuntime().totalMemory() );
		}


	}
	catch (GameEndException& e)
	{
		if (e.GetResult() == 1.0)
			cout << "Blau hat gewonnen." << endl;
		else
			cout << "Rot hat gewonnen." << endl;
	}
	cout << "Nano Seconds per playout = " + std::to_string((ms * 1000000) / expands) << endl;
	std::string i;
	cin >> i;
}

int main() {
	srand( 123456789 );
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);
	MaskManager::initMasks();
	// testFreeArrayManager();
	// testListManager();
	board b;
	BoardManager::FromString(std::string(".11111111." \
		"0........0" \
		"0........0" \
		"0.....C..0" \
		"0........0" \
		"0........0" \
		"0..C.....0" \
		"0........0" \
		"0........0" \
		".11111111." ), b);
	
	testGaming( b );
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
