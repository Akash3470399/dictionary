#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "rfd_utils.h"
#include "bitsarr.h"

char *rfd_file = "data/rfd";

uchar *refdata;
enum Type tp;
int bitpos, rootbit, npsize, mpsize, totalwords;

int init_rfd()
{
    int res = 0, memsize, *num;
    FILE *rfdfp;

    if((rfdfp = fopen(rfd_file, "rb+")) != NULL)
    {
        fseek(rfdfp, 0, SEEK_END);
        memsize = ftell(rfdfp);
        fseek(rfdfp, 0, SEEK_SET);
    
        refdata = malloc(memsize);

        fread(refdata, 1, memsize, rfdfp);

        // pointer to metadata 
        num = (int*)&refdata[memsize - (4*sizeof(int)) -1];

        // extracting metadata
        totalwords = *num++;
        rootbit = *num++;
        npsize = *num++;
        mpsize = *num;

        fclose(rfdfp);

        res = 1;
    }
    return res;
}

int is_word_present(char *word)
{
    int res = 0, i = 0, meaning;
    
    bitpos = rootbit;
    while((word[i+1] != '\0' && word[i+1] != '\n') && (bitpos != -1))
    {
        bitpos = get_nextlevel(word[i++]);
    }

    if(bitpos != -1)
    {
        meaning = get_mmap();
        res = ((meaning & 1<<(word[i]-'a')) >> (word[i])-'a');
    }

    return !(res == 0);
}
