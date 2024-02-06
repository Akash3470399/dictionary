#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "consts.h"
#include "globals.h"
#include "types.h"
typedef struct Node node;

#define tomb(n) ((int)(ceil((float)n/(1<<20))))
#define tokb(n) ((int)(ceil((float)n/(1<<10))))
#define tobyte(n) ((int)(ceil((float)n/(1<<3))))
#define rawsize(n) (n*sizeof(node))
#define ptrbits(n) ((int)ceil((float)(log(n)/log(2))))
#define nodepos(ptrno, blk, size) (type_mmap[blk] + (((ptrno - type_spmap[blk] * size))
#define mbitmap_s NCHRS
#define nbitmsp_s NCHRS


struct Node 
{
    int mlist;                  // meanings available at a node
    uchar mcount;       // total meanings available at a node 
    struct Node *nextNode[26];  // next node pointers
}*root;

int nlarr[27];      // next node pointer count, value at index i gives node count with i next node pointers
int lmarr[27];      // meanings count, value at index i gives total meanings availables for all nodes with i next node pointers 
int zmarr[27];      // zero meaning node count, value at index i gives node with zero meanings with i next node pointers 
int totalnodes = 0; // total nodes generated
int totalwords = 0; // total words in trie
void *refdata;
int refdata_filesize;
int types_cur_pointer[5]; // give next pointer of ith type that can be used
node *create_node();
void insert_node(char *word);
int search_words(char *filename);

void init_ptrsize()
{
    int npmap_nodes = totalnodes - (nlarr[1] + 2*nlarr[2] + 26*nlarr[26]);

    FILE *fp ;
    int meaning_filesize;
    
    // meaning overhead : meaning bitmap + meaning pointer start
    uchar movr = ptrbits(totalwords) + NCHRS;
    
    if((fp = fopen("data/len_meaning", "r")) != NULL)
    {
        fseek(fp, 0, SEEK_END);
        meaning_filesize = ftell(fp);
        fclose(fp);
    }

    type_s[MPMAP_B] = ptrbits(totalwords); 
    nnp_s = ptrbits(totalnodes); 
    nnpmap_s = ptrbits(npmap_nodes);
    mp_s = ptrbits(meaning_filesize);

    // only meaning overhead
    type_s[ZERO_B] = movr;

    // one next node + meaning overhead
    type_s[ONE_B] = nnp_s + 5 + movr;     // 5 bits requied to encode 26 alphabates

    // two next node + meaning overhead
    type_s[TWO_B] = (2*nnp_s) + (2*5) + movr;

    // 26 next nodes + meaning overhead
    type_s[FULL_B] = (26*nnp_s) + movr;

    // others have next node bitmap + next node pointer start + meaning overhead
    type_s[OTR_B] = NCHRS + nnpmap_s + movr;
    
    // next pointer map having next pointer of OTR node 
    npmap_size = tobyte(npmap_nodes * nnp_s);

    // meaing pointer map having meanings byte offset from meaning file
    mpmap_size = tobyte(totalwords * mp_s);

    // recording starting memory by of each block in memory
    type_mmap[ONE_B] = 0, type_mmap[TWO_B] = tobyte(type_s[ONE_B]*nlarr[1]);
    type_mmap[FULL_B] = type_mmap[TWO_B] + tobyte(type_s[TWO_B] * nlarr[2]);
    type_mmap[MPMAP_B] = type_mmap[FULL_B] + tobyte(type_s[FULL_B] * nlarr[26]);
    type_mmap[ZERO_B] = type_mmap[MPMAP_B] + tobyte(totalwords * mp_s);
    type_mmap[OTR_B] = type_mmap[ZERO_B] + tobyte(type_s[ZERO_B] * nlarr[0]);
    // no of nodes of type other_t 
    npmap_nodes = totalnodes - (nlarr[0] + nlarr[1] + nlarr[26] + nlarr[2]);
    type_mmap[NPMAP_B] = type_mmap[OTR_B] + tobyte(type_s[OTR_B] * npmap_nodes);

    // recording start pointer of each type
    type_spmap[ONE_B] = 0, type_spmap[TWO_B] = type_spmap[ONE_B] + nlarr[1];
    type_spmap[FULL_B] = type_spmap[TWO_B] + nlarr[2];
    type_spmap[MPMAP_B] = 0, type_spmap[ZERO_B] = type_spmap[FULL_B] + nlarr[26];
    type_spmap[OTR_B] = type_spmap[ZERO_B] + nlarr[0];
    type_spmap[NPMAP_B] = 0;

    for(int i = 0; i < 7; i++)
        printf("%d  %d\n", type_mmap[i], type_spmap[i]);
}

