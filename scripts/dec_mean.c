
// decodes meaning map file (argv[1]) encoded by enc_mean.c file

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
int main(int argc, char *argv[])
{
    char meaning[250];
    unsigned char ch;
    int fd ;
    if(argc == 2 && (fd= open(argv[1], O_RDONLY)) < 0)
    {
        printf("file not found\n");
        return 1;
    }
    
    while(read(fd, &ch, 1) > 0)
    {
        read(fd, meaning, ch);
        meaning[ch] = '\0';
        printf("%s\n", meaning);
    }

    return 0;
}
