#ifndef BITSARR_H
#define BITSARR_H

typedef unsigned char uchar;
typedef unsigned int uint;

// get (n-sbit) bits from _src, starting from sbit
// 0 < (n-sbit) < 8
uchar get_bits(void *_src, uint sbit, int n);

// put (n-sbit) bits to _dst, starting from sbit from lsb bit of ch
// 0 < (n-sbit) < 8
void put_bits(void *_dst, uint sbit, uchar ch, int n);

// copy n bits from _src starting from srcsbit bit
// to _dst starting from dstsbit bit
void bitscopy(void *_src, uint srcsbit, void *_dst, uint dstsbit, int n);

#endif
