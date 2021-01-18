#include <stdio.h>
#include <stdlib.h>

double divop(double orig, int slots) {
	if (slots == 1 || orig == 0)
		return orig;
	int od = slots & 1;
	printf("od : %.15d\n", od);
	double result = divop(orig /2, od ? (slots + 1) >> 1 : slots >> 1);
	if (od)
		result += divop(result, slots);
	printf("result : %.15lf", result);
	return result;
}

int main()
{
	double answer;
	answer = divop(32.,5);
	printf("answer :: %.15lf\n",answer);

	return 0;
}
