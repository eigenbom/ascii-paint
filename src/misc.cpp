#include <cstring>
#include "misc.h"

void swap(int* a, int* b) {
	int t = *a;
	*a = *b;
	*b = t;
}

void swap(float* a, float* b) {
	float t = *a;
	*a = *b;
	*b = t;
}

int min(int x, int y) {
	if(x < y)
		return x;
	return y;
}

int max(int x, int y) {
	if(x > y)
		return x;
	return y;
}

char *getBaseFilename(const char *filename) {
	char *base = new char[1024];
	base[0] = '0';
	int extensionPos = -1;
	for(int i = 0; filename[i] != '\0'; i++) {
		base[i] = filename[i];
		if(base[i] == '.')
			extensionPos = i;
		if(base[i] == '\\' || base[i] == '/')
			extensionPos = -1;
	}
	if(extensionPos != -1)
		base[extensionPos] = '\0';

	return base;
}
