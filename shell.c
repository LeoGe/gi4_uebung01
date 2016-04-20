#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

//#define DEBUG

int occurence_of(char *content, char c)
{
    // Count occurence of c
    int num = 0;
    char* tmp = content;
    for(; tmp[num]; tmp[num] == c ? num++:*tmp++);
    return num;
}

// split content with delimeter c and returns the seperated elements
char **split_at(char *content, char *c, int *num)
{
    int num_delim = occurence_of(content, c[0]);

    // allocate a array to strings for the elements
    char **arr = malloc((num_delim+1)*sizeof(char*));

    // strsep returns the next found element
    char *token;
    int a = 0;
    while ((token = strsep(&content, c)) != NULL) {
        // content + '\0'
        int token_str = strlen(token)+1;
       
        // allocate, set to \0 and copy slice to it 
        arr[a] = malloc(token_str*sizeof(char));
        memset(arr[a], '\0', token_str);
        strncpy(arr[a], token, token_str-1);

        a++;
    }

    *num = a;

    return arr;
}

// This pseudo-lexer splits every 
char **lexer(char *content, int *num_content)
{
    int num = 0;
    int got_anything = 0;
    int i;
    for(i = 0; i < strlen(content); i++) {
        if(isalpha(content[i]) || isdigit(content[i])) {
            got_anything = 1;
        } else {
            if(got_anything)
                num += 2;
            else
                num += 1;

            got_anything = 0;
        }
    }

    if(got_anything)
        num += 1;

#ifdef DEBUG
    printf("\tcreate %i elements\n", num);
#endif

    // position in content_arr
    int a = 0;
    char **content_arr = (char **) malloc(num*sizeof(char*));

    i = 0;
    int range = 0;
    got_anything = 0;
    while(content[i] != '\0') {
        if(!isalpha(content[i]) && !isdigit(content[i])) {
            if(got_anything == 1) {
                // copy content to array

                content_arr[a] = (char*) malloc((range+1)*sizeof(char));
                memset(content_arr[a], '\0', range+1);
                strncpy(content_arr[a], content + (i-range), range);
#ifdef DEBUG
                printf("\t\t%i\t%i\t%s\n", a, range, content_arr[a]);
#endif
                a ++;
            }

            content_arr[a] = (char*) malloc(2);
            content_arr[a][0] = content[i];
            content_arr[a][1] = '\0';
#ifdef DEBUG
            printf("\t\t%i\t%s\n", a, content_arr[a]);
#endif
            a++;
            range = 0;
            got_anything = 0;
        }
        else {
            got_anything = 1;

            range++;
        }

        i++;
    }

    // copy the last element
    if(range > 0) {
        content_arr[a] = (char*) malloc(range+1);
        memset(content_arr[a], '\0', range+1);
        strncpy(content_arr[a], content + (i-range), range);
#ifdef DEBUG
        printf("\t\t%i\t%i\t%s\n", a, range, content_arr[a]);
#endif
        a++; 
    }

    // num is now the length of content_arr
    *num_content = a;

    return content_arr;
}

char **replace_vars(char **child_argv, int *num) {
    int i = 0;
    int a = 0;

    // replace all descendant of $    
    for(; i < *num; i++) {
        if(child_argv[i][0] == '$' && i+1 < *num) {
            char *content;
            if(strcmp(child_argv[i+1], "?") == 0) {
                content = malloc(2);
                content[1] = '\0';
                sprintf(content, "%i", errno);
            } else
                content = getenv(child_argv[i+1]);

            size_t len_content = 1; // "\0"
            if(content != NULL)
                len_content += strlen(content);

            child_argv[i+1] = (char*) realloc(child_argv[i+1], len_content);
            memset(child_argv[i+1], '\0', len_content);

            if(len_content-1 > 0)
                strncpy(child_argv[i+1], content, len_content-1);

            i++;
            a++;
        }
    }

    // remove all $
    char **new_token_arr = (char**)malloc((*num-a)*sizeof(char*));
    for(i=0, a=0; i < *num; i++) {
        if(child_argv[i][0] != '$') {
            int len_token = strlen(child_argv[i])+1;
            
            new_token_arr[a] = malloc(len_token*sizeof(char));
            
            memset(new_token_arr[a], '\0', len_token);
            strncpy(new_token_arr[a], child_argv[i], len_token-1);

            a++;
        }
            
        // delete the old string
        //free(child_argv[i]);
    }

    *num = a;

    //free(child_argv);
    
    return new_token_arr;
}

char **create_merged_arr(char **content, int num, char c, int *num_out)
{
    if(num == 0)
        return NULL;

    int got_anything = 0;
    int found = 1;
    int i;
    for(i = 0; i < num; i++) {
        if(content[i][0] == c) {
            if(got_anything == 1) {
                found ++;
                got_anything = 0;
            }
        }
        else if(content[i][0] != '$')
            got_anything = 1;
    }

#ifdef DEBUG
    printf("create merged array with %i elements\n", found);
#endif

    char **merged_arr = (char **)malloc(found*sizeof(char*));

    // create char arrays
    int a = 0;
    int len = 0;
    for(i = 0; i < num; i++) {
#ifdef DEBUG
        printf("%i, %i\n", i, num);
#endif

        if(content[i][0] == c || i+1 == num) {
            if(i+1 == num)
                len += strlen(content[i]);

#ifdef DEBUG
            printf("\tfound word with %i elements\n", len);
#endif

            merged_arr[a] = (char*) malloc(len+1);
            memset(merged_arr[a], '\0', len+1);

            len = 0;
            a++;
        } else if(content[i][0] != '$')
            len += strlen(content[i]);
    }

    // copy strings into the array of strings
    a = 0;
    len = 0;
    for(i = 0; i < num; i++) {
        if(content[i][0] == c) {
            a++;
            len = 0;
        } else if(content[i][0] != '$') {
            int str_len = strlen(content[i]);
#ifdef DEBUG
            printf("\tcopy word %i, %s\n", len, content[i]);
#endif
            strncpy(merged_arr[a] + len, content[i], str_len);
            len += str_len;
        }
    }
    *num_out = found;

    return merged_arr;

}

