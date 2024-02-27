#include <stdio.h>

#include "intr/cmptrie.h"
#include "intr/rfd_utils.h"

int search_words(char *filename, refdata_info *rfd)
{
    char word[100], meaning[1000];
    int nwc = -1, cwc = -1;
    FILE *fp = fopen(filename, "rb+");

    long mp; 
    if(fp != NULL)
    {
        nwc = 0, cwc = 0;
        while(fscanf(fp, "%s\n", word) != EOF)
        {
            if(get_meaning(rfd, word, meaning))
                printf("%s\n", meaning);
        }
        fclose(fp);
    }
    return nwc;
}
int main()
{
    char *file = "data/rfd";
    refdata_info *rfd;

    int res = init_refdata_info(file, &rfd);
    
    if(res)
    {
        printf("total words %ld\n", rfd->totalwords);
        printf("np %ld, mp %ld\n", rfd->npsize, rfd->mpsize);
        printf("rootbit %ld\n", rfd->rootsbit);


        search_words("data/words\0", rfd);
        //printf("s %ld\n", is_word_present(rfd, "hello\0"));

    }

    return 0;
}
