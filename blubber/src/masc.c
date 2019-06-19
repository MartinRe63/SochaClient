#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "int128.h"
#include "masc.h"
#include "types.h"

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

int Min(int v1, int v2) {
    return v1 < v2 ? v1 : v2;
}

int Max(int v1, int v2) {
    return v1 > v2 ? v1 : v2;
}

void initMoveDist() {
    memset(m_moveDist, 0, sizeof (m_moveDist));
    for (int y = 0; y < 10; y++) {
        for (int x = 0; x < 10; x++) {
            for (int i = 0; i < 10; i++) {
                m_moveDist[y * 10 + x][dir_RIGHT / 2] = int128_or(m_moveDist[y * 10 + x][dir_RIGHT / 2], int128_setBit(y * 10 + i));
            }
            for (int i = 0; i < 10; i++) {
                m_moveDist[y * 10 + x][dir_UP / 2] = int128_or(m_moveDist[y * 10 + x][dir_UP / 2], int128_setBit(i * 10 + x));
            }
            int k = y - x;
            for (int i = Max(-k, 0); i <= Min(9, 9 - k); i++) {
                m_moveDist[y * 10 + x][dir_UP_RIGHT / 2] = int128_or(m_moveDist[y * 10 + x][dir_UP_RIGHT / 2], int128_setBit((y - x + i) * 10 + i));
            }
            k = y + x;
            //0 = -x + k;
            //9 = -x + k;
            for (int i = Max(k - 9, 0); i <= Min(9, k); i++) {
                m_moveDist[y * 10 + x][dir_UP_LEFT / 2] = int128_or(m_moveDist[y * 10 + x][dir_UP_LEFT / 2], int128_setBit((y + x - i) * 10 + i));
            }
        }
    }
}

void initField() {
    for (int y = 0; y < 10; y++) {
        for (int x = 0; x < 10; x++) {
            m_field[y * 10 + x] = int128_setBit(y * 10 + x);
        }
    }
}
//Das sollte man nicht so machen!!!

void initSkipCheck() {
    for (int y = 0; y < 10; y++) {
        for (int x = 0; x < 10; x++) {
            m_maxMoveDistance[y * 10 + x][dir_DOWN] = y;
            m_maxMoveDistance[y * 10 + x][dir_LEFT] = x;
            m_maxMoveDistance[y * 10 + x][dir_RIGHT] = 9 - x;
            m_maxMoveDistance[y * 10 + x][dir_UP] = 9 - y;

            m_maxMoveDistance[y * 10 + x][dir_UP_LEFT] = Min(m_maxMoveDistance[y * 10 + x][dir_UP], m_maxMoveDistance[y * 10 + x][dir_LEFT]);
            m_maxMoveDistance[y * 10 + x][dir_DOWN_LEFT] = Min(m_maxMoveDistance[y * 10 + x][dir_DOWN], m_maxMoveDistance[y * 10 + x][dir_LEFT]);
            m_maxMoveDistance[y * 10 + x][dir_DOWN_RIGHT] = Min(m_maxMoveDistance[y * 10 + x][dir_DOWN], m_maxMoveDistance[y * 10 + x][dir_RIGHT]);
            m_maxMoveDistance[y * 10 + x][dir_UP_RIGHT] = Min(m_maxMoveDistance[y * 10 + x][dir_UP], m_maxMoveDistance[y * 10 + x][dir_RIGHT]);
        }
    }
    for (int y = 0; y < 10; y++) {
        for (int x = 0; x < 10; x++) {
            for (int i = 0; i < 8; i++) {
                int mmd = m_maxMoveDistance[y * 10 + x][i];
                if (mmd >= 1) {
                    m_fieldSkipCheck[y * 10 + x][i] = malloc(sizeof (int128) * (mmd));
                    if (m_fieldSkipCheck[y * 10 + x][i] == 0) {
                        printf("Out of Memory!\n");
                        exit(-1);
                    }
                    memset(m_fieldSkipCheck[y * 10 + x][i], 0, sizeof (int128) * (mmd));
                    m_fieldSkipCheck[y * 10 + x][i][0] = int128_setBit((y + dirVectors[i][1]) * 10 + x + dirVectors[i][0]);
                    for (int j = 2; j < mmd + 1; j++) {
                        m_fieldSkipCheck[y * 10 + x][i][j - 1] = int128_or(m_fieldSkipCheck[y * 10 + x][i][j - 2], int128_setBit((y + dirVectors[i][1] * j) * 10 + x + dirVectors[i][0] * j));
                    }
                }
            }
        }
    }
}

