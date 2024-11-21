#include <stdlib.h>
#include <string.h>

#include "init.h"
#include "serialise.h"
#include "book.h"
#include "tree.h"
#include "list.h"
#include "error.h"


/*
 *  --- INTERNAL
 */

char * filename = "savefile.bin";


static inline int _add_to_trees(prog_data * pd, list_node * node) {


    int ret;

    tree_data td;
    book_entry * be = node->data;

    void * compare_data[NUM_TREES];

    compare_data[NAME]       = be->name;
    compare_data[OTHER_NAME] = be->other_name;
    compare_data[EMAIL]      = be->email;
    compare_data[PHONE]      = &be->phone;
    compare_data[TOWN]       = be->addr.town;
    compare_data[COUNTRY]    = be->addr.country;

    td.node = node;
    for (int i = 0; i < NUM_TREES; ++i) {

        td.data = compare_data[i];
        ret = tree_set(&pd->trees[i], compare_data[i], &td);
        if (ret == -1) return -1;
    }

    return 0;
}



static inline int _record_savefile(prog_data * pd, char * filename) {
    
    int ret;

    FILE * fs;
    list_node * node;
    book_entry * be;

    fs = fopen(filename, "w");
    if (fs == NULL) {
        error_code = ERR_FILE_OP;
    }

    //setup iteration
    node = pd->data_list.head;

    //save each node
    for (int i = 0; i < pd->data_list.len; ++i) {

        ret = record_entry(node->data, pd->data_list.data_size, fs, 
                           (i == pd->data_list.len - 1) ? true : false);
    
        if (ret == -1) goto record_savefile_finish;
        node = node->next;
    } 

    ret = 0;
    
record_savefile_finish:
    fclose(fs);
    return ret;
}



static inline int _read_savefile(prog_data * pd, char * filename) {

    int ret;

    FILE * fs;
    book_entry be;
    list_node * node;

    fs = fopen(filename, "r");
    if (fs == NULL) {
        error_code = ERR_FILE_OP;
    }

    do {

        ret = read_entry(&be, pd->data_list.data_size, fs);
        if (ret == -1) goto read_savefile_finish;

        node = list_append(&pd->data_list, &be);
        if (node == NULL) {
            ret = -1;
            goto read_savefile_finish;
        }

    } while (ret != 1);

    ret = 0;

read_savefile_finish:
    fclose(fs);
    return 0;
}



/*
 *  --- EXTERNAL
 */

void new_prog_data(prog_data * pd) {

    const size_t key_sizes[NUM_TREES] = {NAME_LEN, NAME_LEN, EMAIL_LEN, sizeof(uint64_t), ADDR_LEN, ADDR_LEN};
    enum tree_eval (*compare[NUM_TREES])(void *, void *) = {compare_name, compare_name, compare_email,
                                                            compare_phone, compare_addr, compare_addr};

    new_list(&pd->data_list, sizeof(book_entry));
    
    //for every sorted tree
    for (int i = 0; i < NUM_TREES; ++i) {

        new_tree(&pd->trees[i], key_sizes[i], sizeof(tree_data), compare[i]);
    }
    
    return;
}



void del_prog_data(prog_data * pd) {

    del_list(&pd->data_list);

    for (int i = 0; i < NUM_TREES; ++i) {
        del_tree(&pd->trees[i]);
    }

    return;
}



int init_prog(prog_data * pd) {

    int ret;

    list_node * node;

    _read_savefile(pd, filename);

    //populate search trees
    node = pd->data_list.head;
    for (int i = 0; i < pd->data_list.len - 1; ++i) {

        ret = _add_to_trees(pd, node);
        if (ret == -1) return -1;
        
        node = node->next;
    }

    return 0;
}


int fini_prog(prog_data * pd) {

    int ret;

    ret = _record_savefile(pd, filename);
    if (ret == -1) return -1;

    return 0;
}



int add_book_entry(prog_data * pd, book_entry * be) {

    int ret;
    list_node * node;
    tree_data td;

    void * compare_data[NUM_TREES];

    compare_data[NAME]       = be->name;
    compare_data[OTHER_NAME] = be->other_name;
    compare_data[EMAIL]      = be->email;
    compare_data[PHONE]      = &be->phone;
    compare_data[TOWN]       = be->addr.town;
    compare_data[COUNTRY]    = be->addr.country;


    //add to general storage
    node = list_append(&pd->data_list, be);
    if (node == NULL) return -1;

    td.node = node;
    for (int i = 0; i < NUM_TREES; ++i) {

        td.data = compare_data[i];
        ret = tree_set(&pd->trees[i], compare_data[i], td.data);
        if (ret == -1) return -1;
    }

    return 0;
}



int del_book_entry(prog_data * pd, query * q) {

    /*
     *  1) Use query type (e.g.: name) to arrive at tree node
     *
     *  2) Tree node stores pointer to list node, which contains all data for this entry
     *
     *  3) Use list entry to remove entry from every tree
     *
     *  4) Then delete list entry
     */


    int ret;

    tree_data td;
    book_entry * be;
    list_node * node;

    void * compare_data[NUM_TREES];

    //set compare_data to the query (all but one invalid)
    compare_data[NAME]       = q->keys.name;
    compare_data[OTHER_NAME] = q->keys.other_name;
    compare_data[EMAIL]      = q->keys.email;
    compare_data[PHONE]      = &q->keys.phone;
    compare_data[TOWN]       = q->keys.addr.town;
    compare_data[COUNTRY]    = q->keys.addr.country;

    //traverse tree with specific query term
    tree_get_val(&pd->trees[q->type], compare_data[q->type], &td);
    
    //fetch full entry from list
    node = td.node;
    be = node->data;

    //re-set compare_data to the full node
    compare_data[NAME]       = be->name;
    compare_data[OTHER_NAME] = be->other_name;
    compare_data[EMAIL]      = be->email;
    compare_data[PHONE]      = &be->phone;
    compare_data[TOWN]       = be->addr.town;
    compare_data[COUNTRY]    = be->addr.country;

    //delete node from every tree
    for (int i = 0; i < NUM_TREES; ++i) {

        ret = tree_remove(&pd->trees[i], compare_data[i]);
        if (ret == -1) return -1;
    }

    return 0;
}



book_entry * get_book_entry(prog_data * pd, query * q) {

    int ret;

    tree_data td;
    list_node * node;

    void * compare_data[NUM_TREES];

    compare_data[NAME]       = q->keys.name;
    compare_data[OTHER_NAME] = q->keys.other_name;
    compare_data[EMAIL]      = q->keys.email;
    compare_data[PHONE]      = &q->keys.phone;
    compare_data[TOWN]       = q->keys.addr.town;
    compare_data[COUNTRY]    = q->keys.addr.country;

    //traverse tree with specific query term
    tree_get_val(&pd->trees[q->type], compare_data[q->type], &td);
    
    //fetch full entry from list
    node = td.node;
    return node->data;
}
