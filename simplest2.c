#include <stdio.h>
#include <stdlib.h>

#define NCHRS 26
#define OFF 'a'

typedef struct Node node;
struct Node 
{
    char hasmean;
    struct Node *nextNode[NCHRS];
}*root;

int nodecount[NCHRS+1], totalnodes = 0, mcount[NCHRS+1];



// assumption 
//  - all nodes can be stored in primary memory i.e malloc allocate node
node *new_node()
{
    node *n;
    n = (node*)malloc(sizeof(node));
    n->hasmean = 0;
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
    int i = 0;                          // to track char from word
    char idx = word[i] - OFF;           // position of word[i] in nextNode array
    node *base = root;  

    while(word[i] != '\0')
    {
        idx = word[i] - OFF;
        if(base->nextNode[idx] == NULL)
            base->nextNode[idx] = new_node();
        base = base->nextNode[idx];
        i += 1;
    }
    base->hasmean = 1;
}

// search word in trie
int search(char *word)
{
    int i = 0;                          // to track char from word
    char result = 0;
    node *base = root; 

    while((word[i] != '\0') && (base != NULL))
        base = base->nextNode[word[i++]-OFF];

    if((base != NULL) && (base->hasmean == 1))
        result = 1;
    return result;
}

void traverse(node *parent)
{
    int ncount = 0;
    for(int i = 0; i < NCHRS; i++)
    {
        if(parent->nextNode[i] != NULL)
        {
            ncount += 1;
            traverse(parent->nextNode[i]);
        }
    }
    mcount[ncount] += parent->hasmean;
    nodecount[ncount] += 1;
}

// requirements
// - words are not having special charecters
// - only small case letters are allowed 
int main(int argc, char *argv[])
{
    char word[100];
    FILE *wordsfp;
   
    if(argc != 2)
    {
        printf("provide words file\n");
        return 1;
    }

    if((wordsfp = fopen(argv[1], "r")) == NULL) 
    {
        printf("Invalid file\n");
        return 2;
    }

    root = new_node();
    while(fscanf(wordsfp, "%s\n", word) != EOF)
    {
        insert_node(word);
    }
/*
    FILE *srfp = fopen("test", "r");
    while(fscanf(srfp, "%s\n", word) != EOF)
    {
        printf("%d\n", search(word));
    }
*/
    traverse(root);
    for(int i = 0; i <= NCHRS; i++)
    {
        float val = (nodecount[i]*100)/totalnodes;
        printf("%8d %8d %8d %8f \n", i,mcount[i],nodecount[i], val );
    }

    fclose(wordsfp);
    //fclose(srfp);
    return 0;
}


