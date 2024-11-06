#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>

#include "list.h"
#include "error.h"


/*
 *  --- INTERNAL ---
 */

static list_node * _traverse(const list * ls, int index) {

    list_node * node = ls->head;

    //traverse, checking for premature NULL pointers
    while ((node != NULL) && (index != 0)) {

        node = node->next;
        --index;
    }

    if (index) {
        error_code = ERR_INTERNAL_INDEX;
        return NULL;
    }
    return node;
}



static list_node * _new_list_node(const list * ls, const void * data) {

    //allocate node structure
    list_node * new_node = malloc(sizeof(list_node));
    if (!new_node) {
        error_code = ERR_MALLOC;
        return NULL;
    }

    //allocate data
    new_node->data = malloc(ls->data_size);
    if (!new_node->data) {
        error_code = ERR_MALLOC;
        return NULL;
    }

    //copy data into node
    memcpy(new_node->data, data, ls->data_size);

    return new_node;
}



static void _del_list_node(list_node * node) {

    free(node->data);
    free(node);

    return;
}



static void _set_head_node(list * ls, list_node * node) {

    node->next = node->prev = NULL;

    ls->head = node;
    ++ls->len;

    return;
}



static void _add_node(list * ls, list_node * node, list_node * prev_node, 
                      list_node * next_node, const int index) {

    prev_node->next = node;
    node->prev = prev_node;

    next_node->prev = node;
    node->next = next_node;

    if (index == 0) ls->head = node;
    ++ls->len;

    return;
}



static void _sub_node(list * ls, list_node * prev_node, 
                      list_node * next_node, const int index) {

    //if there are no nodes left
    if (prev_node == NULL && next_node == NULL) {
        
        ls->head = NULL;

    //if there is one node left
    } else if (prev_node == next_node) {
        
        prev_node->next = next_node->prev = NULL;

    //if there are multiple nodes left
    } else {

        prev_node->next = next_node;
        next_node->prev = prev_node;       
    }

    //update head if it was removed
    if (index == 0) ls->head = next_node;

    return;
}



static int _empty(list * ls) {
 
    list_node * node = ls->head, * next_node;
    int index = ls->len;

    while ((node != NULL) && (index != 0)) {

        next_node = node->next;
        _del_list_node(node);
        node = next_node;
        --index;
    }

    if (index) {
        error_code = ERR_INTERNAL_INDEX;
        return -1;
    }

    ls->len = 0;
    ls->head = NULL;

    return 0;
}



static inline int _assert_index_range(const list * ls, 
                                      const int index, enum _index_mode mode) {
   
    /*
     *  if inserting, maximum index needs to be +1 higher than for other operations
     */
    if (abs(index) >= (ls->len + mode)) {
        error_code = ERR_USER_INDEX;
        return -1;
    }

    return 0;
}



/*
 *  --- EXTERNAL ---
 */

int list_get_val(const list * ls, const int index, void * buf) {

    if (_assert_index_range(ls, index, INDEX)) return -1;

    //get the node
    list_node * node = _traverse(ls, index);
    if (!node) return -1;

    memcpy(buf, node->data, ls->data_size);
    
    return 0;
}



void * list_get_ref(const list * ls, const int index) {

    if (_assert_index_range(ls, index, INDEX)) return NULL;

    //get the node
    list_node * node = _traverse(ls, index);
    if (!node) return NULL;

    return node->data;
}



list_node * list_get_node(const list * ls, const int index) {

    if (_assert_index_range(ls, index, INDEX)) return NULL;

    //get the node
    return _traverse(ls, index);
}



list_node * list_set(list * ls, const int index, const void * data) {

    if (_assert_index_range(ls, index, INDEX)) return NULL;

    //get the node
    list_node * node = _traverse(ls, index);
    if (!node) return NULL;

    memcpy(node->data, data, ls->data_size);

    return node;
}



list_node * list_insert(list * ls, const int index, const void * data) {

    list_node * prev_node, * next_node;

    if (_assert_index_range(ls, index, ADD_INDEX)) return NULL;

    //create new node
    list_node * new_node = _new_list_node(ls, data);
    if (!new_node) return NULL;
    
    //get the _prev_ and _next_ of the new node as required
    if (ls->len == 0) {
        _set_head_node(ls, new_node);
    
    } else if (ls->len == 1) { 
        next_node = prev_node = ls->head; 
        
        _add_node(ls, new_node, prev_node, next_node, index);

    } else { 
        if (index == ls->len) {
            prev_node = ls->head->prev;
        } else {
            prev_node = _traverse(ls, index-1);
            if (!prev_node) {
                _del_list_node(new_node);
                return NULL;
            }
        }
        next_node = prev_node->next;

        _add_node(ls, new_node, prev_node, next_node, index);
    }

    return new_node;
}



list_node * list_append(list * ls, const void * data) {

    list_node * new_node = _new_list_node(ls, data);
    if (!new_node) return NULL;

    //add node to list
    if (ls->len == 0) {
        _set_head_node(ls, new_node);
    
    } else if (ls->len == 1) {
        _add_node(ls, new_node, ls->head, ls->head, -1);
    
    } else {
        _add_node(ls, new_node, ls->head->prev, ls->head, -1);
    }

    return new_node;
}



int list_remove(list * ls, const int index) {
 
    if (_assert_index_range(ls, index, INDEX)) return -1;

    //get the node
    list_node * del_node = _traverse(ls, index);
    if(!del_node) return -1;

    _sub_node(ls, del_node->prev, del_node->next, index);
    _del_list_node(del_node);

    --ls->len;

    return 0;
}



void cm_new_list(list * ls, const size_t data_size) {

    ls->len = 0;
    ls->data_size = data_size;
    ls->head = NULL;
    return;
}



void cm_del_list(list * ls) {

    int len = ls->len;
    list_node * del_node;

    //delete each node in list
    for (int i = 0; i < len; ++i) {

        del_node = ls->head;
        _sub_node(ls, del_node->prev, del_node->next, 0);
        _del_list_node(del_node);
    
    } //end for

    return;
}
