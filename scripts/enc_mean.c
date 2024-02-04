#include <stdio.h>

// take meaing file (argv[1]) and create (len, meaning) map file
// where len is length of meaing (no of char) and it is of 1 byte

unsigned char len(char *line)
{
    unsigned char length = 0;

    while(*(line++) != '\n' || *line != '\0')
        length += 1;
    return length;
}


int main(int argc, char *argv[])
{
    unsigned char mapping[256];
    FILE *fp, *wfp;

    if(argc < 3 || (fp = fopen(argv[1], "rb")) == NULL)
    {
        printf("Provide correct meaning file\n");
        return 1;
    }

    if((wfp = fopen(argv[2], "wb+")) == NULL)
    {
        printf("Unable to create %s\n", argv[2]);
        return 2;
    }

    while(fgets(mapping+1, 255, fp))
    {
        mapping[0] = len(mapping+1);
        fwrite(mapping, mapping[0]+1, 1, wfp);
    }

    return 0;
}
