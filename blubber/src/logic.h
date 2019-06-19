#ifndef __LOGIC_H__
#define __LOGIC_H__


#include "types.h"
#include "state.h"
void logic_setMyColor(int color);
void logic_setMoveRequestFlag();
void logic_setState(State state);
void logic_update();
int logic_isMyTurn();
int logic_isInitialized();
#endif