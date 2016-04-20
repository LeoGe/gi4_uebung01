#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, const char *argv[])
{
    if(argc < 2)
        printf("Usage: isset <env-name>\n");
    else {
        // the position of name in argv
        int pos_name;
        // the position of verbose argument
        int pos_verbose;

        if(argc == 2) {
            pos_name = 1;
            pos_verbose = -1;
        }   
        else {
            // search if there is any "-v" and set position to pos_verbose
            pos_verbose = 1;
            while(pos_verbose < argc && strcmp("-v", argv[pos_verbose]) != 0)
                pos_verbose++;

            pos_name = (pos_verbose==1?2:1);
        }
 
        char *content = getenv(argv[pos_name]); 

        // if the env var is found and the verbose flag set, then print the content to console
        if(content != NULL && pos_verbose > 0 && pos_verbose < argc)
            printf("%s\n", content);
    
        return content != NULL;
    }

    return 0;
}
