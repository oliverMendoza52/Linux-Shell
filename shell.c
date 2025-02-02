#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

// Function prototypes
void get_user_command(char *command);
void parse_command(char *command, char **arguments);
void execute_command(char** arguments);
void user_prompt_loop();
void exit_command();
void proc_command(char **arguments);
void history_command();

// Helper function prototypes
void write_to_file(char *command);

int main()
{
    // Call user_prompt_loop()
    user_prompt_loop();

    return 0;
}

void get_user_command(char *command)
{
    // Get command from the user
    fgets(command, 1000, stdin);

    write_to_file(command);
}

void parse_command(char *command, char **arguments)
{
    int index = 0;
    char *token;

    // Get the first token of the command
    token = strtok(command, " \n");

    // Traverse the command
    while(token != NULL)
    {
        arguments[index] = token;

        token = strtok(NULL, " \n");

        index++;
    }

    arguments[index] = NULL;
}

void execute_command(char** arguments)
{
    pid_t pid;

    // Create a child process
    pid = fork();

    if(pid < 0)
    {
        perror("Fork Failed");
    }
    else if(pid == 0)
    {
        // Child process executes the command
        if(execvp(arguments[0], arguments) == -1)
        {
            perror("Execution Failed");
        }

        exit(1);
    }
    else
    {
        // Parent process waits for the child process to complete
        wait(NULL);
    }
}

void user_prompt_loop()
{
    const int SIZE = 1000;
    char command[SIZE];
    char *arguments[SIZE];

    while(1)
    {
        // Prompt the user to enter a command
        printf(">> ");

        // Get command from the user
        get_user_command(command);

        // Parse the command
        parse_command(command, arguments);

        // exit command
        if(strcmp(arguments[0], "exit") == 0)
        {
            exit_command();
        }

        // proc command
        if(strcmp(arguments[0], "/proc") == 0)
        {
            proc_command(arguments);
        }

        // history command
        if (strcmp(arguments[0], "history") == 0)
        {
            history_command();
        }

        // If the command is either exit or /proc, don't call execute_command()
        if (strcmp(arguments[0], "exit") == 0 || strcmp(arguments[0], "/proc") == 0 || strcmp(arguments[0], "history") == 0)
        {
            continue;
        }

        // Execute the command
        execute_command(arguments);
    }
}

void exit_command()
{
    exit(0);
}

void proc_command(char **arguments)
{
    const int SIZE = 1000;
    char concatenatedCommand[SIZE];
    char *concatenatedArguments[SIZE];

    // Concatenate command. For example, command: /proc /cpuinfo concatenated command: /proc/cpuinfo
    strcpy(concatenatedCommand, arguments[0]);

    strcat(concatenatedCommand, arguments[1]);

    // Create a new arguments array in which the first element is going to be the cat command
    concatenatedArguments[0] = "cat";

    concatenatedArguments[1] = concatenatedCommand;

    // Execute the command
    execute_command(concatenatedArguments);
}

void write_to_file(char *command)
{
    FILE *fptr;

    fptr = fopen(".commands.txt", "a");

    if (strcmp(command, "history\n") != 0)
    {
        fprintf(fptr, "%s", command);
    }

    fclose(fptr);
}

void history_command()
{
    FILE *fptr;

    fptr = fopen(".commands.txt", "r");

    char commands[1000];

    while (fgets(commands, 1000, fptr))
    {
        printf("%s", commands);
    }

    fclose(fptr);
}