#ifndef ERROR_H
#define ERROR_H


extern int error_code;

void get_error();


// --- ERROR CODES ---

// 1XX - user errors
#define ERR_USER_KEY         100
#define ERR_USER_INDEX       101

// 2XX - internal errors
#define ERR_INTERNAL_KEY     200
#define ERR_INTERNAL_INDEX   201

// 3XX - environment errors
#define ERR_MALLOC           300
#define ERR_FILE_OP          301
#define ERR_FILE_SYNC        302


// [error code messages]

// 1XX - user errors
#define ERR_USER_KEY_MSG       "Key not present in tree.\n"
#define ERR_USER_INDEX_MSG     "List index out of range.\n"

// 2XX - internal errors
#define ERR_INTERNAL_KEY_MSG   "Internal error processing tree key.\n"
#define ERR_INTERNAL_INDEX_MSG "Internal error processing list index.\n"

// 3XX - environmental errors
#define ERR_MALLOC_MSG         "Call to malloc() failed.\n"
#define ERR_FILE_OP_MSG        "Failed to read/write to savefile.\n"
#define ERR_FILE_SYNC_MSG      "Savefile corruption detected, out of sync.\n"


#endif
