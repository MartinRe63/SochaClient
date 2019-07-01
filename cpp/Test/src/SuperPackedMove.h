#pragma once
// !!! stupid super pack concept !!! - we can use normal pack here and save a lot of time
#include <stdint.h>
typedef struct {
	unsigned int isSuperPackedMove : 1, isGameEndNode : 1, totValue : 2, packedMove : 28;
}superPackedMove;
typedef struct {
	unsigned int isNoIdx : 1, idx : 31;
} superPackedNode;
struct smallNode {
	union {
		superPackedMove sPM;
		superPackedNode node;
	};
};
typedef struct {
	int x;
	int y;
	int dir;
} LastMove;

typedef uint64_t boardpane[2];
typedef boardpane board[3];
typedef struct {
	int turn;
	int currentPlayer;
	int startingPlayer;
	LastMove lastMove;
	board board;
} State;


