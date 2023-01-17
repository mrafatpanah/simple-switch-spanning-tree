//just for debuging....
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{   char* h="hello bro!";
    int pid = fork();
    printf("hi\n");
    if(pid == 0){
        printf("hello the pid is");
        printf("%s", h);
        printf("\n");
    }
    return 0;




    for (int i=0 ; i <= netSwitch.filled ; i++){
            int namedPipe = open(netSwitch.ports[i], O_RDONLY);
            // if(FD_ISSET(namedPipe, &readfd)){
            char messages[100];
            read(namedPipe, messages, sizeof(messages));
            close(namedPipe);
            if(messages != NULL){
                char code[2];
                strncpy(code, messages, 2);
                char depart ="system";
                strcat(depart, code);
                if(portExist(depart, netSwitch.ports, netSwitch.filled) != -1){
                    write(netSwitch.ports[i], messages, sizeof(messages));
                }
                else{
                    broadCast(messages, netSwitch.ports, i, netSwitch.filled);
                }

            }
            // }
        }
        pthread_join(thread_id, NULL);
    }
    pthread_exit(NULL);

    // for (int i=0 ; i <= netSwitch.filled ; i++){
    //     char messages[100] = readFromPipe(netSwitch.ports[i]);
    //     if(messages != NULL){
    //         char code[2];
    //         strncpy(code, messages, 2);
    //         char depart ="system";
    //         strcat(depart, code);
    //         if(portExist(depart, netSwitch.ports, netSwitch.filled) != -1){
    //             write(netSwitch.ports[i], messages, sizeof(messages));
    //         }
    //         else{
    //             broadCast(messages, netSwitch.ports, i, netSwitch.filled);
    //         }

    //     }

    // }
}