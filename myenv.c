#include <stdio.h>

int main(int argc, char *argv[], char *envp[])
{
    int i = 0;

    // print each environment variable
    while(envp[i] != NULL) {
        printf("%s\n", envp[i]);
        i++;
    }

    printf("Anzahl an Umgebungsvariabeln: %i\n", i);

    return 0;
}
