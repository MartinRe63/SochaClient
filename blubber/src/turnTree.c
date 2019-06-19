/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "turnTree.h"
#include "int128.h"
#include "types.h"
#include "logic.h"
#include "masc.h"

//#define MCTS
#define MY_SELECT
#define SELECT_BEST
#define EPSYLON 0.000001f

#define MAX_NODE_COUNT 75000000

enum {
    gameState_end = 1
};

typedef struct {
    int firstChild;
    float rating;
    int visits;
    Turn turn;
    signed char childCount;
    unsigned char GameEnd;
} treeNode;

int g_tree;
int g_UsedMemory;
treeNode *g_treeNodeMemory;

//Just for Debugging
int g_maxDepth = 0;
int g_totalNodes = 0;
int g_treeSwap = 0;
int g_memSwap = 0;

void tree_initialise() {
    g_treeNodeMemory = malloc(sizeof (treeNode) * MAX_NODE_COUNT);
    if (g_treeNodeMemory == 0) {
        printf("Out of Memory!\n");
        exit(-1);
    }
    g_tree = 0;
    g_treeNodeMemory[g_tree].firstChild = -1;
    g_treeNodeMemory[g_tree].childCount = -1;
    g_treeNodeMemory[g_tree].turn.target = 255;
    g_treeNodeMemory[g_tree].turn.start = 255;
    g_treeNodeMemory[g_tree].rating = 0.5f;
    g_treeNodeMemory[g_tree].visits = 0;
    g_treeNodeMemory[g_tree].GameEnd = 0;
    g_UsedMemory = 1;
}

/**
 * Try's to Allocate Memory
 * @param ammount the ammount of Memory to Allocate
 * @return the index of the first Tree Node
 */
int allocMemory(int ammount) {
    if (g_memSwap > g_treeSwap) {
        if (g_UsedMemory + ammount < g_tree) {
            int index = g_UsedMemory;
            g_UsedMemory += ammount;
            return index;
        } else {
            printf("I'm stuffed :(\n");
            exit(-1);
        }
    } else if (g_UsedMemory + ammount < MAX_NODE_COUNT) {
        int index = g_UsedMemory;
        g_UsedMemory += ammount;
        return index;
    } else {
        g_memSwap++;
        printf("MemSwap\n\n");
        g_UsedMemory = 0;
        return allocMemory(ammount);
    }
}

/**
 * Initialize's a node
 * @param node the node to initialize
 * @param t the Turn to get to this node
 */
void initializeNode(int node, Turn t) {
    g_treeNodeMemory[node].firstChild = -1;
    g_treeNodeMemory[node].childCount = -1;
    g_treeNodeMemory[node].turn = t;
    g_treeNodeMemory[node].rating = 0.5f;
    g_treeNodeMemory[node].visits = 0;
    g_treeNodeMemory[node].GameEnd = 0;
}

/**
 * Returns the id of the Child with the best rating for red
 * @param node the current node to compare
 * @return the best node for red
 */
int getBestChild(int node) {
    assert(node < MAX_NODE_COUNT);
    assert(node >= 0);
    assert(g_treeNodeMemory[node].childCount > 0);
    assert(g_treeNodeMemory[node].firstChild >= 0);
    int best = -1;
    float bestValue = 0.0f;
    int id = g_treeNodeMemory[node].firstChild;
    for (int i = 0; i < g_treeNodeMemory[node].childCount; i++, id++) {
#ifdef MCTS
        float r = g_treeNodeMemory[id].rating / g_treeNodeMemory[id].visits;
        if (r >= bestValue) {
            best = id;
            bestValue = r;
        }
#else
        if (g_treeNodeMemory[id].rating >= bestValue) {
            best = id;
            bestValue = g_treeNodeMemory[id].rating;
        }
#endif
    }
    if (best < 0) {
        printf("Bitte Was ?!?!?!?!?!?!?!?!?!");
    }
    assert(best >= 0);
    return best;
}

/**
 * Returns the best turn for the given Color and set's it as the new start of the tree
 * @param color the Color wich turn it is.
 * @return 
 */
