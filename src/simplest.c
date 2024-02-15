#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "bitsarr.h"

typedef struct Node node;
typedef unsigned char uchar;
enum Type {ZERO_T, ONE_T, TWO_T, ONE_MT, TWO_MT, FULL_T,OTR_T};
enum Type tp;

#define NCHRS 26
#define TYPESN 7
#define tomb(n) ((int)(ceil((float)n/(1<<20))))
#define tokb(n) ((int)(ceil((float)n/(1<<10))))
#define tobyte(n) ((int)(ceil((float)n/(1<<3))))
#define ptrsize(n) ((int)ceil((float)(log(n)/log(2))))

long curbit = 0, bitpos = 0;
int mpsize, npsize, rootbit;
void *refdata;

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

struct Node 
{
    int mlist;          // meanings available at a node
    uchar mcount;       // total meanings available at a node 
    struct Node *nextNode[26];  // next node pointers
}*root;

int nncount[27];    // next node pointer count, value at index i gives node count with i next node pointers
int nnmcount[27];   // meanings count, value at index i gives total meanings availables for all nodes with i next node pointers 
int zmcount;       // store count of nodes without meaning of type ONE_T, TWO_T 
int type_size[TYPESN];
int totalnodes = 0; // total nodes generated
int totalwords = 0; // total words in trie
int othernodes = 0;
long memsize = 0;


node *create_node();
void insert_node(char *word);
int search_words(char *filename);
int cmp_search(char *word);

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
    {
        base->mcount += 1;
        totalwords += 1;
    }
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

        nncount[nf] += 1;
        nnmcount[nf] += parent->mcount;
    
        if(!(nf == 1 || nf == 2 || nf == 0 || nf == 26))
            othernodes += 1;

        if((nf == 1 || nf == 2) &&  parent->mcount == 0)
            zmcount += 1;
    }
}

int compress(node *parent, int level)
{
    int nf = 0, nodepos[NCHRS], selfpos;

    for(int i = 0; i < NCHRS; i++)
    {
        if(parent->nextNode[i] != NULL)
        {
            nf += 1;
            nodepos[i] = compress(parent->nextNode[i], level+1);
            //printf("level %d, letter %c, start %d\n", level+1, (i+'a'), nodepos[i]);
        }
    }
    selfpos = curbit;
  
    switch(nf)
    {
        case 0:
            tp = ZERO_T;
            puttype(tp);
            putmap(parent->mlist);

            for(int i = 0; i < parent->mcount; i++)
            {
                putmp(parent->mlist);
            }
            break;
        case 1:
            tp = (parent->mcount == 0)? ONE_T : ONE_MT;

            puttype(tp);
            for(int i = 0; i < NCHRS; i++)
            {
                if(parent->nextNode[i] != NULL)
                {
                    putchar(i); 
                    putnp(nodepos[i]);
                }
            }
            
            if(tp == ONE_MT)
            {
                putmap(parent->mlist);  // meaning bitmap
                for(int i = 0; i < parent->mcount; i++)
                {   
                    putmp(parent->mlist);   // actual meaning
                }
            }
            break;
        case 2:
            tp = (parent->mcount == 0)? TWO_T : TWO_MT;

            puttype(tp);
            for(int i = 0; i < NCHRS; i++)
            {
                if(parent->nextNode[i] != NULL)
                {
                    putchar(i); 
                }
            }

            for(int i = 0; i < NCHRS; i++)
            {
                if(parent->nextNode[i] != NULL)
                {
                    putnp(nodepos[i]);
                }
            }
            
            if(tp == TWO_MT)
            {
                putmap(parent->mlist);  // meaning bitmap
                for(int i = 0; i < parent->mcount; i++)
                {   
                    putmp(parent->mlist);   // actual meaning
                }
            }
            break;
        case 26:
            tp = FULL_T;
            puttype(tp);
            for(int i = 0 ; i < NCHRS; i++)
            {
                putnp(nodepos[i]);
            }

            putmap(parent->mlist);
            
            for(int i = 0; i < parent->mcount; i++)
            {
                putmp(parent->mlist);
            }
            break;
        default:
            int map = 0,  a;
            tp = OTR_T;
            
            puttype(tp);

            // calculate next pointer map
            for(int i = 0; i < NCHRS; i++)
            {
                if(parent->nextNode[i] != NULL)
                {
                    map |= (1<<i);
                }
            }
            putmap(map); 
            for(int i = 0; i < NCHRS; i++)
            {
                if(parent->nextNode[i] != NULL)
                {
                    putnp(nodepos[i]);  // put next pointers
                }
            }
             
            putmap(parent->mlist);
            for(int i = 0; i < parent->mcount; i++)
            {
                putmp(parent->mlist);
            }
    }
    
    //printf("start %d, cf %d, type %d\n", selfpos, nf, tp);

    return selfpos;
}

