#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>

#include "tree.h"
#include "error.h"

/*
 *  --- INTERNAL ---
 */


static tree_node * _new_tree_node(const tree * tree,
                                  const void * key, const void * data) {

    //allocate node structure
    tree_node * new_node = malloc(sizeof(tree_node));
    if (!new_node) {
        error_code = ERR_MALLOC;
        return NULL;
    }

    //allocate key
    new_node->key = malloc(tree->key_size);
    if (!new_node->key) {
        error_code = ERR_MALLOC;
        return NULL;
    }

    //allocate data
    new_node->data = malloc(tree->data_size);
    if (!new_node->data) {
        error_code = ERR_MALLOC;
        return NULL;
    }

    //copy data into node
    memcpy(new_node->data, data, tree->data_size);

    //null out parent until node is attached to tree
    new_node->parent = NULL;

    return new_node;
}



static void _del_tree_node(tree_node * node) {

    free(node->key);
    free(node->data);
    free(node);

    return;
}


static tree_node * _traverse(const tree * tree, 
                             void * key, enum tree_eval * eval) {

    tree_node * node = tree->root;
    bool found = false;
    *eval = ROOT;


    //traverse tree
    while (node != NULL && !found) {

        *eval = tree->compare(key, node->key);

        switch (*eval) {

            case LESS:
                if (!node->left) return node;
                node = node->left;
                break;

            case EQUAL:
                found = true;
                break;

            case MORE:
                if (!node->right) return node;
                node = node->right;
                break;

            //makes clangd quiet
            case ROOT:
                break;

        } //end switch
    
    } //end while

    return node;
}



static inline void _set_root(tree * tree, tree_node * node) {

    node->parent_eval = ROOT;    
    tree->root = node;

    return;
}



static void _transplant(tree * tree, tree_node * subj_node, tree_node * tgt_node) {

    //if deleting root, disconnect subject and target nodes and set target node as root
    if (tree->root == subj_node) {

        tree->root            = tgt_node;
        tgt_node->parent      = NULL;
        tgt_node->parent_eval = ROOT;
   
    } else {
        
        if (subj_node->parent_eval == MORE) subj_node->parent->right = tgt_node;
        if (subj_node->parent_eval == LESS) subj_node->parent->left  = tgt_node;
        
        if (tgt_node != NULL) {
            tgt_node->parent      = subj_node->parent;
            tgt_node->parent_eval = subj_node->parent_eval;
        }
    }

    return;
}



static tree_node * _right_min(tree_node * node) {

    //bootstrap min traversal
    if (node->right == NULL) return node;
    node = node->right;

    //iterate until min reached
    while (node->left != NULL) {
        node = node->left;
    }

    //return min
    return node;
}



static inline void _empty(tree_node * node) {

    if (node == NULL) return;

    _empty(node->left);
    _empty(node->right);

    _del_tree_node(node);

    return;
}



static inline int _add_node(tree * tree, void * key, void * data, 
                            tree_node * parent, const enum tree_eval eval) {
    int ret;

    //create new node
    tree_node * node = _new_tree_node(tree, key, data);
    if (node == NULL) return -1;

    //if tree is empty, set root
    if (tree->size == 0) {
        _set_root(tree, node);

    //else connect node
    } else {
        if (eval == LESS) parent->left = node;
        if (eval == MORE) parent->right = node;
    }

    //increment tree size
    tree->size += 1;

    return 0;
}



static inline int _remove_node(tree * tree, tree_node * node) {
 
    int ret;
    
    tree_node * min_node;


    //node has no children
    if (node->left == NULL && node->right == NULL) {

        if (tree->root == node) {
            tree->root = NULL;
        } else {
            if (node->parent_eval == MORE) node->parent->right = NULL;
            if (node->parent_eval == LESS) node->parent->left  = NULL;
        }

    //node only has a right child
    } else if (node->left == NULL) {

        _transplant(tree, node, node->right);

    //node only has a left child
    } else if (node->right == NULL) {

        _transplant(tree, node, node->left);

    //node has both a left and a right child
    } else {
       
        //get minimum node in right subtree
        min_node = _right_min(node);

        //cut minimum node from right subtree
        _transplant(tree, min_node, min_node->right);
        
        //re-attach minimum node as root of right subtree
        if (node->right == min_node) {
            min_node->right = NULL;
        } else {
            min_node->right              = node->right; 
            min_node->right->parent      = min_node;
            min_node->right->parent_eval = MORE;
        }

        //set minimum node as new root of whole tree
        _transplant(tree, node, min_node);

        //re-attach minimum node to left subtree
        min_node->left              = node->left;
        min_node->left->parent      = min_node;
        min_node->left->parent_eval = LESS;
    }

    //remove node from tree
    tree->size -= 1;
    _del_tree_node(node);

    return 0;
}



/*
 *  --- EXTERNAL ---
 */

int tree_get_val(const tree * tree, void * key, void * buf) {

    enum tree_eval eval;

    //get the node
    tree_node * node = _traverse(tree, key, &eval);
    if (eval != EQUAL) {
        
        error_code = ERR_USER_KEY;
        return -1;
    }

    memcpy(buf, node->data, tree->data_size);

    return 0;
}



void * tree_get_ref(const tree * tree, void * key) {

    enum tree_eval eval;

    //get the node
    tree_node * node = _traverse(tree, key, &eval);
    if (eval != EQUAL) {
    
        error_code = ERR_USER_KEY;
        return NULL;
    }

    return node->data;
}



int tree_set(tree * tree, void * key, void * data) {

    int ret;

    enum tree_eval eval;
    tree_node * new_node;

    //get relevant node
    tree_node * node = _traverse(tree, key, &eval);

    //if a node already exists for this key, update its value
    if (eval == EQUAL) {
        memcpy(node->data, data, tree->data_size);
        return 0;

    //else create a new node
    } else { 
        ret = _add_node(tree, key, data, node, eval);
        if (ret == -1) return -1;
        return 0;
    }
}



int tree_remove(tree * tree, void * key) {

    int ret;

    enum tree_eval eval;

    //get relevant node
    tree_node * node = _traverse(tree, key, &eval);

    //if a node doesn't exist for this key, error out
    if (eval != EQUAL || node == NULL) {
        error_code = ERR_USER_KEY;
        return -1;
    }

    //remove node
    ret = _remove_node(tree, node);

    return 0;
}



void new_tree(tree * tree, 
              const size_t data_size, const size_t key_size,
              enum tree_eval (*compare)(void *, void*)) {

    tree->size = 0;
    tree->key_size = 0;
    tree->data_size = 0;
    tree->root = NULL;
    tree->compare = compare;

    return;
}



void del_tree(tree * tree) {

    _empty(tree->root);

    return;
}
