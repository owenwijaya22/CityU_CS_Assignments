#include <stdio.h>
unsigned char xyz[200] = "1af22c9f0fa4bbb2e2d4171dd34b197febdbbb07484936c157e81a2ba49cd1edbf7ea70d4f93acfcbca14b0c5d89b9aac4236ee70457c1cb51d77223a4d06772cb7d555d929e539102f846457c573e742749b2128adc310ef8a5f48c6d3b5d433974f1c3898a7a92e3171d3fef822671243768282b7d7dcf7e9684743433a20285562756e747520352e342e302d367562756e7475317e31362e30342e342920352e342e30203230313630363039000000000000000000000";
";
int main()
{
	int i;	
	for (i=0; i<200; i++){
		printf("%x", xyz[i]);
	}
	printf("\n");
}