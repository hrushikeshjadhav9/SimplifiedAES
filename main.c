#include <stdio.h>
#include <stdlib.h>

typedef char State[2];

int main()
{
    State a;
    a[1] = 100;
    printf("%d\n", a[1]);
    return 0;
}
