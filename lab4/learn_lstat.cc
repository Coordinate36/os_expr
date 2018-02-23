#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <time.h>

void ls(char *dir, int depth)
{
    DIR *dp;
    struct dirent *item;
    struct stat statbuf;
    if (((dp = opendir(dir))) == NULL)
    {
        printf("Open Dir Failed!\n");
        return ;
    }
    chdir(dir);
    while( (item = readdir(dp)) != NULL)
    {
        lstat(item->d_name, &statbuf);
        if(S_ISDIR(statbuf.st_mode))      //判断是文件还是文件夹
        {
            if(strcmp(item->d_name, "..") == 0 || strcmp(item->d_name, ".") == 0)
                continue;   //跳过 这层 和 上一层文件夹
            for(int i = 0; i < depth; i++)
                printf("    "); //根据文件所处深度打印出缩进效果
            printf("目录名:%-10s\n", item->d_name);
            ls(item->d_name, depth + 1);      //是文件夹就递归打印里面的文件
        }
        else
        {
            for(int i = 0; i < depth; i++)
                printf("    ");
            printf("文件名:%-10s ", item->d_name);
            printf("大小: %5ld  %20s", (long)statbuf.st_size, asctime(localtime(&statbuf.st_mtime)));
        }
    }
    chdir("..");
}
int main(void)
{
    char dir_name[50];
    printf(">>Input DIR name:\n>>");
    scanf("%s", dir_name);
    ls(dir_name, 0);
    return 0;
}