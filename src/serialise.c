#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <unistd.h>

#include "serialise.h"
#include "list.h"
#include "book.h"
#include "error.h"


//wrapper for fwrite()
static inline int _record_value(void * value, 
                                size_t size, size_t nmemb, FILE * fs) {

    size_t write_last;

    write_last = fwrite(value, size, nmemb, fs);
    if (write_last != 1) {
        error_code = ERR_FILE;
        return -1;
    }

    return 0;
}


//wrapper for fread
static inline int _read_value(void * value, 
                              size_t size, size_t nmemb, FILE * fs) {

    size_t read_last;

    read_last = fread(value, size, nmemb, fs);
    if (read_last != 1) {
        error_code = ERR_FILE;
        return -1;
    }

    return 0;
}


static int _record_entry(void * data, size_t size, FILE * fs) {

    int ret;
    uint16_t sync = SYNC;

    ret = _record_value(data, size, 1, fs);
    if (ret == -1) return -1;

    ret = _record_value(&sync, sizeof(sync), 1, fs);
    if (ret == -1) return -1;

    return 0;
}


static int _read_entry(void * data, size_t size, FILE * fs) {

    int ret;
    uint16_t sync = SYNC;
}
