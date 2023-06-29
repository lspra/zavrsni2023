#include <stdio.h>
#include <stdlib.h>
struct a0
{
    char a2;
};
struct a0 a3(char a6)
{
    struct a0 ba0;
    char a2;
    ba0.a2 = a6;
    return ba0;
}
char a8(struct a0 *ba0)
{
    return ba0->a2;
}
void main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "error - too few arguments");
        exit(1);
    }
    unsigned short a12;
    a12 = argv[1];
    struct a0 a13;
    struct a0 aaa4;
    aaa4 = a3(a12);
    a13 = aaa4;
    char aaa9;
    aaa9 = a8(&a13);
}
