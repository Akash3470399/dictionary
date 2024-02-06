#ifndef GLOGALS_H
#define GLOGALS_H

typedef unsigned char uchar;
typedef unsigned int uint;

enum Blocks {ONE_B, TWO_B, FULL_B, MPMAP_B, ZERO_B, OTR_B, NPMAP_B};

extern uint npmap_size;        // next node pointers map
extern uint mpmap_size;        // meaing pointer map
extern short int type_s[5];    // typesize[i] gives entry size of type structure in bits
extern uchar mpmap_s;          // no of bits for indexing meaning pointer (mp map)
extern uchar nnpmap_s;         // no of bits for indexing next node pointer (np map)
extern uchar mp_s;             // meaning pointer size (byte offset in meaning file)
extern uchar nnp_s;            // no of bits for next node pointer
extern uint type_mmap[7];      // start byte of each type & map
extern uint type_spmap[5];  // start pointer of a type

#define ch_s  5         // 
#endif
