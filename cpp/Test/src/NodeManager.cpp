#include <math.h>
#include <float.h>
#include <stdlib.h>
#include <list>
#include "NodeManager.h"
#include "BoardManager.h"

static double epsilon = (float) 1e-6;
static unsigned nullChildId = (1 << 30)-1;
static unsigned maxVisits = (1 << 30)-1;

NodeManager::NodeManager(int NodeCount, int MyColor, board FirstBoard, int FirstMoveDepth, double DeepFactor)
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
	fam = new FreeArrayManager( &memory[0].free, NodeCount / 40, sizeof ( node ) );
	ilm = new IntListManager(NodeCount / 5);
	firstBoard = FirstBoard;
	
	firstMoveColor = MyColor;
	currentMoveColor = MyColor;
	firstMoveDepth = FirstMoveDepth;
	InitFirstNode();
	CopyNode(firstNodeIdx, previousNodeIdx);
	deepFactor = DeepFactor;
}


NodeManager::~NodeManager()
{
}

void NodeManager::ReInit(int MyColor, board FirstBoard, int FirstMoveDepth)
{
	firstMoveColor = MyColor;
	firstBoard = FirstBoard;
	firstMoveDepth = FirstMoveDepth;
}

bool NodeManager::hasChild( smallNode sN )
{
	if (sN.sPM.isSuperPackedMove == 1)
		return false;
	return memory[sN.node.idx].node.childs.id > -1;
}

void NodeManager::InitFirstNode()
{
	firstNodeIdx.sPM.isSuperPackedMove = 1;
	firstNodeIdx.sPM.packedMove = 0;
	firstNodeIdx.sPM.isGameEndNode = 0;
	firstNodeIdx.sPM.totValue = 1;
	// fam->ReserveNextFree();
	// InitNode(firstNodeIdx, 0, 1);
}

void NodeManager::InitNode(int nodeId, packedMove move, long visitCnt)
{
	m* node = &memory[nodeId];
	node->node.childs.id = nullChildId;
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
	SN->node.isNoIdx = 0;
	SN->node.idx = nodeId;

	// better to have a copy methode
	if (SN == &firstNodeIdx && previousNodeIdx.sPM.isSuperPackedMove)
	{
		previousNodeIdx.node.idx = firstNodeIdx.node.idx;
		previousNodeIdx.node.isNoIdx = firstNodeIdx.node.isNoIdx;
	}

	_ASSERT_EXPR( memory[nodeId].node.childs.id == nullChildId, "Software Issue - Childs available.");

	//
	// and expand all his children
	//
	int childListId = memory[nodeId].node.childs.id = ilm->ReserveList();
	int moveCnt = MoveManager::getMoveList(position, moveColor, moveList);
	IntListManager::WriteIterator* wIt = ilm->GetWriteIterator(childListId);
	for (int i = 0; i < moveCnt; i++) {
		smallNode sN;
		sN.sPM = MoveManager::superPackMove(moveList[i]);
		wIt->AddItem(sN); // children[i] = new TreeNode();
	}
}

double NodeManager::rollOut( int color, board pos, int depth, bool& gameEnd )
{
	// calculate the value of this position
	// here to count number of blocks and calculate the block value
	// check if this is the secondMoveColor to check if moveColor will win = 1 or loss = 0
	return BoardManager::GetValue(pos, color, blockList, blockCnt, depth, firstMoveDepth, gameEnd );
}

smallNode* NodeManager::selectMove(smallNode* sN) 
{
	smallNode* selectedNode = 0;
	double bestValue = DBL_MIN; //  Double.NEGATIVE_INFINITY;
	if (sN->sPM.isSuperPackedMove == 1)
		return sN;
	m* node = &memory[sN->node.idx];
	_ASSERT_EXPR( node->free.isFree == 0 , "node should not be free." );
	_ASSERT_EXPR( node->node.childs.id != nullChildId, "each node should have a child list." );
	IntListManager::ReadIterator* i = ilm->GetReadIterator(node->node.childs.id);
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
			m* childNode = &memory[iSN->node.idx];
			moveValue = MoveManager::moveValuePM(childNode->node.v.move);
			visitCnt = childNode->node.v.visits;
			totValue = childNode->node.totValue;
		}

		// childvisits == 0, when the end is reached
		long lr = rand() % 1024; 
		double r = 1.0 / (lr + 1);
		double uctValue =
			totValue / (visitCnt + epsilon) +
			std::sqrt(deepFactor * std::log(node->node.v.visits + 1) / (visitCnt + epsilon)) +
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


