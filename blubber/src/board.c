#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "board.h"
#include "int128.h"
#include "board.h"
#include "masc.h"
#include "types.h"

//#define RATE_SIMPLE
#define RATE_2
//#define RATE_1

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

#ifndef RATE_SIMPLE

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
#else
signed char getConnectedCount(int id, int128 *data) {
    assert(int128_notNull(int128_and(*data, m_field[id])));
    //Bit auf diesem Feld auf Null
    *data = int128_xor(*data, m_field[id]);
    signed char connections = 1;
    int id2;
    while ((id2 = int128_ffs(int128_and(*data, m_fieldNeighbors[id][0])))) {
        connections += getConnectedCount(id2 - 1, data);
    }
    return connections;
}
#endif
#ifdef RATE_SIMPLE

float board_rateState(Board b, int turn, int turningColor, unsigned char * GameEnd) {
    int currentColor = turningColor;
    for (int t = turn; t <= 60; t++) {
        if (t % 2 == 0) {
            int128 cpy[2] = {b.data[0], b.data[1]};
            int maxSwarmSize[2] = {0};
            int swarmCount[2] = {-1};
            for (int c = 0; c < 2; c++) {
                int id;
                while ((id = int128_ffs(cpy[c]))) {
                    maxSwarmSize[c] = MAX(getConnectedCount(id - 1, cpy + c), maxSwarmSize[c]);
                    swarmCount[c]++;
                }
            }
            if ((!swarmCount[color_red] && !swarmCount[color_blue]) || t == 60) {
                *GameEnd = t == turn;
                if (maxSwarmSize[turningColor] > maxSwarmSize[!turningColor])
                    return 1;
                if (maxSwarmSize[!turningColor] > maxSwarmSize[turningColor])
                    return 0;

                return (float)rand() / RAND_MAX > 0.5f ? 1.0f : 0.0f;
            }
            if (!swarmCount[turningColor]) {
                *GameEnd = t == turn;
                return 1;
            }
            if (!swarmCount[!turningColor]) {
                *GameEnd = t == turn;
                return 0;
            }
        }
        //Since the Game isn't finished do another Random Turn
        Turn tmpTurns[128];
        currentColor = !currentColor;
        int turnCount = board_getPossibleTurns(b, currentColor, tmpTurns);
        if (turnCount > 0) {
            int rndTurnID = (int)((((float)(rand() / 2)) / (RAND_MAX / 2 + 1)) * turnCount);
            board_applyTurn(&b, currentColor, tmpTurns[rndTurnID]);
        } else {
            return turningColor != currentColor;
        }
    }
    assert(0);
    return 0;
}
#endif

