#ifndef TREE_H
#define TREE_H

#include <stdint.h>
#include <unistd.h>


enum tree_eval {LESS, EQUAL, MORE, ROOT};

struct _tree_node {

    char * data;
    char * key;

    struct _tree_node * left;
    struct _tree_node * right;
    
    struct _tree_node * parent;
    enum tree_eval parent_eval; //if this node is parent's left or right

};
typedef struct _tree_node tree_node;


typedef struct {

    int size;
    size_t key_size;
    size_t data_size;
    tree_node * root;

    enum tree_eval (*compare)(void *, void *);

} tree;


int tree_get_val(const tree * tree, void * key, void * buf);
void * tree_get_ref(const tree * tree, void * key);
int tree_set(tree * tree, void * key, void * data);
int tree_remove(tree * tree, void * key);

void new_tree(tree * tree, 
              const size_t key_size, const size_t data_size,
              enum tree_eval (*compare)(void *, void *));
void del_tree(tree * tree);



#endif
