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
    long curbyte = 1, wordno = 0;
    unsigned char mapping[256];
    FILE *meaning_fp, *len_meanfp;

    if(argc < 3 || (meaning_fp = fopen(argv[1], "rb")) == NULL)
    {
        printf("Provide correct meaning file\n");
        return 1;
    }

    if((len_meanfp= fopen(argv[2], "wb+")) == NULL)
    {
        printf("Unable to create %s\n", argv[2]);
        return 2;
    }

    fwrite(mapping, 1, 1, len_meanfp);
    while(fgets((char*)mapping+1, 255, meaning_fp))
    {
        // prints mp for word 
        printf("%ld\n", curbyte);
        mapping[0] = len((char*)(mapping+1));
        curbyte += mapping[0] + 1;
        fwrite(mapping, mapping[0]+1, 1, len_meanfp);
    }
    
    fclose(len_meanfp);
    fclose(meaning_fp);

    return 0;
}
