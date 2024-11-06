#ifndef BOOK_H
#define BOOK_H


#include <stdint.h>

#include "tree.h"


#define NAME_LEN 32
#define EMAIL_LEN 64
#define ADDR_LEN 64

typedef struct {

    char name[NAME_LEN];
    char other_name[NAME_LEN];
    char email[EMAIL_LEN];
    uint64_t phone;

    struct {

        char street[ADDR_LEN];
        char town[ADDR_LEN];
        char country[ADDR_LEN];

    } addr;

} book_entry;



//use for comparisons with trees
enum tree_eval compare_name(void *, void *);
enum tree_eval compare_email(void *, void *);
enum tree_eval compare_phone(void *, void *);
enum tree_eval compare_addr(void *, void *);


#endif
