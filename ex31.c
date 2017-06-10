
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/fcntl.h>
#include <string.h>
#include <signal.h>

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

/*******************************************************************************
* function name : main
* input : int argc, char **argv
* output : 0
* explanation : main function
*******************************************************************************/
int main(int argc, char **argv) {
    pid_t firstPID, secondPID;
    int fifoFD;
    char buffer[20];
    key_t key;
    int shmid;
    char *sharedMemory, *shmBuf;

    // create channel for communication
    if ((mkfifo("fifo_clientTOserver", O_CREAT|0777)) < 0) {
        exitWithError("fifo error");
    }

    // open for read only
    if ((fifoFD = open("fifo_clientTOserver", O_RDWR)) < 0) {
        exitWithError("open error");
    }

    // get first pid
    if ((read(fifoFD, &firstPID, sizeof(pid_t))) < 0) {
        exitWithError("read error");
    }
    printf("first : %d\n", firstPID);
    // get second pid
    if ((read(fifoFD, &secondPID, sizeof(pid_t))) < 0) {
        exitWithError("read error");
    }
    printf("second :%d\n", secondPID);

    // remove the fifo
    if ((unlink("fifo_clientTOserver")) < 0) {
        exitWithError("unlink error");
    }

    key = ftok("ex31.c", 'k');
    if (((key_t) - 1) == key) {
        exitWithError("ftok error");
    }

    // create shared memory
    if ((shmid = shmget(key, 4096, IPC_CREAT | 0644)) < 0) {
        exitWithError("shemget error");
    }

    // attach to the shared memory
    sharedMemory = (char *) shmat( shmid, NULL, 0);
    if (((char *) - 1) == sharedMemory) {
        exitWithError("shmat error");
    }

    // initialize shared memory
    memset(sharedMemory, 0, 4096);

    // signal first player
    if ((kill(firstPID, SIGUSR1)) < 0) {
        exitWithError("kill error");
    }

    // wait until the first player makes his move
    sleep(1);

    // signal first player
    if ((kill(secondPID, SIGUSR1)) < 0) {
        exitWithError("kill error");
    }

    // busy waiting for end of game
    while (sharedMemory[6] != 'e') {
        sleep(1);
    }

    // end game
    printf("GAME OVER !\n");
    if (sharedMemory[7] == 'w') {
        printf("Winning player: White\n");
    } else if (sharedMemory[7] == 'b') {
        printf("Winning player: Black\n");
    } else {
        printf("No winning player");
    }

    // detach from the shared memory
    if ((shmdt(sharedMemory)) <0 ) {
        exitWithError("shmdt error");
    }

    // remove shared memory
    if ((shmctl(shmid, IPC_RMID, NULL)) < 0) {
        exitWithError("shmctl error");
    }

    return 0;
}