void calculations()
{
    int filesize, curptr, expptr = 10;
    FILE *fp;

    if((fp = fopen("data/len_littlem","rb")) == NULL)
        return;
    fseek(fp, 0, SEEK_END);
    filesize = ftell(fp);
    fclose(fp);
  
    mpsize = ptrsize(filesize);
   
    do{
        curptr = expptr;
        memsize = (totalnodes * 3);
        memsize += ((totalnodes-zmcount) * 26);
        memsize += (totalwords*mpsize);
        
        memsize += (totalnodes * curptr);

        memsize += (nncount[1] * 5);
        memsize += (nncount[2] * 10);
        memsize += (othernodes * NCHRS);
        expptr = ptrsize(memsize);
    }while(curptr < expptr);
    npsize = expptr;
    memsize = tobyte(memsize);

    printf("totalnode %d, totalwords %d, zero mp(ONE_T, TWO_T) %d\n", totalnodes, totalwords, zmcount);
    printf("np %d, mp %d, memsize %d\n", npsize, mpsize, memsize);
}
int wc = 0;
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
        {
            words_found += (search(word) == 0)? 0: 1;
            wc += (cmp_search(word) == 0) ? 0 : 1;
        }
        printf("cmp wc %d\n", wc);
        fclose(fp);
    }
    return words_found;
}
int get_mmap()
{
    int res = 0, buf;
    gettype(tp);
    switch(tp)
    {
        case ZERO_T:
            getmap(res);
            break;
        case ONE_MT:
            bitpos += (5 + npsize);
            getmap(res);
            break;
        case TWO_MT:
            bitpos += (10 + (npsize<<1));
            getmap(res);
            break;
        case FULL_T:
            bitpos += (26*npsize);
            getmap(res);
            break;
        case OTR_T:
            getmap(buf);
            if(buf > 0)
            {
                for(int i = 0; i < NCHRS; i++)
                    if(buf & (1<<i))
                        bitpos += npsize;
            }
            getmap(res);

    }
    return res;
}


long get_nextlevel(char ch)
{
    long res = -1, buf;
    unsigned char ch1, ch2;
    gettype(tp);
    switch(tp)
    {
        case ONE_T:
        case ONE_MT:
            getchar(ch1);
            
            if(ch1 == (ch-'a'))
            {
                getnp(res);
            }
            break;
        case TWO_T:
        case TWO_MT:
            getchar(ch1); 
            getchar(ch2);
            if(ch1 == (ch-'a'))
            {
                getnp(res);
            }
            else if(ch2 == (ch-'a'))
            {
                bitpos += npsize;
                getnp(res);
            } 
            break;
        case FULL_T:
            bitpos += (npsize * (ch - 'a'));
            getnp(res);
            break;
        case OTR_T:
            getmap(buf);
            if(buf & (1<<(ch-'a')))
            {
                for(int i = 0; i < (ch - 'a'); i++)
                    if(buf & (1<<i))
                        bitpos += npsize;
                getnp(res);
            }
            break;
    }

    return res;
}


int cmp_search(char *word)
{
    int res = 0, i = 0, meaning;
    
    bitpos = rootbit;
    while((word[i+1] != '\0' && word[i+1] != '\n') && (bitpos != -1))
    {
        bitpos = get_nextlevel(word[i++]);
    }

    if(bitpos != -1)
    {
        meaning = get_mmap();
        res = ((meaning & 1<<(word[i]-'a')) >> (word[i])-'a');
    }

    return res;
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
    }
    
    collect_data(root);     // traverse through trie & collect data

    calculations();


    refdata = malloc(memsize);

    rootbit = compress(root, 0);
   

    printf("root bit pos %d\n",rootbit);
            
    
/*
    FILE *statfp;
    if((statfp= fopen("stat/simplest", "wb+")) != NULL)
    {
        fprintf(statfp, "no of np     node count      mp count\n");
        for(int i = 0; i <= NCHRS; i++)
            fprintf(statfp, "    %-10d  %-10d     %-10d\n", i, nncount[i], nnmcount[i]);
        fprintf(statfp, "nodes with zero meanings count (ONE_T, TWO_T) : %d\n", zmcount);

        fclose(statfp);
    }
*/



/*
    char h[100] = "";
    FILE *f = fopen("data/little", "rb+");

    while(f != NULL && fscanf(f, "%s\n", h) != EOF)
    {       
        printf("%s : %d\n", h, cmp_search(h));
    }
  */  
    printf("\nc %d\n",search_words("data/1m"));
    fclose(fp);
    return 0;
}


