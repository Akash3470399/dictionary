#include <stdio.h>

#include "types.h"

int simplest_main(int argc, char *argv[]);
int main()
{
    one_t node, n;
    node.nnp = 123;
    node.ch = 12;
    node.meaning.mpstart = 345;
    node.meaning.bitmap = 121;

    uchar ch[20];
    char *a[] = {"./a.out", "data/words"};
    simplest_main(2, a);

    one_tobytes(&node, ch, 17);

    bytes_toone_t(&n, ch, 17);

    printf("%d %d %d %d\n", n.nnp, n.ch, n.meaning.mpstart, n.meaning.bitmap);

    return 0;
}