// create & initilize instance of struct Node
node *create_node()
{
    node *n;
    n = (node*)malloc(sizeof(node));
    n->mlist = 0;
    n->mcount = 0;
    for(int i = 0; i < NCHRS; i++)
        n->nextNode[i] = NULL;
    totalnodes += 1;
    return n;
}

// function to add word into trie
// requirements of function
//  - root is initilize
//  - word is having only alphabates only & terminated with '\0'
void insert_node(char *word)
{
    node *base = root;
    int i = 0;
    char idx = word[i]- 'a';

    while(word[i+1] != '\0' && word[i+1] != '\n')
    {
        if(base->nextNode[idx] == NULL)
            base->nextNode[idx] = create_node();

        base = base->nextNode[idx];
        idx = word[++i] - 'a';
    }

    if(!(base->mlist & (1<<idx)))
        base->mcount += 1;
    base->mlist |= 1<<idx;
}


// search word in trie
int search(char *word)
{
    node *base = root;
    int i = 0;
    char idx = word[i] - 'a', result = 0;

    while((base != NULL) && (word[i+1] != '\0'))
    {
        base = base->nextNode[idx];
        idx = word[++i] - 'a';
    }
    
    if((base != NULL) && (word[i+1] == '\0') && (base->mlist & 1<<idx))
        result = 1;

    return result;
}

void collect_data(node *parent)
{
    int nf = 0;
    if(parent != NULL)
    {
        for(int i = 0; i < NCHRS; i++)
        {
            if(parent->nextNode[i] != NULL)
            {
                nf += 1;
                collect_data(parent->nextNode[i]);
            }
        }
        nlarr[nf] += 1;
        lmarr[nf] += parent->mcount;
        if(parent->mcount == 0)
        zmarr[nf] += 1;
    }
}

