#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) 
{
    int i = 1;

    // print each argument to screen
    for(; i < argc; i++) {
        printf(argv[i]);
        
        if(i == argc-1)
            printf("\n");
        else
            printf(" ");
    }

	return 0;
}
