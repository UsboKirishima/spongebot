#include <shell.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_ARGS 256

struct shell_cmd
{
    char *cmd_str[MAX_ARGS];
    size_t cmd_len;
};

struct shell_cmd current_cmd;

void init_shell_command(void)
{
    current_cmd.cmd_len = 0;
    for (int i = 0; i < MAX_ARGS; i++)
    {
        current_cmd.cmd_str[i] = NULL;
    }
}

void free_shell_command(void)
{
    for (size_t i = 0; i < current_cmd.cmd_len; i++)
    {
        free(current_cmd.cmd_str[i]);
        current_cmd.cmd_str[i] = NULL;
    }
    current_cmd.cmd_len = 0;
}

void exec_shell_command(const char *cmd_str, size_t cmd_len)
{
    free_shell_command();

    size_t token_start = 0;
    size_t token_length = 0;
    int in_token = 0;

    for (size_t i = 0; i < cmd_len; ++i)
    {
        if (cmd_str[i] == ' ' || cmd_str[i] == '\n' || cmd_str[i] == '\t')
        {
            if (in_token && current_cmd.cmd_len < MAX_ARGS)
            {
                char *token = malloc(token_length + 1);
                strncpy(token, cmd_str + token_start, token_length);
                token[token_length] = '\0';
                current_cmd.cmd_str[current_cmd.cmd_len++] = token;
                in_token = 0;
            }
        }
        else
        {
            if (!in_token)
            {
                token_start = i;
                in_token = 1;
            }
            token_length = i - token_start + 1;
        }
    }

    if (in_token && current_cmd.cmd_len < MAX_ARGS)
    {
        char *token = malloc(token_length + 1);
        strncpy(token, cmd_str + token_start, token_length);
        token[token_length] = '\0';
        current_cmd.cmd_str[current_cmd.cmd_len++] = token;
    }
}

void print_shell_command(void)
{
#ifdef DEBUG
    printf("Parsed command (%zu args):\n", current_cmd.cmd_len);
    for (size_t i = 0; i < current_cmd.cmd_len; i++)
    {
        printf("  [%zu] '%s'\n", i, current_cmd.cmd_str[i]);
    }
#elif
    return;
#endif
}

int execute_command(void)
{
    if (current_cmd.cmd_len == 0)
        return -1;

    pid_t pid = fork();
    if (pid == 0)
    {
        char *argv[MAX_ARGS + 1];
        for (size_t i = 0; i < current_cmd.cmd_len; i++)
        {
            argv[i] = current_cmd.cmd_str[i];
        }
        argv[current_cmd.cmd_len] = NULL;

        execvp(argv[0], argv);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        int status;
        waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    }
    else
    {
        perror("fork failed");
        return -1;
    }
}
