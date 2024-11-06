#ifndef LIST_H
#define LIST_H

#include <unistd.h>


// [list]
struct _list_node {

    void * data;
    struct _list_node * next;
    struct _list_node * prev;

};
typedef struct _list_node list_node;


typedef struct {

    int len;
    size_t data_size;
    list_node * head;

} list;


//controls if user provided index should be verified for accessing elements
//or for adding new elements
enum _index_mode {INDEX = 0, ADD_INDEX = 1};


//function declarations
int list_get_val(const list * ls, const int index, void * buf);
void * list_get_ref(const list * ls, const int index);
list_node * list_get_node(const list * ls, const int index);

list_node * list_set(list * ls, const int index, const void * data);
list_node * list_insert(list * ls, const int index, const void * data);
list_node * list_append(list * ls, const void * data);
int list_remove(list * ls, const int index);

void new_list(list * ls, const size_t data_size);
void del_list(list * ls);


#endif
