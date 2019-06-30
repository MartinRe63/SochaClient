#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "board.h"
#include "int128.h"
#include "board.h"
#include "masc.h"
#include "types.h"

#define RATE_SIMPLE
// #define RATE_2
// #define RATE_1

const int BLOCKADITY = 16;
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
static position dirVectors[] = {
    {0, 1},
    {0, -1},
    {1, 0},
    {-1, 0},
    {1, 1},
    {-1, -1},
    {1, -1},
    {-1, 1}
};
int128 g_obstacles = {.v0 = 0, .v1 = 0};
const float swarmRating = 1 / 30.0;


inline unsigned long long quickBoardPaneValue(int128 BP)
{
	unsigned long long res = Min(((__popcnt64(BP.v0 & fishValueMasks[4].v0) + __popcnt64(BP.v1 & fishValueMasks[4].v1))), 2ULL) * 8L;
	res += res = Min(((__popcnt64(BP.v0 & fishValueMasks[3].v0) + __popcnt64(BP.v1 & fishValueMasks[3].v1))), 4ULL) * 4L;
	res += res = Min(((__popcnt64(BP.v0 & fishValueMasks[2].v0) + __popcnt64(BP.v1 & fishValueMasks[2].v1))), 8ULL) * 2L;
	return res;
}

signed char getConnectedCount(int id, int128 *data, signed char fish[], signed char *fishCount) {
	//Prüfe ob das feld belegt ist
	assert(int128_notNull(int128_and(*data, m_field[id])));
	fish[*fishCount] = id;
	*fishCount = *fishCount + 1;
	//Bit auf diesem Feld auf Null
	*data = int128_xor(*data, m_field[id]);
	signed char connections = 1;
	int id2;
	//int128_debugWrite(neighbors);
	while ((id2 = int128_ffs(int128_and(*data, m_fieldNeighbors[id][0])))) {
		connections += getConnectedCount(id2 - 1, data, fish, fishCount);
	}
	return connections;
}

#ifdef RATE_SIMPLE

float board_rateState(Board b, int turn, int turningColor, unsigned char * GameEnd) {
    int currentColor = turningColor;

	int128 cpy[2] = { b.data[0], b.data[1] };
	signed char swarmSizes[2][16] = { 0 };
	signed char fish[2][16];
	signed char swarmCount[2] = { 0, 0 };
	signed char fishCount[2] = { 0, 0 };
	signed char maxSwarmSize[2] = { 0 };
	for (int c = 0; c < 2; c++) {
		int id;
		while ((id = int128_ffs(cpy[c]))) {
			swarmSizes[c][swarmCount[c]] = getConnectedCount(id - 1, cpy + c, fish[c], fishCount + c);
			if (swarmSizes[c][maxSwarmSize[c]] < swarmSizes[c][swarmCount[c]]) {
				maxSwarmSize[c] = swarmCount[c];
			}
			swarmCount[c]++;
		}
	}
	if (turn % 2 == 0) {
		if ((fishCount[color_red] == swarmSizes[color_red][maxSwarmSize[color_red]] && fishCount[color_blue] == swarmSizes[color_blue][maxSwarmSize[color_blue]]) || turn == 60) {
			if (swarmSizes[turningColor][maxSwarmSize[turningColor]] > swarmSizes[!turningColor][maxSwarmSize[!turningColor]]) {
				*GameEnd = 1;
				return 1;
			}
			if (swarmSizes[!turningColor][maxSwarmSize[!turningColor]] > swarmSizes[turningColor][maxSwarmSize[turningColor]]) {
				*GameEnd = 1;
				return 0;
			}
			*GameEnd = 1;
			return 0.5f;
		}
		if (fishCount[turningColor] == swarmSizes[turningColor][maxSwarmSize[turningColor]]) {
			*GameEnd = 1;
			return 1;
		}
		if (fishCount[!turningColor] == swarmSizes[!turningColor][maxSwarmSize[!turningColor]]) {
			*GameEnd = 1;
			return 0;
		}
	}
	float bs = (float)swarmSizes[turningColor][maxSwarmSize[turningColor]] / (swarmSizes[0][maxSwarmSize[0]] + swarmSizes[1][maxSwarmSize[1]]);
	// float d = (float)totalFishDist[!turningColor] / (totalFishDist[1] + totalFishDist[0] + 1);

	float d;
	unsigned long long redval = quickBoardPaneValue(b.data[0]);
	unsigned long long blueval = quickBoardPaneValue(b.data[1]);
	if (turningColor)
		d = blueval / (blueval + redval + 1.0);
	else
		d = redval / (blueval + redval + 1.0);


	// float blocking = (blocker[!turningColor]) / (blocker[1] + blocker[0]);
	//float connecting = (float) connector[turningColor] / (connector[!turningColor] + connector[turningColor]);
	//float k_p = /*(-1.0 / 60.0 * turn + 1)*/0;

	float k_bs = (swarmRating * turn);
	float k_d = 1;

	float rating = (bs * k_bs + d * k_d) / (k_bs + k_d);

	if (board_debug || (rating < 0 || rating > 1)) {
		printf("TC: %d\n", turningColor);
		//printf("BL: %lld, %lld\n", blocker[turningColor], blocker[!turningColor]);
		printf("BS: %d, %d\n", swarmSizes[turningColor][maxSwarmSize[turningColor]], swarmSizes[!turningColor][maxSwarmSize[!turningColor]]);
		printf("SC: %d, %d\n", swarmCount[turningColor], swarmCount[!turningColor]);
		//printf("DI: %lld, %lld\n", totalFishDist[turningColor], totalFishDist[!turningColor]);
		//printf("CN: %f, %f\n", connector[turningColor], connector[!turningColor]);
		//printf("Blocker   : %f\n", blocking);
		//printf("Connector : %f\n", connecting);
		printf("Biggest S : %f\n", bs);
		printf("Fish Dist : %f\n", d);
	}
	assert(rating >= 0);
	assert(rating <= 1);
	return rating;
}
#endif

