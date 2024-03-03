#include "defs.h"

#ifndef RFD_UTILS_H
#define RFD_UTILS_H

typedef struct refdata_info refdata_info;

struct refdata_info
{
    long bitpos, rootsbit, totalwords;
    long npsize, mpsize;
    uchar *refdata;
};

#endif
