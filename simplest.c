#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

typedef struct Node node;
typedef unsigned char uchar;
enum Type {ZERO_T, ONE_T, TWO_T, FULL_T, OTR_T};



#define NCHRS 26
#define tomb(n) ((int)(ceil((float)n/(1<<20))))
#define tokb(n) ((int)(ceil((float)n/(1<<10))))
#define tobyte(n) ((int)(ceil((float)n/(1<<3))))
#define rawsize(n) (n*sizeof(node))
#define ptrbits(n) ((int)ceil((float)(log(n)/log(2))))
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
int npmap_size;

short int type_s[5]; // typesize[i] gives entry size of type structure in bits
uchar mpmap_s;      // no of bits for indexing meaning pointer (mp map)
uchar nnpmap_s;     // no of bits for indexing next node pointer (np map)
uchar mp_s;         // meaning pointer size (byte offset in meaning file)
uchar nnp_s;        // no of bits for next node pointer

node *create_node();
void insert_node(char *word);
int search_words(char *filename);

void init_ptrsize()
{
    int othernodes = totalnodes - (nlarr[1] + 2*nlarr[2] + 26*nlarr[26]);

    printf("other %d\n", othernodes);
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

    mpmap_s = ptrbits(totalwords); 
    nnp_s = ptrbits(totalnodes); 
    nnpmap_s = ptrbits(othernodes);
    mp_s = ptrbits(meaning_filesize);

    // only meaning overhead
    type_s[ZERO_T] = movr;

    // one next node + meaning overhead
    type_s[ONE_T] = nnp_s + 5 + movr;     // 5 bits requied to encode 26 alphabates

    // two next node + meaning overhead
    type_s[TWO_T] = (2*nnp_s) + (2*5) + movr;

    // 26 next nodes + meaning overhead
    type_s[FULL_T] = (26*nnp_s) + movr;

    // others have next node bitmap + next node pointer start + meaning overhead
    type_s[OTR_T] = NCHRS + nnpmap_s + movr;
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

void space_calculations()
{
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
                type_s[ZERO_T], nlarr[0], tobyte(type_s[ZERO_T]*nlarr[0]));
        
        fprintf(cfp, "\tOne next node pointer     ONE       %-9d %-10d  %-9d \n", 
                type_s[ONE_T], nlarr[1], tobyte(type_s[ONE_T]*nlarr[1]));
        
        fprintf(cfp, "\tTwo next node pointers    TWO       %-9d %-10d  %-9d \n", 
                type_s[TWO_T], nlarr[2], tobyte(type_s[TWO_T] *nlarr[2]));
        
        fprintf(cfp, "\tAll 26 next node pointers FULL      %-9d %-10d  %-9d \n", 
                type_s[FULL_T], nlarr[26], tobyte(type_s[FULL_T]*nlarr[26]));
        
        fprintf(cfp, "\tOthers                    OTR       %-9d %-10d  %-9d \n", 
                type_s[OTR_T], othernodes, tobyte(type_s[OTR_T] * othernodes));
        fprintf(cfp, "\t-----------------------------------------------------------------\n");
        bytesize = type_s[ZERO_T]*nlarr[0] + type_s[ONE_T]*nlarr[1] + type_s[TWO_T] * nlarr[2] + type_s[FULL_T] * nlarr[26] + type_s[OTR_T] * othernodes;
        bytesize = tobyte(bytesize);
        fprintf(cfp, "\tTOTLAL                                  %d Bytes (%d MB)\n", bytesize, tomb(bytesize));
        
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

// requirements
// - words are not having special charecters
// - only small case letters are allowed 
int main(int argc, char *argv[])
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
    
    printf("\nc %d\n",search_words("data/1m"));
    fclose(fp);
    return 0;
}