void initFielNeighborIDs() {
    for (int y = 0; y < 10; y++) {
        for (int x = 0; x < 10; x++) {
            unsigned char count = 0;
            for (int i = 0; i < 8; i++) {
                int tx = x + dirVectors[i][0];
                int ty = y + dirVectors[i][1];
                if (tx >= 0 && tx < 10 && ty >= 0 && ty < 10) {
                    m_fieldNeighborIDs[y * 10 + x][count + 1] = (unsigned char) (ty * 10 + tx);
                    count++;
                }
            }
            m_fieldNeighborIDs[y * 10 + x][0] = count;
        }
    }
}

int inbound(int x, int y) {
    return x >= 0 && x < 10 && y >= 0 && y < 10;
}

void initFieldNeighbors() {
    for (int y = 0; y < 10; y++) {
        for (int x = 0; x < 10; x++) {
            for (int i = 0; i < 9; i++) {
                m_fieldNeighbors[y * 10 + x][i].v0 = 0;
                m_fieldNeighbors[y * 10 + x][i].v1 = 0;
                for (int ox = -i; ox <= i; ox++) {
                    int tmpX = x + ox;
                    int tmpY = y + i + 1;
                    if (inbound(tmpX, tmpY)) {
                        m_fieldNeighbors[y * 10 + x][i] = int128_or(m_fieldNeighbors[y * 10 + x][i], m_field[tmpY * 10 + tmpX]);
                    }
                    tmpY = y - i - 1;
                    if (inbound(tmpX, tmpY)) {
                        m_fieldNeighbors[y * 10 + x][i] = int128_or(m_fieldNeighbors[y * 10 + x][i], m_field[tmpY * 10 + tmpX]);
                    }
                }
                for (int oy = -i - 1; oy <= i + 1; oy++) {
                    int tmpX = x + i + 1;
                    int tmpY = y + oy;
                    if (inbound(tmpX, tmpY)) {
                        m_fieldNeighbors[y * 10 + x][i] = int128_or(m_fieldNeighbors[y * 10 + x][i], m_field[tmpY * 10 + tmpX]);
                    }
                    tmpX = x - i - 1;
                    if (inbound(tmpX, tmpY)) {
                        m_fieldNeighbors[y * 10 + x][i] = int128_or(m_fieldNeighbors[y * 10 + x][i], m_field[tmpY * 10 + tmpX]);
                    }
                }
                /*if ((x == 0 && y == 5) || (x == 9 && y == 0) || (x == 0 && y == 0) || (x == 3 && y == 4)) {
                    printf("%d, %d, %d\n", x, y, i);
                    int128_debugWrite(m_fieldNeighbors[y * 10 + x][i]);
                }*/
            }
        }
    }
}

void initFieldDistanceSearch() {
    for (int y = 0; y < 10; y++) {
        for (int x = 0; x < 10; x++) {
            memset(m_fieldDistanceSearch, 0, sizeof (int128) * 7);
            for (int i = 1; i < 5; i++) {
                m_fieldDistanceSearch[y * 10 + x][0] = int128_or(m_fieldDistanceSearch[y * 10 + x][0], m_fieldNeighbors[y * 10 + x][i]);
            }
            for (int i = 1; i < 3; i++) {
                m_fieldDistanceSearch[y * 10 + x][1] = int128_or(m_fieldDistanceSearch[y * 10 + x][1], m_fieldNeighbors[y * 10 + x][i]);
            }
            for (int i = 5; i < 7; i++) {
                m_fieldDistanceSearch[y * 10 + x][2] = int128_or(m_fieldDistanceSearch[y * 10 + x][2], m_fieldNeighbors[y * 10 + x][i]);
            }
            m_fieldDistanceSearch[y * 10 + x][3] = m_fieldNeighbors[y * 10 + x][1];
            m_fieldDistanceSearch[y * 10 + x][4] = m_fieldNeighbors[y * 10 + x][3];
            m_fieldDistanceSearch[y * 10 + x][5] = m_fieldNeighbors[y * 10 + x][5];
            m_fieldDistanceSearch[y * 10 + x][6] = m_fieldNeighbors[y * 10 + x][7];

        }
    }
}

