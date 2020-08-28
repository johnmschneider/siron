#ifndef IRAND_H
	#define IRAND_H
	#include <stdint.h>

	/**
	* Generates a number between min and max (inclusive).
	*/
	int32_t irand(int32_t min, int32_t max);
	int32_t irand(int32_t max);
#endif