#ifdef RATE_1

float quick_rateState(Board b, int turn, int turningColor)
{
	unsigned long long redval = quickBoardPaneValue(b.data[0]);
	unsigned long long blueval = quickBoardPaneValue(b.data[1]);
	if (turningColor)
		return blueval / (blueval + redval + 1.0);
	else
		return redval / (blueval + redval + 1.0);
}


void board_diffRate(Board baseBoard, int turn, int tc, treeNode * treeNodes, int nodeCount) {
	//Not turning Color
	int ntc = !tc;
	Board boardCpy = baseBoard;
	signed char swarmSizes[2][16] = { 0 };
	signed char fish[2][16];
	signed char swarmCount[2] = { 0, 0 };
	signed char fishCount[2] = { 0, 0 };
	signed char maxSwarmSize[2] = { 0 };
	for (int c = 0; c < 2; c++) {
		int id;
		while ((id = int128_ffs(boardCpy.data[c]))) {
			swarmSizes[c][swarmCount[c]] = getConnectedCount(id - 1, boardCpy.data + c, fish[c], fishCount + c);
			if (maxSwarmSize[c] < swarmSizes[c][swarmCount[c]]) {
				maxSwarmSize[c] = swarmSizes[c][swarmCount[c]];
			}
			swarmCount[c]++;
		}
	}
	for (int tn = 0; tn < nodeCount; tn++) {
		long long totalFishDist[2] = { 0, 0 };

		signed char currentSwarmSizes[2][16] = { 0 };
		signed char currentFish[2][16];
		signed char currentSwarmCount[2] = { 0, 0 };
		signed char currentFishCount[2] = { 0, 0 };
		signed char currentMaxSwarmSize[2] = { 0, 0 };

		//Apply the turn.
		Board currentBoard = baseBoard;
		board_applyTurn(&currentBoard, tc, treeNodes[tn].turn);
		boardCpy = currentBoard;
		//Check if there is a fhish at the target Position
		int fishEaten = int128_notNull(int128_and(m_field[treeNodes[tn].turn.target], baseBoard.data[ntc]));
		int128 neighborCheck[2] = { 0 };
		if (fishEaten)
			neighborCheck[ntc] = m_fieldNeighbors[treeNodes[tn].turn.target][0];
		neighborCheck[tc] = m_fieldNeighbors[treeNodes[tn].turn.start][0];
		//Adjust the Swarms
		for (int c = 0; c < 2; c++) {
			//Check for swarms that meight have been disconnected
			int id;
			while ((id = int128_ffs(int128_and(neighborCheck[c], boardCpy.data[c])))) {
				currentSwarmSizes[c][currentSwarmCount[c]] = getConnectedCount(id - 1, boardCpy.data + c, currentFish[c], currentFishCount + c);
				if (currentMaxSwarmSize[c] < currentSwarmSizes[c][currentSwarmCount[c]]) {
					currentMaxSwarmSize[c] = currentSwarmSizes[c][currentSwarmCount[c]];
				}
				currentSwarmCount[c]++;
			}
			//Check for swarms that meight have been connected
			if (c == tc && int128_notNull(int128_and(m_field[treeNodes[tn].turn.target], boardCpy.data[c]))) {
				currentSwarmSizes[c][currentSwarmCount[c]] = getConnectedCount(treeNodes[tn].turn.target, boardCpy.data + c, currentFish[c], currentFishCount + c);
				if (currentMaxSwarmSize[c] < currentSwarmSizes[c][currentSwarmCount[c]]) {
					currentMaxSwarmSize[c] = currentSwarmSizes[c][currentSwarmCount[c]];
				}
				currentSwarmCount[c]++;
			}
			//If a swarm has already been found.
			if (currentSwarmCount[c] || (c == ntc && fishEaten)) {
				//Add the rest of the unchanged Swarms
				for (int s = 0, start = 0; s < swarmCount[c]; s++) {
					//Check if the swarm hasn't already been processed
					if (int128_notNull(int128_and(m_field[fish[c][start]], boardCpy.data[c]))) {
						//Copy the fish that are part of the swarm
						memcpy(currentFish[c] + currentFishCount[c], fish[c] + start, swarmSizes[c][s]);
						currentSwarmSizes[c][currentSwarmCount[c]] = swarmSizes[c][s];
						currentMaxSwarmSize[c] = MAX(currentMaxSwarmSize[c], currentSwarmSizes[c][currentSwarmCount[c]]);
						currentFishCount[c] += swarmSizes[c][s];
						currentSwarmCount[c]++;
					}
					start += swarmSizes[c][s];
				}
			}
			else {
				memcpy(currentFish[c], fish[c], fishCount[c]);
				memcpy(currentSwarmSizes[c], swarmSizes[c], swarmCount[c]);
				currentFishCount[c] = fishCount[c];
				currentMaxSwarmSize[c] = maxSwarmSize[c];
				currentSwarmCount[c] = swarmCount[c];
			}
		}
		//Check for Winning Conditions
		if (turn % 2 == 0) {
			if ((currentSwarmCount[color_red] == 1 && currentSwarmCount[color_blue] == 1) || turn == 60) {
				treeNodes[tn].GameEnd = 1;
				if (currentMaxSwarmSize[tc] > currentMaxSwarmSize[ntc]) {
					treeNodes[tn].rating = 1;
				}
				else if (currentMaxSwarmSize[ntc] > currentMaxSwarmSize[tc]) {
					treeNodes[tn].rating = 0;
				}
				else {
					treeNodes[tn].rating = 0.5f;
				}
			}
			else {
				if (currentSwarmCount[tc] == 1) {
					treeNodes[tn].GameEnd = 1;
					treeNodes[tn].rating = 1;
				}
				if (currentSwarmCount[ntc] == 1) {
					treeNodes[tn].GameEnd = 1;
					treeNodes[tn].rating = 0;
				}
			}
		}
		//assert(!treeNodes[tn].GameEnd);
		if (!treeNodes[tn].GameEnd) {
			for (int c = 0; c < 2; c++) {
				long long blocker = 0;
				int start = 0;
				for (int s = 0; s < currentSwarmCount[c]; s++) {
					for (int f1 = start; f1 < start + currentSwarmSizes[c][s]; f1++) {
						for (int f2 = start + currentSwarmSizes[c][s]; f2 < currentFishCount[c]; f2++) {
							int128 check;
							check.v0 = currentBoard.data[!c].v0 & m_blocker[currentFish[c][f1]][currentFish[c][f2]].v0;
							check.v1 = currentBoard.data[!c].v1 & m_blocker[currentFish[c][f1]][currentFish[c][f2]].v1;
#ifndef _WIN
							blocker += (_builtin_popcountll(check.v0) + _builtin_popcountll(check.v1)) / m_blockingFields[currentFish[c][f1]][currentFish[c][f2]];
#else
							long long v = (__popcnt64(check.v0) + __popcnt64(check.v1)) << 20;
							blocker += v / m_blockingFields[currentFish[c][f1]][currentFish[c][f2]];
#endif					
							totalFishDist[c] += ( m_distance[currentFish[c][f1]][currentFish[c][f2]] << 20 );
						}
					}
					start += currentSwarmSizes[c][s];
				}
				totalFishDist[c] += 1 + blocker * BLOCKADITY;
			}
			float bs = (float)currentMaxSwarmSize[tc] / (currentMaxSwarmSize[0] + currentMaxSwarmSize[1]);
			float d = (float)totalFishDist[ntc] / (totalFishDist[1] + totalFishDist[0]);
			float k_bs = (1.0 / 60.0 * turn);
			float k_d = 1;

			treeNodes[tn].rating = (bs * k_bs + d * k_d) / (k_bs + k_d);
		}
		assert(treeNodes[tn].rating >= 0);
		assert(treeNodes[tn].rating <= 1);
	}
}
#endif
#ifdef RATE_2

