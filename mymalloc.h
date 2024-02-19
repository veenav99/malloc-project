#ifndef _MYMALLOC_H
#define _MYMALLOC_H
#include<stddef.h>
#include <stdlib.h>
#include "mymalloc.h"

#define malloc(s)   mymalloc(s, __FILE__, __LINE__)
#define free(p)     myfree(p, __FILE__, __LINE__)

void *mymalloc(size_t size, char *file, int line);
void myfree(void *ptr, char *file, int line);

#endif