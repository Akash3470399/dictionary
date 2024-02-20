#include <stdio.h>


int dump_err(char *base, const char *detailed)
{
    FILE *_errsfp = NULL;
    int ret = 1;
    if((_errsfp = fopen("logs", "ab+")) != NULL)
    {
        fprintf(_errsfp, "%s\n%s\n\n", base, detailed);
        fclose(_errsfp);
        ret = 0;
    }
    return ret;
}