// this function should be called only after collect_data have 
void summarize()
{
    time_t start_time, end_time;
    int bytesize = totalnodes * sizeof(node), othernodes = 0; 
    FILE *sfp = fopen("stat/simplest_trie", "wb+");
    FILE *cfp = fopen("stat/compressed_trie", "wb+");

    if(sfp != NULL && cfp != NULL)
    {
        // simplest trie calculations
        fprintf(sfp, "Total nodes : %d\nTotal words : %d\n", totalnodes, totalwords);
        fprintf(sfp, "Space taken : %d Bytes (%d MB)\n", bytesize, tomb(bytesize));
        fprintf(sfp, "Node next pointers    Node count   Meanings nodes   Nodes with zero meaning\n");
        for(int i = 0; i <= NCHRS; i++)
            fprintf(sfp, "     %-14d     %-12d   %-18d %-d\n", i, nlarr[i], lmarr[i], zmarr[i]);

       
        // compressed trie calculations
        for(int i = 3; i < 26; i++)
            othernodes += nlarr[i];
        
        fprintf(cfp, "Total types : 4\nTypes in detail.\n");
        fprintf(cfp, "1) Nodes with Zero next node pointer\n\tStructure: \n\t\t meaning bitmap \n\t\t meaning pointer start\n");
        fprintf(cfp, "2) Nodes with One next node pointer\n\tStructure: \n\t\t 1 char \n\t\t 1 next node pointer \n\t\t meaning bitmap \n\t\t meaning pointer start \n");
        fprintf(cfp, "3) Nodes with Two next node pointer\n\tStructure: \n\t\t 2 char \n\t\t 2 next node pointers \n\t\t meaning bitmap \n\t\t meaning pointer start\n");
        fprintf(cfp, "4) Node with all 26 next node pointer\n\tStructure: \n\t\t 26 next node pointers \n\t\t meaning bitmap \n\t\t meaning pointer start\n");
        fprintf(cfp, "5) Other nodes\n\t Structure: \n\t\t next node bitmap \n\t\t next node pointer start (np map) \n\t\t meaning bitmap \n\t\t meaning pointer start\n");
        
        fprintf(cfp, "\t           Type            SYM  size (bits) Node count   Space taken (Bytes)\n");
        fprintf(cfp, "\t-----------------------------------------------------------------\n");
        fprintf(cfp, "\tZero next node pointer    ZERO      %-9d %-10d  %-9d \n", 
                type_s[ZERO_B], nlarr[0], tobyte(type_s[ZERO_B]*nlarr[0]));
        
        fprintf(cfp, "\tOne next node pointer     ONE       %-9d %-10d  %-9d \n", 
                type_s[ONE_B], nlarr[1], tobyte(type_s[ONE_B]*nlarr[1]));
        
        fprintf(cfp, "\tTwo next node pointers    TWO       %-9d %-10d  %-9d \n", 
                type_s[TWO_B], nlarr[2], tobyte(type_s[TWO_B] *nlarr[2]));
        
        fprintf(cfp, "\tAll 26 next node pointers FULL      %-9d %-10d  %-9d \n", 
                type_s[FULL_B], nlarr[26], tobyte(type_s[FULL_B]*nlarr[26]));
        
        fprintf(cfp, "\tOthers                    OTR       %-9d %-10d  %-9d \n", 
                type_s[OTR_B], othernodes, tobyte(type_s[OTR_B] * othernodes));
        fprintf(cfp, "\t-----------------------------------------------------------------\n");
        bytesize = type_s[ZERO_B]*nlarr[0] + type_s[ONE_B]*nlarr[1] + type_s[TWO_B] * nlarr[2] + type_s[FULL_B] * nlarr[26] + type_s[OTR_B] * othernodes;
        bytesize = tobyte(bytesize);
        fprintf(cfp, "\tTOTLAL                                  %d Bytes (%d MB)\n", bytesize, tomb(bytesize));
     
        fprintf(cfp, "Next node pointers map size : %d Byte\n", npmap_size);
        fprintf(cfp, "Meaing pointer map size     : %d Byte\n", mpmap_size);
        
        bytesize += npmap_size + mpmap_size;
        refdata_filesize = bytesize;
        fprintf(cfp, "Total space required to store compressed tree : %d Bytes (%d MB)\n", bytesize, tomb(bytesize));

        fclose(cfp);
        fclose(sfp);
    }
}

// search each word on new line from the filename file in trie
// return count of words found, if file not found return -1
int search_words(char *filename)
{
    char word[100];
    int words_found = -1;
    FILE *fp = fopen(filename, "rb+");
    
    if(fp != NULL)
    {
        words_found = 0;
        while(fscanf(fp, "%s\n", word) != EOF)
            words_found += (search(word) == 0)? 0: 1;

        fclose(fp);
    }
    return words_found;
}

void compress(node *n)
{
    int nf = 0;
    if(n != NULL)
    {
        for(int i = 0; i < NCHRS; i++)
        {
            nf += 1;
            if(n->nextNode[i] != NULL)
                compress(n->nextNode[i]);
        }


        switch(nf)
        {
            case 0:
                zero_t node;
                node.ptrno = type_spmap[ZERO_B] + types_cur_pointer[ZERO_B]++;
                node.meaning.bitmap = n->mlist;
                 
                break;
        }
    }
}


// requirements
// - words are not having special charecters
// - only small case letters are allowed 
int simplest_main(int argc, char *argv[])
{
    int n;
    char word[100];
    FILE *fp;
   
    if(argc != 2 || (fp = fopen(argv[1], "r")) == NULL)
    {
        printf("Specify the word file.\n");
        return 1;
    }

    root = create_node();
    while(fscanf(fp,"%s\n",word) != EOF)
    {
        insert_node(word);
        totalwords += 1;
    }
    
    collect_data(root);     // traverse through trie & collect data
    init_ptrsize();
    summarize();            // use collected data and generate stats
    
    //printf("\nc %d\n",search_words("data/1m"));
    fclose(fp);
    return 0;
}


