#include "types.h"
#include "globals.h"
#include "bitsarr.h"


void mpentry_tobytes(mpentry *meaning, uchar *bytearr, uint sbit)
{
    bitscopy(&(meaning->mpstart), 0, bytearr, sbit, mpmap_s);
    bitscopy(&(meaning->bitmap), 0, bytearr, sbit + mpmap_s, NCHRS);
}

void bytes_tompentry(mpentry *meaning, uchar *bytearr, uint sbit)
{
    bitscopy(bytearr, sbit, &(meaning->mpstart), 0, mpmap_s);
    bitscopy(bytearr, sbit + mpmap_s, &(meaning->bitmap), 0, NCHRS);
}

void zero_tobytes(zero_t *node, uchar *bytearr, uint sbit)
{
    mpentry_tobytes(&(node->meaning), bytearr, sbit);
}

void bytes_tozero_t(zero_t *node, uchar *bytearr, uint sbit)
{
    bytes_tompentry(&(node->meaning), bytearr, sbit);
}

void one_tobytes(one_t *node, uchar *bytearr, uint sbit)
{
    bitscopy(&(node->nnp), 0, bytearr, sbit, nnp_s); 
    bitscopy(&(node->ch), 0, bytearr, (sbit+nnp_s), ch_s);
    mpentry_tobytes(&(node->meaning), bytearr, (sbit + nnp_s + ch_s));
}
void bytes_toone_t(one_t *node, uchar *bytearr, uint sbit)
{
    bitscopy(bytearr, sbit, &(node->nnp), 0, nnp_s);
    bitscopy(bytearr, (sbit + nnp_s), &(node->ch), 0, ch_s);
    bytes_tompentry(&(node->meaning), bytearr, (sbit + nnp_s + ch_s));
}

void two_tobytes(two_t *node, uchar *bytearr, uint sbit)
{
    bitscopy(&(node->nnp1), 0, bytearr, sbit, nnp_s);
    bitscopy(&(node->nnp1), 0, bytearr, (sbit+nnp_s), nnp_s);
    bitscopy(&(node->ch1), 0, bytearr, (sbit + 2*nnp_s), ch_s);
    bitscopy(&(node->ch2), 0, bytearr, (sbit + 2*nnp_s + ch_s), ch_s);
    mpentry_tobytes(&(node->meaning), bytearr, (sbit + 2*(nnp_s+ch_s)));
}

void bytes_totwo_t(two_t *node, uchar *bytearr, uint sbit)
{
    bitscopy(bytearr, sbit, &(node->nnp1), 0, nnp_s);
    bitscopy(bytearr, sbit +nnp_s, &(node->nnp1), 0, nnp_s);
    bitscopy(bytearr, (sbit + 2*nnp_s), &(node->ch1), 0, ch_s);
    bitscopy(bytearr, (sbit + 2*nnp_s + ch_s), &(node->ch1), 0, ch_s);
    mpentry_tobytes(&(node->meaning), bytearr, (sbit + 2*(nnp_s+ch_s)));
}

void full_tobytes(full_t *node, uchar *bytearr, uint sbit)
{
    for(int i = 0; i < NCHRS; i++)
        bitscopy(&(node->nnp[i]), 0, bytearr, (sbit + i*nnp_s), nnp_s);
    mpentry_tobytes(&(node->meaning), bytearr, (sbit + 26*nnp_s));
}

void bytes_tofull_t(full_t *node, uchar *bytearr, uint sbit)
{
    for(int i = 0; i < NCHRS; i++)
        bitscopy(bytearr, (sbit + i*nnp_s), &(node->nnp[i]), 0, nnp_s);
    bytes_tompentry(&(node->meaning), bytearr, (sbit + 26*nnp_s));
}

void otr_tobytes(otr_t *node, uchar *bytearr, uint sbit)
{
    bitscopy(&(node->npstart), 0, bytearr, sbit, nnpmap_s);
    bitscopy(&(node->npbitmap), 0, bytearr, (sbit+nnpmap_s), NCHRS);
    mpentry_tobytes(&(node->meaning), bytearr, (sbit + nnpmap_s + NCHRS));
}

void bytes_tootr_t(otr_t *node, uchar *bytearr, uint sbit)
{
    bitscopy(bytearr, sbit, &(node->npstart), 0, nnpmap_s);
    bitscopy(bytearr, (sbit + nnpmap_s), &(node->npbitmap), 0, NCHRS);
    bytes_tompentry(&(node->meaning), bytearr, (sbit + nnpmap_s + NCHRS));
}
