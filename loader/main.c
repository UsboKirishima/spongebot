#include <stdio.h>
#include <dirent.h>

int main() {
    DIR* dir = opendir("/proc");
    if (dir == NULL) {
        perror("opendir");
        return 1;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        printf("Found: %s\n", entry->d_name);
    }

    closedir(dir);
    return 0;
}
