#ifndef BITSARR_H
#define BITSARR_H

typedef unsigned char uchar;
typedef unsigned int uint;

uchar get_bits(void *_src, uint sbit, int n);
void put_bits(void *_dst, uint sbit, uchar ch, int n);
void bitscopy(void *_src, uint srcsbit, void *_dst, uint dstsbit, int n);

#endif
