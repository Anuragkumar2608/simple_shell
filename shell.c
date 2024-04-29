#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<unistd.h>
#include<readline/readline.h>
#include<signal.h>
#include<setjmp.h>
#include<sys/wait.h>

void signal_handler(int);

static sigjmp_buf env;
static volatile sig_atomic_t jump_active = 0;

char** format_input(char* input){
    char **command = (char **)malloc(16 * sizeof(char *));
    char *separator = " ";
    char *parsed;
    int curr = 0;

    char* tok = strtok(input,separator);
    while(tok != NULL){
        command[curr] = tok;
        ++curr;
        tok = strtok(NULL,separator);
    }
    command[curr] = NULL;
    return command;
}

int cd(char* path){
    return chdir(path);
}

int main(){
    char **command;
    char *input;
    pid_t child_pid;
    int stat_loc;

    struct sigaction sig;
    sig.sa_handler = signal_handler;
    sigemptyset(&sig.sa_mask);
    sig.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sig, NULL);

    while(1){
        if(sigsetjmp(env,1) == 1){
            printf("\n");
            continue;
        }
        jump_active = 1;

        input = readline("simple-shell>> ");
        command = format_input(input);

        if(command[0] == NULL){
            free(input);
            free(command);
            continue;
        }

        if(strcmp(command[0],"cd")==0){
            if(cd(command[1])<0){
                perror(command[1]);
            }
            continue;
        }

        child_pid = fork();
        if(child_pid < 0){
            perror("Fork Failed");
            exit(1);
        }

        if(child_pid == 0){
            struct sigaction sig_child;
            sig_child.sa_handler = signal_handler;
            sigemptyset(&sig_child.sa_mask);
            sig_child.sa_flags = SA_RESTART;
            sigaction(SIGINT, &sig_child, NULL);

            if(execvp(command[0],command) < 0){
                perror(command[0]);
                exit(1);
            }
        }else{
            waitpid(child_pid, &stat_loc, WUNTRACED);
        }
        if(input != NULL)
            free(input);
        if(command != NULL)
            free(command);
    }
    return 0;
}

void signal_handler(int signo){
    if(!jump_active){
        return;
    }
    siglongjmp(env,1);
}