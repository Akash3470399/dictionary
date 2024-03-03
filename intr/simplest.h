#include "defs.h"

#ifndef RFD_UTILS_H
#define RFD_UTILS_H

extern int bitpos, npsize, mpsize;
extern enum Type tp;
extern uchar *refdata;

typedef struct Node node;
struct Node 
{
    int mlist;                  // bitmap of meanings available at a node
    uchar mcount, ncount;               // total meanings available at a node 
    struct Node *nextNode[NCHRS];  // next node pointers
    long mps[NCHRS];
}*root;


// macros to get appropite value into parameter provied

// requirement is 
// tp, refdata, bitpos globally define
// bitpos is properly set before using any of this macro
// i.e bitpos is at bit where the attribute is stored

#ifdef getchar
#undef getchar
#endif

#ifdef putchar
#undef putchar
#endif


#define gettype(tp) \
        { \
            tp = 0; \
            bitscopy(refdata, bitpos, &tp, 0, 3); \
            bitpos += 3; \
        }

#define getmap(bitmap) \
        { \
            bitmap = 0; \
            bitscopy(refdata, bitpos, &bitmap, 0, NCHRS); \
            bitpos += NCHRS; \
        }

#define getmp(meaning_ptr) \
        { \
            meaning_ptr = 0; \
            bitscopy(refdata, bitpos, &meaning_ptr, 0, mpsize); \
            bitpos += mpsize; \
        }

#define getchar(ch) \
        { \
            ch = 0; \
            bitscopy(refdata, bitpos, &ch, 0, 5); \
            bitpos += 5; \
        }

#define getnp(next_ptr) \
        { \
            next_ptr = 0; \
            bitscopy(refdata, bitpos, &next_ptr, 0, npsize); \
            bitpos += npsize; \
        }



// put appropiate values of node properties at bit position curbit
// requirement is curbit, refdata is globally defined


#define puttype(tp) \
        { \
            bitscopy(&tp, 0, refdata, curbit, 3); \
            curbit += 3; \
        }

#define putmap(bitmap) \
        { \
            bitscopy(&bitmap , 0, refdata, curbit, NCHRS); \
            curbit += NCHRS; \
        }

#define putmp(meaning_ptr) \
        { \
            bitscopy(&meaning_ptr, 0, refdata, curbit, mpsize); \
            curbit += mpsize; \
        }

#define putchar(ch) \
        { \
            bitscopy(&ch, 0, refdata, curbit, 5); \
            curbit += 5; \
        }

#define putnp(next_ptr) \
        { \
            bitscopy(&next_ptr, 0, refdata, curbit, npsize); \
            curbit += npsize; \
        }


node *create_node();
void insert_node(char *word, long mp);
int get_mmap();
int get_nextlevel(char ch);
long cmp_search(char *word);
long normal_search(char *word);

#endif