char* concat(char *s1, char *s2, char c)
{
    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);
    char *result = malloc(len1+1+len2+1);//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    memcpy(result, s1, len1);
    *(result+len1) = c;
    memcpy(result+len1+1, s2, len2+1);//+1 to copy the null-terminator
    return result;
}

// TODO
void delete_arr(char **arr, int num) {
    for(int i = 0; i < num; i++)
        free(arr[i]);

    free(arr);
}

void print(char **str, int num)
{
    for(int i = 0; i < num; i++) {
        printf("%i: %s\n", i, str[i]);
    }
}

int main(int argc, const char *argv[], char *envp[])
{
    while(1) {
        char *pwd = getenv("PWD");
        if(pwd == NULL)
            return -1;

        // construct our prefix to the command line, containing the current working directory and a delimiter
        char *console = concat(strdup(pwd), "$ ", ' ');
        
        char *content = readline(console);    

        // if there is no input, continue
        if(strcmp("", content) == 0)
            continue;

        // magic history command
        add_history(content);

#ifdef DEBUG
        printf("Lexer:\n");
#endif

        int token_num = 0;
        char **token_arr_tmp = lexer(content, &token_num);
        // search and replace all $<var>
        char **token_arr=replace_vars(token_arr_tmp, &token_num);

#ifdef DEBUG
        printf("Merge:\n");
#endif

        int merged_arr_num = 0;
        char **child_argv = create_merged_arr(token_arr, token_num, ' ', &merged_arr_num);

#ifdef DEBUG
        print(child_argv, merged_arr_num);
#endif
        // check if file exists
        if(child_argv[0][0] == '/' || child_argv[0][0] == '.') {
            if(access(child_argv[0], X_OK) == -1)
                printf("Error: File %s does not exist!\n", child_argv[0]);
        } else {
            // loop through each PATH env and check if the binary exists
            char *path = getenv("PATH");
            if(path == NULL)
                continue;
            else
                // this doesn't leak because the string from getenv shall never be destroid
                path = strdup(path);

            int tmp;
            char **path_arr = split_at(path, ":", &tmp);

            int i, found = 0;
            for(i = 0; i < tmp; i++) {
                char *search_path = concat(path_arr[i], child_argv[0], '/');
                
                if(access(search_path, X_OK) != -1) {
                    child_argv[0] = strdup(search_path);
                    found = 1;
                    break;
                }                  

                free(search_path);

                i++;
            }    

            // TODO
            //delete_arr(path_arr, tmp);

            // no binary was found, check if the command can be parsed
            if(found == 0) {
                // set command
                if(token_num > 1 && strcmp(token_arr[1], "=") == 0) {
                    // e. "test="
                    if(token_num == 2)
                        setenv(token_arr[0], "", 1);
                    // e. "test=test"
                    if(token_num == 3)
                        setenv(token_arr[0], token_arr[2], 1);
                    // e. "test=test1:test2
                    else if(token_num > 3) {
                        int tmp;
                        char **content = create_merged_arr(token_arr+2, token_num-2, ' ', &tmp);
                        setenv(token_arr[0], strdup(content[0]), 1);
                        //ke
                        delete_arr(content, tmp);
                    }
                // cd command
                } else if(strcmp(token_arr[0], "cd") == 0) {
                    int i;
                    if(token_num == 1) {
                        char *home = getenv("HOME");

                        if(home == NULL)
                            i = chdir("/");
                        else
                            i = chdir(getenv("HOME"));
                    }
                    else if(token_num == 3)
                        i = chdir(token_arr[2]);
                    else {
                        int tmp;
                        char **content = create_merged_arr(token_arr+2, token_num-2, ' ', &tmp);
                        i = chdir(content[0]);

                        delete_arr(content, tmp);
                    }


                    if(i != 0)
                        printf("Error: cd has failed: %s!\n", strerror(errno));
                    else {
                        // update PWD to path 
                        char path[512];
                        realpath(".", path);
                        setenv("PWD", path, 1);
                    }

                }
                else
                    printf("Error: Command %s does not exist!\n", child_argv[0]); 

                delete_arr(child_argv, merged_arr_num);
                delete_arr(token_arr, token_num);
                // jump to the head of the loop, don't execute the invalid command
                continue;
            }
            
        }    
        // now execute the command
        int status;
        int pid;

        if(pid = fork() == 0) {
            execve(child_argv[0], &child_argv[0], envp);
        }
        else {
            while(wait(&status)  > 0); // wait for the child to return

            free(console);
            delete_arr(child_argv, merged_arr_num);
            delete_arr(token_arr, token_num);
        }
    }

    return 0;
}
