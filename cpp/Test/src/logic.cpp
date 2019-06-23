#include <string.h>
#include <stdio.h>
#include "serverIO.h"
#include <time.h>

// #include "types.h"
// #include "int128.h"
// #include "masc.h"
#include "logic.h"
//#include "turnTree.h"
//#include "state.h"
#include "SuperPackedMove.h"
#include "NodeManager.h"
#include "BoardManager.h"

board g_currentBoard;
int g_turnCount = 0;
int g_currentColor;
int g_startingColor;
int g_myColor = 0;
int g_moveRequest = 0;
int g_initialized = 0;
packedMove myMove;
clock_t g_myTurnTime;
NodeManager* nodeManager = NULL;


void logic_init() {
	board b;
	nodeManager = new NodeManager(40000000, g_myColor, b, 0, 0.1);
}

void logic_setMyColor(int color){
    printf("My Color: %d\n", color);
    g_myColor = color;
}
int logic_isInitialized(){
    return g_initialized;
}
void logic_setMoveRequestFlag() {
    g_moveRequest = 1;
	g_myTurnTime = clock() + CLOCKS_PER_SEC; // +(CLOCKS_PER_SEC * 7) / 8;
}
void logic_setState(State state){
    if(state.turn == 0)
	{
		BoardManager::Copy(state.board, g_currentBoard);
		nodeManager->ReInit(g_myColor, g_currentBoard, g_myColor);
		g_initialized = 1;
	}
    else if(state.currentPlayer == g_myColor)
	{
		packedMove m = MoveManager::LastMove2packedMove(state.lastMove, g_currentBoard);
		MoveManager::addMoveToBoard(g_currentBoard, !g_myColor, m);
		nodeManager->ImplementMoveToTree(m); 

		g_turnCount++;
        g_currentColor = g_myColor;
        //tree_DebugWriteCurrentTree(g_turnCount);
    }
}

void logic_update() {
    //tree_DebugWriteCurrentTree();
    if (g_moveRequest) {
        int expandCount = 0;
        //printf("Test %d\n", g_currentColor);
		clock_t c = clock();
        while(c < g_myTurnTime){
			nodeManager->SelectAction(true);
            expandCount++;
			c = clock();
        }
        //printf("Expand Count: %d\n", expandCount);
        //printf("Color: %d\n", g_currentColor);
        //printf("Turn: %d\n", g_turnCount);
        //printf("Gegner\n");
        // tree_DebugWriteCurrentTree(g_turnCount);
        // printf("\n");
        //unsigned char test;
        //board_debug = 1;
        //printf("Turn: %d\n", g_turnCount);
        //printf("Rating: %f\n", board_rateState(g_currentBoard, g_turnCount, g_currentColor, &test));
        //board_debug = 0;
        //test = test + 1;
        //printf("\n");
        
		packedMove m = nodeManager->BestMove();
		nodeManager->ImplementMoveToTree(m);

        sIO_sendMove(MoveManager::PackedMove2LastMove(m));
		nodeManager->DisposeTree();

        //board_printCurrentState(g_currentBoard);
        g_moveRequest = 0;
        g_currentColor = !g_myColor;
        g_turnCount++;
        //To Prevent Certain Bugs do one Expand instantly
    }
    else if ( g_initialized ) {
   //     for(int i = 0; i < 100; i++){
			//nodeManager->SelectAction(true);
   //     }
    }
}

int logic_isMyTurn() {
    return g_currentColor == g_myColor;
}
