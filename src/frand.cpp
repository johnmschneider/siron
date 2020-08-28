#include "frand.h"

float frand()
{
    return ((float) rand())/RAND_MAX;
}
