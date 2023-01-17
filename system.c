#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#define CHUNKSIZE 256
#define NUM_THREADS 100

typedef struct
{
	int pipe;
	char filename[32];
} receivepipe_t;

typedef struct
{
	int pipe;
	char filename[32];
    char sysId[2];
} sendpipe_t;


void splitStr(char str[], char newString[3][10]){
   int n=0,i,j=0;

	for(i=0;strlen(str);i++)
	{
		if(str[i]!=' '){
			newString[n][j++]=str[i];
		}
		else{
			newString[n][j++]='\0';//insert NULL
			n++;
			j=0;
		}
		if(str[i]=='\0')
		    break;
	}
}

int receivefile(receivepipe_t *receiving){
    char temp[CHUNKSIZE-2] = "";
    char msg[CHUNKSIZE]="";

    size_t bytesRead = 0;
    char _filename[40];
    strcpy(_filename, receiving->filename);
    strcat(_filename, "-received");
    int fd_f = open(_filename , O_RDWR | O_CREAT , 0666);

    if (fd_f != NULL && receiving->pipe != NULL)
    {
        // read from pipe
        while ((bytesRead = fread(msg, 1, CHUNKSIZE, receiving->pipe)) > 0)
        {
            strncpy(temp, msg + 2, sizeof(temp));
            if( fwrite(temp, 1, bytesRead-2, fd_f) != bytesRead-2)
                printf("Error writing file\n");
                close (fd_f);
                close(receiving->pipe);
                return -1;
        }
        close (fd_f);
        close(receiving->pipe);
        return 0;
    }
    close (fd_f);
    close(receiving->pipe);
    return -1;
}

int sendfile(sendpipe_t *sending)
{
    char temp[CHUNKSIZE-2] = "";
    char msg[CHUNKSIZE] = "";
    size_t bytesRead = 0;

    int fd_f = open(sending->filename , O_RDWR | O_CREAT , 0666);

    if (fd_f != NULL && sending->pipe != NULL)
    {
        // read from pipe
        while ((bytesRead = fread(temp, 1, CHUNKSIZE-2, fd_f)) > 0)
        {
            strcat(msg, sending->sysId);
            strcat(msg,temp);
            usleep(100);
            if( fwrite(msg, 1, CHUNKSIZE, sending->pipe) != bytesRead+2)
                printf("Error sending file\n");
                close (fd_f);
                close(sending->pipe);
                return -1;
        }
        close (fd_f);
        close(sending->pipe);
        return 0;
    }
    close (fd_f);
    close(sending->pipe);
    return -1;
}

int main(int argc, char* argv[]){
    pthread_t threads[NUM_THREADS];
    sendpipe_t sending[NUM_THREADS];
    receivepipe_t receiving[NUM_THREADS];

    char newString[4][10];
    char systemId[2];
    strcpy(systemId, argv[1]);
    // printf("here we are in system!\n");

    char command[100];
    // fd_set readfd;
    // int maxFd= 1000;
    char command_type[10];
    int namedPipe;

    int thread_count=0;

    while(1){
        // FD_ZERO(&readfd);
        // FD_SET(1, &readfd);
        // select(maxFd, &readfd, NULL, NULL, NULL);
        // if(FD_ISSET(1, &readfd)) {
        read(1, command, sizeof(command));
        splitStr(command, newString);
        strcpy(command_type, newString[0]);
        // }

        if (strcmp(command_type, "connect") == 0) {
            namedPipe = open(newString[1], O_RDONLY);
        }

        else if (strcmp(command_type, "send") == 0) {
            sending[thread_count].pipe = namedPipe;
            strcpy(sending[thread_count].filename, newString[2]);
            strcpy(sending[thread_count].sysId, systemId);

            pthread_create(threads[thread_count], 0, sendfile, (void *)&sending[thread_count]);
			pthread_detach(threads[thread_count]);
            thread_count++;
            // if(FD_ISSET(namedPipe, &readfd)){
            //     sendfile(namedPipe, newString[2], systemId);
            // }
        }

        else if(strcmp(command_type, "receive") == 0) {
            receiving[thread_count].pipe = namedPipe;
            strcpy(receiving[thread_count].filename, newString[2]);

            pthread_create(threads[thread_count], 0, receivefile, (void *)&receiving[thread_count]);
			pthread_detach(threads[thread_count]);
            thread_count++;

            // if(FD_ISSET(namedPipe, &readfd)){
            //     sendfile(namedPipe, newString[2], systemId);
            // }
        }
    }
    return 0;
}
