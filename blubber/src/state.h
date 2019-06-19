/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   State.h
 * Author: t_ren
 *
 * Created on 20. Mai 2019, 14:18
 */

#ifndef STATE_H
#define STATE_H
#include "types.h"
#include "board.h"
typedef struct{
    int turn;
    int currentPlayer;
    int startingPlayer;
    LastMove lastMove;
    Board board;
}State;

#endif /* STATE_H */

