#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BOARD_SIZE 8
#define BLACK 2
#define WHITE 1
#define FREE 0

typedef enum {INVALID_SQUARE = 0,NO_SUCH_SQUARE, VALID_MOVE} MoveMode;
typedef enum {BLACK_WIN = 1, WHITE_WIN, DRAW, NO_END} EndMode;
typedef enum {FALSE = 0, TRUE} Boolean;
typedef struct {
    int x;
    int y;
} Point;

// board - global variable
int board[BOARD_SIZE][BOARD_SIZE] = {FREE};

/*******************************************************************************
* function name : exitWithError
* input : message
* output : -
* explanation : write to stderr the message and exit with code -1
*******************************************************************************/
void exitWithError(char *msg) {
    perror(msg);
    exit(-1);
}

void initBoard() {
    board[3][3] = BLACK;
    board[4][4] = BLACK;
    board[4][3] = WHITE;
    board[3][4] = WHITE;
}

void printBoard() {
    int i,j;
    for (i = 0; i < BOARD_SIZE; ++i) {
        for (j = 0; j < BOARD_SIZE; ++j) {
            printf("%d ", board[i][j]);
        }
        printf("\n");
    }
}

void changeCoinsOnBoard(Point points[], int player) {
    int j = 0;
    while (j < BOARD_SIZE) {
        Point p = points[j];
        if (p.x == -1) {
            break;
        }

        // place the player's coins on the board
        board[p.y][p.x] = player;
        j++;
    }
}

MoveMode checkAbove(int x, int y, int player, Boolean writeToBoard) {
    int i, k = 0;
    Point tempArr[8];
    // check above
    for (i = y; i >=0; --i) {
        Point p;
        // didn't find a matching color
        if (board[i][x] == FREE && (i != y)) {
            return  INVALID_SQUARE;
        }

        // only if the same color and not next to it
        if (board[i][x] == player && (i != y - 1)) {
            if (writeToBoard) {
                // set an end
                p.x = -1;
                tempArr[k++] = p;
                changeCoinsOnBoard(tempArr, player);
            }
            return VALID_MOVE;
        }

        // add candidate point to the temp array
        p.x = x;
        p.y = i;
        tempArr[k++] = p;
    }

    return INVALID_SQUARE;
}

MoveMode checkBelow(int x, int y, int player, Boolean writeToBoard) {
    int i, k = 0;
    Point tempArr[8];
    // check above
    for (i = y; i < BOARD_SIZE; ++i) {
        Point p;
        // didn't find a matching color
        if (board[i][x] == FREE && (i != y)) {
            return  INVALID_SQUARE;
        }

        // only if the same color and not next to it
        if (board[i][x] == player && (i != y + 1)) {
            if (writeToBoard) {
                // set an end
                p.x = -1;
                tempArr[k++] = p;
                changeCoinsOnBoard(tempArr, player);
            }
            return VALID_MOVE;
        }

        p.x = x;
        p.y = i;
        tempArr[k++] = p;
    }

    return  INVALID_SQUARE;
}


MoveMode checkRight(int x, int y, int player, Boolean writeToBoard) {
    int i, k = 0;
    Point tempArr[8];
    // check above
    for (i = x; i < BOARD_SIZE; ++i) {
        Point p;
        // didn't find a matching color
        if (board[y][i] == FREE && (i != x)) {
            return  INVALID_SQUARE;
        }

        // only if the same color and not next to it
        if (board[y][i] == player && (i != x + 1)) {
            if (writeToBoard) {
                // set an end
                p.x = -1;
                tempArr[k++] = p;
                changeCoinsOnBoard(tempArr, player);
            }
            return VALID_MOVE;
        }

        p.x = i;
        p.y = y;
        tempArr[k++] = p;
    }

    return  INVALID_SQUARE;
}

MoveMode checkLeft(int x, int y, int player, Boolean writeToBoard) {
    int i, k = 0;
    Point tempArr[8];
    // check above
    for (i = x; i >= 0; --i) {
        Point p;
        // didn't find a matching color
        if (board[y][i] == FREE && (i != x)) {
            return  INVALID_SQUARE;
        }

        // only if the same color and not next to it
        if (board[y][i] == player && (i != x - 1)) {
            if (writeToBoard) {
                // set an end
                p.x = -1;
                tempArr[k++] = p;
                changeCoinsOnBoard(tempArr, player);
            }
            return VALID_MOVE;
        }

        p.x = i;
        p.y = y;
        tempArr[k++] = p;
    }

    return  INVALID_SQUARE;
}

MoveMode checkUpperRight(int x, int y, int player, Boolean writeToBoard) {
    int i, k = 0, j = y;
    Point tempArr[8];
    // check above
    for (i = x; i < BOARD_SIZE; ++i) {
        Point p;
        // didn't find a matching color
        if (board[j][i] == FREE && (i != x)) {
            return  INVALID_SQUARE;
        }

        // only if the same color and not next to it
        if (board[j][i] == player && (i != x + 1)) {
            if (writeToBoard) {
                // set an end
                p.x = -1;
                tempArr[k++] = p;
                changeCoinsOnBoard(tempArr, player);
            }
            return VALID_MOVE;
        }

        p.x = i;
        p.y = j;
        tempArr[k++] = p;
        j--;
    }

    return  INVALID_SQUARE;
}

