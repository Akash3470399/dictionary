#include "defs.h"

#ifndef RFD_GETTER_H
#define RFD_GETTER_H


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
            bitscopy(_rfd, _bpos, &meaning_ptr, 0, mpsize); \
            _bpos += mpsize; \
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
            bitscopy(_rfd, _bpos, &next_ptr, 0, npsize); \
            _bpos += npsize; \
        }

#endif
