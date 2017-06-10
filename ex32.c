#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define BOARD_SIZE 8
#define BLACK 2
#define WHITE 1
#define FREE 0
#define MEM_SIZE 1024

typedef enum {INVALID_SQUARE = 0,NO_SUCH_SQUARE, VALID_MOVE} MoveMode;
typedef enum {BLACK_WIN = 1, WHITE_WIN, DRAW, NO_END} EndMode;
typedef enum {FALSE = 0, TRUE} Boolean;
typedef struct {
    int x;
    int y;
} Point;

// board - global variable
int board[BOARD_SIZE][BOARD_SIZE] = {FREE};
// player - global variable
int curPlayer;
// sMBuf - shared memory pointer, global variable
char *sMBuf;
// end game state - global variable
EndMode gameState;

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

/*******************************************************************************
* function name : initBoard
* input : -
* output : -
* explanation : initialize the board.
*******************************************************************************/
void initBoard() {
    board[3][3] = BLACK;
    board[4][4] = BLACK;
    board[4][3] = WHITE;
    board[3][4] = WHITE;

    // set game state
    gameState = NO_END;
}

/*******************************************************************************
* function name : printBoard
* input : -
* output : -
* explanation : print the board.
*******************************************************************************/
void printBoard() {
    int i,j;
    printf("The board is:\n");
    for (i = 0; i < BOARD_SIZE; ++i) {
        for (j = 0; j < BOARD_SIZE; ++j) {
            printf("%d ", board[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

/*******************************************************************************
* function name : changeCoinsOnBoard
* input : Point points[], int player
* output : -
* explanation : set the correct coins on the board.
*******************************************************************************/
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

/*******************************************************************************
* function name : checkAbove
* input : int x, int y, int player, Boolean writeToBoard
* output : VALID_MOVE if all ok, else INVALID_SQUARE
* explanation : check for a valid move
*******************************************************************************/
MoveMode checkAbove(int x, int y, int player, Boolean writeToBoard) {
    int i, k = 0;
    Point tempArr[8];

    if (board[y - 1][x] == player) return INVALID_SQUARE;

    // check above
    for (i = y; i >=0; --i) {
        Point p;
        // didn't find a matching color
        if (board[i][x] == FREE && (i != y)) {
            return  INVALID_SQUARE;
        }

        // only if the same color and not next to it
        if (board[i][x] == player && (i != y - 1) && (i != y))  {
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

/*******************************************************************************
* function name : checkBelow
* input : int x, int y, int player, Boolean writeToBoard
* output : VALID_MOVE if all ok, else INVALID_SQUARE
* explanation : check for a valid move
*******************************************************************************/
MoveMode checkBelow(int x, int y, int player, Boolean writeToBoard) {
    int i, k = 0;
    Point tempArr[8];

    if (board[y + 1][x] == player) return INVALID_SQUARE;

    // check above
    for (i = y; i < BOARD_SIZE; ++i) {
        Point p;
        // didn't find a matching color
        if (board[i][x] == FREE && (i != y)) {
            return  INVALID_SQUARE;
        }

        // only if the same color and not next to it
        if (board[i][x] == player && (i != y + 1) && (i != y)) {
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

/*******************************************************************************
* function name : checkRight
* input : int x, int y, int player, Boolean writeToBoard
* output : VALID_MOVE if all ok, else INVALID_SQUARE
* explanation : check for a valid move
*******************************************************************************/
MoveMode checkRight(int x, int y, int player, Boolean writeToBoard) {
    int i, k = 0;
    Point tempArr[8];

    if (board[y][x + 1] == player) return INVALID_SQUARE;

    // check above
    for (i = x; i < BOARD_SIZE; ++i) {
        Point p;
        // didn't find a matching color
        if (board[y][i] == FREE && (i != x)) {
            return  INVALID_SQUARE;
        }

        // only if the same color and not next to it
        if (board[y][i] == player && (i != x + 1) && (i != x)) {
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

/*******************************************************************************
* function name : checkLeft
* input : int x, int y, int player, Boolean writeToBoard
* output : VALID_MOVE if all ok, else INVALID_SQUARE
* explanation : check for a valid move
*******************************************************************************/
MoveMode checkLeft(int x, int y, int player, Boolean writeToBoard) {
    int i, k = 0;
    Point tempArr[8];

    if (board[y][x - 1] == player) return INVALID_SQUARE;

    // check above
    for (i = x; i >= 0; --i) {
        Point p;
        // didn't find a matching color
        if (board[y][i] == FREE && (i != x)) {
            return  INVALID_SQUARE;
        }

        // only if the same color and not next to it
        if (board[y][i] == player && (i != x - 1) && (i != x)) {
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

/*******************************************************************************
* function name : checkUpperRight
* input : int x, int y, int player, Boolean writeToBoard
* output : VALID_MOVE if all ok, else INVALID_SQUARE
* explanation : check for a valid move
*******************************************************************************/
MoveMode checkUpperRight(int x, int y, int player, Boolean writeToBoard) {
    int i, k = 0, j = y;
    Point tempArr[8];

    if (board[y - 1][x + 1] == player) return INVALID_SQUARE;

    // check above
    for (i = x; i < BOARD_SIZE; ++i) {
        Point p;
        // didn't find a matching color
        if (board[j][i] == FREE && (i != x)) {
            return  INVALID_SQUARE;
        }

        // only if the same color and not next to it
        if (board[j][i] == player && (i != x + 1) && (i != x)) {
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
        if (j < 0) break;
    }

    return  INVALID_SQUARE;
}

/*******************************************************************************
* function name : checkUpperLeft
* input : int x, int y, int player, Boolean writeToBoard
* output : VALID_MOVE if all ok, else INVALID_SQUARE
* explanation : check for a valid move
*******************************************************************************/
MoveMode checkUpperLeft(int x, int y, int player, Boolean writeToBoard) {
    int i, k = 0, j = y;
    Point tempArr[8];

    if (board[y - 1][x - 1] == player) return INVALID_SQUARE;

    // check above
    for (i = x; i >= 0; --i) {
        Point p;
        // didn't find a matching color
        if (board[j][i] == FREE && (i != x)) {
            return  INVALID_SQUARE;
        }

        // only if the same color and not next to it
        if (board[j][i] == player && (i != x - 1) && (i != x)) {
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
        if (j < 0) break;
    }

    return  INVALID_SQUARE;
}

/*******************************************************************************
* function name : checkDownLeft
* input : int x, int y, int player, Boolean writeToBoard
* output : VALID_MOVE if all ok, else INVALID_SQUARE
* explanation : check for a valid move
*******************************************************************************/
MoveMode checkDownLeft(int x, int y, int player, Boolean writeToBoard) {
    int i, k = 0, j = y;
    Point tempArr[8];

    if (board[y + 1][x - 1] == player) return INVALID_SQUARE;

    // check above
    for (i = x; i >= 0; --i) {
        Point p;
        // didn't find a matching color
        if (board[j][i] == FREE && (i != x)) {
            return  INVALID_SQUARE;
        }

        // only if the same color and not next to it
        if (board[j][i] == player && (i != x - 1) && (i != x)) {
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
        if (j >= BOARD_SIZE) break;
    }

    return  INVALID_SQUARE;
}

/*******************************************************************************
* function name : checkDownRight
* input : int x, int y, int player, Boolean writeToBoard
* output : VALID_MOVE if all ok, else INVALID_SQUARE
* explanation : check for a valid move
*******************************************************************************/
MoveMode checkDownRight(int x, int y, int player, Boolean writeToBoard) {
    int i, k = 0, j = y;
    Point tempArr[8];

    if (board[y + 1][x + 1] == player) return INVALID_SQUARE;

    // check above
    for (i = x; i < BOARD_SIZE; ++i) {
        Point p;
        // didn't find a matching color
        if (board[j][i] == FREE && (i != x)) {
            return  INVALID_SQUARE;
        }

        // only if the same color and not next to it
        if (board[j][i] == player && (i != x + 1) && (i != x)) {
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
        if (j >= BOARD_SIZE) break;
    }

    return  INVALID_SQUARE;
}

/*******************************************************************************
* function name : checkMove
* input : int x, int y, int player, Boolean writeToBoard
* output : VALID_MOVE if all ok, else INVALID_SQUARE
* explanation : start a series of checks to find if a move is valid
*******************************************************************************/
MoveMode checkMove(int x, int y, int player ,Boolean writeToBoard) {
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

/*******************************************************************************
* function name : checkEndGame
* input : int player
* output : NO_END if game is on or WHITE_WIN, BLACK_WIN and DRAW
* explanation : check for if the game has ended
*******************************************************************************/
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
        printf("<--- can play --->\n");
        return NO_END;
    }

    // if one of them has no legal move
    if (!playerHasMoves || !oppPlayerHasMoves) {
        if (white > black) {
            printf("<--- white --->\n");
            return WHITE_WIN;
        }

        if (white < black) {
            printf("<--- black --->\n");
            return BLACK_WIN;
        }
    }

    printf("<--- draw --->\n");
    return DRAW;
}

/*******************************************************************************
* function name : charToPlayer
* input : char c
* output : WHITE or BLACK
* explanation : translate char to player.
*******************************************************************************/
int charToPlayer(char c) {
    if (c == 'w') return WHITE;
    if (c == 0) return curPlayer; // if it's 0 means the first player didn't made a move
    return BLACK;
}

/*******************************************************************************
* function name : playerToChar
* input : int player
* output : 'w' or 'b'
* explanation : translate player to char.
*******************************************************************************/
char playerToChar(int player) {
    if (player == WHITE) return 'w';
    return 'b';
}

/*******************************************************************************
* function name : sendMoveToSharedMemory
* input : int player, int x, int y
* output : -
* explanation : write the move to the shared memory.
*******************************************************************************/
void sendMoveToSharedMemory(int player, int x, int y) {
    char p = playerToChar(player);
    sprintf(sMBuf, "%c%d%d", p, x, y);
}

/*******************************************************************************
* function name : getMoveFromSharedMemory
* input : -
* output : -
* explanation : get the move from the shared memory and execute it.
*******************************************************************************/
void getMoveFromSharedMemory() {
    int x, y;
    int oppPlayer = (curPlayer == BLACK) ? WHITE:BLACK;

    x = sMBuf[1] - '0';
    y = sMBuf[2] - '0';

    // preform other player move
    checkMove(x, y, oppPlayer, TRUE);

    // check end game
    gameState = checkEndGame(curPlayer);
}

/*******************************************************************************
* function name : doOneMove
* input : -
* output : -
* explanation : execute one move of gmaeplay.
*******************************************************************************/
void doOneMove() {
    int x, y;
    MoveMode m;

    printf("Please choose a square\n");
    do {
        scanf("\n[%d,%d]", &x, &y);

        m = checkMove(x, y, curPlayer, TRUE);
        if (m == NO_SUCH_SQUARE) {
            printf("No such square\n");
        } else if (m == INVALID_SQUARE) {
            printf("This square is invalid\n");
        } else {
            // valid move
            break;
        }

        printf("Please choose another square\n");
    } while (TRUE);

    // valid move
    printBoard();
    printf("Waiting for the other player to make a move\n");
    sendMoveToSharedMemory(curPlayer, x, y);
    printf("!!!!!! write to share !!!!!!!!\n");
    // check end game
    gameState = checkEndGame(curPlayer);
    printf("!!!!!! after %d !!!!!!!!\n", gameState);
}

/*******************************************************************************
* function name : start
* input : int signum
* output : -
* explanation : wake up function after pause.
*******************************************************************************/
void start(int signum) {
    // do nothing - just wakup from pause
    //printf("%d\n", getpid());
}

/*******************************************************************************
* function name : main
* input : int argc, char **argv
* output : 0
* explanation : main function
*******************************************************************************/
int main(int argc, char **argv) {
    int x,y;
    int fifoFD;
    char buffer[20] = {0};
    struct sigaction sigUserHandler;
    key_t key;
    int shmid;
    pid_t pid;
    struct shmid_ds ds;

    sigset_t blocked;

    sigemptyset(&blocked);
    // set handler for SIGALRM
    sigUserHandler.sa_handler = start;
    sigUserHandler.sa_mask = blocked;
    sigUserHandler.sa_flags = 0;

    if ((sigaction(SIGUSR1, &sigUserHandler, NULL)) < 0) {
        exitWithError("sigaction error");
    }
/*
    initBoard();
    printBoard();

    scanf("[%d,%d]", &x, &y);

    checkMove(x, y, WHITE, TRUE);

    printBoard(); */

    // create key
    key = ftok("ex31.c", 'k');
    if (((key_t) - 1) == key) {
        exitWithError("ftok error");
    }

    // open the fifo for communication
    if ((fifoFD = open("fifo_clientTOserver", O_WRONLY)) < 0) {
        exitWithError("fifo error");
    }

    // assign pid
    pid = getpid();

    // send pid to server
    if ((write(fifoFD, &pid, sizeof(pid_t))) < 0) {
        exitWithError("write error");
    }

    // close the fifo
    if ((close(fifoFD)) < 0) {
        exitWithError("close error");
    }

    // wait for SIGUSR1
    pause();

    // get the shmid by key
    if ((shmid = shmget(key, MEM_SIZE, 0644 | IPC_CREAT)) < 0) {
        exitWithError("shmget error");
    }

    // attach to the shared memory
    sMBuf = (char *) shmat( shmid, NULL, 0);
    if (((char *) - 1) == sMBuf) {
        exitWithError("shmat error");
    }

    // check to determine which player I am
    if (shmctl(shmid, IPC_STAT, &ds) == -1) {
        exitWithError("shmctl error");
    }

    // determine player
    if (ds.shm_nattch == 2) {
        curPlayer = BLACK;
    } else if (ds.shm_nattch == 3) {
        curPlayer = WHITE;
    }

    // initialize game
    initBoard();

    // first play
    if (curPlayer == BLACK) {
        printBoard();
        doOneMove();
    }

    // game loop
    while (TRUE) {
        // current player move
        if (charToPlayer(sMBuf[0]) != curPlayer) {
            getMoveFromSharedMemory();
            if (gameState != NO_END) break;
            printBoard();
            doOneMove();
            if (gameState != NO_END) break;
        } else {
            // wait for the other player to play
            //printf("Waiting for the other player to make a move\n");
            sleep(1);
        }
    }

    // notify server on game end
    if (sMBuf[6] != 'e') {
        sleep(2);
        sMBuf[6] = 'e';

        // print end results
        switch (gameState) {
            case WHITE_WIN: printf("Winning player: White\n");
                sMBuf[1] = 'w';
                break;
            case BLACK_WIN: printf("Winning player: Black\n");
                sMBuf[1] = 'b';
                break;
            case DRAW:      printf("No winning player\n");
                sMBuf[1] = 'd';
            default:        break;
        }
    } else {
        switch (sMBuf[7]) {
            case 'w': printf("Winning player: White\n");
                break;
            case 'b': printf("Winning player: Black\n");
                break;
            case 'd': printf("No winning player\n");
                break;
            default:
                break;
        }
    }

    // detach from the shared memory
    if ((shmdt(sMBuf)) <0 ) {
        exitWithError("shmdt error");
    }

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