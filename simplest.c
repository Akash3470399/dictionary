#include <stdio.h>
#include <stdlib.h>

typedef struct Node node;
#define NCHRS 26

struct Node 
{
    int mlist;                  // meanings available at a node
    unsigned char mcount;       // total meanings available at a node 
    struct Node *nextNode[26];  // next node pointers
}*root;

int nlarr[27];      // next node pointer count, value at index i gives node count with i next node pointers
int lmarr[27];      // meanings count, value at index i gives total meanings availables for all nodes with i next node pointers 
int zmarr[27];      // zero meaning node count, value at index i gives node with zero meanings with i next node pointers 
int totalnodes = 0; // total nodes generated
int totalwords = 0; // total words in trie

node *create_node();
void insert_node(char *word);


// create & initilize instance of struct Node
node *create_node()
{
    node *n;
    n = (node*)malloc(sizeof(node));
    n->mlist = 0;
    n->mcount = 0;
    for(int i = 0; i < NCHRS; i++)
        n->nextNode[i] = NULL;
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

    FILE *sfp = fopen("stat/simplest_trie", "wb+");
    FILE *cfp = fopen("stat/compressed_trie", "wb+");

    if(sfp != NULL && cfp != NULL)
    {
            
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
        insert_node(word);
    
    printf("found %d\n", search_words("iwords"));

    //collect_data(root);     // traverse through trie & collect data
    //summarize();            // use collected data and generate stats
    
    fclose(fp);
    return 0;
}


