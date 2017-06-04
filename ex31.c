
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/fcntl.h>


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

int main(int argc, char **argv) {
    pid_t firstPID, secondPID;
    int fifoFD;
    char buffer[20];
    key_t key;

    // create channel for communication
    if ((mkfifo("fifo_clientTOserver", 0777)) < 0) {
        exitWithError("fifo error");
    }

    // open for read only
    if ((fifoFD = open("fifo_clientTOserver", O_RDONLY)) < 0) {
        exitWithError("open error");
    }

    // get first pid
    if ((read(fifoFD, buffer, sizeof(buffer))) < 0) {
        exitWithError("read error");
    }

    firstPID = atoi(buffer);

    // get second pid
    if ((read(fifoFD, buffer, sizeof(buffer))) < 0) {
        exitWithError("read error");
    }

    secondPID = atoi(buffer);

    // remove the fifo
    if ((unlink("fifo_clientTOserver")) < 0) {
        exitWithError("unlink error");
    }

    key = ftok("ex31.c", 'k');
    if (((key_t) - 1) == key) {
        exitWithError("ftok error");
    }





    return 0;
}