#include <stdio.h>
void printhex(unsigned char *a, int start, int n)
{
    int count = 0;
    for(int i = start; i < start+n; i++)
    {
        if(count % 8 == 0)
            printf("\n");
        printf("%x ", a[i]);
        count += 1;
    }
    printf("\n");
}
