#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "simplest.h"
#include "defs.h"
#include "bitsarr.h"

#define TYPESN 7
#define tobyte(n) ((int)(ceil((float)n/(1<<3))))
#define ptrsize(n) ((int)ceil((float)(log(n)/log(2))))
#define WORDLEN 100

int nncount[27];    // next node pointer count, value at index i gives node count with i next node pointers
int nnmcount[27];   // meanings count, value at index i gives total meanings availables for all nodes with i next node pointers 
int zmcount;        // store count of nodes without meaning of type ONE_T, TWO_T 

int type_size[TYPESN];
int totalnodes = 0; // total nodes generated
int totalwords = 0;
int othernodes = 0;
int memsize = 0;

enum Type tp;
int rootbit, npsize, mpsize, bitpos = 0, curbit = 0;
uchar *refdata;

char *tosearch_file = "data/w3.txt";
char *word_file = "data/words";
char *len_meaning_file = "data/len_meaning";
char *rfd_file = "data/rfd";

// create & initilize instance of struct Node
node *create_node()
{
    node *n;
    n = (node*)malloc(sizeof(node));
    n->mlist = 0, n->ncount = 0, n->mcount = 0;
    for(int i = 0; i < NCHRS; i++)
    {
        n->nextNode[i] = NULL;
        n->mps[i] = 0;
    }
    totalnodes += 1;
    return n;
}

// function to add word into trie
// requirements of function
//  - root is initilize
//  - word is having only alphabates only & terminated with '\0'
void insert_node(char *word, long mp)
{
    node *base = root;
    int i = 0;
    char idx = word[i]- 'a';

    while(word[i+1] != '\0' && word[i+1] != '\n')
    {
        if(base->nextNode[idx] == NULL)
        {
            base->nextNode[idx] = create_node();
            base->ncount += 1;
        }

        base = base->nextNode[idx];
        idx = word[++i] - 'a';
    }

    if(!(base->mlist & (1<<idx)))
    {
        base->mcount += 1;
        totalwords += 1;
        base->mps[idx] = mp;
    }
    base->mlist |= 1<<idx;
}


// search word in normal trie (with c pointers)
long normal_search(char *word)
{
    node *base = root;
    int i = 0;
    char idx = word[i] - 'a';
    long result = 0;

    while((base != NULL) && (word[i+1] != '\0'))
    {
        base = base->nextNode[idx];
        idx = word[++i] - 'a';
    }
    
    if((base != NULL) && (word[i+1] == '\0') && (base->mlist & 1<<idx))
        result = base->mps[idx];

    return result;
}

int get_meaning(char *word, char *meaning)
{
    long mp = cmp_search(word);
    uchar len ;
    FILE *fp;

    if(mp && ((fp= fopen(len_meaning_file, "rb") ) != NULL))
    {
        fseek(fp, mp, SEEK_SET);
        fread(&len, 1, 1, fp);
        fread(meaning, 1, len, fp);
        meaning[len] = '\0';
        fclose(fp);
    }
    
    return (mp != 0);
}

/*
int cmp_search(char *word)
{
    uchar ch;
    int res = 0, i = 0, meaning;
    
    bitpos = rootbit;
    while((word[i+1] != '\0' && word[i+1] != '\n') && (bitpos != -1))
        bitpos = get_nextlevel(word[i++]);

    if(bitpos != -1)
    {
        ch = word[i] - 'a';
        meaning = get_mmap();
        
        res = ((meaning & 1<<ch) >> ch);

    }

    return !(res == 0);
}
*/

long cmp_search(char *word)
{
    uchar ch;
    int i = 0, meaning;
    long res = 0;
    
    bitpos = rootbit;
    while((word[i+1] != '\0' && word[i+1] != '\n') && (bitpos != -1))
        bitpos = get_nextlevel(word[i++]);

    if(bitpos != -1)
    {
        ch = word[i] - 'a';
        meaning = get_mmap();
        
        res = ((meaning & 1<<ch) >> ch);
        
        for(int i = 0; (i <= ch) && res; i++)
            if(meaning & (1<<i))
                getmp(res);
    }

    return res;
}





