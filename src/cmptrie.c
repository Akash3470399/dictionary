#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "rfd_utils.h"
#include "bitsarr.h"
#include "./debug.c"

#define LEN_MEANING_FILE "data/len_meaning"

long npfor(refdata_info *rfd, long parentnp, char nextchar);
long get_mmap(refdata_info *rfd, long np, int *map);


// initilize the refdata_info object with data read from rfd_file reference
int init_refdata_info(char *rfd_file, refdata_info **rfd)
{
    int res = 0;
    long memsize, *value;
    FILE *rfdfp;
    refdata_info r;

    if(((rfdfp = fopen(rfd_file, "rb+")) != NULL) && rfd != NULL)
    {
        fseek(rfdfp, 0, SEEK_END);
        memsize = ftell(rfdfp);
        fseek(rfdfp, 0, SEEK_SET);
    
        r.refdata = malloc(memsize);

        fread(r.refdata, 1, memsize, rfdfp);

        // pointer to metadata 
        value = (long*)&(r.refdata[memsize - (4*sizeof(long)) -1]);

        // extracting metadata
        r.totalwords = *value++;
        r.rootsbit = *value++;
        r.npsize = *value++;
        r.mpsize = *value;

        *rfd = (refdata_info*)malloc(sizeof(refdata_info));
        **rfd = r;
        fclose(rfdfp);

        res = 1;
    }
    return res;
}

// frees refdata_info object
int free_refdata_info(refdata_info *rfd)
{
    if(rfd)
    {
        free(rfd->refdata);
        free(rfd);
    }
}


long is_word_present(refdata_info *rfd, char *word)
{

    long np = rfd->rootsbit, mp = 0;
    int i = 0,  map = 0;
    while((word[i+1] != '\0') && (np != -1))
        np = npfor(rfd, np, word[i++]);

    if(np != -1)
    {
        char ch = word[i] - 'a'; 
        np = get_mmap(rfd, np, &map);
        mp = ((map & (1<<ch))>>ch);
        for(int i = 0; (i < ch) && mp; i++)
            if(map & (1<<i))       
                np += rfd->mpsize;
        if(mp)
        {
            mp *= 0;
            bitscopy(rfd->refdata, np, &mp, 0, rfd->mpsize);
        }
    }
    return mp;
}

int get_meaning(refdata_info *rfd, char *word, char *meaning)
{
    uchar len ;
    FILE *fp;
    long mp = is_word_present(rfd, word);
    
    if(mp && ((fp= fopen(LEN_MEANING_FILE, "rb") ) != NULL))
    {
        fseek(fp, mp, SEEK_SET);
        fread(&len, 1, 1, fp);
        fread(meaning, 1, len, fp);
        meaning[len] = '\0';
        fclose(fp);
    }
    else
        mp = 0; 
    return (mp != 0);
}

// **** internal methods use to traverse & search for word in rfd ***********

// get next pointer for char nextchar at current node given by parentnp
// if next node found for nextchar then returns pointer (bit level) for that node
// else return -1
long npfor(refdata_info *rfd, long parentnp, char nextchar)
{
    char ch1, ch2;
    enum Type tp = 0;
    long curpos = parentnp, nextnp = -1;

    nextchar -= 'a';
    bitscopy(rfd->refdata, curpos, &tp, 0, 3);
    curpos += 3;

    switch(tp)
    {
        case ONE_T:
        case ONE_MT:
            ch1 = 0;
            bitscopy(rfd->refdata, curpos, &ch1, 0, 5);
            curpos += 5;

            if(ch1 == nextchar)
            {
                nextnp *= 0;
                bitscopy(rfd->refdata, curpos, &nextnp, 0, rfd->npsize);
            }
            break;

        case TWO_T:
        case TWO_MT:
            ch1 = 0;
            bitscopy(rfd->refdata, curpos, &ch1, 0, 5);
            curpos += 5;
            ch2 = 0;
            bitscopy(rfd->refdata, curpos, &ch2, 0, 5);
            curpos += 5;

            if(ch1 == nextchar)
            {
                nextnp *= 0;
                bitscopy(rfd->refdata, curpos, &nextnp, 0, rfd->npsize);
                curpos += rfd->npsize;
            }
            else if(ch2 == nextchar)
            {
                nextnp *= 0;
                curpos += rfd->npsize;
                bitscopy(rfd->refdata, curpos, &nextnp, 0, rfd->npsize);
            }
            break;

        case FULL_T:
            curpos += (rfd->npsize *  nextchar);
            nextnp = 0;
            
            bitscopy(rfd->refdata, curpos, &nextnp, 0, rfd->npsize);
            break;

        case OTR_T:
        int npmap = 0;
        bitscopy(rfd->refdata, curpos, &npmap, 0, NCHRS);
        curpos += NCHRS;

        if(npmap & (1<<nextchar))
        {
            for(int i = 0; i < nextchar; i++)
                if(npmap & (1<<i))
                    curpos += rfd->npsize;

            nextnp *= 0;
            bitscopy(rfd->refdata, curpos, &nextnp, 0, rfd->npsize);
        }
    }

    return nextnp;
}


// retrives meaning bitmap to map from node np
long get_mmap(refdata_info *rfd, long np, int *map)
{
    enum Type tp = 0;
    bitscopy(rfd->refdata, np, &tp, 0, 3);
    *map = 0; 
    np += 3;

    switch(tp)
    {
        case ZERO_T:
            bitscopy(rfd->refdata, np, map, 0, NCHRS);        
            break;
        case ONE_MT:
            np += (5 + rfd->npsize);
            bitscopy(rfd->refdata, np, map, 0, NCHRS);        
            break;
        case TWO_MT:
            np += (5 + rfd->npsize) * 2;
            bitscopy(rfd->refdata, np, map, 0, NCHRS);        
            break;
        case FULL_T:
            np += (NCHRS*(rfd->npsize));
            bitscopy(rfd->refdata, np, map, 0, NCHRS);        
            break;
        case OTR_T:
            bitscopy(rfd->refdata, np, map, 0, NCHRS);      
            np += NCHRS;
            if(*map > 0)
            {
                for(int i = 0; i < NCHRS; i++)
                    if(*map & (1<<i))
                        np += rfd->npsize;
            }
            *map *= 0;
            bitscopy(rfd->refdata, np, map, 0, NCHRS);        
    }
    return (np + NCHRS);
}


long is_word_present(refdata_info *rfd, char *word)
{

    long np = rfd->rootsbit, mp = 0;
    int i = 0,  map = 0;
    while((word[i+1] != '\0') && (np != -1))
        np = npfor(rfd, np, word[i++]);

    if(np != -1)
    {
        char ch = word[i] - 'a'; 
        np = get_mmap(rfd, np, &map);
        mp = ((map & (1<<ch))>>ch);
        for(int i = 0; (i < ch) && mp; i++)
            if(map & (1<<i))       
                np += rfd->mpsize;
        if(mp)
        {
            mp *= 0;
            bitscopy(rfd->refdata, np, &mp, 0, rfd->mpsize);
        }
    }
    return mp;
}
