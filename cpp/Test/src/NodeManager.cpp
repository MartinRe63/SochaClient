#include "NodeManager.h"



NodeManager::NodeManager(int nodeCount)
{
	memory = new m[nodeCount];
	FreeArrayManager* fam = new FreeArrayManager( &memory[0].free, nodeCount / 50 );
}


NodeManager::~NodeManager()
{
}
