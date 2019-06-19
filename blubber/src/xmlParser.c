#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
// #include <unistd.h>
#include "logic.h"
#include "xmlParser.h"
#include "state.h"
//#include "types.h"
#define MAX_OPEN_TAGS 32

#define TAG_COUNT 12
char *g_roomName;
char *tags[] = {
    "protocol",
    "joined",
    "room",
    "data",
    "state",
    "board",
    "fields",
    "field",
    "lastMove",
    "error",
    "originalRequest",
    "left"
};

enum {
    tag_protocol,
    tag_joined,
    tag_room,
    tag_data,
    tag_state,
    tag_board,
    tag_fields,
    tag_field,
    tag_lastMove,
    tag_error,
    tag_originalRequest,
    tag_left,
    tag_comment,
    tag_close
};
#define ATTR_COUNT 11
char *attributes[] = {
    "roomId",
    "class",
    "color",
    "turn",
    "startPlayerColor",
    "currentPlayerColor",
    "displayName",
    "x",
    "y",
    "state",
    "direction"
};

enum {
    attr_roomId,
    attr_class,
    attr_color,
    attr_turn,
    attr_startPlayer,
    attr_currentPlayer,
    attr_displayName,
    attr_x,
    attr_y,
    attr_state,
    attr_direction
};
#define CLASS_COUNT 5
char *classes[] = {
    "welcomeMessage",
    "memento",
    "state",
    "sc.framework.plugins.protocol.MoveRequest",
    "move"
};

enum {
    class_welcomeMessage,
    class_memento,
    class_state,
    class_moveRequest,
    class_move
};
char *colors[] = {
    "red",
    "blue"
};
#define STATE_COUNT 4
char *states[] = {
    "RED",
    "BLUE",
    "OBSTRUCTED",
    "EMPTY"
};
char *directions[] = {
    "UP",
    "DOWN",
    "RIGHT",
    "LEFT",
    "UP_RIGHT",
    "DOWN_LEFT",
    "DOWN_RIGHT",
    "UP_LEFT"
};
int g_currentChar;
int g_openTagCount = 0;
int g_totalTagCount = 0;
int g_opendTag[MAX_OPEN_TAGS];
char g_messageBuffer[16000];
int g_currentLenght = 0;

char getCharAt(int sockFD, int charID) {
    if (charID >= g_currentLenght) {
        g_currentLenght += read(sockFD, (char *) g_messageBuffer + g_currentLenght, sizeof (g_messageBuffer) - g_currentLenght);
        //printf("%s\n%d\n", g_messageBuffer, g_currentLenght);
        g_messageBuffer[g_currentLenght] = 0;
        //printf("Zeichen Gelesen: %d\n", g_currentLenght);
    }
    return g_messageBuffer[charID];
}

int getMatchingStringID(char value[], char **comparators, int comparatorCount) {
    for (int i = 0; i < comparatorCount; i++) {
        if (strcmp(value, comparators[i]) == 0)
            return i;
    }
    return -1;
}
//g_currentPos should be at '<'

int getTagType(int sockFD) {
    g_currentChar++;
    if (getCharAt(sockFD, g_currentChar) == '!') {
        return tag_comment;
    }
    if (getCharAt(sockFD, g_currentChar) == '/') {
        return tag_close;
    }
    char tmp[64];
    {
        int i = 0;
        while (getCharAt(sockFD, g_currentChar) != ' ' && getCharAt(sockFD, g_currentChar) != '>' && getCharAt(sockFD, g_currentChar) != '/') {
            tmp[i] = getCharAt(sockFD, g_currentChar);
            i++;
            g_currentChar++;
        }
        tmp[i] = 0;
    }
    return getMatchingStringID(tmp, tags, TAG_COUNT);
}
//Attribute Name and Attribute Data need to be big enough!!!

