#include "irand.h"
#include "frand.h"

int32_t irand(int32_t min, int32_t max) {
	//add 0.5f before truncating to see if we reach the next integer
	return (int32_t) (min + (frand() * (max - min)) + 0.5f);
}

int32_t irand(int32_t max) {
	return irand(0, max);
}