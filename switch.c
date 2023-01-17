#include "resource.h"

#define NUM_THREADS 100

int switchConnects[10][10];

bool switches_is_connected(int sw1, int sw2)
{
    if (switchConnects[sw1-1][sw2-1]> 0)
        return true;
    return false;
}

int getNumber(char* s){
    char c; 
    int i,digit,number=0; 
    for(i=0;i<strlen(s);i++) 
    { 
        c = s[i]; 
        if(c>='0' && c<='9') //to confirm it's a digit 
        { 
            digit = c - '0'; 
            number = number*10 + digit; 
        } 
    } 
    return number;
}

char* readFromPipe(char* pipe){
    char messages[100];
    int fd = open(pipe , O_RDONLY);
    // if(read(fd, messages, sizeof(messages)) < 0){
    //     printf(strerror(errno));
    //     exit(1);
    // }
    read(fd, messages, sizeof(messages));
    close(fd);
    return messages;
}

int portExist(char* portName, Switch netSwitch){
    for( int i=0 ; i<=netSwitch.filled ; i++){
        if(strcmp(netSwitch.ports[i], portName) == 0){
            return i;
        }
    }
    return -1;
}

void broadCast(char* message, char* inputPort, Switch netSwitch){
    write(netSwitch.selfPipe,  message, sizeof(message));
    for(int i=0 ; i<netSwitch.filled; i++){
        if(strstr(netSwitch.ports[i],"switch") == NULL){
            int switchNum = getNumber(netSwitch.ports[i]);
            if(!switches_is_connected(netSwitch.switchNum, switchNum)){
                continue;
            }
        } 
        if(strcmp(netSwitch.ports[i], inputPort) != 0){
            write(netSwitch.ports[i], message, sizeof(message));
        }
    }
}

int readCommand(void* args){
    Switch *netSwitch = args;
    char command[100];
    if(netSwitch->filled<netSwitch->definedPort){
        int size = read(1, command, sizeof(command));
        fprintf(stdout,"%d", size);
        if(size>0){
            fprintf(stderr, "%s", command);
            netSwitch->ports[netSwitch->filled++] = command;
        }
    }
    else{
        printf("switch port id filled\n");
        return 0;
    }
        
}

int readFile(void* arg){
    readFile_t* info = (readFile_t*)arg;
    int namedPipe = open(info->port, O_RDONLY);
    char messages[100];
    read(namedPipe, messages, sizeof(messages));
    close(namedPipe);
    if(messages != NULL){
        char code[2];
        strncpy(code, messages, 2);
        char depart ="system";
        strcat(depart, code);
        int portIndex = portExist(depart, *info->netSwitch);
        if(portIndex != -1){
            write(info->netSwitch->ports[portIndex], messages, sizeof(messages));
        }
        else{
            broadCast(messages, info->port, *info->netSwitch);
        }
    }
}

int main(int argc, char* argv[]){

    Switch netSwitch;
    netSwitch.filled = 0;
    netSwitch.definedPort = atoi(argv[1]);
    netSwitch.selfPipe = argv[2];
    netSwitch.switchNum = atoi(argv[2]);
    
    pthread_t threads[NUM_THREADS];
    readFile_t args[NUM_THREADS];
    int thread_count=0;

    // printf("switch created!\n");
    while(1){
        char command[100];
        if(netSwitch.filled <netSwitch.definedPort){
            read(1, command, sizeof(command));
            netSwitch.ports[netSwitch.filled++] = command;

            args[thread_count].netSwitch = &netSwitch;
            args[thread_count].port = command;
            pthread_create(threads[thread_count], 0, readFile, (void *)&args[thread_count]);
			pthread_detach(threads[thread_count]);
            thread_count++;
        }
        else{
            printf("switch port id filled\n");
            return 0;
        }

    }  

}