void NodeManager::SelectAction(bool oneCycle) 
{

	while (!stopSelection)
	{
		if (oneCycle)
			stopSelection = true;
		visitedCnt = 0;
		smallNode* cur = &firstNodeIdx;
		visited[visitedCnt++] = cur;
		int nextMoveColor = currentMoveColor;
		BoardManager::Copy(firstBoard, pos);

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
				pM = memory[cur->node.idx].node.v.move;
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
        double result = 0;
        bool gameEnd;
		if ( cur->sPM.isGameEndNode == 0 )
		{
			value = rollOut( nextMoveColor, pos, visitedCnt, gameEnd );
			if ( gameEnd )
			{
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
			int visitedNode = visited[k]->node.idx;
			if ( visited[k]->node.isNoIdx == 0 )
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

void NodeManager::releaseNode(smallNode NodeId, int NodeIdToExclude) 
{
	if ( NodeId.sPM.isSuperPackedMove == 0 )
	{
		int nodeIdx = NodeId.node.idx;
		if (nodeIdx != NodeIdToExclude)
		{
			int ListIdx = memory[NodeId.node.idx].node.childs.id;
			IntListManager::ReadIterator* ri = ilm->GetReadIterator(ListIdx);
			smallNode* sN = ri->GetNextItem();
			while (sN != NULL)
			{
				releaseNode(*sN, NodeIdToExclude);
				sN = ri->GetNextItem();
			}
			ilm->Release(ListIdx);
			fam->DisposeAt(nodeIdx);
		}
	}
}


smallNode* NodeManager::findNode(int nodeIdx, packedMove move) 
{
	IntListManager::ReadIterator* ri = ilm->GetReadIterator(memory[nodeIdx].node.childs.id);
	smallNode* sN = ri->GetNextItem();
	while ( sN != NULL )
	{
		packedMove nodeMove;
		if (sN->sPM.isSuperPackedMove == 1)
			nodeMove = MoveManager::superPack2packMove(sN->sPM);
		else
			nodeMove = memory[sN->node.idx].node.v.move;
		if (move == nodeMove)
		{
			return sN;
		}
		sN = ri->GetNextItem();
	}
	return NULL;
}

void NodeManager::DisposeTree()
{
	// smallNode* node = findNode(firstNodeIdx.node.idx, move1);
	// _ASSERT_EXPR(node != NULL, "The first move must be a child of the first Node.");
	// movePosition(nodeId, fishMove, pos, firstMoveColor );

	// node = findNode(node->node.idx, move2);
	
	if (previousNodeIdx.node.idx != firstNodeIdx.node.idx || previousNodeIdx.node.isNoIdx != firstNodeIdx.node.isNoIdx )
		// movePosition(nodeId, fishMove, pos, (firstMoveColor+1)%2 );
	{
		if (previousNodeIdx.sPM.isSuperPackedMove == 0)
			releaseNode(previousNodeIdx, firstNodeIdx.node.idx);
		else
			releaseNode(previousNodeIdx, -1);
	}
	previousNodeIdx.node = firstNodeIdx.node;
	// if (firstNode < 0)
	//{
	//	InitFirstNode();
	//}
}

void NodeManager::ImplementMoveToTree(packedMove move)
{
	smallNode* node = findNode(firstNodeIdx.node.idx, move);
	if ( node )
	{
		CopyNode( *node, firstNodeIdx );
	}
	else
	{
		// previous will stay until it's all released
		this->InitFirstNode();
	}
	firstMoveDepth += 1;
	currentMoveColor = ! currentMoveColor;
}

void NodeManager::CopyNode( smallNode& from, smallNode& to)
{
	if ( from.sPM.isSuperPackedMove )
	{
		to.sPM = from.sPM;
	}
	else
	{
		to.node = from.node;
	}
}

bool NodeManager::IsDisposeRequired()
{
	return (previousNodeIdx.node.isNoIdx != firstNodeIdx.node.isNoIdx || previousNodeIdx.node.idx != firstNodeIdx.node.idx );
}


packedMove NodeManager::BestMove()
{
	int ret = -1;

	double maxTotal = -1.0;
	packedMove bestMove = 0;

	IntListManager::ReadIterator* ri = ilm->GetReadIterator(memory[firstNodeIdx.node.idx].node.childs.id);
	smallNode* sN = ri->GetNextItem();
	while( sN != 0 )
	{
		double val;
		packedMove pM;
		if (sN->sPM.isSuperPackedMove == 1)
		{
			val = 0.5 + MoveManager::moveValueSPM(sN->sPM) * epsilon;
			pM = MoveManager::superPack2packMove(sN->sPM);
		}
		else
		{
			node n = memory[sN->node.idx].node;
			val = n.totValue / n.v.visits;
			pM = n.v.move;
		}
		if (val > maxTotal)
		{
			maxTotal = val;
			bestMove = pM;
		}
		sN = ri->GetNextItem();
	}

	return bestMove;
}

int NodeManager::GetMaxDepth()
{
	return maxDepth;
}

void NodeManager::ResetMaxDepth()
{
	maxDepth = 0;
}

std::string NodeManager::ValuesToString()
{
	IntListManager::ReadIterator* ri = ilm->GetReadIterator( memory[firstNodeIdx.node.idx].node.childs.id );
	std::list<node> l;
	std::list<node>::iterator it;
	smallNode* sN = ri->GetNextItem();
	while (sN != NULL)
	{
		if ( sN->sPM.isSuperPackedMove == 0 )
			l.push_back(memory[sN->node.idx].node);
		sN = ri->GetNextItem();
	}
	l.sort([](const node & first, const node & second)
	{
		double firstVal = 0.5;
		double secondVal = 0.5;
		firstVal = first.totValue / first.v.visits;
		secondVal = second.totValue / second.v.visits;

		return (firstVal > secondVal);	});
	std::string res = "";
	int k = 0;
	for (it = l.begin(); it != l.end() && k++ < 8; ++it)
	{	
		res += "move:" + MoveManager::packMoveToString(it->v.move) + " val:" + std::to_string(it->totValue / it->v.visits) + " visits:" + std::to_string( it->v.visits ) + "\r\n";
	}
	return res;
}
