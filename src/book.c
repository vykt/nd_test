#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <unistd.h>

#include "book.h"
#include "tree.h"


//use: name, other_name
enum tree_eval compare_name(void * key1, void * key2) {

    int ret;

    ret = strncmp((char *) key1, (char *) key2, NAME_LEN);
    if (!ret) return EQUAL;
    return ret > 0 ? MORE : LESS;
}


//use: email
enum tree_eval compare_email(void * key1, void * key2) {

    int ret;

    ret = strncmp((char *) key1, (char *) key2, EMAIL_LEN);
    if (!ret) return EQUAL;
    return ret > 0 ? MORE : LESS;
}



//use: phone
enum tree_eval compare_phone(void * key1, void * key2) {

    uint64_t num1 = *((uint64_t *) key1);
    uint64_t num2 = *((uint64_t *) key2);
    
    uint64_t ret = num1 - num2;

    if (!ret) return EQUAL;
    return ret > 0 ? MORE : LESS;
}


//use: addr
enum tree_eval compare_addr(void * key1, void * key2) {

    int ret;

    ret = strncmp((char *) key1, (char *) key2, ADDR_LEN);
    if (!ret) return EQUAL;
    return ret > 0 ? MORE : LESS;
}
