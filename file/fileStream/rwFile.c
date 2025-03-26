#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int main(int argc,char* argv[])
{
    // ./myprogram file
    if(argc != 2){
        printf("Please input a file name!\n");
        exit(0);
    }
    char buf[4096];
    FILE * fd = fopen(argv[1],"r+");

    if(fd == NULL){
        perror("Failed to open file");
        return 1;
    }

    //清空buf，防止有脏数据
    memset(buf,0,sizeof(buf));
    fread(buf,sizeof(char),4096,fd);
    printf("buf = %s\n",buf);
    
    printf("Please input some characters to write in file: ");
    memset(buf,0,sizeof(buf));
    fgets(buf,sizeof(buf),stdin);
    //此时会从上次读取文件后的位置进行写入
    fwrite(buf,sizeof(char),strlen(buf),fd);
    //关闭文件
    fclose(fd);
    return 0;
}