#ifdef RATE_1
float board_rateState(Board b, int turn, int turningColor, unsigned char * GameEnd) {
    int128 cpy[2] = {b.data[0], b.data[1]};
    signed char swarmSizes[2][16] = {0};
    signed char fish[2][16];
    signed char swarmCount[2] = {0, 0};
    signed char fishCount[2] = {0, 0};
    signed char maxSwarmSize[2] = {0};
    //Gather basic data like Fish Positions and connections
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
    //check if one Player has already won
    if (turn % 2 == 0) {
        if ((fishCount[color_red] == swarmSizes[color_red][maxSwarmSize[color_red]] && fishCount[color_blue] == swarmSizes[color_blue][maxSwarmSize[color_blue]]) || turn == 60) {
            if (swarmSizes[color_red][maxSwarmSize[color_red]] > swarmSizes[color_blue][maxSwarmSize[color_blue]]) {
                *GameEnd = 1;
                return 1;
            }
            if (swarmSizes[color_blue][maxSwarmSize[color_blue]] > swarmSizes[color_red][maxSwarmSize[color_red]]) {
                *GameEnd = 1;
                return 0;
            }
            *GameEnd = 1;
            return 0.5f;
        }
        if (fishCount[color_red] == swarmSizes[color_red][maxSwarmSize[color_red]]) {
            *GameEnd = 1;
            return 1;
        }
        if (fishCount[color_blue] == swarmSizes[color_blue][maxSwarmSize[color_blue]]) {
            *GameEnd = 1;
            return 0;
        }
    }

    int nearFish[2] = {0};
    int position[2] = {0};
    for (int c = 0, c2 = 1; c < 2; c++, c2--) {
        for (int f = 0; f < fishCount[c]; f++) {
            for (int i = 1, m = 1; i >= 0; i--, m++) {
                nearFish[c] += int128_popcnt(int128_and(m_fieldNeighbors[fish[c][f]][i], b.data[c])) * m;
            }
            position[c] += m_positionValue[fish[c][f]];
        }
        nearFish[c] /= fishCount[c];
        position[c] /= fishCount[c];
    }
    if (board_debug) {
        printf("Near Fish : %d, %d\n", nearFish[0], nearFish[1]);
        printf("Position  : %d, %d\n", position[0], position[1]);
    }

    float n = (float) nearFish[turningColor] / (nearFish[turningColor] + nearFish[!turningColor]);
    float bs = (float) swarmSizes[turningColor][maxSwarmSize[turningColor]] /
            (swarmSizes[turningColor][maxSwarmSize[turningColor]] + swarmSizes[!turningColor][maxSwarmSize[!turningColor]]);
    float p = (float) position[turningColor] / (position[turningColor] + position[!turningColor]);
    float k_p = (-1.0 / 60.0 * turn + 1);
    float k_n = 0.25f;
    float k_bs = (1.0 / 60.0 * turn);
    if (board_debug) {
        printf("P: %f,N %f,BS %f\n", p, n, bs);
    }
    return (p * k_p + n * k_n + bs * k_bs) / (k_bs + k_n + k_p);
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
    float totalFishDist[2] = {0};
    float blocker[2] = {0.0f, 0.0f};
    //float connector[2] = {0};

    //int position[2] = {0};
    for (int c = 0, c2 = 1; c < 2; c++, c2--) {
        for (int s1 = 0; s1 < swarmCount[c]; s1++) {
            int start1 = 0;
            for (int f1 = start1; f1 < start1 + swarmSizes[c][s1]; f1++) {
                for (int f2 = start1 + swarmSizes[c][s1]; f2 < fishCount[c]; f2++) {
                    /*connector[c] += (float) (int128_popcnt(
                            int128_and(b.data[c], m_blocker[fish[c][f1]][fish[c][f2]])) - 2) / 
                            m_blockingFields[fish[c][f1]][fish[c][f2]];*/
                    blocker[c] += (float) int128_popcnt(
                            int128_and(b.data[c2], m_blocker[fish[c][f1]][fish[c][f2]])) /
                            m_blockingFields[fish[c][f1]][fish[c][f2]];
                    int dist = MAX(abs(fish[c][f1] / 10 - fish[c][f2] / 10), abs(fish[c][f1] % 10 - fish[c][f2] % 10));
                    totalFishDist[c] += dist * dist;
                }
            }
            start1 += swarmSizes[c][s1];

        }
        /*for (int f = 0; f < fishCount[c]; f++) {
            for (int i = f + 1; i < fishCount[c]; i++) {
                
            }
            position[c] += m_positionValue[fish[c][f]];
        }*/
        /*connector[c] /= fishCount[c];
        connector[c] += 0.5f;*/
        //blocker[c] += 1;
        //blocker[c] /= (fishCount[c] * fishCount[c]);
        //blocker[c] += 0.5f;
        totalFishDist[c] += 1 + blocker[c] * 16;
        //totalFishDist[c] /= (fishCount[c] * fishCount[c]);
        //position[c] /= fishCount[c];
    }
    //float p = (float) position[turningColor] / (position[0] + position[1]);
    float bs = (float) swarmSizes[turningColor][maxSwarmSize[turningColor]] / (swarmSizes[0][maxSwarmSize[0]] + swarmSizes[1][maxSwarmSize[1]]);
    float d = (float) totalFishDist[!turningColor] / (totalFishDist[1] + totalFishDist[0]);
    float blocking = (blocker[!turningColor]) / (blocker[1] + blocker[0]);
    //float connecting = (float) connector[turningColor] / (connector[!turningColor] + connector[turningColor]);
    //float k_p = /*(-1.0 / 60.0 * turn + 1)*/0;
    float k_bs = (1.0 / 60.0 * turn);
    float k_d = /*MAX(1.0 / 30.0 * turn, 1)*/1;

    float rating = (/*p * k_p + */bs * k_bs + d * k_d/* + blocking + connecting*/) / (k_bs/* + k_p*/ + k_d/* + 1*/);
    if (board_debug || (rating < 0 || rating > 1)) {
        printf("TC: %d\n", turningColor);
        printf("BL: %f, %f\n", blocker[turningColor], blocker[!turningColor]);
        printf("BS: %d, %d\n", swarmSizes[turningColor][maxSwarmSize[turningColor]], swarmSizes[!turningColor][maxSwarmSize[!turningColor]]);
        printf("SC: %d, %d\n", swarmCount[turningColor], swarmCount[!turningColor]);
        printf("DI: %f, %f\n", totalFishDist[turningColor], totalFishDist[!turningColor]);
        //printf("CN: %f, %f\n", connector[turningColor], connector[!turningColor]);
        printf("Blocker   : %f\n", blocking);
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
