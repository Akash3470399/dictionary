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


char *mean_len = "../data/len_meaning";
char *word_mp = "../data/mpfile"; 

int main(int argc, char *argv[])
{
    long curbyte = 1, wordno = 0;
    unsigned char mapping[256];
    FILE *wordfp, *len_meanfp, *mpfp;

    if(argc < 2 || (wordfp = fopen(argv[1], "rb")) == NULL)
    {
        printf("Provide correct meaning file\n");
        return 1;
    }

    if((len_meanfp= fopen(mean_len, "wb+")) == NULL)
    {
        printf("Unable to create %s\n", mean_len);
        return 2;
    }

    if((mpfp = fopen(word_mp, "wb+")) == NULL)
    {
        printf("unable to create %s\n", word_mp);
        return 3;
    }
    
    fwrite(mapping, 1, 1, len_meanfp);
    while(fgets(mapping+1, 255, wordfp))
    {
        fprintf(mpfp, "%ld\n", curbyte);
        mapping[0] = len(mapping+1);
        curbyte += mapping[0] + 1;
        fwrite(mapping, mapping[0]+1, 1, len_meanfp);

    }
    
    fclose(mpfp);
    fclose(len_meanfp);
    fclose(wordfp);

    return 0;
}
