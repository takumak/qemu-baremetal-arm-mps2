#include <stddef.h>

int memcmp(const void *s1, const void *s2, size_t n) {
	const unsigned char *a = s1;
	const unsigned char *b = s2;
	size_t i;

	for (i = 0; i < n; i++) {
		if (a[i] != b[i]) {
			return (int)a[i] - (int)b[i];
		}
	}
	return 0;
}

void *memset(void *b, int c, size_t len) {
	unsigned char *d = b;
	size_t i;

	for (i = 0; i < len; i++) {
		d[i] = c;
	}
	return b;
}

void *memcpy(void *dst, const void *src, size_t n) {
	unsigned char *d = dst;
	const unsigned char *s = src;
	size_t i;

	for (i = 0; i < n; i++) {
		d[i] = s[i];
	}
	return dst;
}

void *memmove(void *dst, const void *src, size_t n) {
	unsigned char *d = dst;
	const unsigned char *s = src;
	size_t i;

	if ((unsigned long)dst == (unsigned long)src || n == 0) {
		return dst;
	}

	if ((unsigned long)dst < (unsigned long)src) {
		for (i = 0; i < n; i++) {
			d[i] = s[i];
		}
	} else {
		for (i = 0; i < n; i++) {
			d[n - 1 - i] = s[n - 1 - i];
		}
	}
	return dst;
}
