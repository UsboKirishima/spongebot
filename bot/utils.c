#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include <utils.h>

char *get_last_line(const char *str)
{
    char *last_newline = strrchr(str, '\n');

    if (last_newline)
    {
        return last_newline + 1;
    }
    
    return (char *)str;
}

char *ip_to_string(uint8_t o1, uint8_t o2, uint8_t o3, uint8_t o4)
{
    char *ip_str = malloc(16); // 15 chars max + null terminator
    if (ip_str == NULL)
    {
        return NULL;
    }

    snprintf(ip_str, 16, "%u.%u.%u.%u", o1, o2, o3, o4);
    return ip_str;
}
