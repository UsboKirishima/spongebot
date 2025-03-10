#include <stdio.h>
#include <dirent.h>

int main() {
    DIR* dir = opendir("/proc");
    if (dir == NULL) {
        perror("opendir");
        return 1;
    }

    struct dirent* entry;
    readdir(dir);
    closedir(dir);
    return 0;
}
