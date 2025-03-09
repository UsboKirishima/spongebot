#include <string.h>

#include "utils.h"

char *get_last_line(const char *str)
{
    char *last_newline = strrchr(str, '\n');

    if (last_newline)
    {
        return last_newline + 1;
    }
    else
    {
        return (char *)str;
    }
}
