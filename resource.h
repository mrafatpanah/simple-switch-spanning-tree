#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>

#define MAXPORT 10
#define MSGSIZE 2048
#define READ 0
#define WRITE 1
#define CHUNKSIZE 256
#define V 10
#define INT_MAX 10



typedef struct{
    char* ports[MAXPORT];
    int filled;
    int definedPort;
    char* selfPipe;
    int switchNum;
}Switch;

typedef struct
{
    Switch *netSwitch;
    char* port;
} readFile_t;

extern int switchConnects[10][10];