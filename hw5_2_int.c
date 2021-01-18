#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

//二分法通過縮小根值範圍的方法來逼近結果
int sqrt1(int n) {
	int min, max, mid; //min代表下邊界，max代表上邊界，mid爲中間值也作爲近似值
	min = 0;
	max = n;
	mid = n >> 1;
	while (min < max)
	{
		mid = (max + min) >> 1;
		int r = mid - (n / mid);
		if (r == 0)
			return mid;
		(r < 0) ? (min = mid) : (max = mid);
	}
	return mid;
}

//牛頓法
int sqrt2(int x){
    
    if (x == 0 || x == 1)
        return x;
    if (x == INT_MAX) // avoid overflow
        x-=1;
    
    unsigned int a; // using right shift
    a = (x + 1) >> 1;
    
    while (a > x/a){
        a = (a + x/a) >> 1;
    }
    
    return a;
}

int main() {
	int a = 2;
	int res1, res2, res3;
	res1 = sqrt1(a);
	res2 = sqrt2(a);
	res3 = sqrt(a);	

	printf("binary :: \t%d\n",res1);
	printf("newton :: \t%d\n",res2);
	printf("sqrt :: \t%d\n",res3);


	return 0;
}