int getNextAttr(int sockFD, char attrName[], char attrData[]) {
    while (isspace(getCharAt(sockFD, g_currentChar))) {
        g_currentChar++;
    }
    if (getCharAt(sockFD, g_currentChar) == '/' || getCharAt(sockFD, g_currentChar) == '>')
        return 0;
    int attrNameCharCount = 0;
    while (getCharAt(sockFD, g_currentChar) != '=' && !isspace(getCharAt(sockFD, g_currentChar))) {
        attrName[attrNameCharCount] = getCharAt(sockFD, g_currentChar);
        attrNameCharCount++;
        g_currentChar++;
    }
    attrName[attrNameCharCount] = 0;
    while (getCharAt(sockFD, g_currentChar) != '"')
        g_currentChar++;
    g_currentChar++;
    int attrDataCharCount = 0;
    while (getCharAt(sockFD, g_currentChar) != '"') {
        attrData[attrDataCharCount] = getCharAt(sockFD, g_currentChar);
        g_currentChar++;
        attrDataCharCount++;
    }
    attrData[attrDataCharCount] = 0;
    g_currentChar++;
    return 1;
}
//Return 1 if tag has is closed.

int gotoTagEnd(int sockFD) {
    while (getCharAt(sockFD, g_currentChar) != '>')
        g_currentChar++;
    if (getCharAt(sockFD, g_currentChar - 1) == '/') {
        return 1;
    }
    return 0;
}

void ignore(int sockFD) {
    int startCount = g_openTagCount;
    while (g_openTagCount >= startCount) {
        if (getCharAt(sockFD, g_currentChar) == '<') {
            if (getCharAt(sockFD, g_currentChar + 1) != '/') {
                g_openTagCount++;
            } else {
                g_openTagCount--;
            }
        } else if (getCharAt(sockFD, g_currentChar) == '/' && getCharAt(sockFD, g_currentChar + 1) == '>') {
            g_openTagCount--;
        }
        g_currentChar++;
    }
}
int g_stateSet = 0;
State g_state;

