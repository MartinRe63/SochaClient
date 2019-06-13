#include <math.h>
#include <float.h>
#include <stdlib.h>
#include "NodeManager.h"
#include "BoardManager.h"

static double epsilon = (float) 1e-6;
static unsigned nullChildId = ~(1 << 30);
static unsigned maxVisits = ~(1 << 30);

NodeManager::NodeManager(int NodeCount, int MyColor, board FirstBoard, int FirstMoveDepth)
{

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
	firstNodeIdx.sPM.isSuperPackedMove = 1;
	firstNodeIdx.sPM.packedMove = 0;
	// fam->ReserveNextFree();
	// InitNode(firstNodeIdx, 0, 1);
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

void NodeManager::expandNode(smallNode* SN, int moveColor, board position, int depth)
{
	// fill the childList with valid moves based on the current position
	_ASSERT_EXPR( SN->sPM.isSuperPackedMove == 1, "This node is already expanded.");

	//
	// expand the packed Node Info
	//
	int nodeId = fam->ReserveNextFree();
	InitNode(nodeId, MoveManager::superPack2packMove(SN->sPM), depth < 60 ? 1 : maxVisits);
	SN->isNoNodeIdx = 0;
	SN->nodeIdx = nodeId;
	_ASSERT_EXPR( memory[nodeId].node.child.id == nullChildId, "Software Issue - Childs available.");

	//
	// and expand all his children
	//
	int childListId = memory[nodeId].node.child.id = ilm->ReserveList();
	int moveCnt = MoveManager::getMoveList(position, moveColor, moveList);
	IntListManager::WriteIterator* wIt = ilm->GetWriteIterator(childListId);
	for (int i = 0; i < moveCnt; i++) {
		smallNode sN;
		sN.sPM = MoveManager::superPackMove(moveList[i]);
		wIt->AddItem(sN); // children[i] = new TreeNode();
	}
}

double NodeManager::rollOut( int color, board pos, int depth )
{
	// calculate the value of this position
	// here to count number of blocks and calculate the block value
	// check if this is the secondMoveColor to check if moveColor will win = 1 or loss = 0
	return BoardManager::GetValue(pos, color, blockList, blockCnt, depth, firstMoveDepth);
}

smallNode* NodeManager::selectMove(smallNode* sN) 
{
	smallNode* selectedNode = 0;
	double bestValue = DBL_MIN; //  Double.NEGATIVE_INFINITY;
	if (sN->sPM.isSuperPackedMove == 1)
		return sN;
	m* node = &memory[sN->nodeIdx];
	_ASSERT_EXPR(node->node.child.id != nullChildId, "each node should have a child list.");
	IntListManager::ReadIterator* i = ilm->GetReadIterator(node->node.child.id);
	smallNode* iSN = i->GetNextItem();
	while ( iSN != 0 ) {
		int moveValue;
		long visitCnt = 1;
		double totValue = 0.5;
		if (iSN->sPM.isSuperPackedMove == 1)
		{
			moveValue = MoveManager::moveValueSPM(iSN->sPM);
		}
		else
		{
			m* childNode = &memory[iSN->nodeIdx];
			moveValue = MoveManager::moveValuePM(childNode->node.v.move);
			visitCnt = childNode->node.v.visits;
			totValue = childNode->node.totValue;
		}

		// childvisits == 0, when the end is reached
		double r = 1 / (rand() + 1);
		double uctValue =
			totValue / (visitCnt + epsilon) +
			std::sqrt(std::log(node->node.v.visits + 1) / (visitCnt + epsilon)) +
			r * epsilon + moveValue * 10 * epsilon;

		// small random number to break ties randomly in unexpanded nodes
		// System.out.println("UCT value = " + uctValue + "  tot = " + totValue + " " + PosManager.packMoveToString(move));

		if (uctValue > bestValue) {
			selectedNode = iSN;
			bestValue = uctValue;
		}
		iSN = i->GetNextItem();
	}
	// System.out.println("Returning: " + selected);
	_ASSERT_EXPR( selectedNode != 0, "Child not found." );
	return selectedNode;
}

void NodeManager::updateStat( int nodeId, double value )
{
	memory[nodeId].node.v.visits ++;
	memory[nodeId].node.totValue += (float)value;
}


void NodeManager::selectAction(bool oneCycle) 
{

	while (!stopSelection)
	{
		if (oneCycle)
			stopSelection = true;
		visitedCnt = 0;
		smallNode* cur = &firstNodeIdx;
		visited[visitedCnt++] = cur;
		int nextMoveColor = firstMoveColor;
		BoardManager::Copy(firstPosition, pos);

		while ( cur->sPM.isSuperPackedMove != 1 ) // ! isLeaf
		{
			cur = selectMove(cur);

			// System.out.println("Adding: " + cur);
			visited[visitedCnt++] = cur; // visited.add(cur);
			packedMove pM;
			if ( cur->sPM.isSuperPackedMove == 1 )
			{
				pM = MoveManager::superPack2packMove(cur->sPM);
			}
			else
			{
				pM = memory[cur->nodeIdx].node.v.move;
			}
			MoveManager::addMoveToBoard( pos, nextMoveColor, pM );
			nextMoveColor = (nextMoveColor + 1) % 2;
		}
		// here cur is always a super packed move
		if ( cur->sPM.isGameEndNode == 0 )
		{
			// expand only if this is no game end node
			expandNode(cur, nextMoveColor, pos, visitedCnt);               //  cur.expand();
			cur = selectMove(cur);                                         // TreeNode newNode = cur.select();

			visited[visitedCnt++] = cur;                                   // visited.add(newNode);
			MoveManager::addMoveToBoard( pos, nextMoveColor, MoveManager::superPack2packMove(cur->sPM) );
			nextMoveColor = (nextMoveColor + 1) % 2;
		}

		_ASSERT_EXPR( cur->sPM.isSuperPackedMove == 1, "Why is this not a small node having a move only?");

		visitedCnt--; // back to the current level it's also showing the current depth
		nextMoveColor = (nextMoveColor + 1) % 2;  // switch also the color back

		double value = 0;

		if ( cur->sPM.isGameEndNode == 0 )
		{
			try
			{
			 	value = rollOut( nextMoveColor, pos, visitedCnt );
			}
			catch ( GameEndException e)
			{
				value = e.GetResult();
				cur->sPM.isGameEndNode = 1;
				cur->sPM.totValue = (unsigned)value*2;
			}
		}
		else
		{
			value = ((double)cur->sPM.totValue) / 2;
		}
		// int visitedMoveColor = nextMoveColor;
		maxDepth = (visitedCnt > maxDepth) ? visitedCnt : maxDepth;
		for (int k = visitedCnt; k >= 0; k--)                  // for (TreeNode node : visited) 
		{
			// System.out.println(node);
			int visitedNode = visited[k]->nodeIdx;
			if ( visited[k]->isNoNodeIdx == 0 )
			{
				updateStat(visitedNode, value);
			}
			// node.updateStats(value);

			// based on some internet python code values to be added and inverted
			value = 1 - value;
		}
		// mark game end node with visited = -1
	}
	stopSelection = false;
}
