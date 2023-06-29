#include <stdio.h>
void main () {
short a2;
double a4;
scanf("%hd", &a2);
float a5 = 18.2;
float a6 = a2*a5;
int a7 = 11;
float a8 = a6+a7;
printf("%f", a8);
float a10 = 9.3578;
int a11 = 11;
int a12 = 22;
int a13 = a11<<a12;
float a14 = a10/a13;
int a15 = 31;
float a16 = a14*a15;
a4 = a16;
unsigned short a18;
goto for0_cond;
for0_begin: ;
printf("%hu", a18);
for0_cont: ;
a18 = a18*a2;
for0_cond: ;
int a19 = 22;
unsigned short a20 = a18-a19;
int a21 = 8;
a2 = a2*a21;
double a22 = a4/a2;
char a23 = a20>a22;
if(a23) {
	goto for0_begin;
}
for0_end: ;
}