float board_rateState(Board b, int turn, int turningColor, unsigned char *GameEnd) {
    int128 cpy[2] = {b.data[0], b.data[1]};
    signed char swarmSizes[2][16] = {0};
    signed char fish[2][16];
    signed char swarmCount[2] = {0, 0};
    signed char fishCount[2] = {0, 0};
    signed char maxSwarmSize[2] = {0};
    for (int c = 0; c < 2; c++) {
        int id;
        while ((id = int128_ffs(cpy[c]))) {
            swarmSizes[c][swarmCount[c]] = getConnectedCount(id - 1, cpy + c, fish[c], fishCount + c);
            if (swarmSizes[c][maxSwarmSize[c]] < swarmSizes[c][swarmCount[c]]) {
                maxSwarmSize[c] = swarmCount[c];
            }
            swarmCount[c]++;
        }
    }
    if (turn % 2 == 0) {
        if ((fishCount[color_red] == swarmSizes[color_red][maxSwarmSize[color_red]] && fishCount[color_blue] == swarmSizes[color_blue][maxSwarmSize[color_blue]]) || turn == 60) {
            if (swarmSizes[turningColor][maxSwarmSize[turningColor]] > swarmSizes[!turningColor][maxSwarmSize[!turningColor]]) {
                *GameEnd = 1;
                return 1;
            }
            if (swarmSizes[!turningColor][maxSwarmSize[!turningColor]] > swarmSizes[turningColor][maxSwarmSize[turningColor]]) {
                *GameEnd = 1;
                return 0;
            }
            *GameEnd = 1;
            return 0.5f;
        }
        if (fishCount[turningColor] == swarmSizes[turningColor][maxSwarmSize[turningColor]]) {
            *GameEnd = 1;
            return 1;
        }
        if (fishCount[!turningColor] == swarmSizes[!turningColor][maxSwarmSize[!turningColor]]) {
            *GameEnd = 1;
            return 0;
        }
    }
    long long totalFishDist[2] = {0, 0};
    long long blocker[2] = {0, 0};
    //float connector[2] = {0};

    //int position[2] = {0};
    for (int c = 0, c2 = 1; c < 2; c++, c2--) {
		int start1 = 0;
		for (int s1 = 0; s1 < swarmCount[c]; s1++) {
            for (int f1 = start1; f1 < start1 + swarmSizes[c][s1]; f1++) {
                for (int f2 = start1 + swarmSizes[c][s1]; f2 < fishCount[c]; f2++) {
                    /*connector[c] += (float) (int128_popcnt(
                            int128_and(b.data[c], m_blocker[fish[c][f1]][fish[c][f2]])) - 2) / 
                            m_blockingFields[fish[c][f1]][fish[c][f2]];*/
					int128 check;
					check.v0 = b.data[!c].v0 & m_blocker[fish[c][f1]][fish[c][f2]].v0;
					check.v1 = b.data[!c].v1 & m_blocker[fish[c][f1]][fish[c][f2]].v1;
#ifndef _WIN
					blocker[c] += ((_builtin_popcountll(check.v0) + _builtin_popcountll(check.v1)) << 20) / m_blockingFields[fish[c][f1]][fish[c][f2]];                    
#else
					long long v = (__popcnt64(check.v0) + __popcnt64(check.v1)) << 20;
					blocker[c] += v / m_blockingFields[fish[c][f1]][fish[c][f2]];
#endif					
					totalFishDist[c] += ( m_distance[fish[c][f1]][fish[c][f2]] << 20 );
				}
            }
            start1 += swarmSizes[c][s1];

        }
        totalFishDist[c] += 1 << 20 + blocker[c] * 16;
    }
    //float p = (float) position[turningColor] / (position[0] + position[1]);
    float bs = (float) swarmSizes[turningColor][maxSwarmSize[turningColor]] / (swarmSizes[0][maxSwarmSize[0]] + swarmSizes[1][maxSwarmSize[1]]);
    float d = (float) totalFishDist[!turningColor] / (totalFishDist[1] + totalFishDist[0] + 1);

    // float blocking = (blocker[!turningColor]) / (blocker[1] + blocker[0]);
    //float connecting = (float) connector[turningColor] / (connector[!turningColor] + connector[turningColor]);
    //float k_p = /*(-1.0 / 60.0 * turn + 1)*/0;

    float k_bs = (swarmRating * turn);
    float k_d = 1;

    float rating = ( bs * k_bs + d * k_d ) / (k_bs + k_d );
    if (board_debug || (rating < 0 || rating > 1)) {
        printf("TC: %d\n", turningColor);
        printf("BL: %lld, %lld\n", blocker[turningColor], blocker[!turningColor]);
        printf("BS: %d, %d\n", swarmSizes[turningColor][maxSwarmSize[turningColor]], swarmSizes[!turningColor][maxSwarmSize[!turningColor]]);
        printf("SC: %d, %d\n", swarmCount[turningColor], swarmCount[!turningColor]);
        printf("DI: %lld, %lld\n", totalFishDist[turningColor], totalFishDist[!turningColor]);
        //printf("CN: %f, %f\n", connector[turningColor], connector[!turningColor]);
        //printf("Blocker   : %f\n", blocking);
        //printf("Connector : %f\n", connecting);
        printf("Biggest S : %f\n", bs);
        printf("Fish Dist : %f\n", d);
    }
    assert(rating >= 0);
    assert(rating <= 1);
    return rating;
}
#endif