MoveMode checkUpperLeft(int x, int y, int player, Boolean writeToBoard) {
    int i, k = 0, j = y;
    Point tempArr[8];
    // check above
    for (i = x; i >= 0; --i) {
        Point p;
        // didn't find a matching color
        if (board[j][i] == FREE && (i != x)) {
            return  INVALID_SQUARE;
        }

        // only if the same color and not next to it
        if (board[j][i] == player && (i != x - 1)) {
            if (writeToBoard) {
                // set an end
                p.x = -1;
                tempArr[k++] = p;
                changeCoinsOnBoard(tempArr, player);
            }
            return VALID_MOVE;
        }

        p.x = i;
        p.y = j;
        tempArr[k++] = p;
        j--;
    }

    return  INVALID_SQUARE;
}

MoveMode checkDownLeft(int x, int y, int player, Boolean writeToBoard) {
    int i, k = 0, j = y;
    Point tempArr[8];
    // check above
    for (i = x; i >= 0; --i) {
        Point p;
        // didn't find a matching color
        if (board[j][i] == FREE && (i != x)) {
            return  INVALID_SQUARE;
        }

        // only if the same color and not next to it
        if (board[j][i] == player && (i != x - 1)) {
            if (writeToBoard) {
                // set an end
                p.x = -1;
                tempArr[k++] = p;
                changeCoinsOnBoard(tempArr, player);
            }
            return VALID_MOVE;
        }

        p.x = i;
        p.y = j;
        tempArr[k++] = p;
        j++;
    }

    return  INVALID_SQUARE;
}

MoveMode checkDownRight(int x, int y, int player, Boolean writeToBoard) {
    int i, k = 0, j = y;
    Point tempArr[8];
    // check above
    for (i = x; i < BOARD_SIZE; ++i) {
        Point p;
        // didn't find a matching color
        if (board[j][i] == FREE && (i != x)) {
            return  INVALID_SQUARE;
        }

        // only if the same color and not next to it
        if (board[j][i] == player && (i != x + 1)) {
            if (writeToBoard) {
                // set an end
                p.x = -1;
                tempArr[k++] = p;
                changeCoinsOnBoard(tempArr, player);
            }
            return VALID_MOVE;
        }

        p.x = i;
        p.y = j;
        tempArr[k++] = p;
        j++;
    }

    return  INVALID_SQUARE;
}

EndMode checkEndGame(int player) {
    // we check if the other player can't make a move
    int oppColor = (player == BLACK) ? WHITE:BLACK;
    int i, j;
    int black = 0, white = 0, free = 0;
    Boolean playerHasMoves = FALSE, oppPlayerHasMoves = FALSE;

    for (i = 0; i < BOARD_SIZE; i++) {
        for (j = 0; j < BOARD_SIZE; j ++) {
            if (checkMove(j, i, oppColor, FALSE) == VALID_MOVE) {
                oppPlayerHasMoves = TRUE;
            }

            if (checkMove(j, i, player, FALSE) == VALID_MOVE) {
                playerHasMoves = TRUE;
            }

            if (board[i][j] == WHITE) {
                white++;
            } else if (board[i][j] == BLACK) {
                black++;
            } else {
                free++;
            }
        }
    }

    // both players can still play
    if (playerHasMoves && oppPlayerHasMoves) {
        return NO_END;
    }

    // if one of them has no legal move
    if (!playerHasMoves || !oppPlayerHasMoves) {
        if (white > black) {
            return WHITE_WIN;
        }

        if (white < black) {
            return BLACK_WIN;
        }

        return DRAW;
    }
}

MoveMode checkMove(int x, int y, int player, Boolean writeToBoard) {
    int i, j;
    int savePos[8] = {-2};

    // trivial checks
    if (x >= BOARD_SIZE || x < 0) {
        return NO_SUCH_SQUARE;
    }

    if (y >= BOARD_SIZE || y < 0) {
        return NO_SUCH_SQUARE;
    }

    if (board[y][x] != FREE) {
        return INVALID_SQUARE;
    }

    // start checking non trivial checks

    savePos[0] = checkAbove(x, y, player, writeToBoard); // check above
    savePos[1] = checkBelow(x, y, player, writeToBoard); // check below
    savePos[2] = checkRight(x, y, player, writeToBoard); // check right
    savePos[3] = checkLeft(x, y, player, writeToBoard); // check left
    savePos[4] = checkUpperRight(x, y, player, writeToBoard); // check upper right
    savePos[5] = checkUpperLeft(x, y, player, writeToBoard); // check upper left
    savePos[6] = checkDownRight(x, y, player, writeToBoard); // check down right
    savePos[7] = checkDownLeft(x, y, player, writeToBoard); // check down left

    for (i = 0; i < BOARD_SIZE; i++) {
        if (savePos[i] != INVALID_SQUARE) {
            return VALID_MOVE;
        }
    }

    return INVALID_SQUARE;
}

int main(int argc, char **argv) {
    int x,y;

    initBoard();
    printBoard();

    scanf("%d,%d", &x, &y);
    checkMove(x, y, WHITE, TRUE);

    printBoard();
    return 0;
}


/*
 * override shared memory each move.
 * each move : sender checks for end game, and receiver also checks.
 * at end game, one player writes to shared memory for server to print end game results.
 *
 * end :
 * 1. board is full.
 * 2. no chips left.
 * 3.
 */