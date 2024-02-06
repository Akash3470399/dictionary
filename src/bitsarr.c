#include <stdio.h>
#include <math.h>

typedef unsigned char uchar;

// 0 < n <= 8
// return n bits staring from sbit bit
// ((1<<n - 1) << (sbit) is the mask to get n bits from byte strting from stbit
uchar get_bits(void *_src, int sbit, int n)
{
    uchar *src = (uchar *)_src;
    uchar ch = ((src[0] & ((uchar)(((1<<n) - 1)<<sbit))) >> sbit);
    n -= (8 - sbit);
    if(n > 0)
        ch |= ((src[1] & (uchar)((1<<n) - 1)) << (8-sbit));
    return ch;
}

// put n bits from ch to dst memory staring from sbit bit
// 0 < n <= 8
// ((1<<n - 1) << (sbit) is the mask to get n bits from byte strting from stbit
void put_bits(void *_dst, int sbit, uchar ch, int n)
{
    uchar *dst = (uchar *)_dst;
    dst[0] &= ~((uchar)(((1<<n) - 1)<<sbit)); 
    dst[0] |= (((uchar)(((1<<n) -1) << sbit))  & ch<<sbit);
    ch = ch >> (8-sbit);
    n -= (8-sbit);
    if(n > 0)
    {
        dst[1] &= ~((uchar)((1<<n) - 1));
        dst[1] |= (((uchar)((1<<n) - 1)) & ch);
    }
}

// copy n bits from src to dst with starts bit specified 
// 0 <= sbit < 8
void bitscopy(void *_src, int srcsbit, void *_dst, int dstsbit, int n)
{
    uchar *src, *dst;
    int wr = 0, space;
    uchar srcpart;

    src = (uchar *) (_src + (srcsbit/8));
    dst = (uchar *) (_dst + (dstsbit/8));

    srcsbit %= 8;
    dstsbit %= 8;

    while(wr <  n)
    {
        // calculate how much space is there in dst byte
        space = ((n - wr) < (8 - dstsbit))? (n - wr) : (8-dstsbit);
        
        srcpart = get_bits(src, srcsbit, space) ;//&((1<<space) -1);
        put_bits(dst, dstsbit, srcpart, space);

        src += ((srcsbit+space)/8);
        srcsbit = ((srcsbit + space) % 8);

        dstsbit = 0;
        dst += 1;

        wr += space;
    }
}
