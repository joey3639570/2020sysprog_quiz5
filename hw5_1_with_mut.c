#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double divop(double orig, int slots) {
	if (slots == 1 || orig == 0)
		return orig;
	int od = slots & 1;
	double result = divop(orig * 0.5, od ? (slots + 1) >> 1 : slots >> 1);
	if (od)
		result += divop(result, slots);
	return result;
}

int main()
{
	double answer;
	double time_taken;
	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC, &start);
	answer = divop(32.,5);
	clock_gettime(CLOCK_MONOTONIC, &end);
	time_taken = (end.tv_sec - start.tv_sec) * 1e9;
	time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;
	printf("Execution Time :: %.20f\n", time_taken);
	printf("answer :: %lf\n",answer);

	return 0;
}
