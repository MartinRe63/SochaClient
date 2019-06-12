#include "NodeManager.h"
#include "BoardManager.h"

static double epsilon = (float) 1e-6;
static unsigned nullChildId = ~(1 << 30);

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
	fam = new FreeArrayManager( &memory[0].free, NodeCount / 50, sizeof ( node ) );
	ilm = new IntListManager(NodeCount / 5);
	firstBoard = FirstBoard;
	
	firstMoveColor = MyColor;
	firstMoveDepth = FirstMoveDepth;
}


NodeManager::~NodeManager()
{
}

bool NodeManager::hasChild( smallNode sN )
{
	if (sN.sPM.isSuperPackedMove == 1)
		return false;
	return memory[sN.nodeIdx].node.child.id > -1;
}

void NodeManager::InitFirstNode()
{
	firstNodeIdx = fam->ReserveNextFree();
	InitNode(firstNodeIdx, 0, 1);
}

void NodeManager::InitNode(int nodeId, packedMove move, long visitCnt)
{
	m* node = &memory[nodeId];
	node->node.child.id = nullChildId;
	node->node.totValue = 0.5f;
	node->node.v.visits = visitCnt;
	node->node.v.move = move;
	// ((IntListManager*)ilm)->GetLength(i);
}

void NodeManager::expandNode(int nodeId, int moveColor, board position, int depth)
{
	// fill the childList with valid moves based on the current position
	_ASSERT_EXPR( fam->IsUsed(nodeId), "Software Issue - Node is not available." );
	_ASSERT_EXPR( memory[nodeId].node.child.id == nullChildId, "Software Issue - Childs available.");

	int childListId = memory[nodeId].node.child.id = ilm->ReserveList();
	int moveCnt = MoveManager::getMoveList(position, moveColor, moveList);
	IntListManager::WriteIterator* wIt = ilm->GetWriteIterator(childListId);
	for (int i = 0; i < moveCnt; i++) {
		int childNodeId;
		smallNode sN; 
		sN.sPM = MoveManager::superPackMove(moveList[i]);
		wIt->AddItem(sN); // children[i] = new TreeNode();
		InitNode(childNodeId, moveList[i], depth < 60 ? 1 : LONG_MAX);
	}
}
