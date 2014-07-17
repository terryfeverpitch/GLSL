#include "svl/svl.h"
#include "stdio.h"
#include "windows.h"

LARGE_INTEGER freq, before, after;

int main() 
{
	Mat m, q;
	
	int i, j;
	int size[5] = {10, 50, 100, 500, 1000};
	double time[5];
	
	QueryPerformanceFrequency(&freq);
	
	for(int ii = 0 ; ii < 5 ; ii++) {
		m.SetSize(size[ii], size[ii]);
		
		for(i = 0 ; i < 5 ; i++) {
			for(j = 0 ; j < 5 ; j++) {
				m[i][j] = i + j;
			}
		}
		
		q = m;

		QueryPerformanceCounter(&before);
		m = m * q;
		QueryPerformanceCounter(&after);
		time[ii] = ((double)(after.QuadPart - before.QuadPart)) / freq.QuadPart;
	}
	
	for(ii = 0 ; ii < 5 ; ii++)
		printf("%e\n", time[ii]);

	return 0;
}
