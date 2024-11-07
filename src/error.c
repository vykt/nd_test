#include <stdio.h>

#include "error.h"


int error_code;


void get_error() {

    switch (error_code) {

        // 1XX - user errors
        case ERR_USER_KEY:
            fprintf(stderr, ERR_USER_KEY_MSG);
            break;

        case ERR_USER_INDEX:
            fprintf(stderr, ERR_USER_INDEX_MSG);
            break;

        // 2XX - internal errors
        case ERR_INTERNAL_KEY:
            fprintf(stderr, ERR_INTERNAL_KEY_MSG);
            break;

        case ERR_INTERNAL_INDEX:
            fprintf(stderr, ERR_INTERNAL_INDEX_MSG);
            break;

        // 3XX - environment errors
        case ERR_MALLOC:
            fprintf(stderr, ERR_MALLOC_MSG);
            break;

        case ERR_FILE_OP:
            fprintf(stderr, ERR_FILE_OP_MSG);
    
        case ERR_FILE_SYNC:
            fprintf(stderr, ERR_FILE_SYNC_MSG);
    }

    return;
}