// traverse through trie & calculate folloing things
//  - count nodes wrt next node available
//  - meanings wrt next node available
//  - node with 1 or 2 next node & dont have meaning
//  - othernodes : nodes with type OTR_T
void collect_data(node *parent)
{
    if(parent != NULL)
    {
        for(int i = 0; i < NCHRS; i++)
            if(parent->nextNode[i] != NULL)
                collect_data(parent->nextNode[i]);

        nncount[parent->ncount] += 1;
        nnmcount[parent->ncount] += parent->mcount;
    
        // nodes with OTR_T
        if(!(parent->ncount == 1 || parent->ncount == 2 || parent->ncount == 0 || parent->ncount == 26))
            othernodes += 1;

        // nodes with ONE_T or TWO_T
        if((parent->ncount == 1 || parent->ncount == 2) &&  parent->mcount == 0)
            zmcount += 1;
    }
}



// encode the normal trie (with c pointers) into compressed trie 
// with appropiate pointers calculated & store it to refdata
int compress(node *parent)
{
    int nodepos[NCHRS], selfpos;

    for(int i = 0; i < NCHRS; i++)
        if(parent->nextNode[i] != NULL)
            nodepos[i] = compress(parent->nextNode[i]);
    
    selfpos = curbit;
  
    switch(parent->ncount)
    {
        case 0:
            tp = ZERO_T;
            puttype(tp);
            putmap(parent->mlist);

            for(int i = 0; i < NCHRS; i++)
            {
                if(parent->mps[i] != 0)
                    putmp(parent->mps[i]);
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
                    break;
                }
            }
            
            if(tp == ONE_MT)
            {
                putmap(parent->mlist);  // meaning bitmap
                for(int i = 0; i < NCHRS; i++)
                {   
                    if(parent->mps[i] != 0)
                    putmp(parent->mps[i]);   // actual meaning
                }
            }
            break;
        case 2:
            tp = (parent->mcount == 0)? TWO_T : TWO_MT;

            puttype(tp);
            for(int i = 0; i < NCHRS; i++)
                if(parent->nextNode[i] != NULL)
                    putchar(i); 

            for(int i = 0; i < NCHRS; i++)
                if(parent->nextNode[i] != NULL)
                    putnp(nodepos[i]);
            
            if(tp == TWO_MT)
            {
                putmap(parent->mlist);  // meaning bitmap
                for(int i = 0; i < NCHRS; i++)
                    if(parent->mps[i] != 0)
                    putmp(parent->mps[i]);   // actual meaning
            }
            break;
        case 26:
            tp = FULL_T;
            puttype(tp);
            for(int i = 0 ; i < NCHRS; i++)
                putnp(nodepos[i]);

            putmap(parent->mlist);
            
            for(int i = 0; i < NCHRS; i++)
                if(parent->mps[i] != 0)
                    putmp(parent->mps[i]);
            break;
        default:
            int map = 0;
            long  lastpos;
            tp = OTR_T;
            
            puttype(tp);

            // shifting forword curbit to nps
            curbit = selfpos + 3 + NCHRS;
            // calculate next pointer map
            for(int i = 0; i < NCHRS; i++)
            {
                if(parent->nextNode[i] != NULL)
                {
                    map |= (1<<i);
                    putnp(nodepos[i]);
                }
            }
            lastpos = curbit;
           
            // shifting curbit back to np bitmap
            curbit = selfpos + 3;
            putmap(map); 
            
            // shifting curbit to mlist position
            curbit = lastpos;
            putmap(parent->mlist);
            for(int i = 0; i < NCHRS; i++)
                if(parent->mps[i] != 0)
                putmp(parent->mps[i]);
    }
    
    //printf("start %d, cf %d, type %d\n", selfpos, nf, tp);

    return selfpos;
}

void ptr_calc()
{
    int filesize, curptr, expptr = 10;
    FILE *fp;

    if((fp = fopen(len_meaning_file,"rb")) == NULL)
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
    memsize += 2 + (4 * sizeof(long));

    printf("totalnode %d, totalwords %d, zero mp(ONE_T, TWO_T) %d\n", totalnodes, totalwords, zmcount);
    printf("np %d, mp %d, memsize %d\n", npsize, mpsize, memsize);
}


// search each word on new line from the filename file in trie
// return count of words found, if file not found return -1
int search_words(char *filename)
{
    char word[WORDLEN], meaning[1000];
    int nwc = -1, cwc = -1;
    FILE *fp = fopen(filename, "rb+");
    
    if(fp != NULL)
    {
        nwc = 0, cwc = 0;
        while(fscanf(fp, "%s\n", word) != EOF)
        {
            nwc += (normal_search(word) != 0);
            cwc += (cmp_search(word) != 0);
        }
        printf("wc (normal trie search)     : %d\n", nwc);
        printf("wc (compressed trie search) : %d\n", cwc);
        fclose(fp);
    }
    return nwc;
}