Turn tree_selectBestTurn() {
    assert(g_tree < MAX_NODE_COUNT);
    assert(g_treeNodeMemory[g_tree].firstChild < MAX_NODE_COUNT);
    assert(g_treeNodeMemory[g_tree].firstChild >= 0);
    int bestChild = getBestChild(g_tree);
    assert(bestChild >= 0);
    assert(bestChild < MAX_NODE_COUNT);
    if (bestChild < g_tree) {
        printf("TreeSwap\n\n");
        g_treeSwap++;
    }
    g_tree = bestChild;
    return g_treeNodeMemory[g_tree].turn;
}

void tree_selectTurn(Turn turn) {
    assert(g_tree < MAX_NODE_COUNT);
    assert(g_tree >= 0);
    int node = g_treeNodeMemory[g_tree].firstChild;
    for (int i = 0; i < g_treeNodeMemory[g_tree].childCount; i++, node++) {
        if (turn.target == g_treeNodeMemory[node].turn.target &&
                turn.start == g_treeNodeMemory[node].turn.start) {
            if (node < g_tree) {
                printf("TreeSwap\n\n");
                g_treeSwap++;
            }
            g_tree = node;
            return;
        }
    }
    printf("Der Zug Existier nicht\n");
    printf("sx: %d, sy: %d, tx: %d, ty: %d, %d, %d\n", turn.start % 10, turn.start / 10, turn.target % 10, turn.target / 10, g_treeNodeMemory[g_tree].childCount, g_treeNodeMemory[g_tree].GameEnd);
    assert(0);
}

int getChildToExpand(int node) {
    assert(node < MAX_NODE_COUNT);
    assert(node >= 0);
    assert(g_treeNodeMemory[node].firstChild >= 0);
    assert(g_treeNodeMemory[node].childCount >= 0);
    int currentChild = -1;
    float maxRating = -1000000.0f;
    float currentRating = 0.0f;
#ifdef MY_SELECT  
#ifndef SELECT_BEST
    float totalChildRating = 0;
    for (int i = 0, n = g_treeNodeMemory[node].firstChild; i < g_treeNodeMemory[node].childCount; i++, n++) {
        if (!g_treeNodeMemory[n].GameEnd) {
            totalChildRating += g_treeNodeMemory[n].rating;
        }
    }
#endif
#endif
    for (int i = 0, n = g_treeNodeMemory[node].firstChild; i < g_treeNodeMemory[node].childCount; i++, n++) {
        if (!g_treeNodeMemory[n].GameEnd) {
#ifdef MY_SELECT
#ifndef SELECT_BEST
            currentRating = -fabs((((float) g_treeNodeMemory[n].visits + 1.0f) / g_treeNodeMemory[node].visits) - ((g_treeNodeMemory[n].rating) / totalChildRating));
#else
            currentRating = g_treeNodeMemory[n].rating
                    /* + 0.075 * sqrtf(log2f(g_treeNodeMemory[node].visits) / (g_treeNodeMemory[n].visits + EPSYLON))*/;
#endif
#else
            currentRating = (g_treeNodeMemory[n].rating / (g_treeNodeMemory[n].visits + EPSYLON))
                    + sqrtf(log2f(g_treeNodeMemory[node].visits) / (g_treeNodeMemory[n].visits + EPSYLON)) + EPSYLON * ((float) rand() / RAND_MAX);
#endif
            if (currentRating > maxRating) {
                currentChild = n;
                maxRating = currentRating;
            }
        }
    }
    return currentChild;
}

/**
 * 
 * @param node Node to Expand
 * @param board the board
 * @param depth 
 * @param turningColor The color that has to moove
 * @return 
 */