int board_getPossibleTurns(Board board, int turningColor, Turn turns[128]) {
    int moveCount = 0;
    int id;
    int128 tmp = board.data[turningColor];
    while ((id = int128_ffs(tmp))) {
        id -= 1;
        tmp = int128_xor(tmp, m_field[id]);
        position start;
        start[0] = id % 10;
        start[1] = id / 10;
        for (int d = 0; d < 4; d++) {
            //Count the numer of fish in the current direction.
            int steps = int128_popcnt(int128_and(m_moveDist[id][d], board.data[turningColor])) + int128_popcnt(int128_and(m_moveDist[id][d], board.data[!turningColor]));
            assert(steps >= 1);
            for (int j = 0; j < 2; j++) {
                //Check if the target is inbounds
                //Check if no enemy is skipped
                if (steps <= m_maxMoveDistance[id][d * 2 + j] &&
                        (steps == 1 || (int128_notNull(int128_and(m_fieldSkipCheck[id][d * 2 + j][steps - 2], board.data[!turningColor])) == 0))) {
                    position target;
                    target[0] = start[0] + dirVectors[d * 2 + j][0] * steps;
                    target[1] = start[1] + dirVectors[d * 2 + j][1] * steps;
                    int targetID = target[1] * 10 + target[0];
                    //check if there is no own Fish and no Obstacle at the target
                    if ((int128_notNull(int128_and(m_field[targetID], board.data[turningColor])) == 0) && (int128_notNull(int128_and(m_field[targetID], g_obstacles)) == 0)) {
                        if (moveCount < 128) {
                            //It's a valid move :) Juhu.
                            turns[moveCount].start = id;
                            turns[moveCount].target = target[1] * 10 + target[0];
                            moveCount++;
                        }
                    }
                }
            }
        }
    }
    return moveCount;
}

