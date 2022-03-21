#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

char *file_name;

int
find_file(char *path){
    char buf[512], *p;
    int fd;
    struct stat st;
    struct dirent de;
    if ((fd = open(path, O_RDONLY)) < 0)
    {
        fprintf(2, "open file error!\n");
        return -1;
    }
    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "open file error!\n");
        return -1;
    }
    if (st.type != T_DIR){
        fprintf(2, "Usage: find <path> <fileName>\n");
        return -1;
    }      
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
    {
        fprintf(2, "find: path too long\n");
        return -1;
    }
    
    strcpy(buf, path);  // 复制路径到buf字符数组
    p = buf + strlen(buf);  // 将P指针指向buf路径的尾部
    *p++ = '/';  // 在尾部填充/字符 并将p指针移动到字符/之后
    while(read(fd, &de, sizeof(de)) == sizeof de){
        if(de.inum == 0)
            continue;
        memmove(p, de.name, DIRSIZ);    // 将文件名复制到p指针
        p[DIRSIZ] = 0;  // 末尾设置0为字符数组结束符
        if (stat(buf, &st) < 0) {   // 将该文件信息放入st结构体中 
            fprintf(2, "find: cannot stat %s\n", buf);
            continue;
        }
        if (st.type == T_DIR && strcmp(p, ".") && strcmp(p, "..") != 0) // 不在 . 和 .. 中递归
        {
            find_file(buf);
        } else if (strcmp(file_name, p) == 0)
        {
            printf("%s\n", buf);
        }
    }
    close(fd);
    return 0;
}

int
main(int argc, char *argv[]){
    
    if (argc != 3)
    {
        printf("Usage: find <path> <fileName>\n");
        exit(1);
    }

    char *path = argv[1];
    file_name = argv[2];
    if (find_file(path) < 0)
        exit(1);    
    
    exit(0);
    
}