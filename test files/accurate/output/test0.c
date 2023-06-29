#include <stdio.h>
#include <stdlib.h>
void main (int argc, char** argv) {
if(argc <2) {
	fprintf(stderr, "error - too few arguments");
	exit(1);
}
unsigned int a2;
a2 =  argv[1];
int a6 = 32;
char a7[a6];
char a10;
goto for0_cond;
for0_begin: ;
int a32 = 1;
int a34 = 8;
a7[a32] = a34;
for0_cont: ;
for0_cond: ;
int a12 = 11;
int a13 = 33;
int a14 = a12-a13;
int a22 = 1;
char a23 = a7[a22]*a10;
char a24 = a14>a23;
if(a24) {
	goto for0_begin;
}
for0_end: ;
int a35;
int a37 = 0;
a35 = a37;
goto for1_cond;
for1_begin: ;
printf("%d", a35);
for1_cont: ;
a35 = a35+1;
for1_cond: ;
char a38 = a35<a10;
if(a38) {
	goto for1_begin;
}
for1_end: ;
}
