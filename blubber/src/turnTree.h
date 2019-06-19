/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   turnTree.h
 * Author: t_ren
 *
 * Created on 18. Mai 2019, 19:21
 */

#ifndef TURNTREE_H
#define TURNTREE_H
#include "int128.h"
#include "types.h"
#include "board.h"
void tree_initialise();
Turn tree_selectBestTurn();
void tree_selectTurn(Turn turn);
void tree_expand(Board board, int depth, int turningColor);
void tree_DebugWriteCurrentTree(int turn);

#endif /* TURNTREE_H */