LastMove board_turnToLastMove(Turn t) {
    int x = t.start % 10;
    int y = t.start / 10;
    int dx = t.target % 10 - x;
    int dy = t.target / 10 - y;
    /*if(!(abs(dx) == abs(dy))){
        printf("sx: %d, sy: %d, tx: %d, ty: %d\n", x, y, t.target % 10, t.target / 10);
    }*/
    assert(abs(dx) == abs(dy) || dy == 0 || dx == 0);
    dx /= abs(dx) ? abs(dx) : 1;
    dy /= abs(dy) ? abs(dy) : 1;
    for (int d = 0; d < 8; d++) {
        if (dx == dirVectors[d][0] && dy == dirVectors[d][1]) {
            LastMove r;
            r.dir = d;
            r.x = x;
            r.y = y;
            printf("Turn x: %d, y: %d, direction: %d, xt: %d, yt: %d\n", r.x, r.y, r.dir, t.target % 10, t.target / 10);
            return r;
        }
    }
    assert(0);
    LastMove l = {0};
    return l;
}

Turn board_lastMoveToTurn(Board board, LastMove m) {
    int start = m.y * 10 + m.x;
    int steps = int128_popcnt(int128_and(m_moveDist[start][m.dir / 2], board.data[0])) + int128_popcnt(int128_and(m_moveDist[start][m.dir / 2], board.data[1]));
    m.x += dirVectors[m.dir][0] * steps;
    m.y += dirVectors[m.dir][1] * steps;
    Turn r;
    r.start = start;
    r.target = (unsigned char) (m.y * 10 + m.x);
    return r;
}

