#ifndef DEFS_H
#define DEFS_H

#define NCHRS 26
typedef unsigned char uchar;

enum Type {ZERO_T, ONE_T, TWO_T, ONE_MT, TWO_MT, FULL_T, OTR_T};

/*
There are total 7 types

1) Nodes with Zero next node pointer    (ZERO_T)
	Structure: 
		 meaning bitmap 
		 meaning pointer start

2) Nodes with One next node pointer     (ONE_T)
	Structure: 
		 1 char 
		 1 next node pointer 
	    
         if meaning available :         (ONE_MT)
            meaning bitmap 
		    meaning pointer start 

3) Nodes with Two next node pointer     (TWO_T)
	Structure: 
		 2 char 
		 2 next node pointers 

         if meaning available :         (TWO_MT)
		    meaning bitmap 
		    meaning pointer start

4) Node with all 26 next node pointer   (FULL_T)
	Structure: 
		 26 next node pointers 
		 meaning bitmap 
		 meaning pointer start

5) Other nodes                          (OTR_T)
	 Structure: 
		 next node bitmap 
		 next node pointer start (np map) 
		 meaning bitmap 
		 meaning pointer start
*/

#endif
