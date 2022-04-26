//Adi Aviv 206962904
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include<sys/wait.h>


/*
 * add the pid to array of command in the correct place
 */

void add_pid(char *s , pid_t pid) {
    char p[100];
    char pid_char[100];
    memset(p, 0, sizeof(p));
    memset(pid_char, 0, sizeof(pid_char));
    //add comma after pid to distinct between the command and his pid
    pid_char[0] = ',';
    sprintf(p, "%d" , (int)pid);
    strcat(pid_char, p);
    strcat(s, pid_char);
}

/*
 * active according cd or history command
 */


void systemCommand(int i, char *command, char commands[100][101]) {
    if (strcmp(command, "history") == 0) {
        add_pid(commands[i] , getpid());
        int k;
        for (k = 0 ; commands[k][0] != '\0' ; ++k) {
            char c[100];
            strcpy(c,commands[k]);
            char *command_name = strtok(c, ",");
            char *pid_number = strtok(NULL, ",");
            printf("%s " , pid_number);
            printf("%s\n" , command_name);
        }
        ++i;
    }
    else {
        add_pid(commands[i] , getpid());
        char *token = strtok(NULL," ");
        if (chdir(token) != 0) {
            printf("%s failed\n" , command);
        }
        ++i;
    }
}


int main(int argc, char *argv[]) {
    int i = 1;
    //add the relevant dirs to path environment
    while (i < argc) {
        setenv("PATH",strcat(strcat(getenv("PATH"),":"), argv[i]),1);
        ++i;
    }
    i = 0;
    char commands[100][101];
    //run at most for 100 commands
    while (i < 100) {
        char input[100] , *parameters[100], *token;
        printf("$ ");
        fflush(stdout);
        fflush(stdin);
        scanf("%[^\n]%*c" , input);
        //save the command
        strcpy(commands[i] , input);
        token = strtok(input," ");
        char *command = token;
        parameters[0] = command;
        //check if the command is history or if the command is cd
        if (strcmp(command, "history") == 0 || strcmp(command, "cd") == 0) {
            systemCommand(i, command, commands);
            ++i;
            continue;

        }
        //check if the command is exit
        if (strcmp(command, "exit") == 0) {
            return 0;
        }
        token = strtok(NULL," ");
        int j = 1;
        //else save the parameters for execvp
        while (token != NULL) {
            parameters[j] = token;
            token = strtok(NULL," ");
            ++j;
        }
        parameters[j] = NULL;
        //create a new process and run the command with execvp
        pid_t pid;
        pid = fork();
        int process;
        if (pid < 0) {
            perror("fork");
            exit(1);
        }
        else if (pid == 0) {
            process = execvp(command,parameters);
            if ( process == -1) {
                perror("execvp failed");
                exit(1);
            }
        }
        else {
            if (waitpid(pid, NULL, 0) == -1) {
                perror("wait failed");
            }
            add_pid(commands[i], pid);
        }
        ++i;
    }
    return 0;
}
