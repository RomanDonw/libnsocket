#ifndef INIT_H
#define INIT_H

#include <stdbool.h>

extern volatile bool inited;

#define ENSURE_INIT
//#define ENSURE_INIT(return_value) { if (!inited) return return_value; }

#endif