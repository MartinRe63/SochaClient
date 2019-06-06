#include "NodeManager.h"


NodeManager::NodeManager(int nodeCount)
{
    static Random r = new Random();
    static double epsilon = (float) 1e-6;

	long crake[] = new long[2];
	// node properties
	int[] fishMove;
	long[] visits;
	float[] totalValue;
	int childListPos[];

	// Array to manage the child lists
	int childArr[];

	memory = new m[nodeCount];
	FreeArrayManager* fam = new FreeArrayManager( &memory[0].free, nodeCount / 50, sizeof ( node ) );
}


NodeManager::~NodeManager()
{
}
