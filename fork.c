#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int main(int argc, const char *argv[])
{
    if(argc < 2) {
        printf("Usage: fork <path to binary>\n");
        return 0;
    }

    int status;
    pid_t id = fork();

    if(id > 0) {
        printf("Eltern PID: %i\n", id);
        // wait for child
        while(wait(&status) > 0);
    }
    else {
        printf("Kind PID: %i\n", id);
        
        // execute the command (remove own binary path)
        char * const* newArgv = (char* const*)(argv + 1);
        execv(argv[1], newArgv);
    }
 
    return 0;
}
