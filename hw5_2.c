#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define M 10000
#define AVG 10
#define PRECISION 1e-3

#include <stdint.h>

/* A union allowing us to convert between a float and a 32-bit integers.*/
typedef union {
	float value;
	uint32_t v_int;
} ieee_float_shape_type;

/* Set a float from a 32 bit int. */
#define  INSERT_WORDS(d, ix0)	        \
	do {                                \
		ieee_float_shape_type iw_u;     \
		iw_u.v_int = (ix0);         \
		(d) = iw_u.value;               \
	} while (0)

/* Get a 32 bit int from a float. */
#define EXTRACT_WORDS(ix0, d)        \
	do {                             \
		ieee_float_shape_type ew_u;  \
		ew_u.value = (d);            \
		(ix0) = ew_u.v_int;          \
	} while (0)

static const float one = 1.0, tiny = 1.0e-30;

float ieee754_sqrt(float x)
{
	float z;
	int32_t sign = 0x80000000;
	uint32_t r;
	int32_t ix0, s0, q, m, t, i;

	EXTRACT_WORDS(ix0, x);

	/* take care of zero */
	if (ix0 <= 0) {
		if ((ix0 & (~sign)) == 0)
			return x; /* sqrt(+-0) = +-0 */
		if (ix0 < 0)
			return (x - x) / (x - x); /* sqrt(-ve) = sNaN */
	}
	/* take care of +INF and NaN */
	if ((ix0 & 0x7f800000) == 0x7f800000) {
		/* sqrt(NaN) = NaN, sqrt(+INF) = +INF,*/
		return x;
	}
	/* normalize x */
	m = (ix0 >> 23);
	if (m == 0) { /* subnormal x */
		for (i = 0; (ix0 & 0x00800000) == 0; i++)
			ix0 <<= 1;
		m -= i - 1;
	}
	m -= 127; /* unbias exponent */
	ix0 = (ix0 & 0x007fffff) | 0x00800000;
	if (m & 1) { /* odd m, double x to make it even */
		ix0 += ix0;
	}
	m >>= 1; /* m = [m/2] */

	/* generate sqrt(x) bit by bit */
	ix0 += ix0;
	q = s0 = 0; /* [q] = sqrt(x) */
	r = 0x01000000;       /* r = moving bit from right to left */

	while (r != 0) {
		t = s0 + r;
		if (t <= ix0) {
			s0 = t + r;
			ix0 -= t;
			q += r;
		}
		ix0 += ix0;
		r >>= 1;
	}

	/* use floating add to find out rounding direction */
	if (ix0 != 0) {
		z = one - tiny; /* trigger inexact flag */
		if (z >= one) {
			z = one + tiny;
			if (z > one)
				q += 2;
			else
				q += (q & 1);
		}
	}

	ix0 = (q >> 1) + 0x3f000000;
	ix0 += (m << 23);

	INSERT_WORDS(z, ix0);

	return z;
}

//二分法
//二分法通過縮小根值範圍的方法來逼近結果
float sqrt_binary(float n) {
	if (n == 0 || n == 1)
		return n;
	float min, max, mid; //min代表下邊界，max代表上邊界，mid爲中間值也作爲近似值
	min = 0;
	max = n;
	mid = n / 2;
	while (mid*mid>n + PRECISION || mid*mid<n - PRECISION)
	{
		mid = (max + min) / 2;
		if (mid*mid < n + PRECISION) {
			min = mid; //根值偏小，升高下邊界
		};
		if (mid*mid > n - PRECISION) {
			max = mid;//根值偏大，降低上邊界
		}
	}
	return mid;
}

//牛頓法
float sqrt_newton(float n) {
	if (n == 0 || n == 1)
		return n;
	float k = n;
	while (1) {
		if (k*k > n - PRECISION && k*k < n + PRECISION) {
			break;
		}
		k = 0.5*(k + n/k);//通過牛頓法得出
	}
	return k;
}

