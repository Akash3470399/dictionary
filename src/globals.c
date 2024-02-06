#ifndef GLOGALS_H
#define GLOGALS_H

typedef unsigned char uchar;
typedef unsigned int uint;

uint npmap_size;        // next node pointers map
uint mpmap_size;        // meaing pointer map
short int type_s[7];    // typesize[i] gives entry size of type structure in bits
uchar mpmap_s;          // no of bits for indexing meaning pointer (mp map)
uchar nnpmap_s;         // no of bits for indexing next node pointer (np map)
uchar mp_s;             // meaning pointer size (byte offset in meaning file)
uchar nnp_s;            // no of bits for next node pointer
uint type_mmap[7];      // start byte of each type & map
uint type_spmap[7];  // start pointer of a type

#define ch_s  5         // 
#endif