void initBlocker() {
    for (int y1 = 0; y1 < 10; y1++) {
        for (int x1 = 0; x1 < 10; x1++) {
            for (int y2 = 0; y2 < 10; y2++) {
                for (int x2 = 0; x2 < 10; x2++) {
                    int dirDiagonal = 0;
                    int dirStraight = 0;
                    //If Horizontal
                    if (abs(x1 - x2) > abs(y1 - y2)) {
                        //Right
                        if (x2 > x1) {
                            dirStraight = dir_RIGHT;
                        } else {
                            dirStraight = dir_LEFT;
                        }
                    }//Vertical
                    else {
                        //UP
                        if (y2 > y1) {
                            dirStraight = dir_UP;
                        } else {
                            dirStraight = dir_DOWN;
                        }
                    }
                    //UP_Right
                    if (x2 > x1 && y2 > y1) {
                        dirDiagonal = dir_UP_RIGHT;
                    }//BOT_Right
                    else if (x2 > x1 && y2 <= y1) {
                        dirDiagonal = dir_DOWN_RIGHT;
                    }//BOT_LEFT
                    else if (x2 <= x1 && y2 <= y1) {
                        dirDiagonal = dir_DOWN_LEFT;
                    }//TOP_LEFT
                    else if (x2 <= x1 && y2 > y1) {
                        dirDiagonal = dir_UP_LEFT;
                    }
                    int cx1 = x1;
                    int cy1 = y1;
                    int cx2 = cx1;
                    int cy2 = cy1;
                    m_blocker[y1 * 10 + x1][y2 * 10 + x2].v0 = 0;
                    m_blocker[y1 * 10 + x1][y2 * 10 + x2].v1 = 0;
                    int d = 0;
                    int s = 0;
                    while (cx2 != x2 || cy2 != y2) {
                        cx2 = cx1;
                        cy2 = cy1;
                        d = 0;
                        while(cx2 != x2 && cy2 != y2){
                            m_blocker[y1 * 10 + x1][y2 * 10 + x2] = int128_or(m_blocker[y1 * 10 + x1][y2 * 10 + x2], m_field[cy2 * 10 + cx2]);
                            d++;
                            cx2 += dirVectors[dirDiagonal][0];
                            cy2 += dirVectors[dirDiagonal][1];
                        }
                        m_blocker[y1 * 10 + x1][y2 * 10 + x2] = int128_or(m_blocker[y1 * 10 + x1][y2 * 10 + x2], m_field[cy2 * 10 + cx2]);
                        d++;
                        s++;
                        cx1 += dirVectors[dirStraight][0];
                        cy1 += dirVectors[dirStraight][1];
                    }
                    //s++;
                    m_blockingFields[y1 * 10 + x1][y2 * 10 + x2] = (unsigned char) MIN(s, d);
                    //assert(m_blockingFields[y1 * 10 + x1][y2 * 10 + x2]);
                    /*if((y1 * 10 + x1 == 34 && y2 * 10 + x2 == 59) || 
                            (y1 * 10 + x1 == 34 && y2 * 10 + x2 == 74) ||
                            (y1 * 10 + x1 == 34 && y2 * 10 + x2 == 30) ||
                            (y1 * 10 + x1 == 34 && y2 * 10 + x2 == 67)){
                        printf("X1: %d, Y1: %d, X2: %d, Y2: %d\nFieldCount: %d\n", x1, y1, x2, y2, m_blockingFields[y1 * 10 + x1][y2 * 10 + x2]);
                        int128_debugWrite(m_blocker[y1 * 10 + x1][y2 * 10 + x2]);
                        printf("\n");
                    }*/
                }
            }
        }
    }
}

void initPositionValue() {
    for (int y = 0; y < 10; y++) {
        for (int x = 0; x < 10; x++) {
            m_positionValue[y * 10 + x] = (unsigned char) ((y <= 4 ? y + 1 : 10 - y) * (x <= 4 ? x + 1 : 10 - x)) + 1;
        }
    }
}

void initMasks() {
    initMoveDist();
    initSkipCheck();
    initField();
    initFielNeighborIDs();
    initFieldNeighbors();
    initPositionValue();
    initFieldDistanceSearch();
    initBlocker();
}

