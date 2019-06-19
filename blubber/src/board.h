/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   board.h
 * Author: t_ren
 *
 * Created on 20. Mai 2019, 13:18
 */

#ifndef BOARD_H
#define BOARD_H
#include "int128.h"
#include "types.h"

int board_debug;

typedef struct{
    int128 data[2];
}Board;
float board_rateState(Board b, int turn, int turningColor, unsigned char * GameEnd);
int board_getPossibleTurns(Board board, int turningColor, Turn turns[128]);
void board_printCurrentState(Board board);
void board_setField(Board * board, int x, int y, int state);
Board board_parseString(char * data);
/**
 * Apply a turn to the given Color States.
 * @param turnColor the that is moving
 * @param otherColor the color that is potentialy getting eaten.
 * @param t the actual turn to execute
 */
void board_applyTurn(Board * board, int turningColor, Turn t);


Turn board_lastMoveToTurn(Board board, LastMove m);
LastMove board_turnToLastMove(Turn t);

#endif /* BOARD_H */