void board_printCurrentState(Board board) {
    char buffer[220];
    int i = 0;
    for (int y = 9; y >= 0; y--) {
        for (int x = 0; x < 10; x++) {
            if (int128_notNull(int128_and(m_field[y * 10 + x], board.data[color_red])))
                buffer[i] = 'R';
            else if (int128_notNull(int128_and(m_field[y * 10 + x], board.data[color_blue])))
                buffer[i] = 'B';
            else if (int128_notNull(int128_and(m_field[y * 10 + x], g_obstacles)))
                buffer[i] = 'H';
            else
                buffer[i] = ' ';
            i++;
            buffer[i] = ' ';
            i++;
        }
        buffer[i] = '\n';
        i++;
    }
    buffer[i] = 0;
    printf("Field:\n%s\n", buffer);
}

void board_setField(Board * board, int x, int y, int state) {
    int fieldID = y * 10 + x;
    switch (state) {
        case state_red:
            board->data[color_red] = int128_or(board->data[color_red], m_field[fieldID]);
            break;
        case state_blue:
            board->data[color_blue] = int128_or(board->data[color_blue], m_field[fieldID]);
            break;
        case state_obstructed:
            g_obstacles = int128_or(g_obstacles, m_field[fieldID]);
            break;
    }
}

/**
 * Apply a turn to the given Color States.
 * @param turnColor the that is moving
 * @param otherColor the color that is potentialy getting eaten.
 * @param t the actual turn to execute
 */
void board_applyTurn(Board *board, int turningColor, Turn t) {
    board->data[turningColor] = int128_xor(board->data[turningColor], m_field[t.start]);
    board->data[turningColor] = int128_xor(board->data[turningColor], m_field[t.target]);
    board->data[!turningColor] = int128_and(board->data[!turningColor], int128_not(m_field[t.target]));
}

Board board_parseString(char * data) {
    assert(strlen(data) == 100);
    Board r = {0};
    for (int i = 0; i < 100; i++) {
        switch (data[i]) {
            case 'R':
                board_setField(&r, i % 10, i / 10, state_red);
                break;
            case 'B':
                board_setField(&r, i % 10, i / 10, state_blue);
                break;
            case 'O':
                board_setField(&r, i % 10, i / 10, state_obstructed);
                break;
            case '.':
                board_setField(&r, i % 10, i / 10, state_empty);
                break;
            default:
                assert(0);
                break;
        }
    }
    return r;
}
