#include "defs.h"

#ifndef RFD_UTILS_H
#define RFD_UTILS_H

#define _rfd refdata 
#define _bpos bitpos
#define _mpsize 0
#define _npsize 0 

extern int bitpos, npsize, mpsize;
extern enum Type tp;
extern uchar *refdata;

// macros to get appropite value into parameter provied

// requirement is 
// tp, refdata, bitpos globally define
// bitpos is properly set before using any of this macro
// i.e bitpos is at bit where the attribute is stored


#define gettype(tp) \
        { \
            tp = 0; \
            bitscopy(_rfd, _bpos, &tp, 0, 3); \
            _bpos += 3; \
        }

#define getmap(bitmap) \
        { \
            bitmap = 0; \
            bitscopy(_rfd, _bpos, &bitmap, 0, NCHRS); \
            _bpos += NCHRS; \
        }

#define getmp(meaning_ptr) \
        { \
            meaning_ptr = 0; \
            bitscopy(_rfd, _bpos, &meaning_ptr, 0, _mpsize); \
            _bpos += _mpsize; \
        }

#define getchar(ch) \
        { \
            ch = 0; \
            bitscopy(_rfd, _bpos, &ch, 0, 5); \
            _bpos += 5; \
        }

#define getnp(next_ptr) \
        { \
            next_ptr = 0; \
            bitscopy(_rfd, _bpos, &next_ptr, 0, _npsize); \
            _bpos += _npsize; \
        }



// put appropiate values of node properties at bit position curbit
// requirement is curbit, refdata is globally defined


#define puttype(tp) \
        { \
            bitscopy(&tp, 0, _rfd, curbit, 3); \
            curbit += 3; \
        }

#define putmap(bitmap) \
        { \
            bitscopy(&bitmap , 0, _rfd, curbit, NCHRS); \
            curbit += NCHRS; \
        }

#define putmp(meaning_ptr) \
        { \
            bitscopy(&meaning_ptr, 0, _rfd, curbit, _mpsize); \
            curbit += _mpsize; \
        }

#define putchar(ch) \
        { \
            bitscopy(&ch, 0, _rfd, curbit, 5); \
            curbit += 5; \
        }

#define putnp(next_ptr) \
        { \
            bitscopy(&next_ptr, 0, _rfd, curbit, _npsize); \
            curbit += _npsize; \
        }


int get_mmap();
int get_nextlevel(char ch);


#endif
