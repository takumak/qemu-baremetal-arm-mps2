#include <stdio.h>

#include "semihosting.h"

#define PI 3.141592653589793

static double circle_area(double r) {
	return r*r*PI;
}

void main() {
	printf("Hello, world\n");
	printf("circle_area(3) = %f\n", circle_area(3));
	semihosting_shutdown();
}