float expand(int node, Board board, int depth, int turningColor) {
    assert(node >= 0 && node < MAX_NODE_COUNT);
    if (depth < 60) {
        float returnRating = 0.5f;
        if (!g_treeNodeMemory[node].visits) {
#ifdef MY_SELECT
            Turn tmpTurns[128];
            int turnCount = board_getPossibleTurns(board, turningColor, tmpTurns);
            //If the Player Can't move he's lost.
            if (turnCount == 0) {
                printf("Upsie\n");
                g_treeNodeMemory[node].rating = 0;
                g_treeNodeMemory[node].GameEnd = 1;
                return 0.0f;
            }
            //Get the Memory
            int n = g_treeNodeMemory[node].firstChild = allocMemory(turnCount);
            g_treeNodeMemory[node].childCount = (signed char) turnCount;
            for (int i = 0; i < turnCount; i++, n++) {
                initializeNode(n, tmpTurns[i]);
                Board tmpBoard = board;
                board_applyTurn(&tmpBoard, turningColor, tmpTurns[i]);
                g_treeNodeMemory[n].rating = board_rateState(tmpBoard, depth + 1, turningColor, &g_treeNodeMemory[n].GameEnd);
            }
#endif
#ifdef MCTS
            returnRating = g_treeNodeMemory[node].rating = board_rateState(board, depth, !turningColor, &g_treeNodeMemory[node].GameEnd);
#endif
        } else {
            assert(!g_treeNodeMemory[node].GameEnd);
#ifdef MCTS
            if (g_treeNodeMemory[node].childCount < 0) {
                Turn tmpTurns[128];
                int turnCount = board_getPossibleTurns(board, turningColor, tmpTurns);
                //If the Player Can't move he's lost.
                if (turnCount == 0) {
                    printf("Upsie\n");
                    g_treeNodeMemory[node].rating = 0;
                    g_treeNodeMemory[node].GameEnd = 1;
                    return 0.0f;
                }
                //Get the Memory
                int n = g_treeNodeMemory[node].firstChild = allocMemory(turnCount);
                g_treeNodeMemory[node].childCount = (signed char) turnCount;
                for (int i = 0; i < turnCount; i++, n++) {
                    initializeNode(n, tmpTurns[i]);
                }
            }
#endif
            int next = getChildToExpand(node);
            if (next >= 0) {
                board_applyTurn(&board, turningColor, g_treeNodeMemory[next].turn);
#ifdef MY_SELECT
                expand(next, board, depth + 1, !turningColor);
#else
                returnRating = 1 - expand(next, board, depth + 1, !turningColor);
                g_treeNodeMemory[node].rating += returnRating;
#endif
            } else {
#ifndef MCTS
                //This should only happen if the opponent can only reach a tied game
                assert(g_treeNodeMemory[node].rating == 0.5f);
#endif
                g_treeNodeMemory[node].GameEnd = 1;
                return 0.0f;
            }
        }
        g_treeNodeMemory[node].visits++;
#ifdef MY_SELECT
        int bestChild = getBestChild(node);
        g_treeNodeMemory[node].rating = 1 - g_treeNodeMemory[bestChild].rating;
        if (g_treeNodeMemory[node].rating != 0.5f)
            g_treeNodeMemory[node].GameEnd = g_treeNodeMemory[bestChild].GameEnd;
#endif
        g_maxDepth = MAX(depth, g_maxDepth);
        return returnRating;
    }
    return 0.5f;
}

void tree_expand(Board board, int depth, int turningColor) {
    if (!g_treeNodeMemory[g_tree].GameEnd) {
        expand(g_tree, board, depth, turningColor);

        //Debugging
        g_totalNodes += 1;
    }
}

void tree_DebugWriteCurrentTree(int turn) {
    turn = turn + 1;
    assert(g_tree >= 0 && g_tree < MAX_NODE_COUNT);
    for (int i = 0, node = g_treeNodeMemory[g_tree].firstChild; i < g_treeNodeMemory[g_tree].childCount; i++, node++) {
        printf("%d, ", g_treeNodeMemory[node].visits);
    }
    printf("\n");
    for (int i = 0, node = g_treeNodeMemory[g_tree].firstChild; i < g_treeNodeMemory[g_tree].childCount; i++, node++) {
        printf("%3.0f, ", g_treeNodeMemory[node].rating);
    }
    printf("\n");
    printf("MaxDepth: %d\n", g_maxDepth - turn + 1);
    g_maxDepth = 0;
    printf("RatingTree: %f\n", g_treeNodeMemory[g_tree].rating);
    printf("Nodes     : %d\n", g_totalNodes);
    g_totalNodes = 0;
}