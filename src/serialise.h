#ifndef SERIALISE_H
#define SERIALISE_H

#include <stdio.h>
#include <stdbool.h>

#include "list.h"


#define SYNC 0x1337
#define END 0xdead

int record_entry(void * data, size_t size, FILE * fs, bool final);
int read_entry(void * data, size_t size, FILE * fs);


#endif
