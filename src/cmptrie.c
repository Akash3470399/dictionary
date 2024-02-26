#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "rfd_utils.h"
#include "bitsarr.h"

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

int free_refdata_info(refdata_info *rfd)
{
    if(rfd)
    {
        free(rfd->refdata);
        free(rfd);
    }
}
