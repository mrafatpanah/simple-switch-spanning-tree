#include "resource.h"

int switchConnects[V][V]={0};

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

// char* strToChar(string s){
//     char * str;
//     str = (char*) malloc(MSGSIZE * sizeof(int));
//     strcpy(str, s.c_str());
//     return str;
// }

void makeNamedPipe(char* fifoName){
    const char* name = fifoName;
    int fd = open(name, O_RDWR | O_CREAT , 0666);
    if(fd < 0){
        exit(1);
    }
    close(fd);
    mkfifo(name , 0666);
}

char* readFromPipe(char* pipes[]){
    char msg[MSGSIZE]="";
    char messages[100];
    for (int i=0 ; i<strlen(pipes); i++){
        int fd = open(pipes[i] , O_RDONLY);
        if(read(fd, msg, MSGSIZE) < 0){
            printf(strerror(errno));
            exit(1);
        }
        close(fd);
        //remove(pipes[i]);
        messages[i] = msg;

    }
    return messages;
}

int minKey(int key[], bool mstSet[])
{
	int min = INT_MAX, min_index;

	for (int v = 0; v < V; v++)
		if (mstSet[v] == false && key[v] < min)
			min = key[v], min_index = v;
	return min_index;
}

void primMST(int graph[V][V])
{
	int parent[V];
	int key[V];
	bool mstSet[V];

	for (int i = 0; i < V; i++)
		key[i] = INT_MAX, mstSet[i] = false;

	key[0] = 0;
	parent[0] = -1;

	for (int count = 0; count < V - 1; count++) {
		int u = minKey(key, mstSet);

		mstSet[u] = true;

		for (int v = 0; v < V; v++) {
			if (graph[u][v] && mstSet[v] == false && graph[u][v] < key[v])
				parent[v] = u, key[v] = graph[u][v];
		}
	}
	int i=0;
	for (i=0;i<V; i++) {
	    if (key[i] != INT_MAX) {
    	    graph[parent[i]][i] = key[i];
    	    graph[i][parent[i]] = key[i];
	    }
	}
}

int main(){

    char str[50];
    char newString[4][10];

    int switchPipes[10][2];
    char* switchNamedPipes[10];
    int switchesNum = 0;

    int systemPipes[100][2];
    char* systemNamedPipes[100];
    int systemsNum = 0;

    char buffer[CHUNKSIZE];

    while(1){
        fgets(str, sizeof str, stdin);
        splitStr(str, newString);
        int pid;
        if(strcmp(newString[0], "MySwitch") == 0){
            pipe(switchPipes[switchesNum]);
            if((pid = fork()) == 0){
                close(switchPipes[switchesNum][WRITE]);
                dup(switchPipes[switchesNum][READ]);
                // close(switchPipes[switchesNum][READ]);
                char pipeName[] = "switch";
                strcat(pipeName, newString[1]);
                // fprintf(stderr, "%s\n", pipeName);
                switchNamedPipes[switchesNum++] = pipeName;
                makeNamedPipe(pipeName);
                char* args[] = {"./switch",newString[2], pipeName ,newString[1], NULL};
                execv(args[0], args);
                exit(1);
            }
            wait(NULL);
        }
        else if(strcmp(newString[0],"MySystem") == 0){
            pipe(systemPipes[systemsNum]);
            if((pid = fork())== 0){
                close(systemPipes[systemsNum][WRITE]);
                dup2(systemPipes[systemsNum][READ], 0);
                close(systemPipes[systemsNum][READ]);
                char pipeName[] = "system";
                strcat(pipeName, newString[1]);
                systemNamedPipes[systemsNum++] = pipeName;
                makeNamedPipe(pipeName);
                char* args[] = {"./system", pipeName , NULL};
                execv(args[0], args);
                exit(1);
            }
            wait(NULL);
        }

        else if(strcmp(newString[0],"connect") == 0){
            int systemNum = atoi(newString[1]);
            int switchNum = atoi(newString[2]);
            int portNum = atoi(newString[3]);
            char command[30] = "connect ";
            strcat(command, switchNamedPipes[switchNum-1]);
            write(switchPipes[switchNum-1][WRITE], systemNamedPipes[systemsNum-1], sizeof(systemNamedPipes[systemsNum-1]));
            write(systemPipes[systemNum-1][WRITE], command, sizeof(command));
            for(int i=0; i<10; i++){
              if (switchConnects[switchNum-1][i] > 0) {
                switchConnects[switchNum-1][i]++;
                switchConnects[i][switchNum]++;
              }
            }
        }
        else if(strcmp(newString[0], "switchConnect") == 0){
            int switchNum1 = atoi(newString[1]);
            int switchNum2 = atoi(newString[2]);
            if(switchNum1 != switchNum2){
                write(switchPipes[switchNum1-1][WRITE], switchNamedPipes[switchNum2-1], sizeof(switchNamedPipes[switchNum2-1]));
                write(switchPipes[switchNum2-1][WRITE], switchNamedPipes[switchNum1-1], sizeof(switchNamedPipes[switchNum1-1]));
                switchConnects[switchNum1-1][switchNum2-1]++;
                switchConnects[switchNum2-1][switchNum1-1]++;

            }
        }
        else if(strcmp(newString[0],"send") == 0){
            int senderSystemNum = atoi(newString[1]);
            int receiverSystemNum = atoi(newString[2]);

            char sender_command[30] = "send ";
            strcat(sender_command, newString[3]);
            write(systemPipes[senderSystemNum-1], sender_command, sizeof(sender_command));

            char receiver_command[30] = "receive ";
            strcat(receiver_command, newString[3]);
            write(systemPipes[receiverSystemNum-1], receiver_command, sizeof(receiver_command));
        }
        else if (strcmp(newString[0], "spanningTree") == 0) {
          primMST(switchConnects);
        }
        // else if(strcmp(newString[0],"recieve") == 0){
        //     int senderSystemNum = atoi(newString[1]);
        //     int receiverSystemNum = atoi(newString[2]);


            // pass receive command via pipe to receiver
            // pass send commnad via pipe to sender
    }
    return 0;
}

