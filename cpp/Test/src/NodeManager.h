#pragma once
#include "SuperPackedMove.h"
#include "FreeArrayManager.h"
#include "IntListManager.h"
#include "BoardManager.h"
#include "MoveManager.h"

struct moveAndVisits
{
	// unsigned int move : 8, visits : 24;
	packedMove move;
	unsigned int visits;
};

struct childId
{
	unsigned int reserved: 1, syncSignal: 1, id : 30;
};

struct node
{
    childId childs;
	float totValue;
	moveAndVisits v;
};
struct m
{
	union {
		node node;
		nextFreeId free;
	};
};

// class IntListManager;
// class IntListManager::ReadIterator;
class NodeManager
{

public:
	NodeManager(int NodeCount, int MyColor, board FirstBoard, int FirstMoveDepth, double DeepFactor, int QuickRateTurn);
	~NodeManager();
	void ReInit(int MyColor, board FirstBoard, int FirstMoveDepth);
	void SelectAction(bool oneCycle);
	smallNode * findNode(int nodeIdx, packedMove move);
	void DisposeTree();
	void ImplementMoveToTree(packedMove move);
	bool IsDisposeRequired();
	packedMove BestMove();
	std::string ValuesToString();

	int GetMaxDepth();
	void ResetMaxDepth();

	

private: 
	bool hasChild(smallNode);
	void InitFirstNode();
	void InitNode(int nodeId, packedMove move, long visitCnt);
	void releaseNode(smallNode NodeId, int NodeIdToExclude);

	void expandNode(smallNode* sN , int moveColor, board position, int depth, int& currentDistance);
	float rollOut( int color, board pos, int depth, bool& gameEnd, int currentDistance);

	smallNode * selectMove(smallNode * sN, int turn);

	void updateStat( int nodeId, double value );
	void CopyNode( smallNode& from, smallNode& to);
	
	m* memory;
	int firstMoveColor;
	int firstMoveDepth;
	int currentMoveColor;
	smallNode firstNodeIdx;
	smallNode previousNodeIdx;
	boardpane* firstBoard;
	FreeArrayManager* fam;
	IntListManager* ilm;
	packedMove moveList[16*8]; // all fishes might move into all directions
	boardpane blockList[2][16]; // all blocks of a board
	int blockCnt[2]; // blocklist for red and blue
	bool stopSelection;
	int visitedCnt;
	smallNode* visited[60];
	board pos;
	int maxDepth;
	double deepFactor;
	long deepMultiplier;
	int quickRateTurn;
	IntListManager::ReadIterator* ri;

};
