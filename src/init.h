#ifndef INIT_H
#define INIT_H

#include "serialise.h"
#include "book.h"
#include "tree.h"
#include "list.h"
#include "error.h"



#define NAME 0
#define OTHER_NAME 1
#define EMAIL 2
#define PHONE 3
#define TOWN 4
#define COUNTRY 5

#define NUM_TREES 6

//main data structure
typedef struct {

    //all book entries
    list data_list;

    //sorted book entries
    tree trees[NUM_TREES];

} prog_data;


typedef struct {

    void * data;
    list_node * node;

} tree_data;


typedef struct {

    book_entry keys;
    int type; //NAME, OTHER_NAME, ...

} query;


void new_prog_data(prog_data * pd);
void del_prog_data(prog_data * pd);

int init_prog(prog_data * pd);
int fini_prog(prog_data * pd);

int add_book_entry(prog_data * pd, book_entry * be);
int del_book_entry(prog_data * pd, query * q);
book_entry * get_book_entry(prog_data * pd, query * q);


#endif
