#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <time.h>

void ls(char *dir_path, int depth) {
    DIR *dir;
    struct dirent *item;
    struct stat stat_buf;
    if ((dir = opendir(dir_path)) == NULL) {
        perror("Open dir failed!\n");
        return;
    }
    chdir(dir_path);
    while ((item = readdir(dir)) != NULL) {
        lstat(item->d_name, &stat_buf);
        if (S_ISDIR(stat_buf.st_mode)) {
            if (item->d_name[0] == '.') {
                continue;
            }
            for (int i = 0; i < depth; ++i) {
                printf("    ");
            }
            printf("dir_name: %-10s\n", item->d_name);
            ls(item->d_name, depth + 1);
        } else {
            for (int i = 0; i < depth; ++i) {
                printf("    ");
            }
            printf("filename: %-20s ", item->d_name);
            printf("size: %8ld  %s", (long)stat_buf.st_size, asctime(localtime(&stat_buf.st_mtime
                )));
        }
    }
    closedir(dir);
    chdir("..");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("dir_name needed!\n");
        return 0;
    }
    ls(argv[1], 0);
    return 0;
}