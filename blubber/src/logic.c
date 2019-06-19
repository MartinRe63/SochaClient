#include <string.h>
#include <stdio.h>
#include "serverIO.h"
#include <time.h>

#include "types.h"
#include "int128.h"
#include "masc.h"
#include "logic.h"
#include "turnTree.h"
#include "state.h"


Board g_currentBoard;
int g_turnCount = 0;
int g_currentColor;
int g_startingColor;
int g_myColor;
int g_moveRequest = 0;
int g_initialized = 0;
clock_t g_myTurnTime;


void logic_setMyColor(int color){
    printf("My Color: %d\n", color);
    g_myColor = color;
}
int logic_isInitialized(){
    return g_initialized;
}
void logic_setMoveRequestFlag() {
    g_moveRequest = 1;
    g_myTurnTime = clock() + CLOCKS_PER_SEC + (CLOCKS_PER_SEC * 7) / 8;
}
void logic_setState(State state){
    if(state.turn == 0){
        tree_initialise();
        g_currentBoard = state.board;
        g_moveRequest = 0;
        g_initialized = 1;
        g_currentColor = state.currentPlayer;
        tree_expand(g_currentBoard, 0, g_currentColor);
        return;
    }
    if(state.currentPlayer == g_myColor){
        Turn t = board_lastMoveToTurn(g_currentBoard, state.lastMove);
        tree_selectTurn(t);
        //printf("Ich\n");
        //tree_DebugWriteCurrentTree(g_turnCount);
        board_applyTurn(&g_currentBoard, !g_myColor, t);
        //board_printCurrentState(g_currentBoard);
        g_turnCount++;
        g_currentColor = g_myColor;
        //tree_DebugWriteCurrentTree(g_turnCount);
    }
}

void logic_update() {
    //tree_DebugWriteCurrentTree();
    //@todo
    if (g_moveRequest) {
        int expandCount = 0;
        //printf("Test %d\n", g_currentColor);
        while(clock() < g_myTurnTime){
            tree_expand(g_currentBoard, g_turnCount, g_currentColor);
            expandCount++;
        }
        //printf("Expand Count: %d\n", expandCount);
        //printf("Color: %d\n", g_currentColor);
        //printf("Turn: %d\n", g_turnCount);
        //printf("Gegner\n");
        tree_DebugWriteCurrentTree(g_turnCount);
        printf("\n");
        unsigned char test;
        board_debug = 1;
        printf("Turn: %d\n", g_turnCount);
        printf("Rating: %f\n", board_rateState(g_currentBoard, g_turnCount, g_currentColor, &test));
        board_debug = 0;
        test = test + 1;
        printf("\n");
        
        Turn t = tree_selectBestTurn();
        sIO_sendMove(board_turnToLastMove(t));
        
        board_applyTurn(&g_currentBoard, g_currentColor, t);
        //board_printCurrentState(g_currentBoard);
        g_moveRequest = 0;
        g_currentColor = !g_myColor;
        g_turnCount++;
        //To Prevent Certain Bugs do one Expand instantly
        tree_expand(g_currentBoard, g_turnCount, g_currentColor);
    }
    else{
        for(int i = 0; i < 100; i++){
            tree_expand(g_currentBoard, g_turnCount, g_currentColor);
        }
    }
}

int logic_isMyTurn() {
    return g_currentColor == g_myColor;
}
