#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void getDDMMSSSS(float x, unsigned int *DD, unsigned int *MM, unsigned int *SSSS)
{
	*DD = x;
	*MM = (x * 100) - ((*DD)*100);
	*SSSS = (x * 1000000) - ((*DD)*1000000) - ((*MM)*10000);
}
