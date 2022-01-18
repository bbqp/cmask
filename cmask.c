#include "cmask.h"

static inline void lt_sc(char a, char b)
{
	return a < b;
}

void cmask_comp_char(const char *array, int length, enum comparison, char key, int *indices, int *nindices)
{
	void (*cmp)(char, char) = lt_sc;
	int nind = 0;
	int i;
	
	memset(indices, -1, length);
	
	for (i = 0; i < length; i++) {
		if (cmp(array[i], key) != 0) {
			indices[nind++] = i;
		}
	}
}