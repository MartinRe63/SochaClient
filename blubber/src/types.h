#ifndef __TYPES_H__
#define __TYPES_H__

#include "int128.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

//A turn;
typedef signed char position[2];

typedef struct {
    int x;
    int y;
    int dir;
} LastMove;

typedef struct {
    unsigned char start;
    unsigned char target;
} Turn;

enum {
    dir_UP,
    dir_DOWN,
    dir_RIGHT,
    dir_LEFT,
    dir_UP_RIGHT,
    dir_DOWN_LEFT,
    dir_DOWN_RIGHT,    
    dir_UP_LEFT
};
enum {
    state_red,
    state_blue,
    state_obstructed,
    state_empty
};
enum {
    color_red,
    color_blue
};
typedef struct {
	int firstChild;
	float rating;
	int visits;
	Turn turn;
	signed char childCount;
	unsigned char GameEnd;
} treeNode;


#endif