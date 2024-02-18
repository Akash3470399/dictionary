#include <stdio.h>

#include "intr/cmptrie.h"

int main()
{

    int res = init_rfd();

    printf("res : %d\n", res);

    printf("search %d\n", is_word_present("hello"));

    return 0;
}
