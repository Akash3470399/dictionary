#include "consts.h"
#include "bitsarr.h"

#ifndef TYPES_H
#define TYPES_H

typedef unsigned char uchar;
typedef unsigned int uint;

typedef struct
{
    uint mpstart;
    uint bitmap;
}mpentry;

typedef struct 
{
    uint ptrno;
    mpentry meaning;
}zero_t; 

typedef struct 
{
    uint ptrno;
    uint nnp;
    char ch;
    mpentry meaning;
}one_t;

typedef struct
{
    uint ptrno;
    uint nnp1, nnp2;
    char ch1, ch2;
    mpentry meaning;
}two_t;

typedef struct
{
    uint ptrno;
    uint nnp[NCHRS];
    mpentry meaning;
}full_t;

typedef struct
{
    uint ptrno;
    uint npstart;
    uint npbitmap;
    mpentry meaning;
}otr_t;

void mpentry_tobytes(mpentry *meaning, uchar *bytearr, uint sbit);
void bytes_tompentry(mpentry *meaning, uchar *bytearr, uint sbit);

void zero_tobytes(zero_t *node, uchar *bytearr, uint sbit);
void bytes_tozero_t(zero_t *node, uchar *bytearr, uint sbit);

void one_tobytes(one_t *node, uchar *bytearr, uint sbit);
void bytes_toone_t(one_t *node, uchar *bytearr, uint sbit);

void two_tobytes(two_t *node, uchar *bytearr, uint sbit);
void bytes_totwo_t(two_t *node, uchar *bytearr, uint sbit);

void full_tobytes(full_t *node, uchar *bytearr, uint sbit);
void bytes_tofull_t(full_t *node, uchar *bytearr, uint sbit);

void otr_tobytes(otr_t *node, uchar *bytearr, uint sbit);
void bytes_tootr_t(otr_t *node, uchar *bytearr, uint sbit);

#endif
