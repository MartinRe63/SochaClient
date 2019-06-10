#include "NodeManager.h"
#include "BoardManager.h"

static double epsilon = (float) 1e-6;


NodeManager::NodeManager(int NodeCount, int MyColor, board FirstBoard, int FirstMoveDepth)
{

	boardpane crake;
	// node properties
	//
	//int[] fishMove;
	//long[] visits;
	//float[] totalValue;
	//int childListPos[];
	//
	//// Array to manage the child lists
	//int childArr[];

	memory = new m[NodeCount];
	FreeArrayManager* fam = new FreeArrayManager( &memory[0].free, NodeCount / 50, sizeof ( node ) );
	firstBoard = FirstBoard;
	
	firstMoveColor = MyColor;
	firstMoveDepth = FirstMoveDepth;
}


NodeManager::~NodeManager()
{
}

bool NodeManager::hasChild( smallNode sN )
{
	if (sN.isSuperPackedMove == 1)
		return false;
	return memory[sN.nodeIdx].node.child.id > -1;
}

smallNode NodeManager::superPackMove(coordinates fromPos, coordinates toPos)
{
	smallNode ret; 
	ret.isSuperPackedMove = 1;
	ret.packedMove = toPos * 100 + fromPos;
	return ret;
}