void parseTag(int sockFD) {
    int type = getTagType(sockFD);
    char attrName[128];
    char attrValue[128];
    g_opendTag[g_openTagCount] = type;
    g_openTagCount++;
    g_totalTagCount++;
    switch (type) {
        case tag_protocol:
            //Nothing todo
            break;
        case tag_joined:
            if (g_totalTagCount == 2) {
                while (getNextAttr(sockFD, attrName, attrValue)) {
                    if (strcmp(attrName, attributes[attr_roomId]) == 0) {
                        int roomIdLen = strlen(attrValue);
                        g_roomName = malloc((roomIdLen + 1) * sizeof (char));
                        strcpy(g_roomName, attrValue);
                    }
                }
            }
            break;
        case tag_room:
            while (getNextAttr(sockFD, attrName, attrValue)) {
                if (strcmp(attrName, attributes[attr_roomId]) == 0 && strcmp(attrValue, g_roomName) != 0) {
                    printf("Falsche Raum ID!\n");
                    ignore(sockFD);
                    return;
                }
            }
            break;
        case tag_data:
            if (getNextAttr(sockFD, attrName, attrValue)) {
                if (strcmp(attrName, attributes[attr_class]) == 0) {
                    int class = getMatchingStringID(attrValue, classes, CLASS_COUNT);
                    switch (class) {
                        case class_memento:
                            //where is nothing to do here.
                            break;
                        case class_move:

                            break;
                        case class_moveRequest:
                            logic_setMoveRequestFlag();
                            break;
                        case class_welcomeMessage:
                            while (getNextAttr(sockFD, attrName, attrValue)) {
                                if (strcmp(attrName, attributes[attr_color]) == 0) {
                                    logic_setMyColor(getMatchingStringID(attrValue, colors, 2));
                                }
                            }
                            break;
                        default:
                            break;
                    }
                }
            }
            break;
        case tag_state:
            g_state.board.data[color_red].v0 = 0;
            g_state.board.data[color_red].v1 = 0;
            g_state.board.data[color_blue].v0 = 0;
            g_state.board.data[color_blue].v1 = 0;
            while (getNextAttr(sockFD, attrName, attrValue)) {
                if (strcmp(attrName, attributes[attr_turn]) == 0) {
                    sscanf(attrValue, "%d", &g_state.turn);
                } else if (strcmp(attrName, attributes[attr_startPlayer]) == 0) {
                    g_state.startingPlayer = getMatchingStringID(attrValue, states, 2);
                } else if (strcmp(attrName, attributes[attr_currentPlayer]) == 0) {
                    g_state.currentPlayer = getMatchingStringID(attrValue, states, 2);
                }
            }
            break;
        case tag_board:
            if (g_state.turn != 0) {
                //printf("Board wird ignoriert da Zug grösser 0\n");
                ignore(sockFD);
                return;
            }
            break;
        case tag_fields:
            //nothing todo
            break;
        case tag_field:
        {
            int x = 0;
            int y = 0;
            int fieldType = 0;
            while (getNextAttr(sockFD, attrName, attrValue)) {
                if (strcmp(attrName, attributes[attr_x]) == 0) {
                    x = attrValue[0] - '0';
                } else if (strcmp(attrName, attributes[attr_y]) == 0) {
                    y = attrValue[0] - '0';
                } else if (strcmp(attrName, attributes[attr_state]) == 0) {
                    fieldType = getMatchingStringID(attrValue, states, STATE_COUNT);
                }
            }
            if (fieldType != 3) {
                board_setField(&g_state.board, x, y, fieldType);
            }
        }
            break;
        case tag_lastMove:
        {
            int tmpX = 0;
            int tmpY = 0;
            while (getNextAttr(sockFD, attrName, attrValue)) {
                if (strcmp(attrName, attributes[attr_x]) == 0) {
                    tmpX = attrValue[0] - '0';
                } else if (strcmp(attrName, attributes[attr_y]) == 0) {
                    tmpY = attrValue[0] - '0';
                } else if (strcmp(attrName, attributes[attr_direction]) == 0) {
                    g_state.lastMove.dir = getMatchingStringID(attrValue, directions, 8);
                }
            }
            g_state.lastMove.x = tmpX;
            g_state.lastMove.y = tmpY;
        }
            break;
        case tag_error:
            //Nothing todo
            break;
        case tag_originalRequest:
            //Nothing todo
            break;
        case tag_left:
            //To make sure that the Programm get's terminated!
            g_openTagCount--;
            //Nothing todo
            break;
        case tag_comment:
            //Because a comment doesn't get closed.
            g_openTagCount--;
            g_totalTagCount--;
            break;
        case tag_close:
            //Because the closing Tag isn't realy a new Tag
            if (g_opendTag[g_openTagCount - 2] == tag_state) {
                logic_setState(g_state);
            }
            g_totalTagCount--;
            g_openTagCount -= 2;
            break;
        case -1:
            //printf("wird Ignoriert da Tag unbekannt ist\n");
            ignore(sockFD);
            return;
        default:
            break;
    }
    if (gotoTagEnd(sockFD))
        g_openTagCount--;
}

int parseMassage(int sockFD) {
    g_currentChar = 0;
    g_currentLenght = 0;
    do {
        while (getCharAt(sockFD, g_currentChar) != '<') {
            g_currentChar++;
        }
        parseTag(sockFD);
        //printf("Last Tag: %d, %d\n", g_opendTag[g_openTagCount-1], g_openTagCount);
    } while ((g_openTagCount > 1 || g_currentChar < g_currentLenght - 5) && g_openTagCount);
    //printf("Server Nachricht: %s\n", g_messageBuffer);
    return g_openTagCount;
}

void parseTurn(LastMove m, char buffer[1024]) {
    sprintf(buffer,
            "<room roomId=\"%s\"><data class=\"move\" x=\"%d\" y=\"%d\" direction=\"%s\" ><hint content=\"Test123\" /></data></room>",
            g_roomName, m.x, m.y, directions[m.dir]);
}