// return the bitmap, provided bitpos is appropiately set to node
int get_mmap()
{
    int res = 0, map;
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
            getmap(map);
            if(map > 0)
            {
                for(int i = 0; i < NCHRS; i++)
                    if(map & (1<<i))
                        bitpos += npsize;
            }
            getmap(res);
    }
    return res;
}



// returns next node pointer for char ch
// provided bitpos is set at appropiate node in which next
// node to be search
// if next level not found then returns -1
int get_nextlevel(char ch)
{
    int res = -1, map;
    unsigned char ch1, ch2;
    
    ch = ch - 'a';
    gettype(tp);
    switch(tp)
    {
        case ONE_T:
        case ONE_MT:
            getchar(ch1);
            
            if(ch1 == ch)
                getnp(res);
            break;

        case TWO_T:
        case TWO_MT:
            getchar(ch1); 
            getchar(ch2);
            if(ch1 == ch)
            {
                getnp(res);
            }
            else if(ch2 == ch)
            {
                bitpos += npsize;
                getnp(res);
            } 
            break;

        case FULL_T:
            bitpos += (npsize * ch);
            getnp(res);
            break;

        case OTR_T:
            getmap(map);
            if(map & (1<<ch))
            {
                for(int i = 0; i < ch; i++)
                    if(map & (1<<i))
                        bitpos += npsize;
                getnp(res);
            }
    }

    return res;
}

void store_rfd()
{
    long *num = (long*)&refdata[memsize - (4*sizeof(long)) -1];
    FILE *rfdfp;

    *num++ = totalwords;
    *num++ = rootbit;
    *num++ = npsize;
    *num = mpsize;

    if((rfdfp = fopen(rfd_file, "wb+")) != NULL)
    {
        fwrite(refdata, 1, memsize, rfdfp);
        fclose(rfdfp);
    }
}

void free_trie(node *parent)
{
    if(parent)
    {
        for(int i = 0; i < NCHRS; i++)
            if(parent->nextNode[i])
                free_trie(parent->nextNode[i]);
        free(parent);
    }
}

// convert any UPPERCASE letter to lowercase in word
// returns 1 if word only contains alphabates else 0
int validate_and_fix(char *word)
{
    int i = 0,res = 1;
    char ch = word[i];
    while(ch != '\0' && ch != '\n' && res == 1)
    {
        if(ch >= 'A' && ch <= 'Z')
            word[i] |= 0x60;
        else if(!(ch >= 'a' && ch <= 'z') || ((i+1) == WORDLEN))
            res = 0;
        
        i += 1;
        ch = word[i];
    }
    
    if(word[i] == '\n')  word[i] = '\0';

    return res;
}


// requirements
// - words are not having special charecters
// - only small case letters are allowed 
int main(int argc, char *argv[])
{
    int n;
    long mp;
    char word[WORDLEN];
    FILE *wordsfp, *statfp;
   
    if(argc != 2 || (wordsfp = fopen(argv[1], "r")) == NULL)
    {
        printf("Specify the word file.\n");
        return 1;
    }
    
    // root of trie
    root = create_node();

    // insert each word to trie
    while(fscanf(wordsfp,"%ld,%s\n", &mp, word) != EOF)
    {
        if(validate_and_fix(word))
            insert_node(word, mp);
    }
    
    // traverse through trie & collect data for stat
    collect_data(root);     

    if((statfp= fopen("stat/simplest", "wb+")) != NULL)
    {
        fprintf(statfp, "no of np     node count      mp count\n");
        for(int i = 0; i <= NCHRS; i++)
            fprintf(statfp, "    %-10d  %-10d     %-10d\n", i, nncount[i], nnmcount[i]);
        fprintf(statfp, "nodes with zero meanings count (ONE_T, TWO_T) : %d\n", zmcount);

        fclose(statfp);
    }
    
    // calculate the pointers needed using stat 
    // set appropiate parameters
    ptr_calc();

    refdata = malloc(memsize);
    // compress trie and store to refdata
    rootbit = compress(root);
    
    search_words(tosearch_file);        
    
    // store refdata to persitent storage
    store_rfd();
    free_trie(root);
    printf("root bit pos %d\n",rootbit);
    fclose(wordsfp);
    return 0;
}


