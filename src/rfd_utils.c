#include "bitsarr.h"
#include "rfd_utils.h"
#include "defs.h"

// return the bitmap, provided bitpos is appropiately set to node
int get_mmap()
{
    int res = 0, map;
    gettype(tp);
    switch(tp)
    {
        case ZERO_T:
            getmap(res);
            break;
        case ONE_MT:
            bitpos += (5 + npsize);
            getmap(res);
            break;
        case TWO_MT:
            bitpos += (10 + (npsize<<1));
            getmap(res);
            break;
        case FULL_T:
            bitpos += (26*npsize);
            getmap(res);
            break;
        case OTR_T:
            getmap(map);
            if(map > 0)
            {
                for(int i = 0; i < NCHRS; i++)
                    if(map & (1<<i))
                        bitpos += npsize;
            }
            getmap(res);
    }
    return res;
}


// returns next node pointer for char ch
// provided bitpos is set at appropiate node in which next
// node to be search
// if next level not found then returns -1
int get_nextlevel(char ch)
{
    int res = -1, map;
    unsigned char ch1, ch2;
    
    ch = ch - 'a';
    gettype(tp);
    switch(tp)
    {
        case ONE_T:
        case ONE_MT:
            getchar(ch1);
            
            if(ch1 == ch)
                getnp(res);
            break;

        case TWO_T:
        case TWO_MT:
            getchar(ch1); 
            getchar(ch2);
            if(ch1 == ch)
            {
                getnp(res);
            }
            else if(ch2 == ch)
            {
                bitpos += npsize;
                getnp(res);
            } 
            break;

        case FULL_T:
            bitpos += (npsize * ch);
            getnp(res);
            break;

        case OTR_T:
            getmap(map);
            if(map & (1<<ch))
            {
                for(int i = 0; i < ch; i++)
                    if(map & (1<<i))
                        bitpos += npsize;
                getnp(res);
            }
    }

    return res;
}



