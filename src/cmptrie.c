#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "rfd_utils.h"
#include "bitsarr.h"

#undef _mpsize
#undef _npsize
#define _mpsize rfd_metadata->mpsize
#define _npsize rfd_metadata->npsize

rfdmeta *rfd_metadata = NULL;
char *rfd_file = "data/rfd";
uchar *refdata;

enum Type tp;
int bitpos;

int init_rfd()
{
    int res = 0, buf;
    FILE *rfdfp;

    if((rfdfp = fopen(rfd_file, "rb+")) != NULL)
    {
        fseek(rfdfp, 0, SEEK_END);
        buf = ftell(rfdfp);
        fseek(rfdfp, 0, SEEK_SET);
    
        refdata = malloc(buf);

        fread(refdata, 1, buf, rfdfp);
        
        rfd_metadata = (rfdmeta *)&refdata[buf-sizeof(rfdmeta)-1];
        fclose(rfdfp);

        res = 1;
    }
    return res;
}

int is_word_present(char *word)
{
    int res = 0, i = 0, meaning;
    
    bitpos = rfd_metadata->rootsbit;
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