int main() {

	int i, j;	
	float RefSpeed=0;
	double RefTotalPrecision=0;
	float Speed=0;
	double TotalPrecision=0;
	double Temp=0;
	struct timespec start, end;
        double time_taken;

	/** sqrt ****************************************************************/

	for(j=0;j<AVG;j++)	{	
		clock_gettime(CLOCK_MONOTONIC, &start);
		
		for(i=1;i<M;i++)
			RefTotalPrecision+=sqrt((float) i);
	
      		clock_gettime(CLOCK_MONOTONIC, &end);
	
		time_taken = (end.tv_sec - start.tv_sec) * 1e9;
        	time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;
		Temp+=time_taken;
	
	}
	
	RefTotalPrecision/=AVG;
	Temp/=AVG;
	RefSpeed=(float)(Temp)/CLOCKS_PER_SEC;

	printf("Sqrt in math.h \n");
	printf("Speed of sqrt as a refrence = 100\n");
	printf("Precision of sqrt as a refrence = 100\n");
	Temp=0;
	

	/************************************************************************/

        /** sqrt_binary *********************************************************/

        for(j=0;j<AVG;j++)  {
                clock_gettime(CLOCK_MONOTONIC, &start);

                for(i=1;i<M;i++)
                        TotalPrecision+=sqrt_binary((float) i);

                clock_gettime(CLOCK_MONOTONIC, &end);

                time_taken = (end.tv_sec - start.tv_sec) * 1e9;
                time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;
                Temp+=time_taken;

        }
	
	// Calulating
        TotalPrecision/=AVG;
        Temp/=AVG;
	Speed=(float)(Temp)/CLOCKS_PER_SEC;

	// Output
	printf("Sqrt with binary method \n");
        printf("Speed = %lf\n", 100*RefSpeed/Speed);
        printf("Precision = %lf\n", (double)(1-abs((TotalPrecision-RefTotalPrecision)/(RefTotalPrecision)))*100);

	// Reset
	TotalPrecision=0;
        Temp=0;


        /************************************************************************/

        /** sqrt_newton *********************************************************/

        for(j=0;j<AVG;j++)  {
                clock_gettime(CLOCK_MONOTONIC, &start);

                for(i=1;i<M;i++)
                        TotalPrecision+=sqrt_newton((float) i);

                clock_gettime(CLOCK_MONOTONIC, &end);

                time_taken = (end.tv_sec - start.tv_sec) * 1e9;
                time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;
                Temp+=time_taken;

        }

	// Calulating
        TotalPrecision/=AVG;
        Temp/=AVG;
        Speed=(float)(Temp)/CLOCKS_PER_SEC;

        // Output
        printf("Sqrt with newton method \n");
        printf("Speed = %lf\n", 100*RefSpeed/Speed);
        printf("Precision = %lf\n", (double)(1-abs((TotalPrecision-RefTotalPrecision)/(RefTotalPrecision)))*100);

        // Reset
        TotalPrecision=0;
        Temp=0;


        /************************************************************************/

        /** ieee754_sqrt ********************************************************/

        for(j=0;j<AVG;j++)  {
                clock_gettime(CLOCK_MONOTONIC, &start);

                for(i=1;i<M;i++)
                        TotalPrecision+=sqrt((float) i);

                clock_gettime(CLOCK_MONOTONIC, &end);

                time_taken = (end.tv_sec - start.tv_sec) * 1e9;
                time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;
                Temp+=time_taken;

        }

	// Calulating
        TotalPrecision/=AVG;
        Temp/=AVG;
        Speed=(float)(Temp)/CLOCKS_PER_SEC;

        // Output
        printf("ieee754_sqrt \n");
        printf("Speed = %lf\n", 100*RefSpeed/Speed);
        printf("Precision = %lf\n", (double)(1-abs((TotalPrecision-RefTotalPrecision)/(RefTotalPrecision)))*100);

        // Reset
        TotalPrecision=0;
        Temp=0;


        /************************************************************************/
	return 0;
}
